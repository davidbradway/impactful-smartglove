#include <Adafruit_NeoPixel.h>

#include <VirtualWire.h>
#include <VirtualWire_Config.h>



// These constants won't change:
const int sensorPin = 2;    // pin that the sensor is attached to
const int ledPin = 13;        // pin that the LED is attached to
const int transmitPin = 5;  // pin that the TX is connected to


#define STRIPLIGHT_PIN 6

// variables:
int sensorValue = 0;         // the sensor value
int sensorMin = 1023;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value



int maxImpact;
byte nLightsToLight = 0;


typedef enum _PAD_MODE {
  MODE_POWER=0,
  MODE_SPEED,
  MODE_CONSISTENCY,
  MODE_NONE
} 
PAD_MODE;  


PAD_MODE mode = MODE_POWER;

boolean bLogging = false;
boolean bLastLogging = false;
const boolean bTransmit =false;

int buff[256];
int nLogs=0;



// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, STRIPLIGHT_PIN, NEO_GRB + NEO_KHZ800);


void clearBar()
{
  for(int i=0;i<strip.numPixels();i++)
  {
    strip.setPixelColor(i,strip.Color(0,0,0));
  }
}


void colorBar(uint16_t nLights, uint32_t c) {
  clearBar();
  if(nLights > strip.numPixels()) nLights = strip.numPixels();  // TODO: ?:
  
  for(uint16_t i=0; i<nLights; i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
}


void setup() {
  int curTime;

  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  Serial.println("Begin");

// initialize the strip
  
  strip.begin();
  strip.show();
  

  // turn on LED to signal the start of the calibration period:

  digitalWrite(ledPin, HIGH);

  // calibrate during the first five seconds 
  while (millis() < 5000) {
    curTime = millis();
    
    int nPixels = (curTime/75)%8;
    colorBar(nPixels, strip.Color(64,0,0));
  

    sensorValue = analogRead(sensorPin);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
      Serial.print("New max: ");
      Serial.println(sensorMax);
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }
  /*  Serial.println("Minimum:");
   Serial.println(sensorMin, HEX);
   Serial.println("Maximum:");
   Serial.println(sensorMax, HEX);
   */
  // signal the end of the calibration period
  digitalWrite(ledPin, LOW);

  if(bTransmit==true)
  {
    vw_setup(2000);	 // Bits per sec
    vw_set_tx_pin(transmitPin);
  }

  buff[0] = 0;
  
  clearBar();
  
}


int getSensorVal() {
  // read the sensor:
  int  sv = analogRead(sensorPin);

  // apply the calibration to the sensor reading
  sv = map(sv, sensorMin, sensorMax, 0, 255);

  // in case the sensor value is outside the range seen during calibration
  sv = constrain(sv, 0, 255);
  delay(1);
  return sv;
}


void processNotLogging()
{      
  if(sensorValue >10) // then we should start logging...
  {
    Serial.println("Start log");
    nLogs = 0;
    buff[nLogs++]=sensorValue;
    bLogging = true;
  }
}

void processLogging()
{
  if(sensorValue <10) // then we should stop logging and transmit the data
  {
    bLogging = false;
  }
  else
  {
    buff[nLogs++]=sensorValue;
  }

}


void processLoggingEnded()
{
      //     Serial.println(sensorValue, HEX);
     maxImpact = 0;
    Serial.print("processLoggingEnded: ");
    Serial.println(nLogs);
    for(int i=0;i<nLogs;i++)
    {
      Serial.print(buff[i], HEX);
      Serial.print(":");        
      if(i%8==0) Serial.println("."); 

      if(buff[i]>maxImpact) maxImpact = buff[i];

    }
    

    nLightsToLight = (maxImpact+1)/4;

    Serial.println("Done:");


    if(bTransmit==true) {
      digitalWrite(ledPin, true); // Flash a light to show transmitting
      vw_send((uint8_t *)buff, nLogs);
      vw_wait_tx(); // Wait until the whole message is gone
      digitalWrite(ledPin, false);
    }      




}

void loop() {
  int sv1, sv2, sv3;
  sv1 = getSensorVal();
  sv2 = getSensorVal();
  sv3 = getSensorVal();

  sensorValue=(sv1+sv2+sv3) / 3;
  
  bLastLogging = bLogging;

  if(bLogging == false)
  {
    processNotLogging();
  }
  else
  {
    processLogging();
  }

  if(bLogging !=bLastLogging)
  {
    if(bLastLogging == true && nLogs >= 2)
    {
      processLoggingEnded();
    }
    // else logging has just started....ignore.....or maybe do stuff to see if we're changing modes.
    
  }


  if(maxImpact >0)
  {
    
    colorBar(maxImpact/16+1, strip.Color(0,63,0));
    maxImpact-=2;
  }  



  // fade the LED using the calibrated value:
  if(sensorValue >10) Serial.println(sensorValue);
  analogWrite(ledPin, sensorValue);
}



