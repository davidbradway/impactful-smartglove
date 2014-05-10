/* Knock Sensor

 * ----------------

 *

 * Program using a Piezo element as if it was a knock sensor.

 *

 * We have to basically listen to an analog pin and detect 

 * if the signal goes over a certain threshold. It writes

 * "knock" to the serial port if the Threshold is crossed,

 * and toggles the LED on pin 13.

 *

 * (cleft) 2005 D. Cuartielles for K3

 * edited by Scott Fitzgerald 14 April 2013

 */



int ledPin = 13;

int knockSensor = 2;               

byte val = 0;

int statePin = LOW;

int THRESHOLD = 10;



void setup() {

 pinMode(ledPin, OUTPUT); 

 Serial.begin(9600);
 Serial.println("Started");
  while(millis() <5000)
  {
    delay(10);  
  }
  
  Serial.println("Setup completed");
}

void loop() {

  val = analogRead(knockSensor);     
  
  //Serial.println(val, HEX);

  if (val >= THRESHOLD) {

    statePin = !statePin;

    digitalWrite(ledPin, statePin);

    Serial.println("Knock!");

  }

  delay(100);  // we have to make a delay to avoid overloading the serial port

}

