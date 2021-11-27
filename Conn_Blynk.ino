#define BLYNK_PRINT Serial
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
SoftwareSerial receiver(4, 5);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "kyY5TTP6hIfB42BcFWz8gdDq_w6OHwXs";
66
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FERI HANDOYO";
char pass[] = "1234567890";
typedef struct{
 uint16_t FUEL_LEVEL = 0; // 0
 uint16_t COOLANT_TEMP = 0; // 1
 uint16_t ENGINE_RPM = 0; // 2
 uint16_t VEHICLE_SPEED = 0; // 3
 uint16_t RUNTIME = 0; // 4
 uint16_t THROTLE = 0; // 5
} PACKET_DATA;
PACKET_DATA data;
BLYNK_READ(V0){
 Blynk.virtualWrite(V0, data.FUEL_LEVEL);
}
BLYNK_READ(V1){
 Blynk.virtualWrite(V1, data.COOLANT_TEMP);
}
BLYNK_READ(V2){
 Blynk.virtualWrite(V2, data.ENGINE_RPM);
}
BLYNK_READ(V3){
 Blynk.virtualWrite(V3, data.VEHICLE_SPEED);
}
BLYNK_READ(V4){
 Blynk.virtualWrite(V4, data.RUNTIME);
}
BLYNK_READ(V5){
 Blynk.virtualWrite(V5, data.THROTLE);
}
void setup()
{
 // Debug console
 Serial.begin(9600);
// receiver.begin(9600);
 Blynk.begin(auth, ssid, pass, "iot.serangkota.go.id", 8080);
}
void loop()
{
 Blynk.run();
 if (Serial.available() > 0){
 byte buffer[20];
67
 int i = 0;
 while (Serial.available()){
 if (i >= sizeof(PACKET_DATA)) Serial.read();
 else buffer[i++] = Serial.read();
 delay(5);
 }
// for (int a = 0; a < i; a++) {
// Serial.print(buffer[a], HEX);
// Serial.print(" ");
// }
// Serial.println();
 memcpy(&data, buffer, sizeof(PACKET_DATA));
// byte *p = (byte *)&data;
// for (int a = 0; a < i; a++) {
// Serial.print(*(p + a), HEX);
// Serial.print(" ");
// }
 /*
 Serial.println("\nreceive data");
 Serial.print(F("FUEL LEVEL: "));
 Serial.println(data.FUEL_LEVEL, DEC);
 Serial.print(F("COOLANT TEMP: "));
 Serial.println(data.COOLANT_TEMP, DEC);
 Serial.print(F("ENGINE RPM: "));
 Serial.println(data.ENGINE_RPM, DEC);
 Serial.print(F("VEHICLE SPEED: "));
 Serial.println(data.VEHICLE_SPEED, DEC);
 Serial.print(F("RUNTIME: "));
 Serial.println(data.RUNTIME, DEC);
 Serial.print(F("THROTLE: "));
 Serial.println(data.THROTLE, DEC);
 */
 }
}