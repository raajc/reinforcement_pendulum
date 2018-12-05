int pwm_pin = 6;
int i = 0;

void setup() {
  // put your setup code here, to run once:
pinMode(pwm_pin, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  pwmWrite(pwm_pin);

  
}

void pwmWrite(int pwm_pin) {
    for(i = 1; i < 255; i++) {
    analogWrite(pwm_pin, i); 
    delay(10);
  }
}
