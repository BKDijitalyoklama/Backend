using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Modules.WebServerModule.Controllers;
using SimpleWebServer;
using SimpleWebServer.Attributes;
using SimpleWebServer.Extensions;

namespace ServerApp_Main.Modules.WebServerModule
{
    internal static class WebServerMain
    {

        private static WebServer? server;

        public static async Task<bool> InitAsync()
        {
            server = new WebServer(Configuration.ConfigMain.config?.WebServerUrl);
            server.AddController<LogsController>();
            server.AddController<MiscController>();
            server.Start();
            return true;
        }
    }
}
