using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Models.MainDB
{
    [Table("users")]
    internal class User
    {
        [PrimaryKey]
        public uint ID { get; set; }

        [Indexed]
        public byte SubjectID { get; set; }

        [Indexed]
        public uint SchoolID { get; set; }

        public string NameSurname { get; set; } = "";

        public DateTime UserCreationDT { get; set; }
    }
}
