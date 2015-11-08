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
Adafruit_DCMotor *shoulder = AFMS.getMotor(2);
Adafruit_DCMotor *elbow = AFMS.getMotor(3);
Adafruit_DCMotor *wrist = AFMS.getMotor(4);
#define gripper_1 13;   // 5th motor's first H-bridge switch
#define gripper_2 14;   // 5th motor's second H-bridge switch
int default_speed = 50;

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
int gripper_speed = 50;     // gripper's speed should be slow, out of 255;


void setup() {
  Serial.begin(9600);       // setup Serial library at 9600 bps
  Serial.println("Motor code!");

  AFMS.begin();     // create w/ default freq 1.6 KHz

  // set default speeds
  rotator->setSpeed(default_speed);
  shoulder->setSpeed(default_speed);
  elbow->setSpeed(default_speed);
  wrist->setSpeed(default_speed);

  // this is for Serial testing
}

void loop() {
  // test motors using serial input
  /*command = Serial.parseInt();
  Serial.print("I received: ");
  Serial.println( command, DEC);
  run_action(command);*/

  Serial.println(BACKWARD);
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
        shoulder->run(FORWARD);
        break;
      case shoulder_out:
        shoulder->run(BACKWARD);
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
        
        break;
      case gripper_open:
        break;
      default:
        // make sure the robot has no movement
        rotator->run(RELEASE);
        shoulder->run(RELEASE);
        elbow->run(RELEASE);
        wrist->run(RELEASE);
        break;
  
    }
}

void gripper_setSpeed(int speed_val){
  gripper_speed = speed_val;
}

void gripper_run(int dir){
  switch (dir){
    case FORWARD:
      digitalWrite(gripper_2, LOW);
      digitalWrite(gripper_1, HIGH);
      delay(250);                     // these two lines
      digitalWrite(gripper_1, LOW);   // because speed control not yet implemented
      break;
    case BACKWARD:
      digitalWrite(gripper_1, LOW);
      digitalWrite(gripper_2, HIGH);
      delay(250);                     // these two lines
      digitalWrite(gripper_2, LOW);   // because speed control not yet implemented
      break;
    default:
      digitalWrite(gripper_1, LOW);
      digitalWrite(gripper_2, LOW);    
  }
  
}


