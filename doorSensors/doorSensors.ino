int doorSensor1 = 2; //pin # of first sensor
int doorSensor2 = 3; //pin # of second sensor


void setup() {
  // put your setup code here, to run once:
  pinMode(doorSensor1, INPUT_PULLUP);
  pinMode(doorSensor2, INPUT_PULLUP);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(doorSensor1) == LOW && digitalRead(doorSensor2) == LOW)
  {
    Serial.println("Door closed");
  }
  else
  {
    Serial.println("Door open");
  }

  delay(1000);
}
