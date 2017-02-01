// Fibonacci Clock
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License Version 2
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You will find the latest version of this code at the following address:
// https://github.com/pchretien/fibo
//
// This project contains code and libraries provided by Adafruit Industries and can be found on their Github account at:
// https://github.com/adafruit
//
// Credits:
// See the credit.txt file for the list of all the backers of the Kickstarter campaign.
// https://www.kickstarter.com/projects/basbrun/fibonacci-clock-an-open-source-clock-for-nerds-wit/description
//

#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>

#define STRIP_PIN 8
#define HOUR_PIN 3
#define MINUTE_PIN 4
#define BTN_PIN 5
#define SET_PIN 6
#define DEBOUNCE_DELAY 10
#define MAX_BUTTONS_INPUT 20
#define MAX_MODES 3
#define MAX_PALETTES 10
#define TOTAL_PALETTES 10
#define CLOCK_PIXELS 5

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, STRIP_PIN, NEO_RGB + NEO_KHZ800);

byte bits[CLOCK_PIXELS];

uint32_t black = strip.Color(0,0,0);
uint32_t colors[TOTAL_PALETTES][4] = 
  {
    {
      // #1 RGB
      strip.Color(255,255,255),    // off
      strip.Color(255,10,10),  // hours
      strip.Color(10,255,10),  // minutes
      strip.Color(10,10,255) // both;
    }, 
    {
      // #2 Mondrian
      strip.Color(255,255,255),    // off
      strip.Color(255,10,10),  // hours
      strip.Color(248,222,0),  // minutes
      strip.Color(10,10,255) // both;
    }, 
    {
      // #3 Basbrun
      strip.Color(255,255,255),    // off
      strip.Color(80,40,0),  // hours
      strip.Color(20,200,20),  // minutes
      strip.Color(255,100,10) // both;
    },
    {
      // #4 80's
      strip.Color(255,255,255),    // off
      strip.Color(245,100,201),  // hours
      strip.Color(114,247,54),  // minutes
      strip.Color(113,235,219) // both;
    }
    ,
    {
      // #5 Pastel
      strip.Color(255,255,255),    // off
      strip.Color(255,123,123),  // hours
      strip.Color(143,255,112),  // minutes
      strip.Color(120,120,255) // both;
    }
    ,
    {
      // #6 Modern
      strip.Color(255,255,255),    // off
      strip.Color(212,49,45),  // hours
      strip.Color(145,210,49),  // minutes
      strip.Color(141,95,224) // both;
    }
    ,
    {
      // #7 Cold
      strip.Color(255,255,255),    // off
      strip.Color(209,62,200),  // hours
      strip.Color(69,232,224),  // minutes
      strip.Color(80,70,202) // both;
    }
    ,
    {
      // #8 Warm
      strip.Color(255,255,255),    // off
      strip.Color(237,20,20),  // hours
      strip.Color(246,243,54),  // minutes
      strip.Color(255,126,21) // both;
    }
    ,
    {
      //#9 Earth
      strip.Color(255,255,255),    // off
      strip.Color(70,35,0),  // hours
      strip.Color(70,122,10),  // minutes
      strip.Color(200,182,0) // both;
    }
    ,
    {
      // #10 Dark
      strip.Color(255,255,255),    // off
      strip.Color(211,34,34),  // hours
      strip.Color(80,151,78),  // minutes
      strip.Color(16,24,149) // both;
    }
  }; 
  
RTC_DS1307 rtc;

boolean on = true;

byte oldHours = 0;
byte oldMinutes = 0;

int lastButtonValue[MAX_BUTTONS_INPUT];
int currentButtonValue[MAX_BUTTONS_INPUT];

int mode = 0;
int palette = 0;

byte error = 0;
byte oldError = 0;

void setup() 
{
  Serial.begin(9600);
  
  // Initialize the strip and set all pixels to 'off'
  strip.begin();
  strip.show(); 
    
  Wire.begin();
  rtc.begin();

  if (! rtc.isrunning()) 
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  // Make the random() function return unpredictable results
  randomSeed(rtc.now().unixtime());
  
  pinMode(HOUR_PIN, INPUT);
  pinMode(MINUTE_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);
  pinMode(SET_PIN, INPUT);
  
  pinMode(13, OUTPUT);
  
  for(int i=0;i<4;i++)
  {
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
  }
  
  // Make sure the time is always displayed the first 
  // time the clock is powered on.
  oldHours = 99;
}

