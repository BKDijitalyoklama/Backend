using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Wrappers
{


    internal static partial class Wrapper
    {

        public static class SchoolFeedbackResults
        {
            public static async Task<(bool, List<SchoolFeedbackResult>?)> GetFeedbackResultsBySchoolID(uint schoolID)
            {
                try
                {
                    await Entrylogs.EstablishDailyDBConnectionAsync();
                    if (DBMain.DailyEntryLogConnection == null) { Logger.Log("DB DailyLogs Not initialized", Logger.LogLevel.Error); return (false, null); }

                    List<SchoolFeedbackResult>? feedbackresults = await DBMain.DailyEntryLogConnection.Table<SchoolFeedbackResult>().Where(x => x.SchoolID == schoolID).ToListAsync();

                    return (true, feedbackresults);
                }
                catch (SQLite.SQLiteException e)
                {
                    Logger.Log("DBErr: " + e.Message, Logger.LogLevel.Error);
                    return (false, null);
                }
                catch (Exception ex)
                {
                    Logger.Log("Err: " + ex.Message, Logger.LogLevel.Error);
                    return (false, null);
                }
            }

            public static async Task<bool> InsertFeedbackResult(SchoolFeedbackResult feedbackresult)
            {
                try
                {
                    await Entrylogs.EstablishDailyDBConnectionAsync();
                    if (DBMain.DailyEntryLogConnection == null) { Logger.Log("DB DailyLogs Not initialized", Logger.LogLevel.Error); return false; }

                    return await DBMain.DailyEntryLogConnection.InsertAsync(feedbackresult) == 1;
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
