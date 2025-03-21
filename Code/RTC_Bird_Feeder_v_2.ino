         /////////////////////////////////////////////  
        //         RTC Bird Feeder v2.0            //
       //        for Poultry Conditioning         //
      //             ---------------             //
     //              (Arduino Nano)             //           
    //             by Kutluhan Aktar           // 
   //                                         //
  /////////////////////////////////////////////

// Feed the poultry in quotidian routine to condition them to improve egg production and hatching process.
// 
// For more information:
// https://www.theamplituhedron.com/projects/RTC-Bird-Feeder-v2-for-Poultry-Conditioning
//
//
// Connections
// Arduino Nano :           
//                                20x4 LCD Screen
// D7 --------------------------- rs
// D6 --------------------------- en
// D5 --------------------------- D4
// D4 --------------------------- D5
// D3 --------------------------- D6
// D2 --------------------------- D7
//                                Servo SG-90 (Joint_1)
// D10 -------------------------- 
//                                Servo SG-90 (Joint_2)
// D9  --------------------------
//                                DS3231 RTC Module
// SDA(or A4) ------------------- SDA  
// SCL(or A5) ------------------- SCL
//                                Buzzer
// D12 -------------------------- +  
//                                5mm Green LED
// D11 --------------------------   
//                                Right Button
// A3 --------------------------- 
//                                OK Button
// A2 --------------------------- 
//                                Left Button
// A1 --------------------------- 
//                                Exit Button
// A0 --------------------------- 


// Include the required libraries.
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// Define SDA (A4) and SCL (A5) pins.
DS3231  rtc(SDA, SCL);

// Define the LCD screen pins(rs, en , D4, D5, D6, D7).
LiquidCrystal lcd(7,6,5,4,3,2);

// Define the servo motors.
Servo Joint_1;
Servo Joint_2;

// Create LCD characters:
byte clocK[8] = {
0b01010,
0b01010,
0b11011,
0b10001,
0b10101,
0b10101,
0b10001,
0b11111,
};

byte X[8] = {
0b00000,
0b10001,
0b01010,
0b00100,
0b01010,
0b10001,
0b00000,
0b00000,
};

byte smile[8] = {
0b00000,
0b01010,
0b00000,
0b00100,
0b10001,
0b10001,
0b10001,
0b11111,  
};

byte celsius[8] = {
0b01111,
0b01001,
0b01001,
0b01111,
0b00000,
0b00000,
0b00000,
0b00000,  
};

// Define button pins.
#define Right_B A3
#define OK_B A2
#define Left_B A1
#define Exit_B A0

int Right, OK, Left, Exit;

// Define Buzzer and LED pins.
#define Buzzer 12
#define LED 11

// Define menu options and variables to adjust the feeding time.
volatile boolean Set_Feed_Time, Get_Remaining_Time, Status_Check;
volatile boolean Activated = false;
int feed_Hour, feed_Min;

// Define a time variable to get data from the DS3231 accurately.
Time t;

