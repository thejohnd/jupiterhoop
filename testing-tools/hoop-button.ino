#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 90


// Hardware SPI
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS);
int BUTTONPIN = 9;

void setup() {
  pinMode(BUTTONPIN, INPUT_PULLUP);
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
}

//uint32_t color = 0x000004;

void loop() {
  
  strip.show();                     // Refresh strip
  
  for(uint8_t bright = 20; bright<255;){
    for(int i=0; i<=NUMPIXELS; i++){
      strip.setPixelColor(i, Wheel(map(bright+i,20,255+NUMPIXELS,0,255)));
      
      //strip.show();
      
      /*if(bright>=255){
        break;
      }
      else{bright++;}
      
      Serial.println(bright);*/
      Serial.println(bCheck());
     }
    
    strip.setBrightness(bright);
    Serial.println(bCheck());
    strip.show(); 
    delay(20);
    bright++;
  }
  
  for(uint8_t bright = 255; bright >=20;){
    for(int i=0; i<=NUMPIXELS; i++){
      strip.setPixelColor(i, Wheel(map(bright+i,20,255+NUMPIXELS,0,255)));
      /*strip.setBrightness(bright);
      strip.show();
      
      if(bright<20){
        break;
      }
      else{bright--;}
      
      Serial.println(bright); 
      */
      Serial.println(bCheck());
    }
  strip.setBrightness(bright); 
  strip.show();
  Serial.println(bCheck());
  delay(20);
  bright--;
  }

}
   
uint32_t Wheel(byte WheelPos) {
if(WheelPos < 85) {
return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 200);
} else if(WheelPos < 170) {
WheelPos -= 85;
return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
} else {
WheelPos -= 170;
return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
}
}

byte bCheck() {
  return digitalRead(BUTTONPIN);
}
