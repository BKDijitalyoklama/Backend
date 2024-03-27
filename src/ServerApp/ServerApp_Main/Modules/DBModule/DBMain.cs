using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule.Models;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Utils;
using SQLite;

namespace ServerApp_Main.Modules.DBModule
{
    internal static class DBMain
    {
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

        public static async Task<bool> InitAsync()
        {
            MainDBConnection = new SQLiteAsyncConnection(Paths.MainDB_FPath, SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
            await MainDBConnection.CreateTableAsync<School>();
            await MainDBConnection.CreateTableAsync<User>();
            await MainDBConnection.CreateTableAsync<Subject>();

            MainDBConnection.UpdateAsync(new School() { SchoolID = 1, DeviceMACAddress = "08D1F9E86E4C".FromHEX(), SchoolName = "Nakkaştepe Bahçeşehir Koleji 50. Yıl Kampüsü" });
            
            await Wrapper.Entrylogs.EstablishDailyDBConnectionAsync();

            return true;
        }

    }
}
