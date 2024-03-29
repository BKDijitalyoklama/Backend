using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Mail;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using ServerApp_Main.Utils;
using ServerApp_Main.Modules.Configuration;
using ServerApp_Main.Modules.DBModule;
using ServerApp_Main.Modules.DBModule.Wrappers;
using ServerApp_Main.Modules.DBModule.Models.MainDB;
using ServerApp_Main.Modules.DBModule.Models.EntrylogsDB;
using Quartz.Impl;
using Quartz;

namespace ServerApp_Main.Modules.ReportBuilderModule
{
    internal static class ReportBuilderMain
    {

        private static ISchedulerFactory schedulerFactory = new StdSchedulerFactory();

        private static async Task ScheduleReports()
        {
            int[][]? times = ConfigMain.config?.DailyReportTimes;
            if (times == null) { Logger.Log($"Report times not found", Logger.LogLevel.Warning); return; }


            IScheduler scheduler = await schedulerFactory.GetScheduler();

            await scheduler.Start();

            IJobDetail job = JobBuilder.Create<ReportSenderTask>()
                .WithIdentity("reportsender", "scheduledtasks")
                .Build();

            List<ITrigger> triggers = new List<ITrigger>();

            for(int i = 0; i<times.Length; i++)
            {

                TriggerBuilder builder = TriggerBuilder.Create()
                    .WithIdentity("trigger" + i, "reporttriggers")
                    .WithSchedule(CronScheduleBuilder.DailyAtHourAndMinute(times[i][0], times[i][1]));
                


                triggers.Add(builder.Build());
            }

            await scheduler.ScheduleJob(job, triggers, true);

        }

        public static async Task<bool> InitAsync()
        {
            ReportBuilder.Client = new SmtpClient(ConfigMain.config?.MAIL_SMTPServer)
            {
                Port = 587,
                Credentials = new NetworkCredential(ConfigMain.config?.MAIL_Username, ConfigMain.config?.MAIL_Password),
                EnableSsl = true
            };

            await ScheduleReports();

            return true;
        }

    }
}
