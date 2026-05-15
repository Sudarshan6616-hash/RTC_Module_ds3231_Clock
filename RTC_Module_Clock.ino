#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int BTN1 = 2;  // Hour/Day
const int BTN2 = 3;  // Minute/Month
const int BTN3 = 4;  // Second/Year
const int BTN4 = 5;  // Cycle modes

enum ClockMode { MODE_DISPLAY, MODE_SET_TIME, MODE_SET_DATE };
ClockMode mode = MODE_DISPLAY;

unsigned long btnTimers[3] = {0,0,0};
const unsigned long firstDelay = 600;
const unsigned long repeatDelay = 200;
bool needsUpdate = true;  // Anti-flicker

// Change detection globals
static DateTime lastNow; 
static ClockMode lastMode = MODE_DISPLAY;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init(); lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("RTC Interface");
  lcd.setCursor (0,1);
  lcd.print("using DS3231");
  delay(1500); 
  lcd.clear();
  
  if (!rtc.begin()) { 
    lcd.print("RTC FAIL"); 
    while(1); 
  }
  
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
}

void loop() {
  DateTime now = rtc.now();
  checkAllButtons(now);
  updateDisplay(now);
  delay(40);
}

void updateDisplay(DateTime now) {
  // Anti-flicker
  if (now == lastNow && mode == lastMode && !needsUpdate) return;  
  lastNow = now;
  lastMode = mode;
  needsUpdate = false;  
  lcd.clear();
  
  if (mode == MODE_DISPLAY) {
    // Labels + fullscreen values
    lcd.setCursor(0,0); lcd.print("TIME ");
    padZero(now.hour()); lcd.print(':');
    padZero(now.minute()); lcd.print(':');
    padZero(now.second());
    
    lcd.setCursor(0,1); lcd.print("DATE ");
    padZero(now.day()); lcd.print('/');
    padZero(now.month()); lcd.print("/20");
    padZero(now.year()%100);
  } 
  else {
    // SET header
    lcd.setCursor(0,0); lcd.print("SET");
    
    if (mode == MODE_SET_TIME) {
      lcd.setCursor(0,1);
      lcd.print("HR:"); padZero(now.hour()); 
      lcd.print(" MN:"); padZero(now.minute()); 
      lcd.print(" SC:"); padZero(now.second());
    } 
    else {  // MODE_SET_DATE
      lcd.setCursor(0,1);
      lcd.print("DD:"); padZero(now.day()); 
      lcd.print("MM:"); padZero(now.month()); 
      lcd.print("Y:20"); padZero(now.year()%100);
    }
  }
}

void checkAllButtons(DateTime now) {
  // Btn4: Cycle modes
  static bool lastToggle = HIGH;
  bool toggleNow = digitalRead(BTN4);
  if (toggleNow == LOW && lastToggle == HIGH) {
    mode = (ClockMode)(((int)mode + 1) % 3);
    Serial.print("MODE: ");
    Serial.println(mode == MODE_DISPLAY ? "DISPLAY" : (mode == MODE_SET_TIME ? "TIME SET" : "DATE SET"));
    needsUpdate = true;  // Force display update
    delay(250);
  }
  lastToggle = toggleNow;
  
  if (mode != MODE_DISPLAY) {
    if (digitalRead(BTN1) == LOW) handleRepeat(BTN1, now, 1); else btnTimers[0] = 0;
    if (digitalRead(BTN2) == LOW) handleRepeat(BTN2, now, 2); else btnTimers[1] = 0;
    if (digitalRead(BTN3) == LOW) handleRepeat(BTN3, now, 3); else btnTimers[2] = 0;
  }
}

void handleRepeat(int btnPin, DateTime now, int field) {
  int index = btnPin - 2;
  unsigned long nowTime = millis();
  
  if (btnTimers[index] == 0 || nowTime - btnTimers[index] > firstDelay) {
    adjustField(now, field);
    btnTimers[index] = nowTime;
  } else if (nowTime - btnTimers[index] > repeatDelay) {
    adjustField(now, field);
    btnTimers[index] = nowTime;
  }
}

void adjustField(DateTime now, int field) {
  int y = now.year(), m = now.month(), d = now.day();
  int h = now.hour(), min = now.minute(), s = now.second();
  
  if (mode == MODE_SET_TIME) {
    if (field == 1) h = (h + 1) % 24;
    if (field == 2) min = (min + 1) % 60;
    if (field == 3) s = (s + 1) % 60;
  } else {
    if (field == 1) d = (d % daysInMonth(y, m)) + 1;
    if (field == 2) m = (m % 12) + 1;
    if (field == 3) y += 1;
  }
  
  rtc.adjust(DateTime(y, m, d, h, min, s));
  needsUpdate = true;  // Force display update
}

int daysInMonth(int year, int month) {
  if (month == 2) return isLeapYear(year) ? 29 : 28;
  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
  return 31;
}

bool isLeapYear(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void padZero(int n) {
  if (n < 10) lcd.print('0');
  lcd.print(n);
}