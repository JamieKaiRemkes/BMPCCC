/*  
 Test the tft.print() viz embedded tft.write() function

 This sketch used font 2, 4, 7
 
 Make sure all the display driver and pin comnenctions are correct by
 editting the User_Setup.h file in the TFT_eSPI library folder.

 Note that yield() or delay(0) must be called in long duration for/while
 loops to stop the ESP8266 watchdog triggering.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */


#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <BlueMagic32.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

#define TFT_GREY 0x5AEB // New colour
#define BUTTON_PIN 37

int buttonState;           // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin

unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers


float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int lastAnalogValue;

void HandleRecPress() {
  int reading = digitalRead(BUTTON_PIN);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    if (reading != buttonState)
    {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH)
      {
        Serial.println("Pressed");
        if (BMDConnection.available())
        {
            BMDControl->toggleRecording();
        }
      }
    }
  }

  lastButtonState = reading;
}

void DrawTimeCode() {
  tft.setCursor(0, 0, 2);
  if (BMDControl->isRecording()) {
    tft.setTextColor(TFT_RED,TFT_BLACK);  tft.setTextSize(1);
  }
  else {
    tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  }
  tft.drawCentreString(String(BMDControl->timecode()), 65, 15, 2);
}

void setup(void) {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_SKYBLUE,TFT_BLACK);  tft.setTextSize(1);
  tft.drawCentreString("Connecting", 65, 15, 2);

  pinMode(BUTTON_PIN, INPUT);
  // attachInterrupt(BUTTON_PIN, HandleRecPress, RISING);

  BMDConnection.begin("BlueMagic32");
  BMDControl = BMDConnection.connect();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_GREEN,TFT_BLACK);  tft.setTextSize(1);
  tft.drawCentreString("Connected", 65, 15, 2);
}

void loop() {
    if (BMDConnection.available())
    {
        DrawTimeCode();
        if (BMDControl->changed())
        {
            // tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_GREY,TFT_BLACK);  tft.setTextSize(1);
            tft.drawCentreString(String(BMDControl->getAperture()), 65, 40, 2);
            tft.drawCentreString(String(BMDControl->getShutter()) + "°", 65, 55, 2);
            tft.drawCentreString(String(BMDControl->getIso()), 65, 70, 2);
            tft.drawCentreString(String(BMDControl->getWhiteBalance()) + "K", 65, 85, 2);
            tft.drawCentreString(String(BMDControl->getTint()), 65, 100, 2);
        }
    }
    HandleRecPress();
}


