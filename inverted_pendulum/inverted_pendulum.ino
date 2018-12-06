// Encoder pins
const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 3;//outoutB digital pin3
const byte motorcoderPinA = 4;//outputA digital pin4
const byte motorcoderPinB = 5;//outoutB digital pin5

//Encoder counts
volatile int count = 0;
volatile int motor_count = 0;
int protectedCount = 0;
int previousCount = 0;

double kp = 15;
double kd = 1;
int offset = 0; //out of 255
int diff = 0;
int count_prev = 0;
//Encoder Interrupts
#define readA digitalRead(2)
#define readB digitalRead(3)
#define motoreadA digitalRead(4)
#define motoreadB digitalRead(5)

//PWM pins
int pwm_pinL = 6;
int pwm_pinR = 7;

// Variables
int duty = 0;
double output = 0;
int control_loop_period = 2;
int readout_loop_period = 50;
int control_loop_timer = 0;
int readout_loop_timer = 0;
int enter_flag = 0;

void setup() {
// PWM setup
pinMode(pwm_pinL, OUTPUT);
pinMode(pwm_pinR, OUTPUT);

//Encoder setup
Serial.begin (115200);

pinMode(encoderPinA, INPUT_PULLUP);
pinMode(encoderPinB, INPUT_PULLUP);
pinMode(motorcoderPinA, INPUT_PULLUP);
pinMode(motorcoderPinB, INPUT_PULLUP);

attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);

attachInterrupt(digitalPinToInterrupt(motorcoderPinA), m_isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(motorcoderPinB), m_isrB, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:

  // Read Encoder
  //noInterrupts();
  //protectedCount = count;
  //interrupts();

if(millis()- control_loop_timer > control_loop_period) { 
  control_loop_timer = millis();
  // Print encoder if different from previous value
  diff = count - count_prev;
  if (count > 0) {
    output = kp*(count)+offset+kd*diff;
    motorL(pwm_pinL, pwm_pinR, min(output,255));
  } else {
    output = -kp*(count)+offset+kd*diff;
    motorR(pwm_pinL, pwm_pinR, min(output, 255));
  }
  count_prev = count;
}

if(millis() - readout_loop_timer> readout_loop_period) { 
  readout_loop_timer = millis();
  Serial.print("Encoder: ");
  Serial.print(count);
  Serial.print(" Output: ");
  Serial.println(min(output,255));
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

// Encoder ISRs
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

// Motor Encoder ISRs
void m_isrA() {
  if(motoreadB != motoreadA) {
    motor_count ++;
  } else {
    motor_count --;
  }
}
void m_isrB() {
  if (motoreadA == motoreadB) {
    motor_count ++;
  } else {
    motor_count --;
  }
}
