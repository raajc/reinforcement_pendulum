// Encoder pins
const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 3;//outoutB digital pin3

//Encoder counts
volatile int count = 0;
int protectedCount = 0;
int previousCount = 0;

//Encoder Interrupts
#define readA digitalRead(2)
#define readB digitalRead(3)

//PWM pins
int pwm_pinL = 6;
int pwm_pinR = 7;

// Variables
int duty = 0;
int control_loop_period = 10;
int control_loop_timer = 0;

void setup() {
// PWM setup
pinMode(pwm_pinL, OUTPUT);
pinMode(pwm_pinR, OUTPUT);

//Encoder setup
Serial.begin (115200);

pinMode(encoderPinA, INPUT_PULLUP);
pinMode(encoderPinB, INPUT_PULLUP);

attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:

  // Read Encoder
  noInterrupts();
  protectedCount = count;
  interrupts();
  
if(control_loop_timer - millis() > control_loop_period) { 
  // Print encoder if different from previous value
  if (protectedCount > 0) {
    motorL(pwm_pinL, pwm_pinR, 128);
  } else {
    motorR(pwm_pinL, pwm_pinR, 128);
  }
  if(protectedCount != previousCount) {
    Serial.println(protectedCount);
  }
  previousCount = protectedCount;
}
}

// Move cart left
void motorL(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, duty);
    analogWrite(pwm_pinR, 0); 
}

// Move cart right
void motorR(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, 0);
    analogWrite(pwm_pinR, duty); 
}

void isrA() {
  if(readB != readA) {
    count ++;
  } else {
    count --;
  }
}
void isrB() {
  if (readA == readB) {
    count ++;
  } else {
    count --;
  }
}
