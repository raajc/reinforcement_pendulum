String inString = ""; //store Input Bytes
uint16_t value = 0; //Final Input Value

// Encoder pins
const byte motorcoderPinA = 4;//outputA digital pin4
const byte motorcoderPinB = 5;//outoutB digital pin5

//Encoder counts
int16_t count = 0;
volatile int32_t motor_count = 0;
int16_t motor_count_mm = 0;

double kp = 10;
double kd = 1;
//double ki = 0.1;
int offset = 130; //out of 255
int diff = 0;
int count_prev = 0;
//int control_iter=0;
//int windup_thres=50;
//int sum_count = 0;

double kp_m = 0.02;
double kd_m = 0.025;
int offset_m = 132; //out of 255
int diff_m = 0;
int32_t desired_motor_count = 0;
//Encoder Interrupts
#define motoreadA digitalRead(4)
#define motoreadB digitalRead(5)

//PWM pins
int pwm_pinL = 6;
int pwm_pinR = 7;

// Variables
int err = 0;
int err_prev = 0;
int duty = 0;
double output = 0;
int control_loop_period = 1;
int readout_loop_period = 200;
int control_loop_timer = 0;
int readout_loop_timer = 0;
int enter_flag = 0;
char inByte = 0;
char control_mode = 0;
char direction_move = 0;

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
count = REG_TC0_CV0-600;
while(Serial.available() >0) //If there is a serial message available
  {
    char inByte = Serial.read(); //read each byte
    if(isDigit(inByte))//if the byte is a number
    {
    inString += (char)inByte; //combine bytes
    }
    if (inByte == 'L') //MUST END PWM VALUE WITH #
    {
      value = inString.toInt(); //Get final int value from string
      inString = ""; //clear string
      inByte = 0;
      motorR(pwm_pinL, pwm_pinR, value, 0); //Motor direction is flipped
    }
    if (inByte == 'R') //MUST END PWM VALUE WITH #
    {
      value = inString.toInt(); //Get final int value from string
      inString = ""; //clear string
      inByte = 0;
      motorL(pwm_pinL, pwm_pinR, value, 0);
    }
    if(inByte == 'E')
    {   
      write16bit(count+32767);
      //Serial.println(count+32767);
      Serial.flush();
    }
    if(inByte == 'M')
    {   
      motor_count_mm = motor_count/13;
      write16bit(motor_count_mm+32767);
      //Serial.println(motor_count_mm+32767);
      Serial.flush();
    }

  }

}

// Move cart left
void motorL(int pwm_pinL, int pwm_pinR, int duty, int fric_offset) {
    analogWrite(pwm_pinL, min(duty+fric_offset,255));
    analogWrite(pwm_pinR, 0); 
}

// Move cart right
void motorR(int pwm_pinL, int pwm_pinR, int duty, int fric_offset) {
    analogWrite(pwm_pinL, 0);
    analogWrite(pwm_pinR, min(duty+fric_offset,255)); 
}

void motorStop(int pwm_pinL, int pwm_pinR) {
    analogWrite(pwm_pinL, 0);
    analogWrite(pwm_pinR, 0); 
}

void write16bit(int16_t ch) { //function to combine two bytes and send over serial
  Serial.write(ch>>8);
  Serial.write(ch & 0x00FF);
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
