//**********************************************************************
//* Project           : Autonomous Maze Bot
//*
//* Program name      : ETTPbot.ino
//*
//* Authors           : bobbybuddha, Lily Nguyen
//*
//* Date created      : 7/25/17
//*
//* Purpose           : An autonomous bot that employs the tape sensor
//*                     for navigating a tape maze, and an ultrasonic
//*                     sensor to find other bots and "kill" them
//*                     (pop their balloon).
//**********************************************************************

// setting pins on arduino board
const int MOTOR_ENABLE_LEFT = 12;
const int MOTOR_ENABLE_RIGHT = 13;
const int FORWARD_RIGHT = 8;
const int BACK_RIGHT = 10;
const int FORWARD_LEFT = 4;
const int BACK_LEFT = 2;
const int PING_PIN = 11;

// thresholds for distance (cm)
const int MIN_THRESHOLD = 5;
const int MAX_THRESHOLD = 330;

// tape variables
static int PreviousTape, CurrentTape;

// Tape stuff
int QRE1113_Pin = 0;
int OnTapeThreshold = 900;
int OffTapeThreshold = 850;
boolean OnTape = false;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // set motors as outputs
  pinMode(MOTOR_ENABLE_LEFT, OUTPUT);
  pinMode(MOTOR_ENABLE_RIGHT, OUTPUT);
  pinMode(FORWARD_RIGHT, OUTPUT);
  pinMode(BACK_RIGHT, OUTPUT);
  pinMode(FORWARD_LEFT, OUTPUT);
  pinMode(BACK_LEFT, OUTPUT);
  pinMode(PING_PIN, OUTPUT);
  digitalWrite(MOTOR_ENABLE_RIGHT, OUTPUT);
  digitalWrite(MOTOR_ENABLE_LEFT, OUTPUT);
}

void loop() {

  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;

  digitalWrite(PING_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(PING_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(PING_PIN, LOW);

  pinMode(PING_PIN, INPUT);
  duration = pulseIn(PING_PIN, HIGH);
  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);


  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  Serial.print(cm);
  Serial.println();
  if (cm < MIN_THRESHOLD) {
    // When the robot is about 5 cm close to the bot he then attacks
    Kill();
  }
  // Hysteresis for the tape sensor code
  CurrentTape = analogRead();
  Serial.println(CurrentTape);
  if (CurrentTape > OnTapeThreshold && PreviousTape < OffTapeThreshold) {
    Serial.println("OnTape");
    PreviousTape = CurrentTape;
    // when the robot is on the tape I have an Evasive function
    Evasive();
  }
  else if (CurrentTape < OffTapeThreshold && PreviousTape > OnTapeThreshold) {
    Serial.println("OFF Tape");
    PreviousTape = CurrentTape;
  }
}

// function definitions

// moves the robot forwards
void Forward() {
  ForwardR();
  ForwardL();
  //delay(1000);
  //Stop();
}

// moves the robot backwards
void Reverse() {
  ReverseR();
  ReverseL();
  delay(300);
  Stop();
}

// turns the robot counterclockwise (left) 90 degrees
void TurnCounterClockwise() {
  ForwardR();
  ReverseL();
  delay(300);
  Stop();
}

// turns the robot clockwise (right) 90 degrees
void TurnClockwise() {
  ForwardL();
  ReverseR();
  delay(300);
  Stop();
}

// spins the robot counterclockwise
void SpinCounterClockwise() {
  ForwardR();
  ReverseL();
  delay(1000);
}

// spins the robot clockwise
void SpinClockwise() {
  ForwardL();
  ReverseR();
  delay(1000);
}

// stops all motors 
void Stop() {
  digitalWrite(FORWARD_RIGHT, LOW);
  digitalWrite(BACK_RIGHT, LOW);
  digitalWrite(FORWARD_LEFT, LOW);
  digitalWrite(BACK_LEFT, LOW);
}

//////////////////////////////////////

// moves the right wheel forward
void ForwardR() {
  digitalWrite(FORWARD_RIGHT, HIGH);
  digitalWrite(BACK_RIGHT, LOW);
}

// moves the left wheel forward
void ForwardL() {
  digitalWrite(FORWARD_LEFT, HIGH);
  digitalWrite(BACK_LEFT, LOW);
}

// moves the right wheel backwards
void ReverseR() {
  digitalWrite(FORWARD_RIGHT, LOW);
  digitalWrite(BACK_RIGHT, HIGH);
}

// moves the left wheel backwards
void ReverseL() {
  digitalWrite(FORWARD_LEFT, LOW);
  digitalWrite(BACK_LEFT, HIGH);
}


// drives forward for a specified distance
void Follow(int dist)
{
  if (dist > MIN_THRESHOLD && dist < MAX_THRESHOLD)
  {
    Forward();
  }
  else if (dist <= MIN_THRESHOLD) {
    // Reverse();
    TurnClockwise();
  }
  else
    Stop();
}

// Evades robot
void Evasive() {
  Reverse();
  delay(1000);
  TurnClockwise();
  delay(400);
}

// converts time to distance (ping sensor)
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

// used to pop balloon on target
void Kill() {
  Stop();
  delay(1000);

  for (int i = 0; i < 5; i++)
  {
    Forward();
    delay(500);
    Reverse();
  }

  //Forward();
  Stop();
  return;
}


// autonomous tape navigation algorithm
void tapeNav(){
    
  CurrentTape = analogRead(QRE1113_Pin);
  Serial.println(CurrentTape);
  
  // currentTape == black, previous tape == white
  if (CurrentTape > OnTapeThreshold && PreviousTape < OffTapeThreshold) {
    Serial.println("OnTape");
  
    // turn car
    TurnClockwise();
    PreviousTape = CurrentTape;
  }
  else if (CurrentTape > OnTapeThreshold && PreviousTape >= OffTapeThreshold) {
    TurnClockwise();
    TurnClockwise();
    PreviousTape = CurrentTape;
  }
  else {
    // check ping sensors
    if (cm < MIN_THRESHOLD) {
      // When the robot is about 5 cm close to the bot he then attacks
      Kill();
    }
    else {
      Forward();
    }
  }
}
