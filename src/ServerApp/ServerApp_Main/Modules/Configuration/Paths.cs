using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.Configuration
{
    internal static class Paths
    {
        public static string Files_DPath => Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "files");

        public static string MainDB_FPath => Path.Combine(Files_DPath, "main.db");

        public static string Entrylogs_DPath => Path.Combine(Files_DPath, "entrylogs");


        public static string Config_FPath => Path.Combine(Files_DPath, "config.json");
    }
}
