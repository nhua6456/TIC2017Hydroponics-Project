#include <connection4g.h>
#include <telstraiot.h>
#include <iotshield.h>
#include <shieldinterface.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <dht11.h>


ShieldInterface shieldif;
IoTShield shield(&shieldif);
Connection4G conn(true,&shieldif);
LiquidCrystal_I2C lcd(0x20,20,4);
dht11 DHT;
#define DHT11_PIN 4

TelstraIoT iotPlatform(&conn,&shield);

const char host[] = "tic2017team024.iot.telstra.com";

char id[8];
char tenant[32];
char username[32];
char password[32];

void setup() {
  Serial.begin(115200);
  delay(500);

  lcd.init();
  Serial.println(F("[START] Starting Send All Measurments Script"));

  lcd.backlight();
  lcd.print("Hello, world!");
  if(!shield.isShieldReady())
  {
     Serial.println("waiting for shield ...");
    shield.waitUntilShieldIsReady();
  } else {
    Serial.println("Shield ready!");
  }
//  while(Serial.available()==0); // Wait for serial character before starting

  Serial.println("Reading credentials...");
  shield.readCredentials(id,tenant,username,password);
  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("Tenant: ");
  Serial.println(tenant); 
  Serial.print("Username: ");
  Serial.println(username);
  Serial.print("Password: ");
  Serial.println(password);
  
  Serial.println("Setting credentials...");
  iotPlatform.setCredentials(id,tenant,username,password,"");
  
  Serial.println("Setting host...");
  iotPlatform.setHost(host,443);
  
  conn.openTCP(host,443);  

}

void loop() {
    delay(1000);
    char lightString[15];
    char tempString[15];
    int moistureInt = 0;
    int humidityInt = 0;

    lcd.clear();
    Serial.println("############################ Preparing to read MEASUREMENTS #############################");
    //Read Light measurement from device
    shield.getLightLevel(lightString);
    Serial.print(F("[    ] Light: "));
    Serial.println(lightString);
    lcd.setCursor(0,0);
    lcd.print("[    ] Light: ");
    lcd.print(lightString);
    
    //Read temperature measurement from device
    shield.getTemperature(tempString);
    Serial.print(F("[    ] Temp: "));
    Serial.println(tempString);
    lcd.setCursor(0,1);
    lcd.print("[    ] Temp: ");
    lcd.print(tempString);

    //Read moisture measurement from sensor
    moistureInt = analogRead(A0);
    Serial.print(F("[    ] Moisture: "));
    Serial.println(moistureInt);
    lcd.setCursor(0,2);
    lcd.print("[    ] Moisture: ");
    lcd.print(moistureInt);

    int chk = DHT.read(DHT11_PIN);    // READ DATA
//  switch (chk){
//    case DHTLIB_OK:  
//                Serial.print("OK,\t"); 
//                break;
//    case DHTLIB_ERROR_CHECKSUM: 
//                Serial.print("Checksum error,\t"); 
//                break;
//    case DHTLIB_ERROR_TIMEOUT: 
//                Serial.print("Time out error,\t"); 
//                break;
//    default: 
//                Serial.print("Unknown error,\t"); 
//                break;
//  }

    //Read humidity measurement from sensor
    humidityInt = DHT.humidity;
    Serial.print(F("[    ] Humidity: "));
    Serial.println(humidityInt);
    lcd.setCursor(0,3);
    lcd.print("[    ] Humidity: ");
    lcd.print(humidityInt);

    Serial.println("############################ Preparing to send MEASUREMENTS #############################");  
    iotPlatform.sendMeasurement("LightMeasurement", "LightMeasurement", "Light level (lux)", lightString, "lux");
    
//    shield.getTemperature(tempString);
    iotPlatform.sendMeasurement("TemperatureMeasurement", "TemperatureMeasurement", "Temperature (degrees Celsius)", tempString, "degrees Celsius");

    iotPlatform.sendMeasurement("MoistureMeasurement","MoistureMeasurement", "Moisture Raw Value", moistureInt, "value");
    
}

