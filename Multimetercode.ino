/*
  Arduino LCD Shield MULTIMETER
  
  You must have had problems with your Electrical components whether they worked or not,
  for example couple of days back I did an arduino project but it was not working I spent
  the next few days rebuilding the project using new parts until I found  that it was the
  problem of 1 JUMPER WIRE!! the whole circuit did not run.(I had many problems like these)
  Then I knew I had to do something to check other components as well,so I came up with 
  this design which has functions that a normal multimeter does not have..This multimeter
  which you can make for a couple of bucks can check: connectivity,voltage,resistance,
  has a built in servo tester,LED tester and motor tester as well .This project can 
  accurately measure and display the accurate results obtained after the internal
  calculations.This multimeter has a menu system to change to the mode of electronic
  components and run through them.

  I hope you will learn something by building this project and never experience the same issues like me.

  modified 21 Jan 2023
  by Nandan Hegde
  modified 6 Feb 2023
  by Nandan Hegde
  modified 20 Feb 2023
  by Nandan Hegde
  modified 1 March 2023
  by Nandan Hegde

  This is a fully functional and accurate multimeter that you can make.
  
  THIS IS A:
  LCD Keypad Shield Multimeter (used with Arduino UNO)

  
  Contact:
  nandanhegde01@gmail.com

  
  Releases:

  v1.1.23 -- 21 Jan 2023
  v2.2.23 -- 6 Feb 2023
  v3.2.23 -- 13 Feb 2023
  
  v4.2.23 -- 20 Feb 2023

  V5 or V6 is last
  
 
*/

/*start of code
---------------------------------------------
---------------------------------------------
*/

const int voltageSensorPin = A2;          // sensor pin
float vIn;                                // measured voltage (3.3V = max. 16.5V, 5V = max 25V)
float vOut;
float voltageSensorVal;                   // value on pin A3 (0 - 1023)
const float factor = 5.128;               // reduction factor of the Voltage Sensor shield
const float vCC = 5.00;                   // Arduino input voltage (measurable by voltmeter)
int pot = A4;  // analog pin used to connect the potentiometer
int analog;    
const int sensorPin = A1;  // Analog input pin that senses Vout
int sensorValue = 0;       // sensorPin default value
float Vin = 5;             // Input voltage
float Vout = 0;            // Vout default value
float Rref = 1000;          // Reference resistor's value in ohms (you can give this value in kiloohms or megaohms - the resistance of the tested resistor will be given in the same units)
float R = 0; 

String menuItems[] = {"1.Connectivity", "2.Voltage", "3.Resistance", "4.Servo Tester", "5.LED test ", "  V4.2.23 "};
 
// Navigation button variables
int readKey;
int savedDistance = 0;

int B=13;
float D,Di=0;
 
// Menu control variables
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;
int ledread;
 
// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};
 
byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};
 
byte menuCursor[8] = {
B00000,
  B00100,// 
  B00010,//       *
  B11111,//       **
  B00010,// **********
  B00100,//       **
  B00000,//       *
  B00000 //
};
 
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>

Servo myservo; 

float vol = 0;
int input = 0;

 
// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 
void setup() {
myservo.attach(5);
  pinMode(B,OUTPUT);
 pinMode(A0, INPUT);
 pinMode(4, OUTPUT);
 pinMode(2, INPUT);
  Serial.begin(9600);   
  // Initializes serial communication
  Serial.begin(9600);
 
  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();
 
  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
 
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(6,OUTPUT);
  
}
 
void loop() {
  servo();
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}
 
// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  Serial.print(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}
 
// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }
 
  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}
 
 
void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            menuItem1();
            break;
          case 1:
            menuItem2();
            break;
          case 2:
            menuItem3();
            break;
          case 3:
            menuItem4();
            break;
          
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
 
        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
 
        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
 
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
 
        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
 
        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}
 
// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
  int result = 0;
  if (x < 50) {
    result = 1; // right
  } else if (x < 195) {
    result = 2; // up
  } else if (x < 380) {
    result = 3; // down
  } else if (x < 790) {
    result = 4; // left
  }
  return result;
}

void drawInstructions() {
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.write(byte(1)); // Up arrow
  lcd.print("/");
  lcd.write(byte(2)); // Down arrow
  lcd.print(" buttons");

}
 
void menuItem1() { 
  digitalWrite(6,HIGH);
  delay(1000);
  digitalWrite(6,LOW);
  
  int activeButton = 0;

byte buzzer [8]=
  {
   0b00001,
   0b00011,
   0b01111,
   0b01111,
   0b01111,
   0b00011,
   0b00001,
   0b00000
  };
  byte sound [8]=
  {
   0b00000,
   0b00100,
   0b00010,
   0b10010,
   0b10010,
   0b00010,
   0b00100,
   0b00000
  };
  byte symbol [8]=
  {
   B00000,
  B00000,
  B10010,
  B11010,
  B11111,
  B11010,
  B10010,
  B00000
  };
  lcd.createChar(11,symbol);
  lcd.createChar(3,buzzer);
  lcd.createChar(4,sound);

lcd.clear();

  lcd.setCursor(3,0);
  lcd.print("Checking.");
  delay(500);

  lcd.setCursor(3,0);
  lcd.print("Checking..");
  delay(500);
  lcd.setCursor(3,0);
  lcd.print("Checking...");
  delay(500);
lcd.clear();

 
  lcd.clear();
  lcd.setCursor(1, 0);
 lcd.print("-Connectivity-");
  lcd.setCursor(5, 1);
  lcd.print("Yes ");
   lcd.write(byte(3));
  lcd.write(byte(4));
 
 
  digitalWrite(13,HIGH);
 
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        digitalWrite(13,LOW);
        lcd.write(byte(1));
  lcd.write(byte(2));
   lcd.write(byte(0));
 
        break;
    }
  }
}
 
