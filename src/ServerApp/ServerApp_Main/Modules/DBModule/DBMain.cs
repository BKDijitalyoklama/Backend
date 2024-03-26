using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule.Models;
using ServerApp_Main.Utils;
using SQLite;

namespace ServerApp_Main.Modules.DBModule
{
    internal static class DBMain
    {
        public static SQLiteAsyncConnection? MainDBConnection;

        public static SQLiteAsyncConnection? DailyEntryLogConnection;

        public static async Task<bool> InitAsync()
        {
            MainDBConnection = new SQLiteAsyncConnection(Paths.MainDB_FPath, SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
            await MainDBConnection.CreateTableAsync<School>();
            await MainDBConnection.CreateTableAsync<User>();

            await Wrapper.EntryLogOperations.EstablishDailyDBConnection();

            return true;
        }

    }
}
