//**********************************************************************
//* Project           : Secure Slug Gate
//*
//* Program name      : Gate.ino
//*
//* Author            : bosdhill
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
  if (depositRead && withdrawRead)
     passwordEntry(); 
}

// FUNCTIONS

// flushReceive()
// flushes serial buffer 
void flushReceive(){
  while(Serial.available())
    Serial.read();
}

// dpasswordEntry()
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
          // break if index is SIZE
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

// wpasswordEntry()
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
          // break if index is SIZE
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

// checkWithdrawPassword()
// checks if password entered by 
// user is the withdraw password
bool checkWithdrawPassword(){
  for (byte i = 0; i < SIZE; i++){
    if (input[i] != WPASSWORD[i])
      return false; 
  }
  return true;  
}

// checkWithdrawPassword()
// checks if password entered by 
// user is the deposit password
bool checkDepositPassword(){
  for (byte i = 0; i < SIZE; i++){
    if (input[i] != DPASSWORD[i])
      return false; 
  }
  return true;  
}

// shiftWrite()
// takes the pin to be written to and 
// desiredState (HIGH or LOW) and sets pin
// to desiredState
void shiftWrite(int desiredPin, boolean desiredState)
{
  // data is a byte (8 bits), desiredPin is the location of the 
  // bit (0 thru 7) and the desired state is HIGH or LOW
  // 1 <=> HIGH, 0 <=> LOW
  bitWrite(data,desiredPin,desiredState);
  // MSBFIST = most siginificant bit first
  // sends data to clockpin which will write HIGH to desired pin
  shiftOut(DATAPIN, CLOCKPIN, MSBFIRST, data);
  // latchpin latches data to correct pin 
  digitalWrite(LATCHPIN, HIGH);
  digitalWrite(LATCHPIN, LOW);
}

// displayGreen()
// lights up the green LED
void displayGreen(){
  for (byte i = 0; i < 5; i++){
    shiftWrite(2,HIGH); // display green
    delay(200); 
    shiftWrite(2,LOW); // display green
    delay(200);  
  }
}

// displayRed()
// lights up red LED
void displayRed(){
  for (byte i = 0; i < 5; i++){
    shiftWrite(1,HIGH); // display red
    delay(200); 
    shiftWrite(1,LOW); // display red
    delay(200);   
  }
}

// withdraw()
// rotates servo so gate opens for withdrawal
void withdraw(){
  myservo.write(180); 
  delay(10000);
  displayRed(); 
  myservo.write(90);   
}

// deposit()
// rotates servo so gate opens for deposit
void deposit(){
  myservo.write(0);
  delay(10000);
  displayRed(); 
  myservo.write(90); 
}

// passwordEntry()
// sets input array to sequence of numbers the user enters,
// then checks against the withdraw and deposit passwords. 
// It then either opens the gate accordingly or it remains 
// closed
void passwordEntry(){
  if (withdrawRead && depositRead){
    int button1State, button2State, button3State, enterState;
    // get state of each button
    button1State = digitalRead(BUT1);
    button2State = digitalRead(BUT2);
    button3State = digitalRead(BUT3); 
    enterState  = digitalRead(ENTER); 
    // if enter has not been pressed
    if (enterState == HIGH){
      if (button1State == LOW && button2State != LOW && button3State != LOW){
        input[i] = 1;
        i++;
        delay(200);  
      } // end if
      else if (button2State == LOW && button1State != LOW && button3State != LOW){
        input[i] = 2;
        i++;
        delay(200);  
      } // end else-if
      else if (button3State == LOW && button1State != LOW && button2State != LOW){
        input[i] = 3; 
        i++; 
        delay(200);  
      } // end else-if
    } // end if
    else{ // if enter has been pressd, check against withdraw and deposit password
      if (checkWithdrawPassword()){
          displayGreen();
          withdraw(); 
      } // end if
      else if (checkDepositPassword()){
          displayGreen(); 
          deposit(); 
      } // end else-if
      else // else display red if incorrect password(s) are entered
          displayRed(); 
      // reset counter
      i = 0;   
    } // end else
  } // end else
}
