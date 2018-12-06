// Encoder pins
const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 13;//outoutB digital pin3
const byte motorcoderPinA = 4;//outputA digital pin4
const byte motorcoderPinB = 5;//outoutB digital pin5

//Encoder counts
volatile int16_t count = 0;
volatile int motor_count = 0;
int protectedCount = 0;
int previousCount = 0;

double kp = 10;
double kd = 1;
int offset = 0; //out of 255
int diff = 0;
int count_prev = 0;
//Encoder Interrupts
#define readA digitalRead(2)
#define readB digitalRead(13)
#define motoreadA digitalRead(4)
#define motoreadB digitalRead(5)

//PWM pins
int pwm_pinL = 6;
int pwm_pinR = 7;

// Variables
int duty = 0;
double output = 0;
int control_loop_period = 5;
int readout_loop_period = 200;
int control_loop_timer = 0;
int readout_loop_timer = 0;
int enter_flag = 0;
char inByte = 0;

void setup() {
// PWM setup
pinMode(pwm_pinL, OUTPUT);
pinMode(pwm_pinR, OUTPUT);

//Encoder setup
Serial.begin (115200);

pinMode(encoderPinA, INPUT);
pinMode(encoderPinB, INPUT);
pinMode(motorcoderPinA, INPUT);
pinMode(motorcoderPinB, INPUT);

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
    motorL(pwm_pinL, pwm_pinR, output);
  } else {
    output = -kp*(count)+offset+kd*diff;
    motorR(pwm_pinL, pwm_pinR, output);
  }
  count_prev = count;
}

if(millis() - readout_loop_timer> readout_loop_period) {
  while(Serial.available() > 0) {
    char inByte = Serial.read();
    if(inByte == 'R') {
      count = -600;
    }
  }
  readout_loop_timer = millis();
  Serial.print("Encoder: ");
  Serial.print(count);
  Serial.print(" Motor: ");
  Serial.print(motor_count);
  Serial.print(" Output: ");
  Serial.print(min(output,255));
  Serial.print(" Deriv: ");
  Serial.print(diff*kd);
  Serial.print(" Prop: ");
  Serial.println(output-diff*kd);
}

}

// Move cart left
void motorL(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, min(output,255));
    analogWrite(pwm_pinR, 0); 
}

// Move cart right
void motorR(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, 0);
    analogWrite(pwm_pinR, min(output,255)); 
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
