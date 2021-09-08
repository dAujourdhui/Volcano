/* ------- PUMP PUMP PUMP ------- */

#include <Wire.h>

#define Addr (0xF6>>1)
#define I2C_DEVICEID  0x00
#define I2C_POWERMODE 0x01
#define I2C_FREQUENCY 0x02
#define I2C_SHAPE 0x03
#define I2C_BOOST 0x04
#define I2C_PVOLTAGE  0x06
#define I2C_P3VOLTAGE 0x06
#define I2C_P2VOLTAGE 0x07
#define I2C_P1VOLTAGE 0x08
#define I2C_P4VOLTAGE 0x09
#define I2C_UPDATEVOLTAGE 0x0A
#define I2C_AUDIO       0x05

//PINS
//Wire Library uses some pins. On Nano, these are A4 and A5
#define TASTER          3 //currrently unused
#define HEATSWITCH      4
#define HEATLED         7
#define TEMPSENSOR      A0 //currently voltage divider reader
#define LED             13
#define PUMPLED         5
#define PUMPSWITCH      A1

//CONST
const int SWITCH_OFF_PUMP = 0;
const int LIQUID_WEAK = 1;
const int LIQUID_MEDIUM = 2;
const int LIQUID_STRONG = 3;

/* ------ HHEEATCOOOOIL ------ */

int heatCoil = 3;

float voltage = 0;
float voltage1 = 0;
int sensorValue = 0;

const int riseHeatTime = 500;    //time it takes the coil to heat up (Milliseconds)
const int pauseHeat = 10000 ;     // (Milliseconds)

const int heatingTime = 8000;

int pwm1 = 0;     // heats up the coil hot (1-255 PWM)var = 0;
int pwm2 = 0;    // keeps the coil hot (1-255 PWM)var = 0;


int var = 0;


void setup() {
  // put your setup code here, to run once:
  // initialize the digital pin as an output.
  Wire.begin();
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(LED, OUTPUT);
  pinMode (TASTER, INPUT_PULLUP);
  pinMode (HEATSWITCH, OUTPUT);
  pinMode (HEATLED, OUTPUT);
  pinMode (PUMPLED, OUTPUT);
  Wire.beginTransmission(Addr);
  Wire.write(I2C_POWERMODE);                    // write address = 0x01
  Wire.write(0x00);                             // Adress 0x01 = 0x01 (enable)
  Wire.write(0x40);                             // Adress 0x02 = 0x40 (100Hz)
  Wire.write(0x00);                             // Adress 0x03 = 0x00 (sine wave)
  Wire.write(0x00);                             // Adress 0x04 = 0x00 (800KHz)
  Wire.write(0x00);                             // Adress 0x05 = 0x00 (audio off)
  Wire.write(0x03);                             // Adress 0x06 = 0x00 (EL1)
  Wire.write(0x03);                             // Adress 0x07 = 0x00 (EL2)
  Wire.write(0x03);                             // Adress 0x08 = 0x00 (EL3)
  Wire.write(0x03);                             // Adress 0x09 = 0x00 (EL4)
  Wire.write(0x01);                             // Adress 0x0A = 0x00 (update)
  Wire.endTransmission();

  pinMode(heatCoil, OUTPUT);


}

