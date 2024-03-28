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

        public static class Users
        {
            public static async Task<(bool, User?)> GetUserByIDAsync(uint userID)
            {
                try
                {
                    if (DBMain.MainDBConnection == null) { Logger.Log("DB Not initialized", Logger.LogLevel.Error); return (false, null); }
                    User? user = await DBMain.MainDBConnection.Table<User>().FirstOrDefaultAsync(x => x.ID == userID);
                    return (true, user);
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
