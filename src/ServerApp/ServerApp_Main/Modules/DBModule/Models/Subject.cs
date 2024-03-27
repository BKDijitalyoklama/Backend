using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Models
{

    [Table("subjects")]
    internal class Subject
    {
        [PrimaryKey]
        public byte ID { get; set; }

        public string Name { get; set; }
    }
}
