/* This code will run motors on the Toy Arm Edge based on given commands.
    Lisa Liu, Oliver ????, Albert ???? 
    11/8/2015

*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which ports for each motor. Arduino shield has four motor ports.
Adafruit_DCMotor *rotator = AFMS.getMotor(1);
Adafruit_DCMotor *shoulder = AFMS.getMotor(2);
Adafruit_DCMotor *grip = AFMS.getMotor(3);
Adafruit_DCMotor *wrist = AFMS.getMotor(4);
#define elbow_1 12   // 5th motor's first H-bridge switch
#define elbow_2 13   // 5th motor's second H-bridge switch
int default_speed = 60;

// action keywords
#define rotate_left 14
#define rotate_right 13
#define shoulder_down 11
#define shoulder_up 12
#define elbow_down 10
#define elbow_up 9
#define wrist_down 8
#define wrist_up 7
#define grip_in 5
#define grip_out 6
int command = 0;

// velostat port
int velostatPin = A1;
int velostatValue = 0;
int velostatThreshold = 250;


void setup() {
  Serial.begin(115200);       // setup Serial library at 115200 bps

  AFMS.begin();     // create w/ default freq 1.6 KHz

  // set default speeds
  rotator->setSpeed(default_speed*2);
  shoulder->setSpeed(default_speed*1.5);
  grip->setSpeed(default_speed/2.5);
  wrist->setSpeed(default_speed);
  int elbowspeed = default_speed;
  // this is for Serial testing
}

void loop() {
  // test motors using serial input
  command = Serial.parseInt();

  velostatValue = analogRead(velostatPin);
  if (velostatValue > velostatThreshold){
    // code for haptic feedback below
    Serial.println(velostatValue - velostatThreshold);
    if (command == 5){
      command = 0;  // do nothing if velostat is over threshold
    }
  }
  run_action(command);
}

void run_action(int action_key) {
  switch (action_key) {
    case rotate_left:
      rotator->run(FORWARD);
      break;
    case rotate_right:
      rotator->run(BACKWARD);
      break;
    case shoulder_down:
      shoulder->run(FORWARD);
      break;
    case shoulder_up:
      shoulder->run(BACKWARD);
      break;
    case grip_in:
      grip->run(FORWARD);
      break;
    case grip_out:
      grip->run(BACKWARD);
      break;
    case wrist_down:
      wrist->run(FORWARD);
      break;
    case wrist_up:
      wrist->run(BACKWARD);
      break;
    case elbow_down:
      elbow_run(FORWARD);
      break;
    case elbow_up:
      elbow_run(BACKWARD);
      break;
    default:
      // make sure the robot has no movement
      rotator->run(RELEASE);
      shoulder->run(RELEASE);
      grip->run(RELEASE);
      wrist->run(RELEASE);
      //gripper_run(RELEASE);
      digitalWrite(elbow_1, LOW);
      digitalWrite(elbow_2, LOW);
      break;
  }
}

void elbow_run(int dir) {
  switch (dir) {
    case FORWARD:
      digitalWrite(elbow_2, LOW);
      digitalWrite(elbow_1, HIGH);
      break;
    case BACKWARD:
      digitalWrite(elbow_1, LOW);
      digitalWrite(elbow_2, HIGH);
      break;
    default:
      digitalWrite(elbow_1, LOW);
      digitalWrite(elbow_2, LOW);
      break;
  } 
}
