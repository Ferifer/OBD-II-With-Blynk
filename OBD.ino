// Service 01 PIDs (more detail: https://en.wikipedia.org/wiki/OBDII_PIDs)
#define PID_ENGINE_LOAD 0x04
#define PID_COOLANT_TEMP 0x05
#define PID_SHORT_TERM_FUEL_TRIM_1 0x06
#define PID_LONG_TERM_FUEL_TRIM_1 0x07
#define PID_SHORT_TERM_FUEL_TRIM_2 0x08
#define PID_LONG_TERM_FUEL_TRIM_2 0x09
#define PID_FUEL_PRESSURE 0x0A
#define PID_INTAKE_MAP 0x0B
#define PID_ENGINE_RPM 0x0C
#define PID_VEHICLE_SPEED 0x0D
#define PID_TIMING_ADVANCE 0x0E
#define PID_INTAKE_TEMP 0x0F
#define PID_MAF_FLOW 0x10
#define PID_THROTTLE 0x11
#define PID_AUX_INPUT 0x1E
#define PID_RUNTIME 0x1F
#define PID_DISTANCE_WITH_MIL 0x21
#define PID_COMMANDED_EGR 0x2C
#define PID_EGR_ERROR 0x2D
#define PID_COMMANDED_EVAPORATIVE_PURGE 0x2E
#define PID_FUEL_LEVEL 0x2F
#define PID_WARMS_UPS 0x30
#define PID_DISTANCE 0x31
#define PID_EVAP_SYS_VAPOR_PRESSURE 0x32
#define PID_BAROMETRIC 0x33
#define PID_CATALYST_TEMP_B1S1 0x3C
#define PID_CATALYST_TEMP_B2S1 0x3D
#define PID_CATALYST_TEMP_B1S2 0x3E
#define PID_CATALYST_TEMP_B2S2 0x3F
#define PID_CONTROL_MODULE_VOLTAGE 0x42
#define PID_ABSOLUTE_ENGINE_LOAD 0x43
#define PID_AIR_FUEL_EQUIV_RATIO 0x44
#define PID_RELATIVE_THROTTLE_POS 0x45
#define PID_AMBIENT_TEMP 0x46
#define PID_ABSOLUTE_THROTTLE_POS_B 0x47
#define PID_ABSOLUTE_THROTTLE_POS_C 0x48
#define PID_ACC_PEDAL_POS_D 0x49
#define PID_ACC_PEDAL_POS_E 0x4A
#define PID_ACC_PEDAL_POS_F 0x4B
#define PID_COMMANDED_THROTTLE_ACTUATOR 0x4C
#define PID_TIME_WITH_MIL 0x4D
#define PID_TIME_SINCE_CODES_CLEARED 0x4E
#define PID_ETHANOL_FUEL 0x52
#define PID_FUEL_RAIL_PRESSURE 0x59
#define PID_HYBRID_BATTERY_PERCENTAGE 0x5B
#define PID_ENGINE_OIL_TEMP 0x5C
#define PID_FUEL_INJECTION_TIMING 0x5D
63
#define PID_ENGINE_FUEL_RATE 0x5E
#define PID_ENGINE_TORQUE_DEMANDED 0x61
#define PID_ENGINE_TORQUE_PERCENTAGE 0x62
#define PID_ENGINE_REF_TORQUE 0x63
//----------------------------------------------
#define CAN_ID_PID 0x7DF //OBD-II CAN frame ID
#include <mcp_can.h>
#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial esp8266(3, 4);
#define CAN0_INT 2 // Set INT to pin 2
<--------- CHANGE if using different pin number
MCP_CAN CAN0(10); // Set CS to pin 10 <-
-------- CHANGE if using different pin number
boolean conection_ok = false;
const byte request[] = {PID_FUEL_LEVEL, PID_COOLANT_TEMP,
PID_ENGINE_RPM
 , PID_VEHICLE_SPEED, PID_RUNTIME,
PID_THROTTLE};
typedef struct{
 uint16_t FUEL_LEVEL = 0;
 uint16_t COOLANT_TEMP = 0;
 uint16_t ENGINE_RPM = 0;
 uint16_t VEHICLE_SPEED = 0;
 uint16_t RUNTIME = 0;
 uint16_t THROTLE = 0;
} PACKET_DATA;
PACKET_DATA data;
void sendPID(unsigned char __pid)
{
 unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
 byte sndStat = CAN0.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);
}
void receivePID(unsigned char __pid)
{
 if (!conection_ok) return;
 byte count = 0;
 long unsigned int rxId;
 unsigned char len = 0;
 unsigned char rxBuf[8];
 repeat:
 if (!digitalRead(CAN0_INT)) { // If CAN0_INT
pin is low, read receive buffer
 CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read data: len =
data length, buf = data byte(s)
 }
64
 else {
 delay(40);
 count++;
 if (count < 5) goto repeat;
 }
 switch (__pid) {
 case PID_FUEL_LEVEL:
 if (rxBuf[3]==PID_FUEL_LEVEL){
 data.FUEL_LEVEL = ((100*rxBuf[3])/255);
 }
 break;
 case PID_COOLANT_TEMP:
 if(rxBuf[2] == PID_COOLANT_TEMP){
 data.COOLANT_TEMP = rxBuf[3] - 40;
 }
 break;
 case PID_ENGINE_RPM:
 if(rxBuf[2] == PID_ENGINE_RPM){
 data.ENGINE_RPM = ((256 * rxBuf[3]) + rxBuf[4]) / 4;
 }
 break;
 case PID_VEHICLE_SPEED:
 if(rxBuf[2] == PID_VEHICLE_SPEED){
 data.VEHICLE_SPEED = rxBuf[3];
 }
 break;
 case PID_RUNTIME:
 if(rxBuf[2] == PID_RUNTIME){
 data.RUNTIME = ((256 * rxBuf[3]) + rxBuf[4]);
 }
 break;
 case PID_THROTTLE:
 if(rxBuf[2] == PID_THROTTLE){
 data.THROTLE = ((rxBuf[3])*100/255);
 }
 break;
 }
}
void setup()
{
 Serial.begin(9600);
 esp8266.begin(9600);
 if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK) { //<
-------- - CHANGE if using different board
 conection_ok = true;
 }
 else {
 conection_ok = false;
 }
 //initialise mask and filter to allow only receipt of 0x7xx CAN IDs
 CAN0.init_Mask(0, 0, 0x07000000); // Init first mask...
65
 CAN0.init_Mask(1, 0, 0x07000000); // Init second
mask...
 for (uint8_t i = 0; i < 6; ++i) {
 CAN0.init_Filt(i, 0, 0x07000000); //Init filters
 }
 CAN0.setMode(MCP_NORMAL); // Set operation mode
to normal so the MCP2515 sends acks to received data.
 pinMode(CAN0_INT, INPUT); // Configuring pin for
/INT input
}
void loop() {
 for (byte i = 0; i < sizeof(request); i++){
 //sendPID(request[i]);
 delay(40);
 //receivePID(request[i]);
 delay(150); //jeda pengambilan data
 }
 esp8266.write((byte*)&data, sizeof(PACKET_DATA));

 Serial.print("fuel: ");
 Serial.println(data.FUEL_LEVEL);
 Serial.print("colant: ");
 Serial.println(data.COOLANT_TEMP);
 Serial.print("rpm: ");
 Serial.println(data.ENGINE_RPM);
 Serial.print("speed: ");
 Serial.println(data.VEHICLE_SPEED);
 Serial.print("run time: ");
 Serial.println(data.RUNTIME);
 Serial.print("throtle: ");
 Serial.println(data.THROTLE);

 Serial.println();
}
