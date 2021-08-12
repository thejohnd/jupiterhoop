#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 90

enum pattern { PULSE, JULIE2C };

class HoopPatterns : public Adafruit_DotStar
{
  public:
  
  pattern ActivePattern;
  
  unsigned long Interval;   // milliseconds between updates
  unsigned long lastUpdate; // last update of position
  
  uint16_t TotalSteps;  // total number of steps in the pattern
  uint16_t Index;  // current step within the pattern
  
  void (*OnComplete)();  // Callback on completion of pattern
  
  // Constructor - calls base-class constructor to initialize strip
  /*HoopPatterns(uint16_t pixels, uint8_t clock, uint8_t data, void (*callback)())
    : Adafruit_DotStar(pixels, data, clock)
    {
        OnComplete = callback;
    }*/
    
   HoopPatterns(uint16_t pixels)
    : Adafruit_DotStar(pixels)
    {
      //OnComplete = callback;
    }
    
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case 0:
                    PulseUpdate();
                    break;
                case 1:
                    Julie2CUpdate();
                    break;
                default:
                    break;
            }
        }
    }
    
    // Increment the Index and reset at the end
    void Increment()
    {
         Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
    }
    
    void Pulse(uint8_t interval)
    {
        ActivePattern = PULSE;
        Interval = interval;
        TotalSteps = 511;
        Index = 0;
    }
    
    // Update the Rainbow Cycle Pattern
    void PulseUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        if (Index <= 255){
         setBrightness(Index);
        }
        else{
          setBrightness(255-(Index-255));
        }
        show();
        Increment();
    }
    
    void Julie2C(uint8_t interval)
    {
        ActivePattern = JULIE2C;
        Interval = interval;
        TotalSteps = 2;
        Index = 0;
        setBrightness(200);
    }
    
    void Julie2CUpdate()
    {
      if (Index==0)
      {
        for(int i = 0; i<=numPixels(); i++){
          if(i%4==0) {   
            setPixelColor(i,240,20,100);
            setPixelColor((i+1),255,20,20);
            show();  
            i++;  
          }
          else {
            setPixelColor(i,i*5,255,230);
            setPixelColor((i+1),i*5,255,230);
            show();
          }
        }
        Increment();
      }
      else
      {
   for(int i = 0; i<=numPixels(); i++){  
    if(i%4==0) {   
       setPixelColor((i),i*10,255,230);
       setPixelColor((i+1),i*10,255,230);
       show();  
       i++; 
    }
    else {
       setPixelColor(i,240,20,100);
       setPixelColor(i,240,20,100);
       show();
    }
  }Increment();
      }
      //Increment();
    }
    
uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85)
        {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        else if(WheelPos < 170)
        {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        else
        {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }
};    

//real loop n stuff

HoopPatterns hoop(NUMPIXELS);
int BUTTONPIN = 9;
byte mode = 0;
byte modeNum = 2;

void setup()
{
   pinMode(BUTTONPIN, INPUT_PULLUP);
   hoop.begin(); // Initialize pins for output
   hoop.Julie2C(120);
   hoop.Pulse(20);
   hoop.show();  // Turn all LEDs off ASAP
}

// Main loop
void loop()
{
    // Update the rings.
    hoop.Update();
    hoop.show();
    
    if (digitalRead(BUTTONPIN) == LOW) // Button #1 pressed
    {
      mode++;
      if (mode>=modeNum){
        mode = 0;
      }
    switch(mode){
    case 0:
      hoop.Pulse(20);
      hoop.ActivePattern = PULSE;
      break;
     case 1:
       hoop.Julie2C(70);
       hoop.ActivePattern =  JULIE2C;
       break;
     default:
       hoop.Pulse(20);
       hoop.ActivePattern =  PULSE;
       break;
  }
    }
}      
