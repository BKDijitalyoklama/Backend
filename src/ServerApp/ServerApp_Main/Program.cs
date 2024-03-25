using ServerApp_Main.Modules.DBModule;
using ServerApp_Main.Modules.WebServerModule;
using ServerApp_Main.Utils;
using System.Reflection;

namespace ServerApp_Main
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            Logger.Log("APP Started, Initializing");

            while(!await InitModules())
            {
                Logger.Log("Initializing failed, restarting", Logger.LogLevel.Error);
            }

            Logger.Log("Initialization complete", Logger.LogLevel.Init);

            await Task.Delay(-1);
        }



        static async Task<bool> InitModules()
        {
            bool init_webserver = await InitSingle(WebServerMain.InitAsync, "Initializing WebServer", "Failed to initialize WebServer");
            if (!init_webserver) return false;

            bool init_db = await InitSingle(DBMain.InitAsync, "Initializing WebServer", "Failed to initialize WebServer");
            if (!init_webserver) return false;

            return true;

        }

        private static async Task<bool> InitSingle(Delegate initfunc, string initializingText, string failedInitializationText)
        {
            Task<bool>? func = (Task<bool>?)initfunc.DynamicInvoke();

            if (func == null) return false;

            Logger.Log(initializingText, Logger.LogLevel.Init);

            bool result = await func;

            if (!result)
            {
                Logger.Log(failedInitializationText, Logger.LogLevel.Error);
            }

            return result;
        }
    }
}