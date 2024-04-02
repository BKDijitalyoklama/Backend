using ServerApp_Main.Modules.DBModule;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Modules.WebServerModule.Utils;
using ServerApp_Main.Utils;
using SimpleWebServer.Attributes;
using SimpleWebServer.Extensions;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.WebServerModule.Controllers
{
    internal class MiscController
    {

        [WebPath("/ping")]
        public async Task ping(HttpListenerContext ctx)
        {
            int secs = (int)(DateTime.Now - Process.GetCurrentProcess().StartTime).TotalSeconds;

            if(!await ctx.CreateStringResponseAsync($"pong {secs}")) Logger.Log("Failed to send response", Logger.LogLevel.Error);
        }

        [WebPath("/deviceping")]
        public async Task deviceping(HttpListenerContext ctx)
        {
            if (!WebServerUtils.GetMACAddrFromStringQuery(ctx, out byte[] macBytes)) return;

            (bool schoolsuc, School? school) = await Wrapper.Schools.GetSchoolByMacID(macBytes);

            if(!schoolsuc)
            {
                if (!await ctx.CreateStringResponseAsync($"error", 500)) Logger.Log("Failed to send response", Logger.LogLevel.Error);
                return;
            }

            if(school == null)
            {
                if (!await ctx.CreateStringResponseAsync($"macnotfound", 404)) Logger.Log("Failed to send response", Logger.LogLevel.Error);
                return;
            }
            
            if (!await ctx.CreateStringResponseAsync($"ok")) Logger.Log("Failed to send response", Logger.LogLevel.Error);

            (bool updsuc, bool updres) = await Wrapper.Schools.UpdateSchoolLastSeenBySchoolID(school.SchoolID, DateTime.Now);

            if (!updsuc || !updres) Logger.Log($"Failed to update device lastseen", Logger.LogLevel.Error);
            else Logger.Log("Updated device lastseen");
        }
    }
}
