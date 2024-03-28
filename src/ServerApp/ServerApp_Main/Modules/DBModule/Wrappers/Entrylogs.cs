using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;
using ServerApp_Main.Utils;
using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Wrappers
{
    internal static partial class Wrapper
    {

        public static class Entrylogs
        {
            private static string GetEntryLogDBPath(DateTime? date = null)
            {
                return Path.Combine(Paths.Entrylogs_DPath, (date ?? DateTime.Now).ToString("dd-MM-yyyy") + ".db");
            }

            public static async Task EstablishDailyDBConnectionAsync()
            {
                try
                {
                    DateTime today = DateTime.Now;

                    string todaysFile = GetEntryLogDBPath();

                    if (!File.Exists(todaysFile))
                    {
                        SQLiteAsyncConnection conn = new SQLiteAsyncConnection(todaysFile, SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
                        await conn.CreateTableAsync<EntryLog>();
                        await conn.CreateTableAsync<SchoolFeedbackResult>();

                        if (DBMain.DailyEntryLogConnection != null) await DBMain.DailyEntryLogConnection.CloseAsync();

                        DBMain.DailyEntryLogConnection = conn;
                    }

                    if (DBMain.DailyEntryLogConnection == null)
                    {
                        DBMain.DailyEntryLogConnection = new SQLiteAsyncConnection(todaysFile);
                        await DBMain.DailyEntryLogConnection.CreateTableAsync<EntryLog>();
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



            public static async Task<(bool, List<EntryLog>?)> GetEntryLogs(uint schoolID, DateTime? __date = null)
            {
                if (DBMain.MainDBConnection == null) { Logger.Log("DB DailyLogs Not initialized", Logger.LogLevel.Error); return (false, null); }

                try
                {

                    DateTime date = __date ?? DateTime.Now;

                    SQLiteAsyncConnection? conn;
                    bool closeConn = false;

                    if (date >= DateTime.Today)
                    {
                        await EstablishDailyDBConnectionAsync();
                        if (DBMain.DailyEntryLogConnection == null) { Logger.Log("DB DailyLogs Not initialized", Logger.LogLevel.Error); return (false, null); }
                        conn = DBMain.DailyEntryLogConnection;
                    }
                    else
                    {
                        conn = new SQLiteAsyncConnection(GetEntryLogDBPath(date), SQLiteOpenFlags.Create | SQLiteOpenFlags.FullMutex | SQLiteOpenFlags.ReadWrite);
                        closeConn = true;
                        await conn.CreateTableAsync<EntryLog>();
                    }

                    List<EntryLog> entryLogs = await conn.Table<EntryLog>().Where(x => x.SchoolID == schoolID).ToListAsync();

                    if (closeConn) await conn.CloseAsync();

                    return (true, entryLogs);
                }
                catch(Exception ex)
                {
                    Logger.Log("Err: " + ex.Message, Logger.LogLevel.Error);
                    return (false, null);
                }

            }


            public static async Task<bool> NewEntryLogAsync(EntryLog elog)
            {
                try
                {
                    await EstablishDailyDBConnectionAsync();
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
        
            public static async Task<(bool, bool)> UserHasCooldown()
            {
                try
                {
                    await EstablishDailyDBConnectionAsync();
                    if (DBMain.DailyEntryLogConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return (false, false); }
                    DateTime limit = DateTime.Now - TimeSpan.FromMinutes(1);
                    return (true, (await DBMain.DailyEntryLogConnection.Table<EntryLog>().CountAsync(x => x.DT > limit)) > 0);
                }
                catch (SQLite.SQLiteException e)
                {
                    Logger.Log("DBErr: " + e.Message, Logger.LogLevel.Error);
                    return (false, false);
                }
                catch (Exception ex)
                {
                    Logger.Log("Err: " + ex.Message, Logger.LogLevel.Error);
                    return (false, false);
                }
            }
        }
    }
}