void loop() 
{
  // 9:25
//  setPixel(0, strip.Color(255,255,255));
//  setPixel(1, strip.Color(255,10,10));
//  setPixel(2, strip.Color(10,255,10));
//  setPixel(3, strip.Color(10,10,255));
//  setPixel(4, strip.Color(255,10,10));
//  strip.show();
//  return;  

  
  // Read buttons
  int set_button = debounce(SET_PIN);
  int hour_button = debounce(HOUR_PIN);
  int minute_button = debounce(MINUTE_PIN);
  int button = debounce(BTN_PIN);
  
  if(set_button && button && hasChanged(BTN_PIN))
  {    
    for(int i=0; i<100; i++)
    {
      if(!debounce(SET_PIN) || !debounce(BTN_PIN))
        break;
    }
    
    if(debounce(SET_PIN) && debounce(BTN_PIN))
    {
      checkErrors();
    }
  }  
  else if( set_button && hour_button && hasChanged(HOUR_PIN))
  {
    DateTime newTime = DateTime(rtc.now().unixtime()+3600);
    rtc.adjust( newTime );
    
    displayCurrentTime();
  }
  else if( set_button && minute_button && hasChanged(MINUTE_PIN))
  {
    DateTime fixTime = rtc.now();
    
    DateTime newTime = DateTime(
          fixTime.year(), 
          fixTime.month(), 
          fixTime.day(), 
          fixTime.hour(), 
          ((fixTime.minute()-fixTime.minute()%5)+5)%60, 
          0);
          
    rtc.adjust( newTime );
          
    displayCurrentTime();
  }
  else if( minute_button && hasChanged(MINUTE_PIN))
  {
    toggleOnOff();
  } 
  else if( hour_button && hasChanged(HOUR_PIN))
  {
    palette = (palette+1)%MAX_PALETTES;
    oldHours = 99;
    oldError = 99;
  }   
  else if( button && hasChanged(BTN_PIN))
  {
    mode = mode + 1;
    
    if(mode >= MAX_MODES)
      mode = 0;
  }

  // Store buttons new values
  resetButtonValues();
  switch(mode)
  {
    case 0:  
      displayCurrentTime();
      break;
      
    case 1:
      oldHours = 99;
      rainbowCycle(20);
      break;
      
    case 2:
      oldHours = 99;
      rainbow(20);
      break;
      
    case 3:
      oldHours = 99;
      // Display error code
      displayErrorCode();
      break;
  }  
}

int debounce(int pin)
{
  int val = digitalRead(pin);
  if( val == lastButtonValue[pin] )
  {
    currentButtonValue[pin] = val;
    return val;
  }
    
  delay(DEBOUNCE_DELAY);
  
  val = digitalRead(pin);
  if( val != lastButtonValue[pin] )
  {
    currentButtonValue[pin] = val;
    return val;
  }
  
  currentButtonValue[pin] = lastButtonValue[pin];
  return lastButtonValue[pin];
}

boolean hasChanged(int pin)
{
  return lastButtonValue[pin] != currentButtonValue[pin];
}

void resetButtonValues()
{
  for(int i=0; i<MAX_BUTTONS_INPUT; i++)
    lastButtonValue[i] = currentButtonValue[i];
}

void displayCurrentTime()
{
  DateTime now = rtc.now();  
  setTime(now.hour()%12, now.minute()); 
}

void setTime(byte hours, byte minutes)
{
  if(oldHours == hours && oldMinutes/5 == minutes/5)
    return;
    
  oldHours = hours;
  oldMinutes = minutes;
  
  for(int i=0; i<CLOCK_PIXELS; i++)
    bits[i] = 0;
    
  setBits(hours, 0x01);
  setBits(minutes/5, 0x02);

  for(int i=0; i<CLOCK_PIXELS; i++)
  {   
    setPixel(i, colors[palette][bits[i]]);
    strip.show();
  }
}

void displayErrorCode()
{
  if(oldError == error)
    return;
    
  oldError = error;
  
  for(int i=0; i<CLOCK_PIXELS; i++)
    bits[i] = 0;
  
  if(error == 0)
  {
    setBits(12, 0x02);
  }
  else
  {
    setBits(error, 0x01);
  }

  for(int i=0; i<CLOCK_PIXELS; i++)
  {   
    setPixel(i, colors[palette][bits[i]]);
  }
  
  strip.show();
}

