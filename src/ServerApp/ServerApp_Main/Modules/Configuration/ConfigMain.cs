using Newtonsoft.Json.Linq;
using ServerApp_Main.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.Configuration
{

    public class Configuration
    {

    }

    internal static class ConfigMain
    {
        public static Configuration? config;

        public static async Task<bool> InitAsync()
        {
            try
            {
                config = Newtonsoft.Json.JsonConvert.DeserializeObject<Configuration>(await System.IO.File.ReadAllTextAsync("config.json"));
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
