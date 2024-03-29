using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Modules.ReportBuilderModule;
using ServerApp_Main.Utils;
using SQLite;

namespace ServerApp_Main.Modules.DBModule
{
    internal static class DBMain
    {
        public static async Task<bool> InitAsync()
        {
            MainDBConnection = new SQLiteAsyncConnection(Paths.MainDB_FPath, SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
            await MainDBConnection.CreateTableAsync<School>();
            await MainDBConnection.CreateTableAsync<User>();
            await MainDBConnection.CreateTableAsync<Subject>();

            await Wrapper.Entrylogs.EstablishDailyDBConnectionAsync();

            return true;
        }

        public static SQLiteAsyncConnection? MainDBConnection;

        public static SQLiteAsyncConnection? DailyEntryLogConnection;

        static async Task Subjects()
        {
            byte[] content = File.ReadAllBytes("subjects.bin");
            
            for(int i = 0; i< content.Length; i+= 11)
            {
                byte[] bytes = new byte[11];

                Array.Copy(content, i, bytes, 0, 11);

                byte subjectID = bytes[0];

                byte[] namebytes = new byte[10];

                Array.Copy(content, i + 1, namebytes, 0, 10);

                string name = Encoding.UTF8.GetString(namebytes.Where(x => x != 0).ToArray());

                Subject sub = new Subject()
                {
                    ID = subjectID,
                    Name = name
                };

                await MainDBConnection.InsertAsync(sub);

                Console.WriteLine($"{(i + 1) / 11}/{content.Length / 11}: {name}");
            }
        }

        static async Task Users()
        {
            string[] paths = Directory.GetFiles("users");

            for(int i = 0; i<paths.Length; i++)
            {
                string path = paths[i];

                byte[] userdata = File.ReadAllBytes(path);

                byte subjectID = userdata[0];

                byte[] idbytes = new byte[3];

                Array.Copy(userdata, 1, idbytes, 0, 3);
                uint userID = (uint)((int)(idbytes[0] << 16) | (idbytes[1] << 8) | (idbytes[2]));

                byte[] namesurnamebytes = new byte[20];

                Array.Copy(userdata, 4, namesurnamebytes, 0, 20);

                string namesurname = Encoding.UTF8.GetString(namesurnamebytes.Where(x => x != 0).ToArray());

                User usr = new User()
                {
                    ID = userID,
                    NameSurname = namesurname,
                    SchoolID = 1,
                    SubjectID = subjectID,
                    UserCreationDT = DateTime.Now
                };

                

                await MainDBConnection.InsertAsync(usr);

                Console.WriteLine($"{i + 1}/{paths.Length}: {namesurname}");
            }
        }

        static async Task Entrylogs()
        {
            string[] paths = Directory.GetFiles("entrylogs");

            for (int i = 0; i < paths.Length; i++)
            {
                string path = paths[i];

                byte[] entrylogs = File.ReadAllBytes(path);

                DateTime entryfiledate = DateTime.Parse(Path.GetFileNameWithoutExtension(path));

                SQLiteAsyncConnection conn = new SQLiteAsyncConnection(Path.Combine(Paths.Entrylogs_DPath, entryfiledate.ToString("dd-MM-yyyy") + ".db"), SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
                await conn.CreateTableAsync<EntryLog>();
                await conn.CreateTableAsync<SchoolFeedbackResult>();

                for (int j = 0; j < entrylogs.Length; j+= 11)
                {
                    byte[] log = new byte[11];

                    Array.Copy(entrylogs, j, log, 0, 11);

                    byte[] idbytes = new byte[3];
                    Array.Copy(log, 0, idbytes, 0, 3);
                    uint userID = (uint)((int)(idbytes[0] << 16) | (idbytes[1] << 8) | (idbytes[2]));

                    byte[] dtbytes = new byte[8];
                    Array.Copy(log, 3, dtbytes, 0, 8);
                    ulong unixtime = BitConverter.ToUInt64(dtbytes);
                    DateTime dt = DateTime.UnixEpoch.AddSeconds(unixtime).AddHours(3);

                    EntryLog elog = new EntryLog()
                    {
                        UserID = userID,
                        SchoolID = 1,
                        DT = dt
                    };

                    await conn.InsertAsync(elog);


                    Console.WriteLine($"{Path.GetFileNameWithoutExtension(path)} // {(j / 11) + 1}/{(entrylogs.Length / 11)}: {dt}");
                }

            }

        }
        

    }
}