void setBits(byte value, byte offset)
{
  switch(value)
  {
    case 1:
      switch(random(2))
      {
        case 0:
          bits[0]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          break;
      }
      break;
    case 2:
      switch(random(2))
      {
        case 0:
          bits[2]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[1]|=offset;
          break;
      }
      break;
    case 3:
      switch(random(3))
      {
        case 0:
          bits[3]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[2]|=offset;
          break;
        case 2:
          bits[1]|=offset;
          bits[2]|=offset;
          break;
      }
      break;
    case 4:
      switch(random(3))
      {
        case 0:
          bits[0]|=offset;
          bits[3]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          bits[3]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[2]|=offset;
          break;
      }
      break;
    case 5:
      switch(random(3))
      {
        case 0:
          bits[4]|=offset;
          break;
        case 1:
          bits[2]|=offset;
          bits[3]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[3]|=offset;
          break;
      }
      break;
    case 6:
      switch(random(4))
      {
        case 0:
          bits[0]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          bits[4]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
        case 3:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
      }
      break;
    case 7:
      switch(random(3))
      {
        case 0:
          bits[2]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[4]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
      }
      break;
    case 8:
      switch(random(3))
      {
        case 0:
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[2]|=offset;
          bits[4]|=offset;
          break;
        case 2:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[4]|=offset;
          break;
      }      
      break;
    case 9:
      switch(random(3))
      {
        case 0:
          bits[0]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 3:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
      }      
      break;
    case 10:
      switch(random(2))
      {
        case 0:
          bits[2]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
      }            
      break;
    case 11:
      switch(random(2))
      {
        case 0:
          bits[0]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;      
          break;
        case 1:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          bits[4]|=offset; 
          break;
      }          

      break;
    case 12:
      bits[0]|=offset;
      bits[1]|=offset;
      bits[2]|=offset;
      bits[3]|=offset;
      bits[4]|=offset;        
      
      break;
  }
}

void setPixel(byte pixel, uint32_t color)
{
  if(!on)
    return;
  
  switch(pixel)
  {
    case 0:
      strip.setPixelColor(0, color);
      break;
    case 1:
      strip.setPixelColor(1, color);
      break;
    case 2:
      strip.setPixelColor(2, color);
      break;
    case 3:
      strip.setPixelColor(3, color);
      strip.setPixelColor(4, color);
      break;
    case 4:
      strip.setPixelColor(5, color);
      strip.setPixelColor(6, color);
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      break;
  };
}

void rainbow(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i< CLOCK_PIXELS; i++) 
    {
      setPixel(i, Wheel((i+j) & 255));
    }
    
    strip.show();
    delay(wait);
    
    if(debounce(BTN_PIN) && hasChanged(BTN_PIN))
    {
      mode = (mode + 1)%MAX_MODES;
      resetButtonValues();
      return;
    }
    
    if(debounce(MINUTE_PIN) && hasChanged(MINUTE_PIN))
    {
      toggleOnOff();
      resetButtonValues();
      return;
    }
    
    resetButtonValues();
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256*5; j++) 
  { 
    // 5 cycles of all colors on wheel
    for(i=0; i< CLOCK_PIXELS; i++) 
    {
      setPixel(i, Wheel(((i * 256 / CLOCK_PIXELS) + j) & 255));
    }
    strip.show();
    delay(wait);
    
    if(debounce(BTN_PIN) && hasChanged(BTN_PIN))
    {
      mode = (mode + 1)%MAX_MODES;
      resetButtonValues();
      return;
    }
    
    if(debounce(MINUTE_PIN) && hasChanged(MINUTE_PIN))
    {
      toggleOnOff();
      resetButtonValues();
      return;
    }
    
    resetButtonValues();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
  if(WheelPos < 85) 
  {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) 
  {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else 
  {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void toggleOnOff()
{
  if( on )
  {
    for( int i=0; i<CLOCK_PIXELS; i++)
      setPixel(i, black);
      
    strip.show();
  }
  
  on = !on;
  
  if(on)
  {
    oldHours = 99;
    oldError = 99;
  }
}

void checkErrors()
{
  error = 0;
  oldError = 99;
  mode = 3;
  palette = 0;
  
  // Test sequence
  if (! rtc.isrunning())
  {
    error |= 0x1;
  }
  
  int time1 = rtc.now().unixtime();
  delay(1200);
  int time2 = rtc.now().unixtime();
  if(time1 == time2)
  {
    error |= 0x02;
  }  
}

void printDateTime(DateTime now)
{
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  
  Serial.print(" since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");
  
  // calculate a date which is 7 days and 30 seconds into the future
  DateTime future (now.unixtime() + 7 * 86400L + 30);
  
  Serial.print(" now + 7d + 30s: ");
  Serial.print(future.year(), DEC);
  Serial.print('/');
  Serial.print(future.month(), DEC);
  Serial.print('/');
  Serial.print(future.day(), DEC);
  Serial.print(' ');
  Serial.print(future.hour(), DEC);
  Serial.print(':');
  Serial.print(future.minute(), DEC);
  Serial.print(':');
  Serial.print(future.second(), DEC);
  Serial.println();
  
  Serial.println();
  
  delay(1000);
}


