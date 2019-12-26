
// set clock to 1MHz; 1 microsecond = 1 clock cycle
#define F_CPU 1000000L

#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdio.h>
#include <avr/pgmspace.h>


#define DEBUG_ENABLED 1
#define DEBUG(x) if(DEBUG_ENABLED){Serial.print(x);}
#define DEBUG2(x,y) if(DEBUG_ENABLED){Serial.print(x,y);}

RTC_DS1307 rtc;

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#define BAR_XPOS         0
#define BAR_YPOS         0
#define BAR_HEIGHT       16
#define BAR_WIDTH        128

#define MAINPANEL_XPOS   0
#define MAINPANEL_YPOS   16
#define MAINPANEL_HEIGHT 48
#define MAINPANEL_WIDTH  128

#define PREG_WKS         40
#define PREGDUE_Y        2016
#define PREGDUE_M        8
#define PREGDUE_D        18

#define SEC_IN_WK        604800L

#define REFRESH_INTERVAL 3600000

static DateTime due;

static long int due_ut = 0;

static char sbuffer[64] = "";
static char fbuffer[64] = "";
const char fmt_timenow_P[] PROGMEM =   { "Now: %04d/%02d/%02d %02d:%02d:%02d\n" };
const char fmt_wksremain_P[] PROGMEM = { "Weeks remaining: %d (%d%%)\n" };

void setup () {

  Serial.begin(9600);
  if (! rtc.begin()) {
    DEBUG(F("Couldn't find RTC\n"));
    while (1);
  }

  if (! rtc.isrunning()) {
    DEBUG(F("RTC is NOT running!\n"));
  }
  
  DEBUG(F("Initializing the display.\n"));
  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, false);
  display.display();
  delay(200);
  display.clearDisplay();
  
  DEBUG(F("Calcuating due date.\n"));
  due = DateTime(PREGDUE_Y, PREGDUE_M, PREGDUE_D, 0, 0, 0);
  due_ut = due.unixtime();
  
  print_text("baby!");
}

void loop () {
    DateTime now = rtc.now();
    
    strcpy_P(fbuffer, fmt_timenow_P);
    sprintf(sbuffer, fbuffer,
      now.year(), now.month(), now.day(),
      now.hour(), now.minute(), now.second());
    DEBUG(sbuffer);
    
    int wks_rem = int((due_ut - now.unixtime()) / SEC_IN_WK);
    float wks_pct = float(PREG_WKS - wks_rem) / float(PREG_WKS);

    strcpy_P(fbuffer, fmt_wksremain_P);
    sprintf(sbuffer, fbuffer,
      wks_rem, int(wks_pct*100.0));
    DEBUG(sbuffer);
    
    print_progress(wks_pct);
    
    sprintf(sbuffer, "%dwks", wks_rem);
    print_text(sbuffer);
    
    DEBUG(F("\n"));
    delay(REFRESH_INTERVAL);
}

void print_progress(const float percent) {
  int bar_sz = int(percent * BAR_WIDTH);
  display.fillRect(BAR_XPOS, BAR_YPOS, bar_sz, BAR_HEIGHT, WHITE);
  display.display();
}

void print_text(const char* text) {
  display.fillRect(MAINPANEL_XPOS, MAINPANEL_YPOS, MAINPANEL_WIDTH, MAINPANEL_HEIGHT, BLACK);
  display.setCursor(MAINPANEL_XPOS, MAINPANEL_YPOS+15);
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.print(text);
  display.display();
}
