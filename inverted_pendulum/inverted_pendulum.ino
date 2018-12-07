// Encoder pins
const byte motorcoderPinA = 4;//outputA digital pin4
const byte motorcoderPinB = 5;//outoutB digital pin5

//Encoder counts
int16_t count = 0;
volatile int motor_count = 0;

double kp = 10;
double kd = 1;
int offset = 130; //out of 255
int diff = 0;
int count_prev = 0;
//Encoder Interrupts
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

Serial.begin (115200);

//Motor Encoder setup
pinMode(motorcoderPinA, INPUT);
pinMode(motorcoderPinB, INPUT);

//Encoder Setup
pmc_enable_periph_clk(ID_TC0);
TC_Configure(TC0,0, TC_CMR_TCCLKS_XC0); 

TC0->TC_BMR = TC_BMR_QDEN          // Enable QDEC (filter, edge detection and quadrature decoding)
              | TC_BMR_POSEN       // Enable the position measure on channel 0
              //| TC_BMR_EDGPHA    // Edges are detected on PHA only
              //| TC_BMR_SWAP      // Swap PHA and PHB if necessary (reverse acting)
              | TC_BMR_FILTER      // Enable filter
              | TC_BMR_MAXFILT(63) // Set MAXFILT value
             ;                       

// Enable and trigger reset
TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN;

attachInterrupt(digitalPinToInterrupt(motorcoderPinA), m_isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(motorcoderPinB), m_isrB, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:

if(millis()- control_loop_timer > control_loop_period) { 
  count = REG_TC0_CV0-600;
  control_loop_timer = millis();
  // Print encoder if different from previous value
  diff = count - count_prev;
  if (count > 0 && count < 150 && abs(motor_count) < 80000) {
    output = kp*(count)+offset+kd*diff;
    motorL(pwm_pinL, pwm_pinR, output);
  } else if (count <=0 && count > -150 && abs(motor_count) < 80000) {
    output = -kp*(count)+offset+kd*diff;
    motorR(pwm_pinL, pwm_pinR, output);
  } else {
    output=0;
    motorR(pwm_pinL, pwm_pinR, output);
  }
  count_prev = count;
}

if(millis() - readout_loop_timer> readout_loop_period) {
  count = REG_TC0_CV0-600;
//  while(Serial.available() > 0) {
//    char inByte = Serial.read();
//    if(inByte == 'R') {
//      REG_TC0_CV0 = -600;
//    }
//  }
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
