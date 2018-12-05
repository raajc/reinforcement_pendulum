
String inString = ""; //store Input Bytes
uint16_t value = 0; //Final Input Value

// Encoder pins
const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 3;//outoutB digital pin3

//Encoder counts
int16_t count = 0;
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
int updateFlag = 0;

void setup() 
{
  // PWM setup
pinMode(pwm_pinL, OUTPUT);
pinMode(pwm_pinR, OUTPUT);

Serial.begin(115200); // set the baud rate

pinMode(encoderPinA, INPUT_PULLUP);
pinMode(encoderPinB, INPUT_PULLUP);

attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);

}

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
  write16bit(count);
  delay(100);
    }
  }
    //write16bit(count);
    //delay(100);
}

void write16bit(int16_t ch) { //function to combine two bytes and send over serial
  Serial.write(ch>>8);
  Serial.write(ch & 0x00FF);
}

// Move cart left
void motorL(int pwm_pinL, int pwm_pinR, int duty) {
    analogWrite(pwm_pinL, duty);
    analogWrite(pwm_pinR, 0); 
}

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
