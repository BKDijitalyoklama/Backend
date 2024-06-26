﻿using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule;
using ServerApp_Main.Modules.ReportBuilderModule;
using ServerApp_Main.Modules.WebServerModule;
using ServerApp_Main.Utils;
using System.Reflection;

namespace ServerApp_Main
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            Version? version = Assembly.GetExecutingAssembly().GetName().Version;
            if (version == null) Logger.Log("APP Started (V Unknown)");
            else Logger.Log($"APP Started (V{version.Major}.{version.Minor}.{version.Build})");

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
                if (
                    !await InitSingle(ConfigMain.InitAsync, "Initializing Configuration", "Failed to initialize Configuration") ||
                    !await InitSingle(DBMain.InitAsync, "Initializing DB", "Failed to initialize DB") ||
                    !await InitSingle(WebServerMain.InitAsync, "Initializing WebServer", "Failed to initialize WebServer") ||
                    !await InitSingle(ReportBuilderMain.InitAsync, "Initializing ReportBuilder", "Failed to initialize ReportBuilder")
                    ) return false;
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