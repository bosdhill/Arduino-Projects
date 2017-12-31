//**********************************************************************
//* Project           : Secure Slug
//*
//* Program name      : Gate.ino
//*
//* Author            : bobbybuddha
//*
//* Date created      : 11/30/17
//*
//* Purpose           : Operate password IO and operation of gate states
//**********************************************************************

#include <Servo.h>
Servo myservo; 

// INPUT PINS
// buttons
#define BUT1 13 // 1
#define BUT2 12 // 2
#define BUT3 11 // 3
#define ENTER 10 // ENTER

// OUTPUT PINS
// for shift register
#define DATAPIN 2
#define LATCHPIN 3
#define CLOCKPIN 4

// OTHER
// array stuff
const byte SIZE = 4;
byte DPASSWORD[SIZE]; // password
byte WPASSWORD[SIZE]; // password
byte input[SIZE]; 
byte i = 0; 
byte data = 0; // byte representation of indices to be set

// OTHER
// for serialInOut 
boolean depositRead = false; 
boolean withdrawRead = false;  

// set up arduino pins
void setup() {
  // Set the three SPI pins to be outputs
  pinMode(DATAPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);  
  pinMode(LATCHPIN, OUTPUT);
  // Set the two outputs for the buttons
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  // Set up servo
  myservo.attach(9);
  // Set up the serial port for input/output
  Serial.begin(9600);
}

// main loop
void loop() {
  // gather deposit and withdraw passwords
  dpasswordEntry();
  flushReceive(); 
  wpasswordEntry(); 

  // if read so LEDs dont light up without passwords being set
  if (depositRead && withdrawRead){
     passwordEntry(); 
  }
}

// FUNCTIONS

// flushes buffer
// flushes the serial buffer 
void flushReceive(){
  while(Serial.available())
    Serial.read();
}

// deposit password
// gets input for deposit password
void dpasswordEntry(){
  if (!depositRead){
    byte index = 0;
    Serial.println("Please enter a unique password of length 4 for deposit:");
    
    // runs this infinitely so it wont keep displaying print statements
    while (true){
      // while there is still data in buffer
      while (Serial.available() > 0){
        // read in data if index is less than SIZE
        if (index < SIZE){
          // read in digit
          DPASSWORD[index] = Serial.read() - '0';
          index++;
          // needed to put condition in here or else it doesnt work
          if (index == SIZE){
            depositRead = true; 
            break; 
          } // end if
        } // end if
      } // end while Serial.available
      if (depositRead == true){
        Serial.print("Deposit Password set as: "); 
        for (byte i = 0; i < SIZE; i++)
          Serial.print(DPASSWORD[i]);
        break; // break out of infinite loop       
      } // end if
    } // end while true
  } // end if not read
}

// withdraw password
// gets input for withdraw password
void wpasswordEntry(){
  if (!withdrawRead){
    byte index = 0;
    Serial.println("\nPlease enter a password of length 4 for withdrawal:");
    
    // runs this infinitely so it wont keep displaying print statements
    while (true){
      // while there is still data in buffer
      while (Serial.available() > 0){
        // read in data if index is less than SIZE
        if (index < SIZE){
          // read in digit
          WPASSWORD[index] = Serial.read() - '0';
          index++;
          // needed to put condition in here or else it doesnt work
          if (index == SIZE){
            withdrawRead = true; 
            break; 
          } // end if
        } // end if
      } // end while Serial.available
      if (withdrawRead == true){
        Serial.print("Withdraw Password set as: "); 
        for (byte i = 0; i < SIZE; i++)
          Serial.print(WPASSWORD[i]);
        break; // break out of infinite loop       
      } // end if
    } // end while true
  } // end if not read
}

// 
bool checkWithdrawPassword(){
  for (byte i = 0; i < SIZE; i++){
    if (input[i] != WPASSWORD[i])
      return false; 
  }
  return true;  
}

bool checkDepositPassword(){
  for (byte i = 0; i < SIZE; i++){
    if (input[i] != DPASSWORD[i])
      return false; 
  }
  return true;  
}

void shiftWrite(int desiredPin, boolean desiredState)
{
  // First we'll alter the global variable "data", changing the
  // desired bit to 1 or 0:

  bitWrite(data,desiredPin,desiredState);
  // data is a byte (8 bits), desiredPin is the location of the 
  // bit (0 thru 7) and the desired state is HIGH or LOW
  // 1 <=> HIGH, 0 <=> LOW

  shiftOut(DATAPIN, CLOCKPIN, MSBFIRST, data);
  // MSBFIST = most siginificant bit first
  // sends data to clockpin which will write HIGH to desired pin

  // latchpin latches data to correct pin 
  digitalWrite(LATCHPIN, HIGH);
  digitalWrite(LATCHPIN, LOW);
}

void displayGreen(){
  for (byte i = 0; i < 5; i++){
    shiftWrite(2,HIGH); // display green
    delay(200); 
    shiftWrite(2,LOW); // display green
    delay(200);  
  }
}

void displayRed(){
  for (byte i = 0; i < 5; i++){
    shiftWrite(1,HIGH); // display red
    delay(200); 
    shiftWrite(1,LOW); // display red
    delay(200);   
  }
}

void withdraw(){
  myservo.write(180); 
  delay(10000);
  displayRed(); 
  myservo.write(90);   
}
void deposit(){
  myservo.write(0);
  delay(10000);
  displayRed(); 
  myservo.write(90); 
}

// buttons are not reading in
void passwordEntry(){
  if (withdrawRead && depositRead){
    int button1State, button2State, button3State, enterState;
    button1State = digitalRead(BUT1);
    button2State = digitalRead(BUT2);
    button3State = digitalRead(BUT3); 
    enterState  = digitalRead(ENTER); 

    
    if (enterState == HIGH){
      if (button1State == LOW && button2State != LOW && button3State != LOW){
        input[i] = 1;
        i++;
        delay(200);  
      }
      else if (button2State == LOW && button1State != LOW && button3State != LOW){
        input[i] = 2;
        i++;
        delay(200);  
      } 
      else if (button3State == LOW && button1State != LOW && button2State != LOW){
        input[i] = 3; 
        i++; 
        delay(200);  
      }
    }
    else{
      if (checkWithdrawPassword()){
          displayGreen();
          withdraw(); 
      }
      else if (checkDepositPassword()){
          displayGreen(); 
          deposit(); 
      }
      else
          displayRed(); 
      // reset counter
      i = 0;   
    }
  }
}
