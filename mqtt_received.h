
void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  //Serial.print("incoming: ");
  //Serial.print(topic);
  //Serial.print(" - ");
  //Serial.println(payload);

  char json[1200];
  payload.toCharArray(json, 1200);

  StaticJsonBuffer<1200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  //___________________________________________________________________________________________________________
  if (root.containsKey("classroom_unique_code"))
    //{"classroom_unique_code":"uiyiuy"}
  {
    const char* classroom_code = root["classroom_unique_code"];
    classroom_id = String(classroom_code);
    mqtt_topic = "classroom/" + classroom_id + "/robot/" + WiFi.macAddress();
  }
  //___________________________________________________________________________________________________________

  if (root.containsKey("sensor_update"))
    //{"sensor_update":{"S_A":1,"S_B":1,"S_C":1,"S_D":1,"S_E":1,"S_F":1,"S_G":1,"S_H":1,"S_I":1,"S_J":1}}
    //{"sensor_update":{"SP":[0,0,0,0]}}

  {
    int S_A =  root["sensor_update"]["SP"][0];
    int S_B =  root["sensor_update"]["SP"][1];
    int S_C =  root["sensor_update"]["SP"][2];
    int S_D =  root["sensor_update"]["SP"][3];
    String cmd = "{\"S\":[" + String(S_A) + "," + String(S_B) + "," + String(S_C) + "," + String(S_D) + "]}";
    Serial.println(cmd);
    cmd.toCharArray(Sensor_list, sizeof(Sensor_list));
    for (int temp_k = 0; temp_k < sizeof(Sensor_list); temp_k++) {
      EEPROM.write(temp_k + sizeof(WIFI_ssid) + sizeof(WIFI_password) + sizeof(CLASSROOM_code), Sensor_list[temp_k]);
      EEPROM.commit();
    }
    //Serial.print("Sensor list updated to ");
    //Serial.println(Sensor_list);
    delay(500);
    ESP.restart();//soft restart
  }

  if (root.containsKey("RTTTL")) {
    //{"RTTTL":{"RTTTL_type":1,"RTTTL_speed":20,"RTTTL_vol":20,"RTTTL_music_list":1}}
    //{"RTTTL":{"RTTTL_type":2,"RTTTL_speed":20,"RTTTL_vol":20,"RTTTL_music":"Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6"}}
    //{"RTTTL":{"RTTTL_type": 3,"RTTTL_freq":[1,2,3,4,5,6],"RTTTL_vol": 100, "RTTTL_speed":20,"RTTTL_pause":20}}
    //if speed=0, normal speed;

    int RTTTL_type = root["RTTTL"]["RTTTL_type"];
    int RTTTL_vol = root["RTTTL"]["RTTTL_vol"];
    int RTTTL_speed = root["RTTTL"]["RTTTL_speed"];
    int RTTTL_pause = root["RTTTL"]["RTTTL_pause"];

    if (RTTTL_type == 1) {
      int music_list = root["RTTTL"]["RTTTL_music_list"];
      if (music_list == 1)play_rtttl(song1, RTTTL_vol, RTTTL_speed);
      if (music_list == 2)play_rtttl(song2, RTTTL_vol, RTTTL_speed);
    }
    if (RTTTL_type == 2) {
      const char* RTTTL_music = root["RTTTL"]["RTTTL_music"];
      play_rtttl(RTTTL_music, RTTTL_vol, RTTTL_speed);
    }
    if (RTTTL_type == 3) {
      for (int i = 0; i < 50; i++) {
        int buzzer_freq = root["RTTTL"]["RTTTL_freq"][i];
        if (buzzer_freq > 0 && buzzer_freq < 10) {
          buzzer(buzzer_freq, RTTTL_vol, RTTTL_speed, RTTTL_pause);
        }
      }
    }
  }
  if (root.containsKey("led_ring")) {

    //{"led_ring":{"led_type":1,"led_position": 3, "led_RGB": [1, 4, 7],"led_time":0}}
    //{"led_ring":{"led_type":2, "led_RGB": [1, 4, 7] ,"led_time":0}}
    //{"led_ring":{"led_type":3}}
    //{"led_ring":{"led_type":4,"led_position": [1, 4, 7, 10], "led_red": 55, "led_green": 0, "led_blue": 0,"led_time":0}}
    //{"led_ring":{"led_type":5,"led_position1": [1, 2, 7, 9],"led_position2": [1, 2, 7, 9], "led_red1": 0, "led_green1": 0, "led_blue1": 255,"led_red2": 0, "led_green2": 0, "led_blue2": 255,"led_time":50,"led_loop":5}}
    //{"led_ring":{"led_type":6,"led_position": [1, 4, 7, 10], "led_red1": 0, "led_green1": 0, "led_blue1": 255,"led_red2": 0, "led_green2": 0, "led_blue2": 255,"led_red3": 0, "led_green3": 0, "led_blue3": 255,"led_red4": 0, "led_green4": 0, "led_blue4": 255,"led_time":50,"led_loop":20}}
    //{"led_ring":{"led_type":7,"led_position": [1, 4, 7, 10], "led_red1": 0, "led_green1": 0, "led_blue1": 255,"led_red2": 0, "led_green2": 0, "led_blue2": 255,"led_red3": 0, "led_green3": 0, "led_blue3": 255,"led_red4": 0, "led_green4": 0, "led_blue4": 255,"led_time":50,"led_loop":20}}

    int led_type = root["led_ring"]["led_type"];
    if (led_type == 1) {
      int led_position = root["led_ring"]["led_position"];
      int led_red = root["led_ring"]["led_RGB"][0];
      int led_green = root["led_ring"]["led_RGB"][1];
      int led_blue = root["led_ring"]["led_RGB"][2];
      int led_time = root["led_ring"]["led_time"];
      pixels.setPixelColor(led_position - 1, pixels.Color(led_red, led_green, led_blue)); // Moderately bright green color.
      pixels.show();
      if (led_time > 0) {
        delay(led_time * 1000);
        for (int i = 0; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
          pixels.show();
        }
      }
    }
    if (led_type == 2) {
      int led_red = root["led_ring"]["led_RGB"][0];
      int led_green = root["led_ring"]["led_RGB"][1];
      int led_blue = root["led_ring"]["led_RGB"][2];
      int led_time = root["led_ring"]["led_time"];
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(led_red, led_green, led_blue));
        pixels.show();
      }
      if (led_time > 0) {
        delay(led_time * 1000);
        for (int i = 0; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
          pixels.show();
        }
      }
    }
  }
  if (root.containsKey("serial") ) {
    //{"serial":2,"sd1":1000,"sd2":1000}
    Motor_status = root["serial"];
    int sd1 = root["sd1"];
    int sd2 = root["sd2"];
    //int sp1 = root["sp1"];
    //int sp2 = root["sp2"];
    String cmd = String(    "{\"sd1\":" + String(sd1) + "," +  "\"sd2\":" + String(sd2) + "}"       );
    Serial.println(cmd);


  }
  if (root.containsKey("ext") ) {

    //{"ext":{"ext_pin":2, "ext_type": 1,"ext_time":500}}
    //{"ext":{"ext_pin":3, "ext_pos": 1,"ext_time":500}}
    int ext_pin = root["ext"]["ext_pin"];

    if (ext_pin == 2 && sensor_status[2] == 1) {
      int ext_type = root["ext"]["ext_type"];
      int ext_time = root["ext"]["ext_time"];
      digitalWrite(ext1_pin, HIGH);
      delay(ext_time);
      digitalWrite(ext1_pin, LOW);
    }
    if (ext_pin == 3 && sensor_status[3] == 1) {
      int ext_pos = root["ext"]["ext_pos"];
      int ext_time = root["ext"]["ext_time"];
      myservo.attach(ext2_pin);
      myservo.write(ext_pos);              // tell servo to go to position in variable 'pos'
      delay(ext_time);                       // waits 15ms for the servo to reach the position
      myservo.detach();
    }
  }
  /*
    if (root.containsKey("analog") ) {
    //{"analog":"request"}

    String cmd = String(    "{\"ag\":1}"       );
    Serial.println(cmd);


    }
  */
  if (root.containsKey("robot_factory_reset"))
    //{"robot_factory_reset":"true"}
  {
    const char* factory_reset = root["robot_factory_reset"];
    if (String(factory_reset) == "true") {
      eeprom_default = "{\"ws\":\"blank\",\"wp\":\"blank\",\"wm\":2,\"ss\":[1,1,1,1]}";
      reset_EEPROM();
      
      delay(500);
      ESP.restart();//soft restart
    }
  }
  mqtt_client.publish(mqtt_topic, "{\"finished\":\"true\"}");  //**********************************************************publish 3: mqtt listening feed back
}
