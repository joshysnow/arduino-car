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

#define START_SPEED     120 // default speed to get car moving
#define MAIN_SPEED      100 // normal speed

#define STATE_IDLE      0
#define STATE_RUNNING   1

HCSR04 ultrasonic(TRIG, ECHO, US_MIN_RANGE, US_MAX_RANGE);
Servo carServo;
byte carState;

void setup() 
{
  Serial.begin(9600);
  
  setupServo();
  setupMotors();
  stopCar();
}

void loop() 
{
  delay(1000);

  if (carState == STATE_IDLE)
  {
    startCar();
  }
  else
  {
    updateUS();
  }
}

void startCar()
{
  // Get initial inertia
  forward(START_SPEED);

  // Let car move
  delay(250);

  // Change to main speed
  forward(MAIN_SPEED);

  // Update state
  carState = STATE_RUNNING;
}

void setupServo()
{
  carServo.attach(SERVO_PIN);
  carServo.write(85);
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

void stopCar()
{
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  carState = STATE_IDLE;
}

void forward(int speed)
{
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  
  digitalWrite(MOTOR_1, HIGH);
  digitalWrite(MOTOR_2, LOW);
  digitalWrite(MOTOR_3, LOW);
  digitalWrite(MOTOR_4, HIGH);
}

byte _position = 0;

void updateUS()
{
  Serial.println(ultrasonic.distanceInMillimeters());

  if (_position == 0)
  {
    // RIGHT
    carServo.write(40);
  }
  else if (_position == 1)
  {
    // MIDDLE
    carServo.write(85);
  }
  else
  {
    // LEFT
    carServo.write(140);
  }

  _position = (_position + 1) % 3;
}

