//##################################################################################################################
//##                                      ELET2415 DATA ACQUISITION SYSTEM CODE                                   ##
//##################################################################################################################

// LIBRARY IMPORTS
#include <rom/rtc.h> 
#include <math.h>  
#include <ctype.h>

// ADD YOUR IMPORTS HERE
#include <DHT.h>
#include <FastLED.h> // Required for the 7-bit LED Array

#ifndef _WIFI_H 
#include <WiFi.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif 
 
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

// DEFINE VARIABLES
#define ARDUINOJSON_USE_DOUBLE 1 

// DEFINE THE CONTROL PINS FOR THE DHT22 
#define DHTPIN 26          
#define DHTTYPE DHT22    

// DEFINE THE CONTROL PINS FOR THE LED ARRAY
#define LED_PIN   18
#define NUM_LEDS  7

// MQTT CLIENT CONFIG  
static const char* pubtopic    = "620170239_pub";                    
static const char* subtopic[]  = {"620170239_sub","/elet2415"};  
static const char* mqtt_server = "www.yanacreations.com";         
static uint16_t mqtt_port      = 1883;

// WIFI CREDENTIALS
const char* ssid     = "iPhone";     
const char* password = "kaola123"; 

// TASK HANDLES 
TaskHandle_t xMQTT_Connect       = NULL; 
TaskHandle_t xNTPHandle          = NULL;  
TaskHandle_t xLOOPHandle         = NULL;  
TaskHandle_t xUpdateHandle       = NULL;
TaskHandle_t xButtonCheckeHandle = NULL;  

// FUNCTION DECLARATION   
void initMQTT(void);                
unsigned long getTimeStamp(void);   
void callback(char* topic, byte* payload, unsigned int length);
void initialize(void);
bool publish(const char *topic, const char *payload); 
void vButtonCheck(void * pvParameters);
void vUpdate(void * pvParameters);  
bool isNumber(double number);
double calcHeatIndex(double Temp, double Humid);

// Init class Instances
DHT dht(DHTPIN, DHTTYPE); 
CRGB leds[NUM_LEDS];

//############### IMPORT HEADER FILES ##################
#include "NTP.h"
#include "mqtt.h"

void setup() {
    Serial.begin(115200);

    dht.begin();

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    delay(1000);

    initialize();   // MUST BE LAST
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

//####################################################################
//#                          TASK FUNCTIONS                           #       
//####################################################################
void vButtonCheck(void * pvParameters) {
    for (;;) {
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void vUpdate(void * pvParameters) {
    for (;;) {
        double h = dht.readHumidity();
        double t = dht.readTemperature();

        if (isNumber(t)) {

    // 🔒 WAIT UNTIL MQTT IS CONNECTED
    if (!mqtt.connected()) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        continue;
    }

    JsonDocument doc;
    char message[250];

    doc["id"]          = "620170239";
    doc["timestamp"]   = getTimeStamp();
    doc["temperature"] = t;
    doc["humidity"]    = h;
    doc["heatindex"]   = calcHeatIndex(t, h);

    serializeJson(doc, message);
    publish(pubtopic, message);
}
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void vUpdateFunction(void) {
    xTaskCreatePinnedToCore(
        vUpdate, "vUpdate", 4096, (void*)1, 6, &xUpdateHandle, 1
    );
}

unsigned long getTimeStamp(void) {
    time_t now;
    time(&now);
    return now;
}

void callback(char* topic, byte* payload, unsigned int length) {
    char received[length + 1];
    for (int i = 0; i < length; i++) received[i] = (char)payload[i];
    received[length] = '\0';

    JsonDocument doc;
    if (deserializeJson(doc, received)) {
        return;
    }

    const char* type = doc["type"];
    if (strcmp(type, "controls") == 0) {
        int nodes = doc["leds"];
        int brightness = doc["brightness"];
        int r = doc["color"]["r"];
        int g = doc["color"]["g"];
        int b = doc["color"]["b"];

        Serial.printf("Received: nodes=%d, brightness=%d, color=(%d,%d,%d)\n", nodes, brightness, r, g, b);

        FastLED.setBrightness(brightness);

        for (int i = 0; i < NUM_LEDS; i++) {
            if (i < nodes) {
                leds[i] = CRGB(r, g, b);
            } else {
                leds[i] = CRGB::Black;
            }
        }

        FastLED.show();
    }
}

bool publish(const char *topic, const char *payload) {
    return mqtt.publish(topic, payload);
}

double calcHeatIndex(double Temp, double Humid) {
    return -8.78469475556 + 1.61139411 * Temp + 2.33854883889 * Humid;
}

bool isNumber(double number) {
    return !isnan(number);
}