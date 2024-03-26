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

            if (!await InitModules())
            {
                Logger.Log("Initializing failed", Logger.LogLevel.Error);
                Console.ReadLine();
                return;
            }

            Logger.Log("Initialization complete", Logger.LogLevel.Init);

            await Task.Delay(-1);
        }



        static async Task<bool> InitModules()
        {
            try
            {
                bool init_webserver = await InitSingle(WebServerMain.InitAsync, "Initializing WebServer", "Failed to initialize WebServer");
                if (!init_webserver) return false;

                bool init_db = await InitSingle(DBMain.InitAsync, "Initializing DB", "Failed to initialize DB");
                if (!init_db) return false;
            }
            catch(Exception ex)
            {
                Logger.Log("Exception during initialization: " + ex.Message, Logger.LogLevel.Error);
                return false;
            }

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