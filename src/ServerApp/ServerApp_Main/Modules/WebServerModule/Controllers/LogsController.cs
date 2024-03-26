using ServerApp_Main.Modules.DBModule;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Modules.DBModule.Models;
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
using ServerApp_Main.Modules.WebServerModule.Utils;

namespace ServerApp_Main.Modules.WebServerModule.Controllers
{
    internal class LogsController
    {
        [WebPath("/logs/new")]
        public async Task ping(HttpListenerContext ctx)
        {
            if (!WebServerUtils.GetMACAddrFromStringQuery(ctx, out byte[] macBytes)) return;
            if (!WebServerUtils.GetUserIDFromStringQuery(ctx, out uint userID)) return;

            (bool macsuc,School? school) = await Wrapper.Schools.GetSchoolByMacID(macBytes);

            if(!macsuc)
            {
                await ctx.CreateStringResponseAsync("error", 500);
                return;
            }

            if(school == null)
            {
                await ctx.CreateStringResponseAsync("macnotfound", 404);
                return;
            }

            (bool usersuc, User? user) = await Wrapper.Users.GetUserByIDAsync(userID);

            if(!usersuc)
            {
                await ctx.CreateStringResponseAsync("error", 500);
                return;
            }

            if(user == null)
            {
                await ctx.CreateStringResponseAsync("usernotfound", 404);
                return;
            }

            EntryLog log = new EntryLog()
            {
                SchoolID = school.SchoolID,
                DT = DateTime.Now,
                UserID = userID
            };

            
            if(await Wrapper.Entrylogs.NewEntryLogAsync(log)) await ctx.CreateStringResponseAsync(user.NameSurname, 200);
            else await ctx.CreateStringResponseAsync("error", 500);
        }
    }
}
