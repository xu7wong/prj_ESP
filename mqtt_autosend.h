int mqtt_autosend_count = 0;
int mqtt_autosend_set = 88000;//loop
void mqtt_autosend() {
  mqtt_autosend_count++;
  if (mqtt_autosend_count == mqtt_autosend_set) {
    mqtt_autosend_count = 0;
    if (sensor_status[1] == 1) {
      String cmd = String(    "{\"ag\":1}"       );
      Serial.println(cmd);
    }
    if (sensor_status[0] == 1) {
      digitalWrite(Echo_trig, HIGH);
      delay(10);                  // waits for a 10ms
      digitalWrite(Echo_trig, LOW);    // sets the LED off
      unsigned long t = pulseIn(Echo_receive, HIGH, 100000);
      if (t > 0) {
        float distance = float(t ) / 5882.35;
        //float distance = float(t ) ;
        mqtt_client.publish(mqtt_topic, "{\"ultrasonic\":" + String(distance, 2) + "}");
      }
    }
  }
}