void loop() {
  // WHAT'S HAPPENING :)
  
  analogWrite(heatCoil, 0);
  engagePumpLow();              // engagePumpLow entspicht einem tropfen liquid
  delay(5000);                 // voltage reading fix because first measurment is wrong, voltage drop or something
  
  otherBurst();
  otherBurst();
  
  burst();                    //tropfen liquid
  delay(60000);

  engagePumpLow();            //tropfen liquid
  shortStrongBurst();
  delay(40000);

  burst();                    //tropfen liquid
  delay(10000);

  longBurst();          // combi burst() und longBurst() mit 10000 ms dazwischen ergibt schöner rauch, kurz aber intensiv :))
  delay(20000);

  burst();                    //tropfen liquid
  otherBurst();
  delay(15000);
  engagePumpLow();           //tropfen liquid
  engagePumpLow();           //tropfen liquid
  burst();                   //tropfen liquid
  otherBurst();
  
  delay(3000);
  engagePumpLow();            //tropfen liquid
  otherBurst();
  delay(70000);

 
  otherBurst();
  otherBurst();
  
  burst();                    //tropfen liquid
  delay(60000);

  engagePumpLow();            //tropfen liquid
  shortStrongBurst();
  delay(40000);

  burst();                    //tropfen liquid
  delay(10000);

  longBurst();         // combi burst() und longBurst() mit 10000 ms dazwischen ergibt schöner rauch, kurz aber intensiv :))
  delay(20000);

  burst();                    //tropfen liquid
  otherBurst();
  delay(15000);
  engagePumpLow();            //tropfen liquid
  engagePumpLow();            //tropfen liquid
  burst();                    //tropfen liquid
  otherBurst();
  engagePumpLow();
  burst();                    //tropfen liquid
  otherBurst();
  delay(3000);
  engagePumpLow;              //tropfen liquid
  otherBurst();
  delay(70000);

  otherBurst();           //drying cicle
  delay(15000);
  otherBurst();
  delay(5000);
  otherBurst();
  delay(20000);
  otherBurst();
  delay(7000);
  otherBurst();
  delay(90000);          // cooling time before start from the top

  engagePumpLow();            //tropfen liquid
  engagePumpLow();            //tropfen liquid
  otherBurst();
  otherBurst();
  
  burst();                    //tropfen liquid
  delay(60000);

  engagePumpLow();            //tropfen liquid
  shortStrongBurst();
  delay(40000);

  burst();                    //tropfen liquid
  delay(10000);

  longBurst();          // combi burst() und longBurst() mit 10000 ms dazwischen ergibt schöner rauch, kurz aber intensiv :))
  delay(20000);

  burst();                    //tropfen liquid
  otherBurst();
  delay(15000);
  engagePumpLow();           //tropfen liquid
  engagePumpLow();           //tropfen liquid
  burst();                   //tropfen liquid
  otherBurst();
  
  delay(3000);
  engagePumpLow();            //tropfen liquid
  otherBurst();
  delay(90000);
}


/*_____________________________________ VOLTAGE AND PWM STUFF _______________________________________*/
/*___________________________________________________________________________________________________*/
/*___________________________________________________________________________________________________*/

void getVoltage() {

  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);  // read the input on analog pin 0:

  float voltage = sensorValue * (4.89 / 1023.0);  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  voltage = (voltage * 10000 + voltage * 4220) / 4220; // convert to actual battery status
  voltage1 = voltage;

  Serial.println(voltage);
  Serial.println(sensorValue);  // print out the value you read
  return voltage;
}

/*______________________________________________*/

void setPwm() {

  if (voltage < 16.8 && voltage1 > 16.4) {
    pwm1 = 9;
    pwm2 = 5;
  }

  else if (voltage < 16.4 && voltage1 > 16.0) {
    pwm1 = 10;
    pwm2 = 6;
  }

  else if (voltage < 16.0 && voltage1 > 15.0) {
    pwm1 = 11;
    pwm2 = 7;
  }

  else if (voltage < 15.0 && voltage1 > 13.8) {
    pwm1 = 12;
    pwm2 = 8;
  }

  else if (voltage < 13.8 && voltage1 > 13.0) {
    pwm1 = 13;
    pwm2 = 10;
  }

  else if (voltage < 13.0 && voltage1 > 12.6) {
    pwm1 = 14;
    pwm2 = 10;
  }

  else if (voltage < 12.6 && voltage1 > 12.3) {
    pwm1 = 15;
    pwm2 = 9;
  }

  else if (voltage < 12.3 && voltage1 > 12.0) {
    pwm1 = 16;
    pwm2 = 10;
  }

  else if (voltage < 12.0 && voltage1 > 11.7) {
    pwm1 = 17;
    pwm2 = 11;
  }
  else if (voltage < 11.7 && voltage1 > 11.5) {
    pwm1 = 18;
    pwm2 = 12;
  }
  else if (voltage < 11.5 && voltage1 > 11.1) {
    pwm1 = 19;
    pwm2 = 13;
  }
  else if (voltage < 11.1 && voltage1 > 10.9) {
    pwm1 = 20;
    pwm2 = 14;
  }
  else if (voltage < 10.9 && voltage1 > 10.7) {
    pwm1 = 21;
    pwm2 = 15;
  }
  else if (voltage < 10.7 && voltage1 > 10.3) {
    pwm1 = 22;
    pwm2 = 16;
  }
  else if (voltage < 10.3 && voltage1 > 10.0) {
    pwm1 = 23;
    pwm2 = 17;
  }
  else if (voltage < 9.9 && voltage1 > 9.5) {
    pwm1 = 25;
    pwm2 = 19;
  }
  else if (voltage < 9.5 && voltage1 > 9.1) {
    pwm1 = 29;
    pwm2 = 23;
  }
  else if (voltage < 9.1 && voltage1 > 8.7) {
    pwm1 = 31;
    pwm2 = 25;
  }
  else if (voltage < 8.7 && voltage1 > 8.5) {
    pwm1 = 35;
    pwm2 = 29;
  }
  else {
    digitalWrite(heatCoil, 0);
  }
  Serial.println(pwm1);
  Serial.println(pwm2);
}


