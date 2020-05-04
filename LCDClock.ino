#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int second = 56;
int minute = 59;
int hour = 11;
int day = 19;
int year = 2020;
bool am = false;
char dowArr[8][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"}; //day of week array
int dowNum = 5; // day of week number
char monthArr[13][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; // month of year array
int monthNum = 1;


void setup() {
  
  Serial.begin(9600);
  // Set up interrupt timer
  cli(); 
  TCCR1A = 0;
  TCCR1B = (1<<CS12)|(1<<CS10); // Prescaler of 1024
  TCNT1 = 0xFFFF - 15626; // 1 second 
  TIMSK1 = 1<<TOIE1;
  sei(); 
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("      :  .     "); 
  Serial.println("----- TIME AND DATE CONFIGURATION ------");
  Serial.println("Press and enter any key to start");  
 
}

// Interupt Service Routine - precisely update screen every second
ISR(TIMER1_OVF_vect){
  updateSecond();
  updateMinute();
  updateHour();
  TCNT1 = 0xFFFF - 15626;
  
}

// Main Loop
void loop() {
  readUserInput();
  updateScreen();
}

char serialData;

void readUserInput(){
  if(Serial.available()){
    int input_month, input_day, input_year, input_dow, input_hour, input_minute, input_second;
    char input_am_pm;
    Serial.println("Enter Month [1...12]");  
    while(1){
      while (Serial.available() == 0){} 
       input_month = Serial.parseInt() - 1; 
       if (input_month > 11 || input_month < 0 ){
        Serial.println("Please choose a month between 1 and 12 . . .");
       }
       else{
        Serial.print("Month: ");
        Serial.println(monthArr[input_month]); 
        break;
       }  
    }
    Serial.print("Enter the day of ");
    Serial.println(monthArr[input_month]);
    if (input_month == 0 || input_month == 2 || input_month == 4 || input_month == 6 || input_month == 7 || input_month == 11) { //input month has 31 days
      while(1){
        while (Serial.available() == 0){} 
         input_day = Serial.parseInt(); 
         if (input_day > 31 || input_day <= 0 ){
          Serial.println("Please choose a day between 1 and 31 . . .");
         }
         else{
          Serial.print("Day: ");
          Serial.println(input_day); 
          break;
         }  
      }
    }
    else if (input_month == 3 || input_month == 5 || input_month == 8 || input_month == 9 || input_month == 10) { // input month has 30 days
      while(1){
        while (Serial.available() == 0){} 
         input_day = Serial.parseInt(); 
         if (input_day > 30 || input_day <= 0 ){
          Serial.println("Please choose a day between 1 and 30 . . .");
         }
         else{
          Serial.print("Day: ");
          Serial.println(input_day); 
          break;
         }  
      }
    }
    else { // input month is Feb
      while(1){
        while (Serial.available() == 0){} 
         input_day = Serial.parseInt(); 
         if (input_day > 28 || input_day <= 0 ){
          Serial.println("Please choose a day between 1 and 28 . . .");
         }
         else{
          Serial.print("Day: ");
          Serial.println(input_day); 
          break;
         }  
      }
    }
    Serial.println("Enter a year [999...9999]");
    while(1){
        while (Serial.available() == 0){} 
         input_year = Serial.parseInt(); 
         if (input_year > 9999 || input_year <= 999 ){
          Serial.println("Please choose a year between 1 and 9999 . . .");
         }
         else{
          Serial.print("Year: ");
          Serial.println(input_year); 
          break;
         }  
      }

    Serial.println("Enter day of the week [1...7] --> [Mon...Sun]");
    while(1){
      while (Serial.available() == 0){} 
       input_dow = Serial.parseInt() - 1; 
       if (input_dow > 6 || input_dow < 0 ){
        Serial.println("Please choose a day of the week between 1 and 7 . . .");
       }
       else{
        Serial.print("Day Of Week: ");
        Serial.println(dowArr[input_dow]); 
        break;
       }  
    }

    Serial.println("Enter hour [1...12]");
    while(1){
      while (Serial.available() == 0){} 
       input_hour = Serial.parseInt(); 
       if (input_hour > 12 || input_hour < 1 ){
        Serial.println("Please choose an hour between 1 and 12 . . .");
       }
       else{
        Serial.print("Hour: ");
        Serial.println(input_hour); 
        break;
       }  
    }

    Serial.println("Enter minute [0...59]");
    while(1){
      while (Serial.available() == 0){} 
       input_minute = Serial.parseInt(); 
       if (input_minute > 59 || input_minute < 0 ){
        Serial.println("Please choose a minute between 0 and 59 . . .");
       }
       else{
        Serial.print("Minute: ");
        Serial.println(input_minute); 
        break;
       }  
    }

    Serial.println("Enter second [0...59]");
    while(1){
      while (Serial.available() == 0){} 
       input_second = Serial.parseInt(); 
       if (input_second > 59 || input_second < 0 ){
        Serial.println("Please choose a second between 0 and 59 . . .");
       }
       else{
        Serial.print("Second: ");
        Serial.println(input_second); 
        break;
       }  
    }

    Serial.println("Enter 'a' for AM or 'p' for PM");
    while(1){
      while (Serial.available() == 0){} 
       input_am_pm = Serial.read(); 
       if (input_am_pm == 'a' ){
        am = true;
        Serial.print("Time of Day: ");
        Serial.println("AM");
        break;
       }
       else if (input_am_pm == 'p' ){
        am = false;
        Serial.print("Time of Day: ");
        Serial.println("PM");
        break;
       }
       else{
        Serial.println("Please type in 'a' for AM or 'p' for PM. . .");
       }  
    }
    // Update global variables based on user input
    monthNum = input_month;
    day = input_day;
    year = input_year;
    dowNum = input_dow;
    hour = input_hour;
    minute = input_minute;
    second = input_second;
    Serial.end(); // reset the user input
    Serial.begin(9600);
    Serial.println("DATE AND TIME UPDATED");
    Serial.println("Enter Month [1...12]"); 
  }
}

void updateSecond(){
  second++;
  if (second >= 60){
    second = 0;
    minute++;
  }
}

void updateMinute(){
  if (minute >= 60){
    minute = 0;
    hour++;
    if(hour == 12 && am){ // noon
      am = false;
    }
    else if(hour == 12 && !am){ // midnight - new day
      dowNum++;
      am = true;  
      if(!strcmp(monthArr[monthNum],"Jan") || !strcmp(monthArr[monthNum],"Mar") || !strcmp(monthArr[monthNum],"May") || !strcmp(monthArr[monthNum],"Jul") || !strcmp(monthArr[monthNum],"Aug") || !strcmp(monthArr[monthNum],"Dec")){
        if(day == 31){
          if(!strcmp(monthArr[monthNum],"Dec")){
            year++;
            monthNum = -1; // reset month counter
          }
          monthNum++;
          day = 1;
        }
        else{
          day++; 
        }
      }
      else if(!strcmp(monthArr[monthNum],"Apr") || !strcmp(monthArr[monthNum],"Jun") || !strcmp(monthArr[monthNum],"Sep") || !strcmp(monthArr[monthNum],"Oct")){
        if(day == 30){
          monthNum++;
          day = 1;
        }
        else{
          day++;
        }
      }
      else if(!strcmp(monthArr[monthNum],"Feb")){ //does not consider leap years
        if(day == 28){
          monthNum++;
          day = 1;
        }
        else{
          day++;
        }
        
      }
      if(dowNum == 7){ // reset day of week
        dowNum = 0;
      } 
    }
    
  }
}

void updateHour(){
  
  if (hour > 12){
    hour = 1;
  }
    
  
}


void updateScreen(){
//  update date
  lcd.setCursor(0,0);
  lcd.print(monthArr[monthNum]);

  char new_day[3];
  char spaceDay[] = " ";
  itoa(day, new_day, 10);
  if(strlen(new_day) < 2){
    strcat(spaceDay, new_day);
    lcd.setCursor(4,0);
    lcd.print(spaceDay);
  }
  else{
    lcd.setCursor(4,0);
    lcd.print(new_day);
  } 

  lcd.setCursor(9,0);
  lcd.print(year);

  lcd.setCursor(0,1);
  lcd.print(dowArr[dowNum]);
 

//  update seconds
  char new_second[3];
  itoa(second, new_second, 10);
  char zeroSec[] = "0";

  itoa(second, new_second, 10);
  if(strlen(new_second) < 2){
    strcat(zeroSec, new_second);
    lcd.setCursor(10,1);
    lcd.print(zeroSec);
  }
  else{
    lcd.setCursor(10,1);
    lcd.print(new_second);
  }

//  update minutes
  char new_minute[3];
  char zeroMin[] = "0";
  itoa(minute, new_minute, 10);
  if(strlen(new_minute) < 2){
    strcat(zeroMin, new_minute);
    lcd.setCursor(7,1);
    lcd.print(zeroMin);
  }
  else{
    lcd.setCursor(7,1);
    lcd.print(new_minute);
  }
  
//  update hour
  char new_hour[3];
  char spaceHr[] = " ";
  itoa(hour, new_hour, 10);
  if(hour < 10){
    strcat(spaceHr, new_hour);
    lcd.setCursor(4,1);
    lcd.print(spaceHr);
  }
  else{
    lcd.setCursor(4,1);
    lcd.print(new_hour);
  } 

//  update AM/PM
  lcd.setCursor(13,1);
  if(am){
    lcd.print("AM");
  }
  else{
    lcd.print("PM");
  }
}
