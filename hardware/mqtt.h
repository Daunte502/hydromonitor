//#############################################################################################
//#                                     MQTT HEADER FILE                                      #
//#############################################################################################

#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>

static char deviceName[50] = "H";
WiFiClient espClient;
PubSubClient mqtt(espClient);

void vUpdateFunction(void);
void callback(char* topic, byte* payload, unsigned int length);

void MQTT_Connect( void * pvParameters ){
   /* The parameter value is expected to be 1 as 1 is passed in the pvParameters value in the call to xTaskCreate() below. */
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

  
  for ( ;; ){

    vTaskDelay(10000 / portTICK_PERIOD_MS);
    // Serial.printf("mqtt connected %d  %d\n",mqtt.connected(),mqtt.state());

    while (!mqtt.connected() ) {        

        char clientID[50]       = {0}; 
        char clientName[50]     = {0};         

        /* Intializes random number generator */
        time_t t;
        srand((unsigned) time(&t));

        // Client ID              
        snprintf(clientID ,sizeof(clientID ),"IOT_%s_%d",deviceName, rand() % 5000);            
        Serial.printf("\nMQTT Connection ID: %s ", clientID);      

        /* connect now */
        if (mqtt.connect(clientID)) { 
            Serial.println("\n\n***** MQTT CONNECTED! *****\n\n");             
            
            const uint8_t size = sizeof(subtopic)/sizeof(subtopic[0]);
            for(int x = 0; x< size ; x++){
              mqtt.subscribe(subtopic[x]);
            } 
            break;
        } 
        else {
          Serial.printf("\nConnection failed with status code : %d ,  re-trying in 10 seconds\n", mqtt.state());   
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }   
   
  }


}

void vLOOP(void * pvParameters) {
    for (;;) {
        mqtt.loop();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void initMQTT(void) {
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(callback);
    mqtt.setBufferSize(2048);

    xTaskCreatePinnedToCore(MQTT_Connect, "MQTT Connect", 4096, (void*)1, 8, &xMQTT_Connect, 1);
    xTaskCreatePinnedToCore(vLOOP, "MQTT Loop", 8096, (void*)1, 5, &xLOOPHandle, 1);
}

void initialize(void) {

    Serial.printf("Connecting to WiFi: %s\n", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.print(".");
    }
    Serial.println("\n***** WiFi CONNECTED! *****");

    vNTPFunction();      // ✅ FIX: WiFi FIRST
    initMQTT();
    vUpdateFunction();
}

#endif
