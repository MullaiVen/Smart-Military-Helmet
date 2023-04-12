#include <VirtualWire.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);
int show;

char buffer[64]; 
int idx = 0;

void setup()

{
  
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(0x27);
  
  lcd.begin(16, 2); // initialize the lcd
  show = 0;
  lcd.setBacklight(255);
  vw_set_ptt_inverted(true);
  
  // Required for DR3100
  
  vw_set_rx_pin(11);
  vw_set_tx_pin(12);
  
  vw_setup(4000); // Bits per sec
  
  vw_rx_start(); // Start the receiver PLL running

}

void loop()

{

      while (Serial.available() > 0) 
       {
        char c = Serial.read();
        buffer[idx] = c;
        idx++;
        buffer[idx] ='\0' ;  // keep the char array null terminated.
      }

     if (idx > 0 )
      {
        vw_send((uint8_t *)buffer, idx);  // Send message
        idx = 0;
        buffer[idx] = '\0';
        delay(400);
        Serial.println(" Message sent!");
      } 


  uint8_t buf[VW_MAX_MESSAGE_LEN];
  
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  if (vw_get_message(buf, &buflen))
  
    // Non-blocking
    //Serial.println(buf[0]);
    {
    
    if(buf[0] == '1')
    
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Fall Detected");
      Serial.println("Fall Detected");
      delay(5000);
      lcd.clear();
    
    }
  
    if(buf[0] == '2')
    
    {
    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Help Nearby");
      lcd.setCursor(0, 1);
      lcd.print("Soldier");
      Serial.println("Help Nearby Soldier");
      delay(5000);
      lcd.clear();
    
    
    }

    if(buf[0] == '9')
    
    {
    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Test ");
      lcd.setCursor(0, 1);
      lcd.print("OK");
      Serial.println("Test Ok");
      delay(5000);
      lcd.clear();
    
    
    }


  
  }
}
