
String inString = ""; //store Input Bytes
uint16_t value = 0; //Final Input Value

void setup() 
{
Serial.begin(115200); // set the baud rate
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
  }
    if (inByte == 'R') //MUST END PWM VALUE WITH #
  {
    value = inString.toInt(); //Get final int value from string
    inString = ""; //clear string
  }
    if(inByte == 'E')
    {
        write16bit(130); //Write the value to serial
    }
  }

  write16bit(value); //Write the value to serial
  delay(100); //simmer down a bit

}

void write16bit(uint16_t ch) { //function to combine two bytes and send over serial
  Serial.write(ch>>8);
  Serial.write(ch & 0x00FF);
}
