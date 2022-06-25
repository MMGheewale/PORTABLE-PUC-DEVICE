#include<SoftwareSerial.h>
#include<ArduinoJson.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int CO2_Sensor = A7;                                        //Sensor pin 
float m = -0.3376;                                          //Slope 
float b = 0.7165;                                           //Y-Intercept 
float R0 = 2.82;                                            //Sensor Resistance in fresh air from previous code

int CO_Sensor = A6;                                         //Sensor pin 
float m1 = -0.6527;                                         //Slope 
float b1 = 1.30;                                            //Y-Intercept 
float R01 = 7.22;                                           //Sensor Resistance

const int buzzer  = 6 , button  = 9;
unsigned long millisBefore;
double AddCO = 0,AddCO2 = 0,Avg_CO,Avg_CO2;

void setup() {
  
  Serial.begin(115200);      
  pinMode(CO2_Sensor, INPUT);
  pinMode(CO_Sensor,INPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
  pinMode(button,INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("   PORTABLE");
  lcd.setCursor(0, 1);
  lcd.print("  PUC DEVICE");
  delay(10000);
  lcd.clear();
 } 

double CO(){
  float sensor_volt1;                               //Define variable for sensor voltage 
  float RS_gas1;                                    //Define variable for sensor resistance  
  float ratio1;                                     //Define variable for ratio
  float sensorValue1 = analogRead(CO_Sensor);       //Read analog values of sensor  
  sensor_volt1 = sensorValue1 * (5.0/1023.0);       //Convert analog values to voltage 
  RS_gas1 = ((5.0*10.0)/sensor_volt1)-10.0;         //Get value of RS in a gas
  ratio1 = RS_gas1/R01;                             // Get ratio RS_gas/RS_air
  double ppm_log1 = (log10(ratio1)-b1)/m1;          //Get ppm value in linear scale according to the the ratio value  
  double ppm1 = pow(10, ppm_log1);                  //Convert ppm value to log scale 
  double CO_in_Vol  = ppm1  / 100;
  return CO_in_Vol;  
}

double CO2(){
  float sensor_volt;                                //Define variable for sensor voltage 
  float RS_gas;                                     //Define variable for sensor resistance  
  float ratio;                                      //Define variable for ratio
  float sensorValue = analogRead(CO2_Sensor);       //Read analog values of sensor  
  sensor_volt = sensorValue*(5.0/1023.0);           //Convert analog values to voltage 
  RS_gas = ((5.0*10.0)/sensor_volt)-10.0;           //Get value of RS in a gas
  ratio = RS_gas/R0;                                // Get ratio RS_gas/RS_air
  double ppm_log = (log10(ratio)-b)/m;              //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log);                    //Convert ppm value to log scale 
  double CO2_in_Vol = ppm / 100;
  return CO2_in_Vol;
}

void loop() { 

  double CO_in_Vol = CO();
  double CO2_in_Vol = CO2();

  lcd.setCursor(0, 0);
  lcd.print("CO  % VOL : ");
  lcd.print(CO_in_Vol);
  lcd.setCursor(0, 1);
  lcd.print("CO2 % VOL : ");
  lcd.print(CO2_in_Vol);
  delay(1000);

  if(digitalRead(button)==0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);          
    lcd.print("CALCULATING...");
    int i = 0;
    millisBefore = millis();
    while(digitalRead(button) == 0)
       {
         if(i >- 1){
        AddCO = AddCO + CO();
        AddCO2 = AddCO2 + CO2();
        i++;
       }
        if (i > -1 && millis() - millisBefore > 5000)
        {
          Avg_CO = AddCO / i;
          Avg_CO2 = AddCO2 / i;

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("CO  % VOL : ");
          lcd.print(Avg_CO);
          lcd.setCursor(0, 1);
          lcd.print("CO2 % VOL : ");
          lcd.print(Avg_CO2);
  
          StaticJsonBuffer<1000>jsonBuffer;
          JsonObject& root = jsonBuffer.createObject();
          root["data1"] = Avg_CO;
          root["data2"] = Avg_CO2;
          root.printTo(Serial);

          if(Avg_CO > 3.5 || Avg_CO2 > 2.0){
          digitalWrite(buzzer,HIGH);
          delay(5000);
          digitalWrite(buzzer,LOW);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("TEST FAILED");
          lcd.print("EXCEEDS LIMIT");
          }
          else{
          digitalWrite(buzzer,LOW);
          delay(5000);
          lcd.clear();
          lcd.setCursor(0, 0);          
          lcd.print("TEST PASSED");
          }  
          i = -1;
          AddCO = 0;
          AddCO2 = 0;
        }
      }
    }
 }
