using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Models
{
    [Table("schools")]
    internal class School
    {
        [PrimaryKey]
        public int SchoolID { get; set; }

        public byte[] DeviceMACAddress { get; set; }

        public string SchoolName { get; set; }
    }
}
