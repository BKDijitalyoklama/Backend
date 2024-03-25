using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Modules.DBModule.Models;
using ServerApp_Main.Utils;
using SQLite;

namespace ServerApp_Main.Modules.DBModule
{
    internal static class DBMain
    {
        private static SQLiteAsyncConnection DBConnection;

        public static async Task<bool> InitAsync()
        {
            DBConnection = new SQLiteAsyncConnection("test.db");
            await DBConnection.CreateTableAsync<EntryLog>();
            await DBConnection.CreateTableAsync<School>();
            await DBConnection.CreateTableAsync<User>();
            return true;
        }
    }
}
