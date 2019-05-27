#include <hcsr04.h>
#include <Servo.h>

#define SERVO_PIN   3

// HCSR 04 ultrasonic sensor
#define ECHO        A4
#define TRIG        A5

// Motor pins
#define ENA         5
#define ENB         6
#define MOTOR_1     7
#define MOTOR_2     8
#define MOTOR_3     9
#define MOTOR_4     11

#define US_MIN_RANGE    20
#define US_MAX_RANGE    4000
#define US_THRESHOLD    250    // max distance to evade object

#define US_CENTER_ANGLE   85
#define US_LEFT_ANGLE     170
#define US_RIGHT_ANGLE    10

#define START_SPEED     130   // default speed to get car moving
#define TURN_SPEED      230   // speed when rotating
#define MAIN_SPEED      105   // normal speed of car

#define START_TIME      330   // time for the car to move at start speed in ms
#define MOVE_TIME       500  // time to move before checking in ms
#define TURN_TIME       500   // time to perform turn manoeuvre in ms

#define STATE_IDLE      0
#define STATE_RUNNING   1

HCSR04 ultrasonic(TRIG, ECHO, US_MIN_RANGE, US_MAX_RANGE);
Servo carServo;

/**
 * INITIALIZE
 */
void setup() 
{
  Serial.begin(9600);
  
  setupServo();
  setupMotors();
  stopCar();
}

void setupServo()
{
  carServo.attach(SERVO_PIN);
  lookAhead();
}

void setupMotors()
{
  pinMode(MOTOR_1, OUTPUT);
  pinMode(MOTOR_2, OUTPUT);
  pinMode(MOTOR_3, OUTPUT);
  pinMode(MOTOR_4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

/**
 * UPDATE
 */
void loop() 
{
  startCar();
  delay(MOVE_TIME);
  
  stopCar();
  updateUltrasonic();
}

void startCar()
{
  // Get initial inertia
  goForward(START_SPEED);

  // Let car move
  delay(START_TIME);

  // Change to main speed
  goForward(MAIN_SPEED);
}

void stopCar()
{
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
}

void goForward(int speed)
{
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  
  digitalWrite(MOTOR_1, HIGH);
  digitalWrite(MOTOR_2, LOW);
  digitalWrite(MOTOR_3, LOW);
  digitalWrite(MOTOR_4, HIGH);
}

void reverse()
{
  analogWrite(ENA, START_SPEED);
  analogWrite(ENB, START_SPEED);
  
  digitalWrite(MOTOR_1, LOW);
  digitalWrite(MOTOR_2, HIGH);
  digitalWrite(MOTOR_3, HIGH);
  digitalWrite(MOTOR_4, LOW);
}

void turnLeft()
{
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  
  digitalWrite(MOTOR_1, LOW);
  digitalWrite(MOTOR_2, HIGH);
  digitalWrite(MOTOR_3, LOW);
  digitalWrite(MOTOR_4, HIGH);
}

void turnRight()
{
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  
  digitalWrite(MOTOR_1, HIGH);
  digitalWrite(MOTOR_2, LOW);
  digitalWrite(MOTOR_3, HIGH);
  digitalWrite(MOTOR_4, LOW);
}

void lookAhead()
{
  carServo.write(US_CENTER_ANGLE);
}

void lookLeft()
{
  carServo.write(US_LEFT_ANGLE);
}

void lookRight()
{
  carServo.write(US_RIGHT_ANGLE);
}

void updateUltrasonic()
{
  float distanceAhead;
  float distanceLeft;
  float distanceRight;
  bool sufficientClearance = false;

  while(!sufficientClearance)
  {
    distanceAhead = ultrasonic.distanceInMillimeters();
    Serial.print("O-O ahead: ");
    Serial.println(distanceAhead);
    
    lookLeft();
    delay(750);
    distanceLeft = ultrasonic.distanceInMillimeters();
    Serial.print("O-O left: ");
    Serial.println(distanceLeft);
    
    lookRight();
    delay(800);
    distanceRight = ultrasonic.distanceInMillimeters();
    Serial.print("O-O right: ");
    Serial.println(distanceRight);

    // finish routine in start position
    lookAhead();

    if (distanceAhead > US_THRESHOLD)
    {
      // keep going forward
      Serial.println("O-O FORWARD");
      sufficientClearance = true;
      break;
    }

    if (distanceLeft > distanceRight)
    {
      // turn left
      Serial.println("O-O LEFT");
      turnLeft();
      delay(TURN_TIME);
    }
    else
    {
      // turn right
      Serial.println("O-O RIGHT");
      turnRight();
      delay(TURN_TIME);
    }

    stopCar();
    delay(500);
  }
}
