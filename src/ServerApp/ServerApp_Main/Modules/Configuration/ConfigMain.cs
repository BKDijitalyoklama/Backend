using Newtonsoft.Json.Linq;
using ServerApp_Main.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.Configuration
{

    public class Config
    {
        public string? WebServerUrl { get; set; }


        public string? MAIL_SMTPServer { get; set; }
        public string? MAIL_SMTPPort { get; set; }
        public string? MAIL_Username { get; set; }
        public string? MAIL_Password { get; set; }
    }

    internal static class ConfigMain
    {
        public static Config? config;

        public static async Task<bool> InitAsync()
        {
            try
            {
                config = Newtonsoft.Json.JsonConvert.DeserializeObject<Config>(await System.IO.File.ReadAllTextAsync(Paths.Config_FPath));
            }
            catch(Exception ex)
            {
                Logger.Log("Error while reading config.json: " + ex.Message, Logger.LogLevel.Error);
                return false;
            }

            return config != null;
        }

    }
}
