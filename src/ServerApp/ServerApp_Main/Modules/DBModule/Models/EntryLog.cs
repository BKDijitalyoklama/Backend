using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Models
{
    [Table("entrylogs")]
    internal class EntryLog
    {
        [PrimaryKey]
        public uint UserID { get; set; }

        public DateTime DT { get; set; }


    }
}
