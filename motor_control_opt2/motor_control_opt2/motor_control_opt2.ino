/* This code will run motors on the Toy Arm Edge based on given commands.
    Lisa Liu - Nov 7, 2015

*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which ports for each motor. Arduino shield has four motor ports.
Adafruit_DCMotor *rotator = AFMS.getMotor(1);
//Adafruit_DCMotor *shoulder = AFMS.getMotor(2);
Adafruit_DCMotor *elbow = AFMS.getMotor(2);
Adafruit_DCMotor *wrist = AFMS.getMotor(3);
Adafruit_DCMotor *gripper = AFMS.getMotor(4);
#define shoulder_1 12   // 5th motor's first H-bridge switch
#define shoulder_2 13   // 5th motor's second H-bridge switch
int default_speed = 255;
int gripper_speed = 40;

// action keywords
#define rotate_left 1
#define rotate_right 2
#define shoulder_in 3
#define shoulder_out 4
#define elbow_in 5
#define elbow_out 6
#define wrist_in 7
#define wrist_out 8
#define gripper_close 9
#define gripper_open 10
int command = 0;


void setup() {
  Serial.begin(9600);       // setup Serial library at 9600 bps
  Serial.println("Motor code 2!");

  AFMS.begin();     // create w/ default freq 1.6 KHz

  // set default speeds
  rotator->setSpeed(default_speed);
  elbow->setSpeed(default_speed);
  wrist->setSpeed(default_speed);
  gripper->setSpeed(gripper_speed);


  // this is for Serial testing
}

void loop() {
  // test motors using serial input
  command = Serial.parseInt();
  Serial.print("I received: ");
  Serial.println( command, DEC);
  run_action(command);
  delay(100);



}

void run_action(int action_key) {
  switch (action_key) {
    case rotate_left:
      rotator->run(FORWARD);
      Serial.println("I am rotated left");
      delay(10);
      break;
    case rotate_right:
      rotator->run(BACKWARD);
      break;
    case shoulder_in:
      shoulder_run(FORWARD);
      break;
    case shoulder_out:
      shoulder_run(BACKWARD);
      break;
    case elbow_in:
      elbow->run(FORWARD);
      break;
    case elbow_out:
      elbow->run(BACKWARD);
      break;
    case wrist_in:
      wrist->run(FORWARD);
      break;
    case wrist_out:
      wrist->run(BACKWARD);
      break;
    case gripper_close:
      gripper->run(FORWARD);
      break;
    case gripper_open:
      gripper->run(BACKWARD);
      break;
    default:
      // make sure the robot has no movement
      rotator->run(RELEASE);
      shoulder_run(RELEASE);
      elbow->run(RELEASE);
      wrist->run(RELEASE);
      gripper->run(RELEASE);

      break;

  }
}

void shoulder_run(int dir) {
  switch (dir) {
    case FORWARD:
      digitalWrite(shoulder_2, LOW);
      digitalWrite(shoulder_1, HIGH);
      Serial.print(shoulder_1);
      Serial.println(" is on");
      delay(500);
      break;
    case BACKWARD:
      digitalWrite(shoulder_1, LOW);
      digitalWrite(shoulder_2, HIGH);
      Serial.print(shoulder_2);
      Serial.println(" is on");
      delay(500);
      break;
    default:
      digitalWrite(shoulder_1, LOW);
      digitalWrite(shoulder_2, LOW);
      break;
  }

}


