using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Models.MainDB
{
    [Table("schools")]
    internal class School
    {
        [PrimaryKey]
        [AutoIncrement]
        public uint SchoolID { get; set; }

        [Indexed]
        public byte[] DeviceMACAddress { get; set; } = { };

        public string SchoolName { get; set; } = "";
    }
}
