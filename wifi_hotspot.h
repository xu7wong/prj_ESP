String webpage_index;
String eeprom_default = "{\"ws\":\"ssdfsdf\",\"wp\":\"ssdfsdf\",\"wm\":2,\"ss\":[1,1,1,1]}";
int webpage_flag = 1;
void reset_EEPROM() {
  //Serial.println(eeprom_default);
  char EEPROM_s[512];

  eeprom_default.toCharArray(EEPROM_s, 512);
  Serial.println(EEPROM_s);
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, EEPROM_s[i]);
    EEPROM.commit();
    //Serial.print(EEPROM_s[i]);
  }

}

void WIFI_search(String sub1, String sub2,int sub3) {
  //generate a list of accessable WIFI; then change hotspot index webpage, based on this list

  webpage_index = \
                  "<html>\
  <head>\
    <meta name='viewport' content='width=device-width'>\
    <title>Robot Wifi Connection</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h2>Step 1: Setup your WIFI connection.</h2>\
    <p>Robot MAC address: " + WiFi.macAddress() + "</p>\
    <form action=\"" + sub1 + "\" method=\"POST\">Choose a SSID:<br />";

  if(sub3==1) {

    webpage_index += "<select name=\"ssid_list\"><option value = \"AAAA\">SELECT A WIFI</option>";

    delay(100);
    int n = WiFi.scanNetworks();
    if (n == 0);
    //bug?
    else
    {

      for (int i = 0; i < n; ++i)
        {

        webpage_index += "<option value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</option>";

        delay(10);
        }

    }
  webpage_index += "</select>";
  }
  if(sub3==2){
  webpage_index += "<br />ssid:<br /> <input name=\"psw\" type=\"text\" value=\"" + String(WIFI_ssid) + "\"/>";
  }

webpage_index += \
                 "</select>\
    <br />password:<br /> <input name=\"psw\" type=\"text\" value=\"" + String(WIFI_password) + "\"/>\
    <br /><input type=\"submit\" value=\"" + sub2 + "\"/></form>\
  </body>\
</html>";


}

//<br />classroom_ID:<br /> <input name=\"classroom_ID\" type=\"text\" value=\"B11-112\"/>\

void page_root() {
  
  Serial.println(AP_server.args());
  if (AP_server.args() == 2) {
    AP_server.arg ( 0 ).toCharArray(WIFI_ssid, 32);
    AP_server.arg ( 1 ).toCharArray(WIFI_password, 32);
    Serial.println(WIFI_ssid);
    Serial.println(WIFI_password);
    //WiFi.begin(WIFI_ssid, WIFI_password);
    //Serial.println("WIFI start B!");
  }
  
  
    WIFI_search("update", "Save WIFI",1);
  
  AP_server.send ( 2000, "text/html", webpage_index );
}
void page_update() {
  char temp2[2000];
  Serial.println(AP_server.args());
  if (AP_server.args() == 2) {
    AP_server.arg ( 0 ).toCharArray(WIFI_ssid, 32);
    AP_server.arg ( 1 ).toCharArray(WIFI_password, 32);
    Serial.println(WIFI_ssid);
    Serial.println(WIFI_password);
  }
    snprintf ( temp2, 2000,
               "<html>\
  <head>\
    <meta name='viewport' content='width=device-width'>\
    <title>Robot Wifi Connection</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h2>AP-MODE will be turned OFF in 5 seconds.</h2>\
    <p>WIFI_SSID: %s</p>\
    <p>WIFI_password: %s</p>\
    <a href=\"http://139.59.233.107:3000/\">Visit our Blockly</a>\
  </body>\
</html>",
               WIFI_ssid, WIFI_password);

  AP_server.send ( 2000, "text/html", temp2 );

  if (AP_server.args() == 2) {
    //AP_server.arg ( 0 ).toCharArray(WIFI_ssid, 32);
    //AP_server.arg ( 1 ).toCharArray(WIFI_password, 32);
    //Serial.println(WIFI_ssid);
    //Serial.println(WIFI_password);
    //AP_server.arg ( 2 ).toCharArray(CLASSROOM_code, 32);
    eeprom_default = "{\"ws\":\"" + String(WIFI_ssid) + "\",\"wp\":\"" + String(WIFI_password) + "\",\"wm\":1,\"ss\":[1,1,1,1]}";
    reset_EEPROM();
    //delay(2000);
    ESP.restart();//soft restart
  }
}

void WIFI_AP_init() {
  //WiFi.softAP(AP_ssid, AP_password);
  String AP_ssid_string = "Robot-" + WiFi.macAddress();
  char AP_ssid[40];
  AP_ssid_string.toCharArray(AP_ssid, 40);

  WiFi.softAP(AP_ssid); //open WIFI hotspot, no password
  IPAddress myIP = WiFi.softAPIP();
  AP_server.on("/", page_root);
  AP_server.on("/update", page_update);
  AP_server.begin();
}
