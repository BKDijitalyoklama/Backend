using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Mail;
using System.Text;
using System.Threading.Tasks;
using static ServerApp_Main.Modules.DBModule.Wrappers.Wrapper;

namespace ServerApp_Main.Modules.ReportBuilderModule
{
    internal static class ReportBuilder
    {


        class UserEntrylogConnection
        {
            private string namesurname;
            private string subjectname;

            private EntryLog? entry1 = null;
            private EntryLog? entry2 = null;

            public UserEntrylogConnection(uint userID)
            {

                (bool userSuccess, User? user) = Wrapper.Users.GetUserByIDAsync(userID).Result;
                if (!userSuccess)
                {
                    namesurname = "Hata";
                    return;
                }
                else if (user == null)
                {
                    namesurname = "Bilinmiyor";
                    subjectname = "Bilinmiyor";
                    return;
                }
                else namesurname = user.NameSurname;

                (bool subjectSuccess, Subject? subject) = Wrapper.Subjects.GetSubjectByID(user.SubjectID).Result;
                if (!subjectSuccess)
                {
                    subjectname = "Hata";
                    return;
                }
                else if (subject == null) subjectname = "Bilinmiyor";
                else subjectname = subject.Name;
            }

            public void AddEntry(EntryLog entry)
            {
                if (entry1 == null) entry1 = entry;
                else entry2 = entry;
            }
            
            private string GetCSVLineFromData(DateTime? entry_in, DateTime? entry_out)
            {
                string str_in = entry_in?.ToString("HH:mm") ?? "-";
                string str_out = entry_out?.ToString("HH:mm") ?? "-";

                return $"{namesurname}, {subjectname}, {str_in}, {str_out}\n";
            }

            public string GetCSVLine()
            {
                DateTime? entrydt_in = null;
                DateTime? entrydt_out = null;


                if (entry1 != null && entry2 != null)
                {
                    EntryLog entry_in = entry1.DT < entry2.DT ? entry1 : entry2;
                    EntryLog entry_out = entry1.DT > entry2.DT ? entry1 : entry2;

                    entrydt_in = entry_in.DT;
                    entrydt_out = entry_out.DT;
                }
                else
                {
                    EntryLog? entry = entry1 ?? entry2;

                    if(entry != null)
                    {
                        EntryLog? entry_in = entry.DT.TimeOfDay <= new TimeSpan(12, 30, 0) ? entry : null;
                        EntryLog? entry_out = entry.DT.TimeOfDay > new TimeSpan(12, 30, 0) ? entry : null;

                        entrydt_in = entry_in?.DT;
                        entrydt_out = entry_out?.DT;
                    }


                }


                return GetCSVLineFromData(entrydt_in, entrydt_out);
            }
        }

        public static async Task<(bool, string?)> BuildCSV(uint schoolID, DateTime? __date = null)
        {
            string CSV = "AD SOYAD, BRANŞ, GİRİŞ, ÇIKIŞ\r\n";

            (bool entrylogssuc, List<EntryLog>? entrylogs) = await Wrapper.Entrylogs.GetEntryLogs(schoolID, __date);
            (bool userssuc, List<User>? users) = await Wrapper.Users.GetAllUsers((int)schoolID);
            if (!entrylogssuc || entrylogs == null || !userssuc || users == null)
            {
                return (false, null);
            }

            Dictionary<uint, UserEntrylogConnection> userConnections = new Dictionary<uint, UserEntrylogConnection>();

            foreach (User user in users)
            {
                userConnections.Add(user.ID, new UserEntrylogConnection(user.ID));
            }

            foreach(EntryLog entrylog in entrylogs)
            {
                if(userConnections.ContainsKey(entrylog.UserID))
                userConnections[entrylog.UserID].AddEntry(entrylog);
            }

            foreach (UserEntrylogConnection connection in userConnections.Values)
            {
                CSV += connection.GetCSVLine();
            }

            return (true, CSV);

        }

        public static SmtpClient? Client = null;

        public static async Task<bool> SendReportAsMail(Stream reportContent, DateTime date, string[] recipients)
        {
            try
            {
                if (Client == null)
                {
                    Logger.Log("Mail client not initialized", Logger.LogLevel.Error);
                    return false;
                }

                string dateStr = date.ToString("dd-MM-yyyy");

                MailMessage mail = new MailMessage(ConfigMain.config?.MAIL_Username, string.Join(',', recipients), $"{dateStr} Giriş Çıkış Raporu", $"{dateStr} tarihli giriş çıkış raporu ektedir.");


                mail.Attachments.Add(new Attachment(reportContent, dateStr + ".csv"));

                await Client.SendMailAsync(mail);
                return true;
            }
            catch (Exception ex)
            {
                Logger.Log("Mail Exception: " + ex.Message, Logger.LogLevel.Error);
                return false;
            }

        }


        public static async Task BuildExistingEntrylogs()
        {
            string[] dbpaths = Directory.GetFiles(Paths.Entrylogs_DPath);

            if (!Directory.Exists("csvfiles")) Directory.CreateDirectory("csvfiles");

            Logger.Log(DateTime.Now.ToShortDateString());

            foreach (string path in dbpaths)
            {
                DateTime date = DateTime.Parse(Path.GetFileNameWithoutExtension(path).Replace('-', '.'));

                (bool suc, string? csv) = await BuildCSV(1, date);

                if (!suc || csv == null)
                {
                    Logger.Log($"Couldnt build CSV for {path}", Logger.LogLevel.Error);
                    continue;
                }

                string csvfilename = "csvfiles\\" + date.ToString("dd-MM-yyyy") + ".csv";
                if (!File.Exists(csvfilename)) File.Create(csvfilename).Close();

                File.WriteAllText(csvfilename, csv);

                Logger.Log("CSV Success " + date.ToString("dd-MM-yyyy"));
            }
        }
    }
}
