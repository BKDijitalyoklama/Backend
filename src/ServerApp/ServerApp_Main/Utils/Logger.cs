using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Utils
{
    internal static class Logger
    {
        public enum LogLevel
        {
            Init = 0,
            Debug = 1,
            Info = 2,
            Warning = 4,
            Error = 8
        }

        private static byte disabledLevels = 0;


        private static Dictionary<LogLevel, (string, ConsoleColor)> logLevelInfo = new()
        {
            {LogLevel.Init, ("[INIT]", ConsoleColor.Magenta)},
            {LogLevel.Debug, ("[D]   ", ConsoleColor.DarkGray)},
            {LogLevel.Info, ("[I]   ", ConsoleColor.Cyan)},
            {LogLevel.Warning, ("[W]   ", ConsoleColor.DarkYellow)},
            {LogLevel.Error, ("[E]   ", ConsoleColor.DarkRed)}
        };

        private static void WriteWithColor(string text, ConsoleColor color)
        {
            ConsoleColor oldcolor = Console.ForegroundColor;

            Console.ForegroundColor = color;
            Console.Write(text);
            Console.ForegroundColor = oldcolor;
        }

        public static void Log(string text, LogLevel level = LogLevel.Info)
        {
            if (((byte)level & disabledLevels) != 0) return;

            string prefix;
            ConsoleColor color;

            (prefix, color) = logLevelInfo[level];

            ConsoleColor oldcolor = Console.ForegroundColor;

            WriteWithColor($"[{DateTime.Now}]{prefix} ", color);
            Console.WriteLine(text);
        }

    }
}
