// Include external libraries
#include <SPI.h>
#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <BMA222.h>

// Wireless network parameters
char ssid[] = "test";          // Your wireless network name also called SSID
char password[] = "focq9542";       // Your wireless network password

// IBM IoT Foundation Cloud Settings
/* When adding a device on internetofthings.ibmcloud.com the following information will be generated:
    org=<org>
    type=iotsample-ti-energia
    id=<mac>
    auth-method=token
    auth-token=<password>
*/

#define MQTT_MAX_PACKET_SIZE 4096       // Maximum length of MQTT message in bytes
#define IBMSERVERURLLEN  64
#define IBMIOTFSERVERSUFFIX "messaging.internetofthings.ibmcloud.com"

char organization[] = "w6p0vg";     // Your BlueMix Organisation ID
char typeId[] = "cc3200";            // Type of device
char pubtopic[] = "iot-2/evt/status/fmt/json";    // MQTT publication topic
char deviceId[] = "d4f51303ee34";                 // Unique device identifier typically the MAC address of the IoT device
char clientId[64];

char mqttAddr[IBMSERVERURLLEN];
int mqttPort = 1883;                              // Port for MQTT connection

// Authentication method. Should be use-token-auth when using authenticated mode
char authMethod[] = "use-token-auth";          
char authToken[] = "OmA!r1dbTi@Nw?Fg+M";          // The authentication token generated by your BlueMix application

const char MQTTSTRING[] = "{\"d\":{\"myName\":\"TILaunchPad\",\"open\":%s,\"prevState\":%s,\"X\":%i,\"Y\":%i,\"Z\":%i,\"RSSI\":%i}}";

// Create MAC address and WiFiIPStack objects
MACAddress mac;
  
WifiIPStack ipstack;  
MQTT::Client<WifiIPStack, Countdown, MQTT_MAX_PACKET_SIZE> client(ipstack);

// Define external sensors, inputs and outputs.
BMA222 accSensor;            // Three axis acceleration sensor

#include <Wire.h>

long n = 0;
int initx;
int inity;
int initz;
boolean prevOpen;

// Setup function runs once when microprocessor is powered up
void setup() {
  uint8_t macOctets[6];
  
  Serial.begin(115200);          //Initialise serial port for local monitoring on the Serail Monitor via USB
  Serial.print("Attempting to connect to Network named: ");
  Serial.println(ssid);
  
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print(".");         // print dots while we wait to connect
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");        // print dots while we wait for an ip addresss
    delay(300);
  }

  // We are connected and have an IP address.
  Serial.print("\nIP Address obtained: ");
  Serial.println(WiFi.localIP());

  mac = WiFi.macAddress(macOctets);
  Serial.print("MAC Address: ");
  Serial.println(mac);
  
  // Use MAC Address as deviceId
  sprintf(deviceId, "%02x%02x%02x%02x%02x%02x", macOctets[0], macOctets[1], macOctets[2], macOctets[3], macOctets[4], macOctets[5]);
  Serial.print("deviceId: ");
  Serial.println(deviceId);

  sprintf(clientId, "d:%s:%s:%s", organization, typeId, deviceId);
  sprintf(mqttAddr, "%s.%s", organization, IBMIOTFSERVERSUFFIX);
  
  // start the accel sensor
  accSensor.begin();
  uint8_t chipID = accSensor.chipID();
  Serial.print("ChipID: ");
  Serial.println(chipID);
  
  // get initial sensor values at base position
  initx = accSensor.readXData();
  inity = accSensor.readYData();
  initz = accSensor.readZData();
  prevOpen = false;
  
  Serial.println("Setup end");
  Serial.println();

}

// Main loop. Runs continuously
void loop() {
  int rc = -1;
  
  // If the MQTT service is not connected then open connection
  if (!client.isConnected()) {
    Serial.print("Connecting to ");
    Serial.print(mqttAddr);
    Serial.print(":");
    Serial.println(mqttPort);
    Serial.print("With client id: ");
    Serial.println(clientId);
    
    while (rc != 0) {
      rc = ipstack.connect(mqttAddr, mqttPort);
    }

    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    connectData.MQTTVersion = 3.1;
    connectData.clientID.cstring = clientId;
    connectData.username.cstring = authMethod;
    connectData.password.cstring = authToken;
    connectData.keepAliveInterval = 10;
    
    rc = -1;
    while ((rc = client.connect(connectData)) != 0);
    Serial.println("Connected\n");
  }
  
  // Poll sensors
  int x = accSensor.readXData();
  int y = accSensor.readYData();
  int z = accSensor.readZData();
  
  // Compare to initialised values
  boolean opened = !( (initx - x) * (initx - x) + (inity - y) * (inity - y) + (initz - z) * (initz - z) < 225);
  
  // Publish JSON data to MQTT service
  Serial.print("Publishing: ");
  Serial.print(n++);
  char string[64]; // = aJson.print(json);   // Convert JSON data to MQTT string
  sprintf(string, MQTTSTRING, opened ? "true" : "false", prevOpen ? "true" : "false", x, y, z, -WiFi.RSSI());
  Serial.println(string);             // Print MQTT string to serial terminal
  
  prevOpen = opened;
  
  // Create MQTT message
  MQTT::Message message;              
  message.qos = MQTT::QOS0; 
  message.retained = false;
  message.payload = string; 
  message.payloadlen = strlen(string);
  
  // Publish MQTT message
  rc = client.publish(pubtopic, message);
  if (rc != 0) {
    Serial.print("Message publish failed with return code : ");
    Serial.println(rc);
  }
  
  // Wait before publishing again
  client.yield(500);
}
