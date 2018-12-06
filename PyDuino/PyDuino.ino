
String inString = ""; //store Input Bytes
uint16_t value = 0; //Final Input Value

// Encoder pins
const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 3;//outoutB digital pin3
const byte motorcoderPinA = 4;//outputA digital pin4
const byte motorcoderPinB = 5;//outoutB digital pin5

//Encoder counts
volatile int16_t count = 0;
volatile int16_t motor_count = 0;
int mCountPrev = 0;
int pCountPrev = 0;

//Encoder Interrupts
#define readA digitalRead(2)
#define readB digitalRead(3)
#define motoreadA digitalRead(4)
#define motoreadB digitalRead(5)

//PWM pins
int pwm_pinL = 6;
int pwm_pinR = 7;

// Variables


void setup() 
{
  // PWM setup
pinMode(pwm_pinL, OUTPUT);
pinMode(pwm_pinR, OUTPUT);

Serial.begin(115200); // set the baud rate

pinMode(encoderPinA, INPUT_PULLUP);
pinMode(encoderPinB, INPUT_PULLUP);
pinMode(motorcoderPinA, INPUT_PULLUP);
pinMode(motorcoderPinB, INPUT_PULLUP);

attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);
attachInterrupt(digitalPinToInterrupt(motorcoderPinA), m_isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(motorcoderPinB), m_isrB, CHANGE);

}
char inByte = 0;
void loop() 
{
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
      motorL(pwm_pinL, pwm_pinR, value);
    }
    if (inByte == 'R') //MUST END PWM VALUE WITH #
    {
      value = inString.toInt(); //Get final int value from string
      inString = ""; //clear string
      inByte = 0;
      motorR(pwm_pinL, pwm_pinR, value);
    }
    if(inByte == 'E')
    {   
      write16bit(count+32767);
      Serial.flush();
    }
    if(inByte == 'M')
    {   
      write16bit(motor_count+32767);
      Serial.flush();
    }

  }

}


void write16bit(int16_t ch) { //function to combine two bytes and send over serial
  Serial.write(ch>>8);
  Serial.write(ch & 0x00FF);
}

// Move cart left
void motorL(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, min(250, duty));
    analogWrite(pwm_pinR, 0); 
}

void motorR(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, 0);
    analogWrite(pwm_pinR, min(250, duty)); 
}

//Pendulum Encoder ISRs
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
