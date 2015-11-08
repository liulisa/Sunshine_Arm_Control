int switch1 = 12;
int switch2 = 13;

void setup() {
  // output pins
  pinMode(switch1, OUTPUT);
  pinMode(switch2, OUTPUT);

  // set up motor to not run
  digitalWrite(switch1, LOW);   
  digitalWrite(switch2, LOW);

  Serial.begin(9600);
  
}

void loop() {

//  // set up motor to run in one direction
//  digitalWrite(switch1, HIGH);
//  delay(2000);  // 5 s
//  
//  // set up motor to break
//  digitalWrite(switch2, HIGH);
//  delay(2000);  // 5 s
//  
//  // set up motor to go in other direction
//  digitalWrite(switch1, LOW);
//  delay(2000);
//
//  // set up motor to stop
//  digitalWrite(switch2, LOW);
//  delay(2000);

  // set up motor to run in one direction
//  digitalWrite(switch1, HIGH);
//  Serial.println("direction 1");
//  delay(1000);
  
  // motor runs in other direction
  digitalWrite(switch1, LOW);
  delay(10);
  digitalWrite(switch2, HIGH);
  Serial.println("direction 2");
  delay(1000);

//  // stop runnign
//  digitalWrite(switch2, LOW);
//  Serial.println("not running");
//  delay(100);
  
}
