/* 
 *  Dispenser control V1.0 for AHM
 */

#include <Servo.h>
#include <EEPROM.h>

// EEPROM to save usage data
#define MALEDISPENSESADDRESS    0
#define FEMALEDISPENSESADDRESS  1
#define USAGEDELAYADDRESS       2

// Pins
#define LEDPIN          12
#define SWITCHPIN       2
#define MALEPIN         11
#define FEMALEPIN       6
#define RELEASESERVOPIN 16
#define LOCKERSERVOPIN  17

// dispenser
#define FEMALE  0
#define MALE 1
#define OPEN 0
#define CLOSE 1

//  LEDS
#define ON 1
#define OFF 0

// Automatic testing
int nr_of_buttons = 100; // in total (combined 2 tubes)

Servo releaseservo;  // create servo object to control a servo
Servo lockerservo;

// Servo positions
int pos = 84;    // variable to store the servo position

// Releaser
int releaserNeutralPos = 90;
int releaserFEMALEDropPos = 0;
int releaserMALEDropPos = 180;

// Locker
int LockerOpenPos = 175;
int LockerClosePos = 75;

// Usage timeout
int usageTimeout = 0;

uint8_t buttonState;


void setup() {
  // Pin definitions
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW); // Use 3 as GND
  pinMode(SWITCHPIN, INPUT_PULLUP);

  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  pinMode(MALEPIN, INPUT);

  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  pinMode(FEMALEPIN, INPUT);

  pinMode(LEDPIN, OUTPUT);  // Switches on/off all leds
  digitalWrite(LEDPIN, ON);

  releaseservo.attach(RELEASESERVOPIN);  // attaches the servo on pin 9 to the servo object
  lockerservo.attach(LOCKERSERVOPIN);  // attaches the servo on pin 9 to the servo object

  //analogReference(INTERNAL1V1);  // 1.1v for current measurement
  
  Serial.begin(9600);
  
  releaseservo.write(releaserNeutralPos);
  lockerservo.write(LockerClosePos);
  // initialize the pushbutton pin as an input:
  pinMode(1, OUTPUT);
  digitalWrite(1, 0);
  pinMode(2, INPUT_PULLUP);

  lockerservo.detach();
  releaseservo.detach();
  //EEPROM.update(USAGEDELAYADDRESS, 10);
  //EEPROM.update(MALEDISPENSESADDRESS, 0);
  //EEPROM.update(FEMALEDISPENSESADDRESS, 0);
  doSettings();
  
  doVisuals2();
  Serial.println("Ready!");
}

void loop() {
  //Serial.println("SWITCH=" + String(digitalRead(SWITCHPIN)) + " MALE=" + String(digitalRead(MALEPIN)) + " FEMALE="  + String(digitalRead(FEMALEPIN)));

  if(digitalRead(SWITCHPIN) == LOW)
  {

      if((digitalRead(MALEPIN) == HIGH) && ((digitalRead(FEMALEPIN) == LOW)))
      {
        Serial.println("Drop male button");
        doVisuals2(); 
        dropButton(MALE);
        EEPROM.update(MALEDISPENSESADDRESS, EEPROM.read(MALEDISPENSESADDRESS)+1);
        for(int i = 0; i < (usageTimeout*10); i++)
        {
          if(digitalRead(SWITCHPIN) == LOW)
          {
            controlLeds(OFF);
            delay(200); 
            controlLeds(ON);
            delay(200);
            controlLeds(OFF);
            delay(200);
            controlLeds(ON);
            delay(200);
            i+=9;
          }
          else
          delay(100);
        }
        //delay(usageTimeout*1000);
        Serial.println("Ready");
      }
      else if((digitalRead(FEMALEPIN) == HIGH) && ((digitalRead(MALEPIN) == LOW)))
      {
        Serial.println("Drop female button");
        doVisuals2 (); 
        dropButton(FEMALE);
        EEPROM.update(FEMALEDISPENSESADDRESS, EEPROM.read(FEMALEDISPENSESADDRESS)+1);
        for(int i = 0; i < (usageTimeout*10); i++)
        {
          if(digitalRead(SWITCHPIN) == LOW)
          {
            controlLeds(OFF);
            delay(200); 
            controlLeds(ON);
            delay(200);
            controlLeds(OFF);
            delay(200);
            controlLeds(ON);
            delay(200);
            i+=9;
          }
          else
          delay(100);
        }
        //delay(usageTimeout*1000);
        Serial.println("Ready");
      }
  }
  else
  {
    int a = Serial.read();
  
    if (a == '+')
    {
      pos+=2;
      releaseservo.write(pos);  
      Serial.println("Pos = " + String(pos));
    }
  
    if (a == '-')
    {
      pos-=2;
      releaseservo.write(pos);  
      Serial.println("Pos = " + String(pos));
    }
  
    if (a == 'l')
    {
      dropButton(FEMALE);
    }
  
    if (a == 'r')
    {
      dropButton(MALE);
    }
  
    if (a == 'm')
    {
      for(int i = 0; i < 15 ; i++)
      {
        dropButton(FEMALE);
        //delay(30000);
      }
    } 
  
    if (a == 'b')
    {
      for(int i = 0; i < 15 ; i++)
      {
        dropButton(FEMALE);
        dropButton(MALE);
        //delay(30000);
      }
    }
  
    if (a == 's')
    {
      for(int i = 0; i < 15 ; i++)
      {
        dropButton(FEMALE);
        delay(30000);
      }
    }
  
    if (a == 'o')
    {
      lockerservo.attach(LOCKERSERVOPIN);  // attaches the servo on pin 9 to the servo object
      controlSlot(OPEN);
      Serial.println("Lock open");
      delay(800);
      lockerservo.detach();
    }
  
    if (a == 'c')
    {
      lockerservo.attach(LOCKERSERVOPIN);  // attaches the servo on pin 9 to the servo object
      controlSlot(CLOSE);
      Serial.println("Lock closed");
      delay(800);
      lockerservo.detach();
    }

    if (a == 'O')
    {
      releaseservo.attach(RELEASESERVOPIN);  // attaches the servo on pin 9 to the servo object
      releaseservo.write(releaserFEMALEDropPos);
      Serial.println("Gate open");
      delay(800);
      releaseservo.detach();
    }
  
    if (a == 'N')
    {
      releaseservo.attach(RELEASESERVOPIN);  // attaches the servo on pin 9 to the servo object
      releaseservo.write(releaserNeutralPos);
      Serial.println("Gate neutral");
      delay(800);
      releaseservo.detach();
    }

    if (a == 'C')
    {
      releaseservo.attach(RELEASESERVOPIN);  // attaches the servo on pin 9 to the servo object
      releaseservo.write(releaserMALEDropPos);
      Serial.println("Gate closed");
      delay(800);
      releaseservo.detach();
    }
  
    if (a == 't')
    {
      for(int i = 0; i < 1500 ; i++)
      {
         dropButton(FEMALE);
         dropButton(MALE);
      }
    }  
  }
}


