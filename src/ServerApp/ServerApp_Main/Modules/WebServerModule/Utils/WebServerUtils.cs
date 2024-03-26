using ServerApp_Main.Utils;
using SimpleWebServer.Extensions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.WebServerModule.Utils
{
    internal static class WebServerUtils
    {
        public static bool GetUserIDFromStringQuery(HttpListenerContext ctx, out uint userID)
        {
            userID = 0;

            string? userIDStr = ctx.Request.QueryString["userid"];

            if (userIDStr == null || !uint.TryParse(userIDStr, out userID))
            {
                ctx.CreateStringResponse("Invalid UserID", 400);
                return false;
            }
            return true;
        }

        public static bool GetMACAddrFromStringQuery(HttpListenerContext ctx, out byte[] macAddress)
        {
            macAddress = new byte[6];

            string? deviceMacAddr = ctx.Request.QueryString["mac"];

            if (deviceMacAddr == null || deviceMacAddr.Length != 12)
            {
                ctx.CreateStringResponse("Invalid MAC", 400);
                return false;
            }

            byte[]? macBytes = deviceMacAddr.FromHEX();
            if (macBytes == null || macBytes.Length != 6)
            {
                ctx.CreateStringResponse("Invalid MAC", 400);
                return false;
            }

            macAddress = macBytes;
            return true;
        }
    }
}
