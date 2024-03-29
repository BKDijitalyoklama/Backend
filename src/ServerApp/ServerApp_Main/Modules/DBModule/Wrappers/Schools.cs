using ServerApp_Main.Modules.DBModule.Models.MainDB;
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

        public static class Schools
        {
            public static async Task<(bool, School?)> GetSchoolByMacID(byte[] macID)
            {
                try
                {
                    if (DBMain.MainDBConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return (false, null); }
                    School? school = await DBMain.MainDBConnection.Table<School>().FirstOrDefaultAsync(x => x.DeviceMACAddress == macID);

                    return (true, school);
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

            public static async Task<(bool, School?)> GetSchoolByID(uint ID)
            {
                try
                {
                    if (DBMain.MainDBConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return (false, null); }
                    School? school = await DBMain.MainDBConnection.Table<School>().FirstOrDefaultAsync(x => x.SchoolID == ID);

                    return (true, school);
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

            public static async Task<(bool,List<School>?)> GetAllSchools()
            {
                try
                {
                    if (DBMain.MainDBConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return (false, null); }
                    return (true, await DBMain.MainDBConnection.Table<School>().ToListAsync());

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
