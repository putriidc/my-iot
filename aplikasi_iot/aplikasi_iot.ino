#include <ESP8266WiFi.h>
#include <MQTT.h>
#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>
#include <NusabotSimpleTimer.h>


WiFiClient net;
MQTTClient client;
NusabotSimpleTimer timer;

const char ssid[] = "Rp.5000/jam";
const char pass[] = "123456789";

DHTesp dhtSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  dhtSensor.setup(2, DHTesp::DHT11);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, pass);
  client.begin("ceksuhu.cloud.shiftr.io", net);

  lcd.setCursor(0, 0);
  lcd.print("Welcome");
  delay(500);
  lcd.clear();

  client.onMessage(subscribe);
  timer.setInterval(1000, publish);

  connect();
  temphumi();
  
}

void temphumi() {
  client.loop();
  timer.run();

  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(String(data.temperature, 2));
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Kelembapan:");
  lcd.print(String(data.humidity, 1));
  lcd.print("%");

  delay(2000);
}

void loop() {
  client.loop();
  timer.run();

  if (!client.connected()) {
    connect();
  }
  
  
}

void subscribe(String &topic, String &data) {
  if (topic == "ceksuhu/suhu") {
    lcd.clear();
    if (data == "0 C") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Terlalu Dingin");
      delay(1000);
      lcd.clear();
    } else if (data == "27 C") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Suhu Optimal");
      delay(1000);
      lcd.clear();
    } else if (data == "75 C") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Terlalu Panas");
      delay(1000);
      lcd.clear();
    }
  }

  if (topic == "ceksuhu/led") {
    if (data == "nyala") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor Menyala");
      delay(500);
      lcd.clear();
      
      temphumi();

    } else if (data == "mati") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor Mati");
      delay(500);
      lcd.clear();
    }
  }
}

void publish() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  client.publish("ceksuhu/suhu", (String(data.temperature, 2)), true, 1);
  client.publish("ceksuhu/kelembapan", (String(data.humidity, 1)), true, 1);
}

void connect() {
  // lcd.setCursor(0, 0);
  // lcd.print("Menghubungkan");
  // lcd.setCursor(0, 1);
  // lcd.print("Ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    lcd.print(".");
    delay(500);
  }

  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("WiFi Terhubung");
  // delay(100);
  // lcd.clear();

  while (!client.connect("mqttx_0c321a8a", "ceksuhu", "RJThizpxvqXkmtfF")) {
    lcd.print(".");
    delay(500);
  }

  client.subscribe("ceksuhu/#", 1);
}