void setup(){
  // Initiate the screen.
  lcd.begin(20, 4);
  
  // Initiate the RTC module.
  rtc.begin();

  // Attach servo motors to PWM pins.
  Joint_1.attach(10);
  Joint_2.attach(9);

  Joint_1.write(0);
  Joint_2.write(0);

  pinMode(Right_B, INPUT);
  pinMode(OK_B, INPUT);
  pinMode(Left_B, INPUT);
  pinMode(Exit_B, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
} 

void loop(){
  // Create LCD characters.
  lcd.createChar(1, clocK);
  lcd.createChar(2, X);
  lcd.createChar(3, smile);
  lcd.createChar(4, celsius);

  readButtons();

  get_time_rtc();

  home_screen();

  set_feeding_time(); // OK Button on the home screen.

  get_remaining_time(); // Left Button on the home screen.

  status_check(); // Right Button on the home screen.

  Feeding_Timer();
  
}

void readButtons(){
  Right = digitalRead(Left_B);
  OK = digitalRead(OK_B);
  Left = digitalRead(Right_B);
  Exit = digitalRead(Exit_B);
}

void get_time_rtc(){
  //  Uncomment the following lines to set the date and time manually.
  /*
  rtc.setDOW(THURSDAY);     // Set Day-of-Week to Thursday
  rtc.setTime(12, 00, 00);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(1, 10, 2020);   // Set the date to October 1st, 2020
  */
  // Get the current hour and minute from DS3231. // t.hour; and t.min;
  t = rtc.getTime();
}

void home_screen(){
  lcd.setCursor(0, 0);
  lcd.print("Bird Feeder");
  // Print Date
  lcd.setCursor(12, 0);
  lcd.print(rtc.getDateStr(FORMAT_SHORT));
  // Print Time
  lcd.setCursor(12, 1);
  lcd.print(rtc.getTimeStr());
  // Write Temperature
  lcd.setCursor(0, 3);
  lcd.print(rtc.getTemp());
  lcd.write(4);
  lcd.print("C");
    // Write Dow
  lcd.setCursor(8, 3);
  lcd.print(rtc.getDOWStr(FORMAT_SHORT));
}

void set_feeding_time(){
  if(OK == HIGH){
    Set_Feed_Time = true;
    Activated = true;
    lcd.clear();
    // If selected, turn the home screen to the feeding time setting screen.
    while(Set_Feed_Time == true){
      // Adjust the feeding time by either increasing or decreasing variables - feed_Hour and feed_Min.
      readButtons();
      lcd.setCursor(0, 0);
      lcd.print("Set Feeding Time:");
      lcd.setCursor(0, 1);
      lcd.print("Time:  ");
      lcd.print(feed_Hour);
      lcd.print(" : ");
      lcd.print(feed_Min);
      // Change feed_Hour and feed_Min by pressing Right (min+) or Left (hr+) buttons.
      if(Right == HIGH){
        feed_Min++;
        delay(300);
          if(feed_Min > 59){
            // Adjust the minute between 0 and 59.
            feed_Min = 0;
            lcd.clear();
          }
      }
      if(Left == HIGH){
        feed_Hour++;
        delay(300);
        if(feed_Hour > 23){
          // Adjust the hour between 0 and 23 in 24hr format.
          feed_Hour = 0;
          lcd.clear();
        }
      }
      // If the Exit button is pressed, return to the home screen.
      if(Exit == HIGH){
        Set_Feed_Time = false;
        lcd.clear();
      }
    }
  }
}


void get_remaining_time(){
  if(Left == HIGH){
    Get_Remaining_Time = true;
    lcd.clear();
    // If selected, turn the home screen to the remaining time screen.
    while(Get_Remaining_Time == true){
      readButtons();
      // Get the remaining time until the given feeding time.
      lcd.setCursor(0, 0);
      lcd.print("Remaining Time to ");
      lcd.write(1);
      lcd.setCursor(0, 1);
      if(Activated == false){
        lcd.print("Not Activated - ");
        lcd.write(2);
      }else if(Activated == true){
        if(t.hour > feed_Hour){
          lcd.print(24 - (t.hour - feed_Hour));
        }else if(t.hour < feed_Hour){
          lcd.print(abs(t.hour - feed_Hour));
        }else if(t.hour == feed_Hour){
          if(t.min <= feed_Min) lcd.print("0");
          if(t.min > feed_Min) lcd.print("23");
        }
        lcd.print(" hours ");
        if(t.min > feed_Min){
          lcd.print(60 - (t.min - feed_Min));
        }else if(t.min <= feed_Min){
          lcd.print(abs(t.min - feed_Min));
        }
        lcd.print(" minutes ");
      }
      // If the Exit button is pressed, return to the home screen. 
      if(Exit == HIGH){
         Get_Remaining_Time = false;
         lcd.clear();
      } 
    }
  }
}

void status_check(){
  if(Right == HIGH){
   Status_Check = true;
   lcd.clear();
   // If selected, turn the home screen to the status check screen.
   while(Status_Check == true){
     readButtons();
     // Check whether the feeding mechanism is activated or not.
     lcd.setCursor(0, 0);
     lcd.print("Status Check :");
     lcd.setCursor(0, 1);
     if(Activated == false){
       lcd.print("Not Activated - ");
       lcd.write(2);
       }else if(Activated == true){
         lcd.print("Running - ");
         lcd.write(3);
         digitalWrite(LED, HIGH);
       }
     // If the Exit button is pressed, turn to the home screen.
     if(Exit == HIGH){
       Status_Check = false;
       digitalWrite(LED, LOW);
       lcd.clear();
     } 
    }
  }
}

void Feeding_Timer(){
  // Check whether the feeding time is set or not.
  if(Activated == true){
    if(t.hour == feed_Hour && t.min == feed_Min){
      // Activate the feeding mechanism and warnings:
      digitalWrite(LED, HIGH);
      tone(Buzzer, 500);
      Joint_2.write(180);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Feeding Time!!!");
      delay(10 * 1000);
      // Wait 10 seconds to turn off the feeding mechanism and warnings...
      digitalWrite(LED, LOW);
      noTone(Buzzer);
      Joint_2.write(0);
      lcd.clear();
    }
  }    
}
