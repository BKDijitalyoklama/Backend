using SQLite;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.DBModule.Models.EntrylogsDB
{
    [Table("schoolfeedbackresults")]
    internal class SchoolFeedbackResult
    {
        [Indexed]
        public uint SchoolID { get; set; }

        public DateTime date { get; set; }

        public uint retries { get; set; }

        public bool success { get; set; }

    }
}
