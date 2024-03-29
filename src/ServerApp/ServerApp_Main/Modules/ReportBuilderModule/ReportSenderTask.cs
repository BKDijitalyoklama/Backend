using Quartz;
using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerApp_Main.Modules.ReportBuilderModule
{
    internal class ReportSenderTask : IJob
    {
        public async Task Execute(IJobExecutionContext context)
        {
            (bool schoolsuc, List<School>? schools) = await Wrapper.Schools.GetAllSchools();

            if (!schoolsuc || schools == null)
            {
                Logger.Log("Failed to get schools", Logger.LogLevel.Error);
                return;
            }

            foreach (School school in schools)
            {
                Logger.Log("Sending report of the school " + school.SchoolName);

                SchoolFeedbackResult result = new SchoolFeedbackResult();
                result.date = DateTime.Now;
                result.SchoolID = school.SchoolID;

                string? CSV = null;

                for (int i = 0; i < 3; i++)
                {
                    (bool csvsuc, string? _csv) = await ReportBuilder.BuildCSV(school.SchoolID, result.date);

                    if (!csvsuc || _csv == null) continue;
                    else
                    {
                        CSV = _csv;
                        break;
                    }
                }

                if (CSV == null)
                {
                    result.SchoolID = school.SchoolID;
                    result.retries = 0;
                    result.success = false;
                }
                else
                {
                    bool mailsuccess = false;
                    for (int i = 0; i < 5; i++)
                    {
                        result.retries = (uint)i;
                        MemoryStream memoStream = new MemoryStream(Encoding.UTF8.GetBytes(CSV));

                        memoStream.Seek(0, SeekOrigin.Begin);

                        mailsuccess = await ReportBuilder.SendReportAsMail(memoStream, result.date, school.ReportRecipients.Split(',') ?? new string[] { });
                        if (mailsuccess) break;
                        else
                        {
                            Logger.Log($"Failed to send mail to {school.SchoolName}, Retrying...", Logger.LogLevel.Warning);
                            continue;
                        }
                    }

                    if (!mailsuccess) Logger.Log($"Failed to send mail to {school.SchoolName}", Logger.LogLevel.Warning);
                    else Logger.Log($"Successfully sent mail to {school.SchoolName}");

                    result.success = mailsuccess;
                    
                }

                if(!await Wrapper.SchoolFeedbackResults.InsertFeedbackResult(result)) Logger.Log("Failed to insert feedback result", Logger.LogLevel.Warning);
            }


        }
    }
}
