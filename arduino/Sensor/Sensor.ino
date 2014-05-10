
/*
 * LilyPad tutorial: sensing (sensors)
 *
 * Reads data from a LilyPad light sensor module
 * and then sends that data to the computer
 * so that you can see the sensor values
 */

int ledPin = 13;	// LED is connected to digital pin 13
int sensorPin = 16;	// light sensor is connected to analog pin 0
int sensorValue;	// variable to store the value coming from the sensor

void setup()	 
{	 
         pinMode(ledPin, OUTPUT);	// sets the ledPin to be an output
         Serial.begin(9600);            //initialize the serial port
         digitalWrite(ledPin, HIGH);	// turn the LED on
         Serial.println("setup");	// send that value to the computer
          
}

void loop()	// run over and over again
{	 
          sensorValue = analogRead(sensorPin);	// read the value from the sensor
          Serial.println(sensorValue);	// send that value to the computer
          delay(100);	// delay for 1/10 of a second
}
 
