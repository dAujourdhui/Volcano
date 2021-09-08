

float voltage = 0;
float voltage1 = 0;
int sensorValue = 0;

void setup() {

  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

}

void loop() {

  getVoltage();

  delay(500);
}


void getVoltage() {

  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);  // read the input on analog pin 0:

  float voltage = sensorValue * 5.0 / 1023.0;  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  voltage = (voltage * 9900 + voltage * 4180) / 4180; // convert to actual battery status
  voltage1 = voltage;

  Serial.println(voltage);
  Serial.println(sensorValue);  // print out the value you read
  Serial.println("_______");
  return voltage;
}

void longBurst() {
  
  getVoltage();
  setPwm();

  analogWrite(heatcoil,pwm1);
  delay(5000);
  
  var = pwm1;
  var = var/2;
  while (var < pwm1) {
      var = var + 1;
      analogWrite(heatCoil, var);
      delay(500);
  }
  delay(5000);
  analogWrite(heatCoil, 0);
}