void menuItem2() {
  digitalWrite(6,HIGH);
  delay(1000);
  digitalWrite(6,LOW);
  int activeButton = 0;

  byte battery1 [8]=
  {
  B01110,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111
  };
  byte battery2 [8]=
  {
  B01110,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111
  };
 
  lcd.createChar(5,battery1);
  lcd.createChar(6,battery2);


 
  lcd.print("Volt Meter");

   lcd.clear();
  lcd.setCursor(3, 0);
 lcd.setCursor(3,0);
  
  lcd.setCursor(1,0);
  lcd.print("Calculating.");
  delay(500);
  lcd.setCursor(3,0);
 
  lcd.setCursor(1,0);
  lcd.print("Calculating..");
  delay(500);
  lcd.setCursor(3,0);
 
  lcd.setCursor(1,0);
  lcd.print("Calculating...");
  delay(500);
lcd.clear();
lcd.setCursor(3,0);


lcd.print("-Voltage is-");

voltageSensorVal = analogRead(voltageSensorPin);    // read the current sensor value (0 - 1023) 
  vOut = (voltageSensorVal / 1024) * vCC;             // convert the value to the real voltage on the analog pin
  vIn =  vOut * factor;   
  
  delay(500);
  lcd.setCursor(5, 1);
  lcd.print(vIn);
lcd.print(" V");
  delay(1000);
  


 
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
   lcd.write(byte(1));
  lcd.write(byte(2));
   lcd.write(byte(0));
        break;
        lcd.write(byte(0));
    }
  }
}
 
void menuItem3() { 
  digitalWrite(6,HIGH);
  delay(1000);
  digitalWrite(6,LOW);
  int activeButton = 0;
 
  lcd.clear();

  byte resistor1 [8]=
  {
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B00100,
  B00100
  };
  byte resistor2 [8]=
  {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01010,
  B11011,
  B00000
  };
 
 
  lcd.createChar(7,resistor1);
  lcd.createChar(10,resistor2);
 
lcd.setCursor(3,0);
  
  lcd.setCursor(1,0);
  lcd.print("Calculating.");
  delay(500);
  lcd.setCursor(3,0);
 ;
  lcd.setCursor(1,0);
  lcd.print("Calculating..");
  delay(500);
  lcd.setCursor(3,0);
 
  lcd.setCursor(1,0);
  lcd.print("Calculating...");
  delay(500);
lcd.clear();

sensorValue = analogRead(sensorPin);  // Read Vout on analog input pin A0 (Arduino can sense from 0-1023, 1023 is 5V)
  Vout = (Vin * sensorValue) / 1023;    // Convert Vout to volts
  R = Rref * (1 / ((Vin / Vout) - 1));
  // Formula to calculate resistor's value

   lcd.setCursor(2,0);
  lcd.print("-Resistance-");
  lcd.setCursor(4,1);
 
  lcd.print(R); 
   lcd.setCursor(11,1);
 lcd.write(byte(10));

                   
  

 lcd.setCursor(0,0);
  lcd.write(byte(7));
 
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
 lcd.write(byte(1));
  lcd.write(byte(2));
   lcd.write(byte(0));
        break;
    }
  }
}
 
void menuItem4() { 
  digitalWrite(6,HIGH);
  delay(1000);
  digitalWrite(6,LOW);

  

  int activeButton = 0;
  byte servo1 [8]=
  {
   B00000,
  B11111,
  B00100,
  B01110,
  B11111,
  B11111,
  B11111,
  
  B00000
  };


 
  lcd.createChar(8,servo1);
  
 ledread = analogRead(A3);
  lcd.clear();
  lcd.setCursor(0, 0);
 lcd.setCursor(0,0);
  
  lcd.setCursor(3,0);
  lcd.print("Testing.");
  delay(500);
  lcd.setCursor(0,0);
 
  lcd.setCursor(3,0);
  lcd.print("Testing..");
  delay(500);
  lcd.setCursor(2,0);
 
  lcd.setCursor(3,0);
  lcd.print("Testing...");
  delay(0);
  lcd.clear();
lcd.setCursor(0,0);
lcd.write(byte(8));
lcd.setCursor(1,0);
lcd.print("-Servo results-");
  digitalWrite(4,HIGH);

   delay(500);
 
    lcd.setCursor(2,1);
lcd.print("Use the Knob");
  
 
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
       lcd.write(byte(0));
        button = 0;
        activeButton = 1;
        digitalWrite(4,LOW);
       lcd.write(byte(1));
       lcd.write(byte(2));
   lcd.write(byte(0));
        break;
    }
    
  }
  
  
}
void servo(){
  
  analog = analogRead(pot);            // reads the value of the potentiometer (value between 0 and 1023)
  Serial.print(analog);
  Serial.print(" ");
  analog = map(analog, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(analog);                  // sets the servo position according to the scaled value

  Serial.print(analog);
  Serial.println();
    }

/*Code written by Nandan Hegde

credits-- 
1)learnelectronics for the basic menu system for design

 End of code
------------------------------------------------
------------------------------------------------
*/
