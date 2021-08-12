#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 90

enum pattern { PULSE, JULIE2C, SCANNER };

class HoopPatterns : public Adafruit_DotStar
{
  public:
  
  pattern ActivePattern;
  
  unsigned long Interval;   // milliseconds between updates
  unsigned long lastUpdate; // last update of position
  
  uint32_t Color1;
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
                case 2:
                    ScannerUpdate();
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
        if ((Index*2) <= TotalSteps){
         setBrightness(map(Index,0,TotalSteps,20,255));
        }
        else{
          setBrightness(map((TotalSteps-Index),0,TotalSteps,20,255));
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
            setPixelColor((i+1),240,20,100);
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
    
// Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
        setBrightness(255);
    }
 
    // Update the Scanner Pattern
    void ScannerUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }
            else // Fading tail
            {
              if(getPixelColor(i)==0x000000){
                 setPixelColor(i, 0, 50, 00);
              }
           else{
              setPixelColor(i, DimColor(getPixelColor(i)));
           }
            }
        }
        show();
        Increment();
        Color1 = Wheel(map(Index,0,TotalSteps,0,255));
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
    
    uint32_t DimColor(uint32_t color)
    {
        // Shift R, G and B components one bit to the right
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }
 
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }
    
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }
 
    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }
 
    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }
};    

//real loop n stuff

HoopPatterns hoop(NUMPIXELS);
int BUTTONPIN = 9;
byte mode = 0; //mode index
byte modeNum = 3; //total # of modes. actual total number of modes, not #modes-1
bool pinState = 1;

void setup()
{
   pinMode(BUTTONPIN, INPUT_PULLUP);
   hoop.begin(); // Initialize pins for output
   //hoop.Julie2C(120); //init patterns
   hoop.Pulse(50);  //""
   hoop.show();  // Turn all LEDs off ASAP
}

// Main loop
void loop()
{
    // Update
    hoop.Update();
    hoop.show();
    
    if (digitalRead(BUTTONPIN) == LOW) // Button #1 pressed
    {
      pinState = 0;
    }
    else if (digitalRead(BUTTONPIN) != pinState)
    {
      pinState = 1;
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
          hoop.Julie2C(120);
          hoop.ActivePattern =  JULIE2C;
          break;
       case 2:
          hoop.Scanner(hoop.Wheel(hoop.Index), 75);
          hoop.ActivePattern = SCANNER;
          break;  
       default:
          hoop.Pulse(50);
          hoop.ActivePattern =  PULSE;
          break;
      }
    }
}      
