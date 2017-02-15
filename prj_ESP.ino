#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <MQTTClient.h>

ESP8266WebServer AP_server(80);
WiFiClient wificlient;
MQTTClient mqtt_client;

#include <Wire.h>
#define i2c_SDA  14 //D5 I2C data
#define i2c_SDL  12 //D6 I2C clock

#include <Servo.h>

Servo myservo;

//#define GPIO_I2C_ADDRESS  0x21  //100
#define MPU6050_I2C_ADDRESS 0x68 //AN0=0
//#define MINI_I2C_ADDRESS 0x08 //AN0=0

int EEEPROM_length = 512;
char WIFI_ssid[32];
char WIFI_password[32];
int wifi_mode = 1;

char CLASSROOM_code[32];
char Sensor_list[64];
String classroom_id;  //for changing the topic of mqtt

String mqtt_topic;
char mqtt_id[30];

bool AP_flag = false; //if true, go into AP mode
bool AP_psw_updated = false;  //after user updated the WIFI password from AP, change this flag;
bool mqtt_flag = false; //if true, in the normal mode, mqtt client keeps listenting


int mqtt_try_set = 10;
int WIFI_try_set = 30;
int temp_led_count = 1;

int Motor_status = 0;
int analog_buffer[4] = {0, 0, 0, 0};

int sensor_status[4] = {0, 0, 0, 0};



#define interrupt_pin 13 //D7

#define Echo_trig 5 //D1
#define Echo_receive 16 //D0
#define ext1_pin 4 //D2
#define ext2_pin 0 //D3
#include "wifi_hotspot.h"
#include "LED_ring.h"
#include "buzzer.h"
#include "OLED.h"
#include "mqtt_received.h"
#include "mqtt_autosend.h"



//#include "RGB.h"
//#include "compass.h"
//___________________________________________________________________________________________________________
void setup() {
  //ESP.wdtDisable();

  pinMode(interrupt_pin, OUTPUT);//D7 as interrupt trigger
  digitalWrite(interrupt_pin, LOW);

  Serial.begin(115200);

  Wire.begin(i2c_SDA, i2c_SDL); //begin(sda,scl) D5/D6
  Wire.setClock(400000L);//test result is 400K
  //Wire.setClock(100000);//test result is 100K

  oled_init();
  led_ring_init();
  buzzer_init();


  char EEPROM_string[512];
  EEPROM.begin(512);

  for (int temp_j = 0; temp_j < 512; temp_j++) {
    EEPROM_string[temp_j] = EEPROM.read(temp_j);

  }
  Serial.println(EEPROM_string);
  StaticJsonBuffer<512> json_EEPROM;
  JsonObject& root_EEPROM = json_EEPROM.parseObject(EEPROM_string);
  if (root_EEPROM.success()) {

    if (!root_EEPROM.containsKey("ws") || !root_EEPROM.containsKey("wp") || !root_EEPROM.containsKey("wm") || !root_EEPROM.containsKey("ss")) {
      reset_EEPROM();
    }
    else {
      const char* eeprom_ssid = root_EEPROM["ws"];
      const char* eeprom_psw = root_EEPROM["wp"];
      wifi_mode = root_EEPROM["wm"];
      sensor_status[0] = root_EEPROM["ss"][0];
      sensor_status[1] = root_EEPROM["ss"][1];
      sensor_status[2] = root_EEPROM["ss"][2];
      sensor_status[3] = root_EEPROM["ss"][3];
      strncpy(WIFI_ssid, eeprom_ssid, strlen(eeprom_ssid));
      strncpy(WIFI_password, eeprom_psw, strlen(eeprom_psw));
    }
  }
  else reset_EEPROM();


  if (wifi_mode == 2) {
    WiFi.mode(WIFI_AP_STA);
    Serial.println("mode 2");

    //WIFI_search();  //generate a list of avaliable WIFI SSID (do not try to connect)
    delay(1000);
    WIFI_AP_init();
  }
  if (wifi_mode == 1) {
    Serial.println("mode 1");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_ssid, WIFI_password);
    int wificonnect_count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      wificonnect_count++;
      if (wificonnect_count >= WIFI_try_set) {
        eeprom_default = "{\"ws\":\"" + String(WIFI_ssid) + "\",\"wp\":\"" + String(WIFI_password) + "\",\"wm\":2,\"ss\":[1,1,1,1]}";
        reset_EEPROM();
        Serial.println("ESP will reset");
        ESP.restart();//soft restart and enter AP mode next time "wm"=2
        //break;
      }
    }
    mqtt_connect();
  }
  /*
    //___________________________________________________________________________________________________________
    if (sensor_status[0] == 1) {
      pinMode(Echo_trig, OUTPUT);
      pinMode(Echo_receive, INPUT);
    }
    if (sensor_status[2] == 1) {
      pinMode(ext1_pin, OUTPUT);//D2
      digitalWrite(ext1_pin, LOW);
    }
    if (sensor_status[3] == 1) {

    }
  */
  /*
    //___________________________________________________________________________________________________________
    WiFi.mode(WIFI_STA);
    String mqtt_id_str = "robot/" + WiFi.macAddress();
    mqtt_id_str.toCharArray(mqtt_id, 30);
    //___________________________________________________________________________________________________________
    WiFi.begin(WIFI_ssid, WIFI_password); //use the ssid stored in the EEPROM

    int wificonnect_count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      temp_led_count++;
      Serial.print("x");
      delay(500);
      if (wificonnect_count == WIFI_try_set) {
        //  very important, about the timeout length, may have some problems in some worse condition
        AP_flag = true; //flag to turn on AP mode in LOOP;
        WIFI_search();  //generate a list of avaliable WIFI SSID (do not try to connect)
        delay(1000);
        WiFi.mode(WIFI_AP_STA);
        delay(1000);
        break;
      }
      wificonnect_count++;
      AP_flag = false;  //continue to skip AP mode in LOOP;
    }

    if (WiFi.status() == WL_CONNECTED) {
      mqtt_connect();
    }
    //___________________________________________________________________________________________________________
    if (AP_flag) {
      WIFI_AP_init();
    }
  */
}

