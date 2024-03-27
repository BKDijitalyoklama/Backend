using ServerApp_Main.Modules.DBModule.Models;
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

        public static class Subjects
        {
            public static async Task<(bool, Subject?)> GetSubjectByID(uint ID)
            {
                try
                {
                    if (DBMain.MainDBConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return (false, null); }
                    Subject? subject = await DBMain.MainDBConnection.Table<Subject>().FirstOrDefaultAsync(x => x.ID == ID);

                    return (true, subject);
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
        }
    }

}