void dropButton(uint8_t side)
{
    releaseservo.attach(RELEASESERVOPIN);  // attaches the servo on pin 9 to the servo object
    lockerservo.attach(LOCKERSERVOPIN);  // attaches the servo on pin 9 to the servo object
    if(side == MALE)
    {
      //Serial.print("MALE button...");
      lowerButton();
      releaseservo.write(releaserMALEDropPos);
      delay(1200);
      releaseservo.write(releaserNeutralPos);
      delay(800);
      //Serial.println(" done!");
    }
    else if(side == FEMALE)
    {
      //Serial.print("FEMALE button...");
      lowerButton();
      releaseservo.write(releaserFEMALEDropPos);
      delay(1200);
      releaseservo.write(releaserNeutralPos);
      delay(800);
      //Serial.println(" done!");
    }
    lockerservo.detach();
    releaseservo.detach();
}

void controlSlot(uint8_t state)
{
  if(state == OPEN)
  {
    lockerservo.write(LockerOpenPos); 
  }

  if(state == CLOSE)
  {
    lockerservo.write(LockerClosePos); 
  }
  delay(750);
}

void lowerButton(void)
{
  controlSlot(OPEN);
  delay(250);
  controlSlot(CLOSE);
  //delay(500);
}

void controlLeds(uint8_t state)
{
  if(state == ON)
  {
    digitalWrite(LEDPIN, HIGH);
    digitalWrite(7, LOW);
    digitalWrite(10, LOW);
  }

  if(state == OFF)
  {
    digitalWrite(LEDPIN, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(10, HIGH);
  }
}

void doVisuals(void)
{
  for(int i = 0; i < 5; i++)
  {
    controlLeds(OFF);
    delay(500);
    controlLeds(ON);
    delay(500);
  }
}

void doVisuals2(void)
{
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(10, LOW);
    digitalWrite(7, LOW);
    digitalWrite(LEDPIN, LOW);
    delay(500);
    digitalWrite(10, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(LEDPIN, HIGH);
    delay(500);
  }
    digitalWrite(10, LOW);
    digitalWrite(7, LOW);
    digitalWrite(LEDPIN, LOW);
    delay(500);
    digitalWrite(10, LOW);
    digitalWrite(7, LOW);
    digitalWrite(LEDPIN, HIGH);
}

void doSettings(void)
{
  checkButtons();
  usageTimeout = EEPROM.read(USAGEDELAYADDRESS);
  Serial.println("--- USAGE STATUS ---");
  Serial.println("TIMEOUT SETTING = " + String(usageTimeout) + " sec.");
  Serial.println("OVERALL DISPENSES: MALE = " + String(EEPROM.read(MALEDISPENSESADDRESS)) + " FEMALE = " + String(EEPROM.read(FEMALEDISPENSESADDRESS)));
  Serial.println("--- ---");
}

uint8_t checkButtons(void)
{
  if(digitalRead(SWITCHPIN) == LOW) // pressed at the startup? -> set usagetimout
  {
    if((digitalRead(MALEPIN) == LOW) && ((digitalRead(FEMALEPIN) == LOW))){usageTimeout = 10;}
    if((digitalRead(MALEPIN) == HIGH) && ((digitalRead(FEMALEPIN) == LOW))){usageTimeout = 20;}
    if((digitalRead(MALEPIN) == LOW) && ((digitalRead(FEMALEPIN) == HIGH))){usageTimeout = 30;}
    if((digitalRead(MALEPIN) == HIGH) && ((digitalRead(FEMALEPIN) == HIGH))){usageTimeout = 60;}
    EEPROM.update(USAGEDELAYADDRESS, usageTimeout);
  }
}
