
//Required for BMA222 Accelerometer sensor
#include "BMA222.h"


// Define external sensors, inputs and outputs.
BMA222 accSensor;            // Three axis acceleration sensor

//Include libraries required for TMP006 Temperature sensor and create instance
#include <Wire.h>
#include "Adafruit_TMP006.h"
Adafruit_TMP006 tmp006(0x41);

int prevx;
int prevy;
int prevz;
int defaultx;
int defaulty;
int defaultz;

// Setup function runs once when microprocessor is powered up
void setup() {
  Serial.begin(115200);          //Initialise serial port for local monitoring on the Serail Monitor via USB
  
// Start Temperature Sensor
  if (!tmp006.begin()) {
    Serial.println("No sensor found");
    while (1);
  }
  
// Start the accel sensor
  accSensor.begin();
  uint8_t chipID = accSensor.chipID();
  Serial.print("ChipID: ");
  Serial.println(chipID);
  
  //Serial.print("Acc X: ");
    prevx = accSensor.readXData();
    defaultx = prevx;
    
    //Serial.print(", Y: ");
    prevy = accSensor.readYData();
    defaulty = prevy;
    //Serial.print(", Z: ");
    prevz = accSensor.readZData();
    defaultz = prevz;
}

// Main loop. Runs continuously
void loop() {
    /*Serial.print("Object (Die) Temperature: ");
    Serial.print(tmp006.readObjTempC());
    Serial.print((char)176);                //Print degree symbol
    Serial.print("C (");
    Serial.print(tmp006.readDieTempC());
    Serial.print((char)176);                //Print degree symbol
    Serial.println("C)");*/
    
    //Serial.print("Acc X: ");
    int x = accSensor.readXData();
    
    //Serial.print(", Y: ");
    int y = accSensor.readYData();
    
    //Serial.print(", Z: ");
    int z = accSensor.readZData();
    
    if( ( (x - prevx) * (x - prevx) + (y - prevy) * (y - prevy) + (z - prevz) * (z - prevz) ) > 100) {
     Serial.print("Accelerometer changed x=");
     Serial.print(x);;
     Serial.print(" y=");
     Serial.print(y);
     Serial.print(" z=");
     Serial.println(z);
    } 
    
    if ( ( (x - defaultx) * (x - defaultx) + (y - defaulty) * (y - defaulty) + (z - defaultz) * (z - defaultz) ) > 200) {
     Serial.print("Door opened x=");
     Serial.print(x);
     Serial.print(" y=");
     Serial.print(y);
     Serial.print(" z=");
     Serial.println(z);
    }
    
    prevx = x;
    prevy = y;
    prevz = z;
    
    delay(1000);
}


