#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "OneWire.h"
#include "DallasTemperature.h"

WiFiClient espClient; // Wifi
PubSubClient client(espClient); // MQTT client
/*===== MQTT broker/server and TOPICS ========*/
const char* mqtt_server = "Saisir l’ip";
#define TOPIC_TEMP "temp"
#define port_mqtt 1883
/*============= WIFI CONFIGURATION ==================*/
const char* ssid = "Saisir le ssid";
const char* password = "Saisir le mot de passe";

/*============= GPIO ======================*/
float temperature = 0;
//Capteur température :
int portOneWire = 23;
int capteurAcquisition = 0;
OneWire oneWire(portOneWire);
DallasTemperature tempSensor(&oneWire);

/*================ WIFI =======================*/
void print_connection_status() {
Serial.print("WiFi status : \n");
Serial.print("\tIP address : ");
Serial.println(WiFi.localIP());
Serial.print("\tMAC address : ");
Serial.println(WiFi.macAddress());
}
void connect_wifi() {
Serial.println("Connection au réseau Wifi...");
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
Serial.println("En attente de connection wifi ...");
delay(1000);
}
Serial.print("Connecté au réseau wifi\n");
print_connection_status();
}
/*=============== SETUP =====================*/
void setup() {
tempSensor.begin();
Serial.begin(9600);
connect_wifi();
client.setServer(mqtt_server, port_mqtt);
client.setCallback(mqtt_pubcallback);
}
/*============== CALLBACK ===================*/
void mqtt_pubcallback(char* topic, byte* message, unsigned int length) {
Serial.print("Nouveau message sur le topic :\n ");
Serial.print(topic);
Serial.print(". Message: ");
Serial.write(message, length);
Serial.println();
if (strcmp(topic, TOPIC_TEMP) == 0){
float temperatureActuelle = get_Temperature();
char temperatureActuelleStr[5];
dtostrf(temperatureActuelle,5,2,temperatureActuelleStr);
client.publish("temp",temperatureActuelleStr);
}
}

/*============= SUBSCRIBE =====================*/
void mqtt_mysubscribe(char *topic) {
while (!client.connected()) { // Loop until we're reconnected
Serial.print("Attempting MQTT connection...");
// Attempt to connect
String nomClient = "esp32_giraud_ortega";
if (client.connect(nomClient.c_str(), "try", "try")) {

Serial.println("Vous êtes bien connecté.");
client.subscribe(topic);
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println("Nouvel essai dans 5 secondes");
delay(5000);
}
}
}
float get_Temperature(){
float temperature;
tempSensor.requestTemperaturesByIndex(capteurAcquisition);
temperature = tempSensor.getTempCByIndex(capteurAcquisition);
return temperature;
}
/*================= LOOP ======================*/
void loop() {
int32_t period = 5000;
if (!client.connected()) {
mqtt_mysubscribe((char *)(TOPIC_TEMP));
}
/*--- Publish Temperature periodically */
delay(period);
//temperature = get_Temperature();
// Convert the value to a char array
//char tempString[8];
//dtostrf(temperature, 1, 2, tempString);
// Serial info
//Serial.print("Published Temperature : "); Serial.println(tempString);
// MQTT Publish
// client.publish(TOPIC_TEMP, tempString);
client.loop();
}
