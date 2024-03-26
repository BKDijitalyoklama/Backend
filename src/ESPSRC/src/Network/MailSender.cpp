#include "ESP_Mail_Client.h"
#include "DataSaving/Config.h"
#include "Debugger.h"
#include "Report/ReportHandler.h"

namespace MailSender
{
    String SMTPHost = "smtp.gmail.com";
    int SMTPPort = 465;
    String AuthorMail = "nbk.girislog@gmail.com";
    String AuthorPassword = "ccfs waot vykv zwwp";

    bool Initialize()
    {
        MailClient.networkReconnect(false);

        return true;
    }

    SMTPSession SMTP;

    bool SendMail(String content)
    {

        SMTP.callback([](SMTP_Status status)
        {
            Serial.println(status.info());

            for (size_t i = 0; i < SMTP.sendingResult.size(); i++)
            {
            /* Get the result item */
            SMTP_Result result = SMTP.sendingResult.getItem(i);

            // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
            // your device time was synched with NTP server.
            // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
            // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
      
            ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
            ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
            ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
            ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
            ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
            }

            SMTP.sendingResult.clear(); 
        });

        SMTP.debug(1);

        Session_Config config;
        config.server.host_name = SMTPHost;
        config.server.port = SMTPPort;
        config.login.email = AuthorMail;
        config.login.password = AuthorPassword;
        config.login.user_domain = "";
        config.time.ntp_server = F("pool.ntp.org, time.nist.gov");
        config.time.gmt_offset = 3;
        config.time.day_light_offset = 0;

        SMTP_Message message;

        message.enable.chunking = true;

        message.sender.name = "TESTNAME";

        message.sender.email = AuthorMail;
        message.subject = "TEST SUBJECT";
        message.addRecipient("Test", "nbk.girislog@gmail.com");

        message.text.content = content;
        message.text.charSet = "UTF-8";
        message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
        message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
        message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

        SMTP_Attachment att;

        att.descr.filename = "data.csv";
        att.descr.mime = "text/plain";
        att.file.path = ReportHandler::GetTodaysFileName();
        att.file.storage_type = esp_mail_file_storage_type_sd;
        att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;

        message.addAttachment(att);

        if (!SMTP.connect(&config))
        {
            ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", SMTP.statusCode(), SMTP.errorCode(), SMTP.errorReason().c_str());
            return false;
        }

        if (!SMTP.isLoggedIn())
        {
            Serial.println("\nNot yet logged in.");
        }
        else
        {
            if (SMTP.isAuthenticated())
                Serial.println("\nSuccessfully logged in.");
            else
                Serial.println("\nConnected with no Auth.");
        }

        return MailClient.sendMail(&SMTP, &message) && SMTP.statusCode() >= 200 && SMTP.statusCode() < 300;
    }

}