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
    }
}
