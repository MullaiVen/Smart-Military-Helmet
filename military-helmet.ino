#include <SPI.h>     //Libraries for the OLED display
#include <Adafruit_GFX.h>   //library for Arduino provides a common syntax and set of graphics functions for OLEDs
#include <Adafruit_SSD1306.h>  //To control the OLED display
#include <VirtualWire.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 //

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)

//pin A4 is SDA and pin A5 is SCL, pin A0 is Input.


char *rfdata;
const int sos_button = 2;

//Transmitter Pin is 12
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
boolean fall = false; //stores if a fall has occurred
boolean trigger1 = false; //stores if first trigger (lower threshold) has occurred
boolean trigger2 = false; //stores if second trigger (upper threshold) has occurred
boolean trigger3 = false; //stores if third trigger (orientation change) has occurred
byte trigger1count = 0; //stores the counts past since trigger 1 was set true
byte trigger2count = 0; //stores the counts past since trigger 2 was set true
byte trigger3count = 0; //stores the counts past since trigger 3 was set true
int angleChange = 0;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  //Serial.println("Wrote to IMU");

  pinMode(sos_button, INPUT_PULLUP);
  
  vw_set_ptt_inverted(true);
  vw_set_rx_pin(11);
  vw_set_tx_pin(12);
  vw_setup(4000); // speed of data transfer Kbps
  vw_rx_start();

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      while (true);
    }

   delay(2000);         // wait for initializing
   oled.clearDisplay(); // clear display

  

}
void loop() {

  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)){
    if(buf[0] == '3'){
      oled.clearDisplay();
      oled.setTextSize(2);          // text size
      oled.setTextColor(WHITE);     // text color
      oled.setCursor(0, 10);        // position to display
      oled.println("Move East");
      oled.display();
      Serial.println("Move East");
    }

    if(buf[0] == '4'){
      oled.clearDisplay();
      oled.setTextSize(2);          // text size
      oled.setTextColor(WHITE);     // text color
      oled.setCursor(0, 10);        // position to display
      oled.println("Move West");
      oled.display();
      Serial.println("Move West");
    }
    if(buf[0] == '5'){
      oled.clearDisplay();
      oled.setTextSize(2);          // text size
      oled.setTextColor(WHITE);     // text color
      oled.setCursor(0, 10);        // position to display
      oled.println("Move North");
      oled.display();
      Serial.println("Move North");
    }
    if(buf[0] == '6'){
      oled.clearDisplay();
      oled.setTextSize(2);          // text size
      oled.setTextColor(WHITE);     // text color
      oled.setCursor(0, 10);        // position to display
      oled.println("Move South");
      oled.display();
      Serial.println("Move South");
    }
  }
  
  
  
  Wire.write(0);
  mpu_read();
  ax = (AcX - 2050) / 16384.00;
  ay = (AcY - 77) / 16384.00;
  az = (AcZ - 1947) / 16384.00;
  gx = (GyX + 270) / 131.07;
  gy = (GyY - 351) / 131.07;
  gz = (GyZ + 136) / 131.07;
  // calculating Amplitute vactor for 3 axis
  float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
  int Amp = Raw_Amp * 10;  // Mulitiplied by 10 bcz values are between 0 to 1
  Serial.println(Amp);
  if (Amp <= 2 && trigger2 == false) { //if AM breaks lower threshold (0.4g)
    trigger1 = true;
    //Serial.println("TRIGGER 1 ACTIVATED");
  }
  if (trigger1 == true) {
    trigger1count++;
    if (Amp >= 12) { //if AM breaks upper threshold (3g)
      trigger2 = true;
      //Serial.println("TRIGGER 2 ACTIVATED");
      trigger1 = false; trigger1count = 0;
    }
  }
  if (trigger2 == true) {
    trigger2count++;
    angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
    //Serial.println(angleChange);
    if (angleChange >= 30 && angleChange <= 400) { //if orientation changes by between 80-100 degrees
      trigger3 = true; trigger2 = false; trigger2count = 0;
      //Serial.println(angleChange);
      //Serial.println("TRIGGER 3 ACTIVATED");
    }
  }
  if (trigger3 == true) {
    trigger3count++;
    if (trigger3count >= 10) {
      angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
      //delay(10);
      //Serial.println(angleChange);
      if ((angleChange >= 0) && (angleChange <= 50)) { //if orientation changes remains between 0-10 degrees
        fall = true; trigger3 = false; trigger3count = 0;
        //Serial.println(angleChange);
      }
      else { //user regained normal orientation
        trigger3 = false; trigger3count = 0;
        //Serial.println("TRIGGER 3 DEACTIVATED");
      }
    }
  }
  if (fall == true) { //in event of a fall detection
    
    rfdata="1" ;
    vw_send((uint8_t *)rfdata, strlen(rfdata));
    vw_wait_tx();
    // Wait until the whole message is gone
    delay(1000);
    Serial.println("FALL DETECTED");

    fall = false;
  }
  if (trigger2count >= 6) { //allow 0.5s for orientation change
    trigger2 = false; trigger2count = 0;
    //Serial.println("TRIGGER 2 DECACTIVATED");
  }
  if (trigger1count >= 6) { //allow 0.5s for AM to break upper threshold
    trigger1 = false; trigger1count = 0;
    //Serial.println("TRIGGER 1 DECACTIVATED");
  }
  delay(100);

  int sos_value = digitalRead(sos_button);
  //Serial.println(sos_value);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 2 when the
  // button's pressed, and off when it's not:
  
  if (sos_value == LOW) {
    
    rfdata="2" ;
    vw_send((uint8_t *)rfdata, strlen(rfdata));
    vw_wait_tx();
    // Wait until the whole message is gone
    delay(1000);
    Serial.println("Emergency");
    
    oled.clearDisplay();
    oled.setTextSize(2);          // text size
    oled.setTextColor(WHITE);     // text color
    oled.setCursor(0, 10);        // position to display
    oled.println("HELP!"); // text to display
    oled.display();
  } 


}
void mpu_read() {

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
