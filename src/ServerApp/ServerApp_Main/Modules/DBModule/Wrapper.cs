using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule.Models;
using ServerApp_Main.Utils;
using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule
{

    internal static class Wrapper
    {
        public static class UserOperations
        {
            public static async Task<User?> GetUserByIDAsync(uint userID)
            {
                try
                {
                    if (DBMain.MainDBConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return null; }
                    User user = await DBMain.MainDBConnection.GetAsync<User>(userID);
                    return user;
                }
                catch (SQLite.SQLiteException e)
                {
                    if (e.Result != SQLite3.Result.NotFound) Logger.Log("DBErr: " + e.Message, Logger.LogLevel.Error);
                    return null;
                }
                catch(Exception ex)
                {
                    Logger.Log("Err: " + ex.Message, Logger.LogLevel.Error);
                    return null;
                }
            }
        }

        public static class EntryLogOperations
        {

            public static async Task EstablishDailyDBConnection()
            {
                try
                {
                    DateTime today = DateTime.Now;

                    string todaysFile = Path.Combine(Paths.Entrylogs_DPath, today.ToString("dd-MM-yyyy") + ".db");

                    if (!File.Exists(todaysFile))
                    {
                        SQLiteAsyncConnection conn = new SQLiteAsyncConnection(todaysFile, SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
                        await conn.CreateTableAsync<EntryLog>();
                        if (DBMain.DailyEntryLogConnection != null) await DBMain.DailyEntryLogConnection.CloseAsync();

                        DBMain.DailyEntryLogConnection = conn;
                    }
                }
                catch (SQLite.SQLiteException e)
                {
                    Logger.Log("DBErr: " + e.Message, Logger.LogLevel.Error);
                    return;
                }
                catch (Exception ex)
                {
                    Logger.Log("Err: " + ex.Message, Logger.LogLevel.Error);
                    return;
                }
            }

            public static async Task<bool> NewEntryLog(EntryLog elog)
            {
                try
                {
                    await EstablishDailyDBConnection();
                    if (DBMain.DailyEntryLogConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return false; }

                    return await DBMain.DailyEntryLogConnection.InsertAsync(elog) == 1;
                }
                catch (SQLite.SQLiteException e)
                {
                    Logger.Log("DBErr: " + e.Message, Logger.LogLevel.Error);
                    return false;
                }
                catch (Exception ex)
                {
                    Logger.Log("Err: " + ex.Message, Logger.LogLevel.Error);
                    return false;
                }
            }
        }

        
    }
}
