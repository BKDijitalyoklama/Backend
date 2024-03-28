using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Mail;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Utils;
using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;

namespace ServerApp_Main.Modules.ReportBuilderModule
{
    internal static class ReportBuilderMain
    {

        public static async Task<bool> InitAsync()
        {
            Client = new SmtpClient(ConfigMain.config?.MAIL_SMTPServer)
            {
                Port = 587,
                Credentials = new NetworkCredential(ConfigMain.config?.MAIL_Username, ConfigMain.config?.MAIL_Password),
                EnableSsl = true,
            };

            

            return true;
        }


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

            public string GetCSVLine()
            {
                string line = $"{namesurname}, {subjectname}, ";

                if (entry1 != null && entry2 != null)
                {
                    EntryLog entry_in = entry1.DT < entry2.DT ? entry1 : entry2;
                    EntryLog entry_out = entry1.DT > entry2.DT ? entry1 : entry2;

                    line += $"{entry_in.DT.ToString("HH:mm")}, {entry_out.DT.ToString("HH:mm")}";
                }
                else
                {
                    EntryLog? entry = entry1 ?? entry2;

                    if (entry == null)
                    {
                        line += "-,-";
                    }
                    else
                    {
                        EntryLog? entry_in = entry.DT.TimeOfDay <= new TimeSpan(12, 30, 0) ? entry : null;
                        EntryLog? entry_out = entry.DT.TimeOfDay > new TimeSpan(12, 30, 0) ? entry : null;

                        line += $"{entry_in?.DT.ToString("HH:mm")}, {entry_out?.DT.ToString("HH:mm")}";
                    }


                }

                line += "\r\n";

                return line;
            }
        }

        static async Task<(bool, string?)> BuildCSV(uint schoolID, DateTime? __date = null)
        {
            string CSV = "Ad Soyad, Branş, Giriş, Çıkış\r\n";

            (bool entrylogssuc, List<EntryLog>? entrylogs) = await Wrapper.Entrylogs.GetEntryLogs(schoolID, __date);
            if (!entrylogssuc || entrylogs == null)
            {
                return (false, null);
            }

            Dictionary<uint, UserEntrylogConnection> userConnections = new Dictionary<uint, UserEntrylogConnection>();

            foreach (EntryLog entry in entrylogs)
            {
                if (!userConnections.ContainsKey(entry.UserID)) userConnections[entry.UserID] = new UserEntrylogConnection(entry.UserID);

                userConnections[entry.UserID].AddEntry(entry);
            }

            foreach (UserEntrylogConnection connection in userConnections.Values)
            {
                CSV += connection.GetCSVLine();
            }

            return (true, CSV);

        }

        private static SmtpClient? Client = null;

        static async Task<bool> SendReportAsMail(Stream reportContent, DateTime date, string[] recipients)
        {
            try
            {
                if (Client == null)
                {
                    Logger.Log("Mail client not initialized", Logger.LogLevel.Error);
                    return false;
                }

                MailMessage mail = new MailMessage(ConfigMain.config?.MAIL_Username, string.Join(',', recipients));

                string dateStr = date.ToString("dd-MM-yyyy");

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


        static async Task BuildExistingEntrylogs()
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