void loop() {
  if (!mqtt_flag) {
    if (wifi_mode == 2) {
      AP_server.handleClient();



    }

  }
  else {
    if (wifi_mode == 1) {
      mqtt_client.loop();
      if (!wificlient.connect("139.59.233.107", 9001)) {
        Serial.println("Connection failed.");
      }

    }
  }
}
/*
  if (wificlient.connect("mqtt.3dprintingsystems.com", 9003)) {
    Serial.println("Connected");
    Serial.println(WIFI_ssid);
    Serial.println(WIFI_password);
  } else {
    Serial.println("Connection failed.");
  }
*/
/*
  if (Serial.available()) process_line();
  //___________________________________________________________________________________________________________
  if (AP_flag) {
  AP_server.handleClient();
  //one problem is after upload, AP_flag will become to be false, can the updated webpage be displayed?
  delay(10);
  AP_server.handleClient();
  delay(10);
  AP_server.handleClient();
  delay(10);
  if (AP_psw_updated) {
    //after user updated the WIFI password from AP, change this flag;
    AP_flag = false;
    mqtt_flag = false;
  }
  }
  //___________________________________________________________________________________________________________
  if (!AP_flag && WiFi.status() != WL_CONNECTED) {
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(WIFI_ssid, WIFI_password);

  int wificonnect_count = 0;
  temp_led_count = 1;
  while (WiFi.status() != WL_CONNECTED) {
    temp_led_count++;
    delay(1000);
    wificonnect_count++;
    if (wificonnect_count == WIFI_try_set) {
      delay(500);
      ESP.restart();//soft restart
      break;
    }
  }
  mqtt_connect();
  }
  if (mqtt_flag) {
  mqtt_client.loop(); //in the normal mode, mqtt always keeps working;
  mqtt_autosend();
  }
*/
}

void mqtt_connect() {
  mqtt_client.begin("139.59.233.107", 1886, wificlient);
  int mqttconnect_count = 0;
  while (!mqtt_client.connected()) {
    mqtt_client.connect(mqtt_id, "try", "try");
    //Serial.print("*");
    delay(500);
    if (mqttconnect_count == mqtt_try_set) {
      //Serial.println("MQTT timeout! restart ROBOT!");
      break;
    }
    mqttconnect_count++;
  }
  String mac_string = "robot/" + WiFi.macAddress();
  if (mqtt_client.connected()) {
    mqtt_client.subscribe(String("to/robot/" + WiFi.macAddress())); //**********************************************************subscribe 2
    //mqtt_topic = "classroom/" + String(CLASSROOM_code) + "/robot/" + WiFi.macAddress();
    mqtt_topic = "robot/" + WiFi.macAddress();
    mqtt_client.publish(mqtt_topic, "{\"message\":\"classroom_request\"}");
    delay(1000);
    mqtt_flag = true;
  }
}


void process_line() {
  String t = Serial.readString();
  //analog_buffer=[0,0,0,0];
  char json_serial[128];
  t.toCharArray(json_serial, 128);

  StaticJsonBuffer<128> json_serial_Buffer;
  JsonObject& root_serial = json_serial_Buffer.parseObject(json_serial);

  if (root_serial.success()) {
    //Serial.println("parseObject() OK!");
    if (root_serial.containsKey("mt") ) {
      //{"mt":2}
      int motor_finish = root_serial["mt"];
      if (motor_finish == 2) {
        if (Motor_status == 1) {
          mqtt_client.publish(mqtt_topic, "{\"step\":1}");
          Motor_status = 0;
        }
        if (Motor_status == 2) {
          mqtt_client.publish(mqtt_topic, "{\"step\":2}");
          Motor_status = 0;
        }
      }
    }
    if (root_serial.containsKey("ag1") && root_serial.containsKey("ag2")) {
      //{"ag1":2,"ag2":222}
      analog_buffer[1] = root_serial["ag1"];
      analog_buffer[2]  = root_serial["ag2"];
      analog_buffer[0]  = analogRead(A0);
      //mqtt_client.publish(mqtt_topic, "{\"step\":2}");

      mqtt_client.publish(mqtt_topic, "{\"analog\":[" + String(analog_buffer[0]) + "," + String(analog_buffer[1]) + "," + String(analog_buffer[2]) + "," + String(analog_buffer[3]) + "]}");

    }
  }



}

