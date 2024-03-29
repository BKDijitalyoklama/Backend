using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Utils
{
    internal static class GenericExtensions
    {
        public static bool IsNullOrEmpty(this string str)
        {
            return string.IsNullOrEmpty(str);
        }

        public static byte[]? FromHEX(this string hex)
        {
            try
            {
                if (hex.Length % 2 == 1)
                    throw new Exception("The binary key cannot have an odd number of digits");

                byte[] arr = new byte[hex.Length >> 1];

                for (int i = 0; i < hex.Length >> 1; ++i)
                {
                    arr[i] = (byte)((GetHexVal(hex[i << 1]) << 4) + (GetHexVal(hex[(i << 1) + 1])));
                }

                return arr;
            }
            catch(Exception)
            {
                return null;
            }


            int GetHexVal(char hex)
            {
                int val = (int)hex;
                return val - (val < 58 ? 48 : (val < 97 ? 55 : 87));
            }
        }
    }
}
