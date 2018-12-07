int timer = 0;
int period = 100;
void setup() {

  Serial.begin(115200);
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

  PIO_Configure(PIOC, PIO_PERIPH_B, PIO_PC25B_TIOA6, PIO_DEFAULT);
  PIO_Configure(PIOC, PIO_PERIPH_B, PIO_PC26B_TIOB6, PIO_DEFAULT);

  pmc_enable_periph_clk(ID_TC2);
  TC_Configure(TC2,2, TC_CMR_TCCLKS_XC2); 
 
  TC2->TC_BMR = TC_BMR_QDEN          // Enable QDEC (filter, edge detection and quadrature decoding)
                | TC_BMR_POSEN       // Enable the position measure on channel 0
                //| TC_BMR_EDGPHA    // Edges are detected on PHA only
                //| TC_BMR_SWAP      // Swap PHA and PHB if necessary (reverse acting)
                | TC_BMR_FILTER      // Enable filter
                | TC_BMR_MAXFILT(63) // Set MAXFILT value
               ;                       

  // Enable and trigger reset
  TC2->TC_CHANNEL[2].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN;
 
}

void loop() {
  int16_t count = REG_TC0_CV0;
  int16_t motor_count = REG_TC2_CV0;
  if(millis() - timer > period) {
    timer = millis();
    Serial.print("Cart: ");
    Serial.print(motor_count);
    Serial.print(" Pole: ");
    Serial.println(count);
  }
}
