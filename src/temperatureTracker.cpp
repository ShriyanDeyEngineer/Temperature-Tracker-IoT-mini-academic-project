// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

int tempReading = 0;
double celciusReading = 0;
double fahrenheitReading = 0;
double targetTemp = 73.0;


#define COOL 0
#define OFF 1
#define HEAT 2

#define REDALERT 3
bool havePublishedRedAlert = false;

int mode = OFF;

int setModeFromString(String inputString);
int setTargetTempFromString(String inputString);

#define PIXEL_PIN SPI
Adafruit_NeoPixel light = Adafruit_NeoPixel(1, PIXEL_PIN, WS2812);

// setup() runs once, when the device is first turned on
void setup() 
{
  Particle.variable("temperatureReading", fahrenheitReading);
  Particle.variable("cV_targetTemp", targetTemp);

  Particle.function("cF_setMode", setModeFromString);
  Particle.function("cF_setTargetTemp", setTargetTempFromString);

  light.begin();

  //start with default white light color for the LED
  light.setPixelColor(0, light.Color(127, 127, 127)); 
  light.show();
}


// loop() runs over and over again, as quickly as it can execute.
void loop() 
{
  tempReading = analogRead(A1);

  Serial.print("Analog voltage reading: ");
  Serial.println(tempReading);

  celciusReading = (tempReading - 620) * 0.0806; //convert ADC reading to celcius reading
  Serial.print("Celsius reading: ");
  Serial.println(celciusReading);

  fahrenheitReading = 1.8 * celciusReading + 32; //convert celcius reading to fahrenheit reading
  Serial.print("Fahrenheit reading: ");
  Serial.println(fahrenheitReading);
  Serial.println("----------------------------------");

  Particle.publish("TEMPevent", celciusReading);
  delay(3000); //wait three seconds before next reading
}


int setModeFromString(String inputString)
{
  if(inputString == "Cool")
  {
    mode = COOL;
    light.setPixelColor(0, light.Color(0, 0, 127)); //Blue to symbolize cool mode
    light.show();
    return 0;
  }
  else if(inputString == "Off")
  {
    mode = OFF;
    light.setPixelColor(0, light.Color(127, 127, 127)); //White to symbolize off mode
    light.show();
    return 1;
  }
  else if(inputString == "Heat")
  {
    mode = HEAT;
    light.setPixelColor(0, light.Color(127, 254, 0)); //Orange/Yellow to symbolize heat mode
    light.show();
    return 2;
  }
  else if(inputString == "REDALERT" && havePublishedRedAlert == false)
  {
    mode = REDALERT;
    havePublishedRedAlert = true;

    //Publish an event to the Particle Cloud to notify that the system is in red alret mode
    Particle.publish("RED ALERT DETECTED");

    light.setPixelColor(0, light.Color(0, 127, 0)); //Red to symbolize red alert mode
    light.show();
    return 3;
  }
  else
  {
    return -1;
  }
}

int setTargetTempFromString(String inputString)
{
  if(inputString.toFloat() < 50 || inputString.toFloat() > 90)
  {
    targetTemp = 70.0;
    return 0;
  }
  else
  {
    targetTemp = inputString.toFloat();
    return 0;
  }
}
