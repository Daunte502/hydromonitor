//#############################################################################################
//#                                      NTP HEADER FILE                                      #
//#############################################################################################

#ifndef NTP_H
#define NTP_H

#include <WiFi.h>
#include "time.h"
#include "sntp.h"

class Ntp {
  private:
    const char* ntpServer1 = "pool.ntp.org";
    const char* ntpServer2 = "time.nist.gov";
    const char* time_zone = "EST+5EDT,M3.2.0/2,M11.1.0/2";

  public:
    Ntp(const char* process) { Serial.println(process); }

    static void timeavailable(struct timeval *t) {
        Serial.println("NTP: Time Sync Success!");
    }

    void setup() {
        sntp_set_time_sync_notification_cb(timeavailable);
        sntp_servermode_dhcp(1);
        configTzTime(time_zone, ntpServer1, ntpServer2);
    }
};

void vNTP(void * pvParameters) {

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    Ntp NTP = Ntp("NTP PROTOCOL INITIATED");
    NTP.setup();

    for (;;) {
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}

void vNTPFunction(void) {
    xTaskCreatePinnedToCore(vNTP, "NTP Task", 2048, (void*)1, 12, &xNTPHandle, 1);
}

#endif