/*________________________________________ BURST STUFF ______________________________________________*/
/*___________________________________________________________________________________________________*/
/*___________________________________________________________________________________________________*/


void burst() {

  getVoltage();
  setPwm();

  analogWrite(heatCoil, pwm1);

  engagePumpLow();

  var = pwm1;

  while (var > pwm2) {
    var = var - 1;
    analogWrite(heatCoil, var);
    delay(125);
  }
  delay(5000);
  analogWrite(heatCoil, 0);

  delay(pauseHeat);
}

/*______________________________________________*/


void otherBurst() {

  getVoltage();
  setPwm();

  analogWrite(heatCoil, pwm1);
  delay(6000);

  var = pwm1;

  while (var > pwm2) {
    var = var - 2;
    analogWrite(heatCoil, var);
    delay(4000);
  }

  analogWrite(heatCoil, 0);

  delay(pauseHeat);
}

/*________________________________________________*/

void longBurst() {

  getVoltage();
  setPwm();

  analogWrite(heatCoil, pwm1);
  delay(5000);

  var = pwm1;
  var = var / 2;
  while (var < pwm1) {
    var = var + 1;
    analogWrite(heatCoil, var);
    delay(500);
  }
  delay(5000);
  analogWrite(heatCoil, 0);
}

/*________________________________________________*/

void shortStrongBurst() {

  getVoltage();
  setPwm();

  var = pwm1;

  var = var + var / 2;

  analogWrite(heatCoil, var);

  delay(4000);

  analogWrite(heatCoil, 0);
}


/*________________________________________ PUMP STUFF  ______________________________________________*/
/*___________________________________________________________________________________________________*/
/*___________________________________________________________________________________________________*/

void engagePumpLow() {
  activatePump(0x10, 0x00);
  delay(3000);
  deactivatePump();
}

/*______________________________________________*/

void engagePumpMedium() {
  activatePump(0x10, 0x00);
  delay(7000);
  deactivatePump();
}

/*______________________________________________*/

void engagePump() {
  activatePump(0x09, 0x00);
  delay(5000);
  deactivatePump();
}

/*______________________________________________*/

void pumpLow() {
  activatePump(0x04, 0x00);
}

void pumpMedium() {
  activatePump(0x07, 0x00);
}

void pumpStrong() {
  activatePump(0x10, 0x00);
}

/*_______________________________________________*/


void activatePump(uint8_t strength, uint8_t frequency) {
  digitalWrite(LED, HIGH);
  digitalWrite(PUMPLED, HIGH);
  //Switch on for a bit, then off
  Wire.beginTransmission(Addr);
  Wire.write(I2C_POWERMODE);                     // write address = 0x01
  Wire.write(0x01);                             // Adress 0x01 = 0x01 (enable)
  Wire.write(frequency);                        // Adress 0x02 = 0x40 (100Hz)
  Wire.write(0x00);                             // Adress 0x03 = 0x00 (sine wave)
  Wire.write(0x00);                             // Adress 0x04 = 0x00 (800KHz)
  Wire.write(0x00);                             // Adress 0x05 = 0x00 (audio off)
  Wire.write(strength);                         // Adress 0x06 = 0x00 (EL1)
  Wire.write(strength);                         // Adress 0x07 = 0x00 (EL2)
  Wire.write(strength);                         // Adress 0x08 = 0x00 (EL3)
  Wire.write(strength);                         // Adress 0x09 = 0x00 (EL4)
  Wire.write(0x01);                             // Adress 0x0A = 0x00 (update)
  Wire.endTransmission();
}

void deactivatePump() {
  Wire.beginTransmission(Addr);
  Wire.write(I2C_POWERMODE);   // start adress
  Wire.write(0x00);            // disable
  Wire.endTransmission();
  digitalWrite(LED, LOW);
  digitalWrite(PUMPLED, LOW);
}
