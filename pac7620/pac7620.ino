/**********************************************
This demo is write by Xiangnan.
This demo initialized the PAC7620 sensor with 
  near_normal_mode_V5_6.15mm_121017 for 940nm
  and achieve recognizing 9 basic gestures.
**********************************************/

#include <Wire.h>
#include "pac7620.h"


void setup()
{
  uint8_t error = 0;
  Wire.begin();

  Serial.begin(9600);
  // Keyboard.begin();
  Serial.println("\nPAJ7620U2 TEST DEMO");

  error = pajInit();
  if (error) {
    Serial.print("INIT ERROR,CODE:");
    Serial.println(error);
  }
  else {
    Serial.println("INIT OK lambor");
  }
  pajSelectBank(BANK0);  //gesture flage reg in Bank0
}


void loop()
{

  uint8_t data = 0, error;  
  uint32_t time_start = 0;
  
  time_start = millis();
  error = pajReadData(0x43, 1, &data);
  if (!error) {
    if (data) {
      switch (data) {
        case GES_RIGHT_FLAG:          
		  while ( millis() - time_start <= 500 && data != GES_LEFT_FLAG) {			
			pajReadData(0x43, 1, &data);		    	
	 	  }
		  if(data == GES_LEFT_FLAG) {
			Serial.println("Right-Left");
		  }
		  else {
			Serial.println("Right");
		  }          
          break;
        case GES_LEFT_FLAG:          		  
		  while ( millis() - time_start <= 500 && data != GES_RIGHT_FLAG) {			
			pajReadData(0x43, 1, &data);		    			
	 	  }
		  if(data == GES_RIGHT_FLAG) {
			Serial.println("Left-Right");
		  }
		  else {
			Serial.println("Left");
		  }          
          break;
        case GES_UP_FLAG:
		  while ( millis() - time_start <= 500 && data != GES_DOWN_FLAG) {			
			pajReadData(0x43, 1, &data);		    			
	 	  }
		  if(data == GES_DOWN_FLAG) {
			Serial.println("Up-Down");
		  }
		  else {
            Serial.println("Up");
		  }
          break;
        case GES_DOWN_FLAG:
		  while ( millis() - time_start <= 500 && data != GES_UP_FLAG) {			
			pajReadData(0x43, 1, &data);		    			
	 	  }
		  if(data == GES_UP_FLAG) {
			Serial.println("Down-Up");
		  }
		  else {
            Serial.println("Down");
		  }          
          break;
        case GES_FORWARD_FLAG:
		  while ( millis() - time_start <= 500 && data != GES_BACKWARD_FLAG) {			
			pajReadData(0x43, 1, &data);		    			
	 	  }
		  if(data == GES_BACKWARD_FLAG) {
			Serial.println("beat");
		  }
		  else {
            Serial.println("Forward");
		  }
          break;
        case GES_BACKWARD_FLAG:		  
            Serial.println("Backward");		  
          break;
        case GES_CLOCKWISE_FLAG:
          Serial.println("Clockwise");
          break;
		case GES_COUNT_CLOCKWISE_FLAG:
          Serial.println("anti-clockwise");
          break;  
        default:
          break;
      }
    }
  }
  error = pajReadData(0x44, 1, &data);
  if (!error) {
    if (data == GES_WAVE_FLAG) {
      Serial.println("wave");

    }
  }
  delay(100);
}

/********************************************************
Description:PAJ7620 REG INIT
Input:none
Return: error code, no error return 0;
********************************************************/
uint8_t pajInit(void) {
  //Near_normal_mode_V5_6.15mm_121017 for 940nm
  int i = 0;
  uint8_t error;
  uint8_t data0 = 0, data1 = 0;
  //wakeup the sensor
  delay(1);
  Serial.println("INIT SENSOR...");
  if(!pajWakeUp()) {
	  Serial.println("err: wakeup failed!");
  }

  pajSelectBank(BANK0);
  pajSelectBank(BANK0);

  error = pajReadData(0, 1, &data0);
  if (error) {
    return error;
  }
  error = pajReadData(1, 1, &data1);
  if (error) {
    return error;
  }
  Serial.print("Addr0 =");
  Serial.print(data0 , HEX);
  Serial.print(",  Addr1 =");
  Serial.println(data1 , HEX);

  if ( (data0 != 0x20 ) || (data1 != 0x76) )
  {
    return 0xff;
  }

  for (i = 0; i < INIT_REG_ARRAY_SIZE; i++) {
    pajWriteCmd(init_register_array[i][0], init_register_array[i][1]);
  }
  Serial.println("Paj7620 initialize register.");
  return 0;
}

/********************************************************
Description:PAJ7620 wake up command
Input:none
Return: uint8_t
********************************************************/
__inline uint8_t pajWakeUp() {
  uint8_t error;
  uint8_t data;  
  Wire.beginTransmission(PAJ7620_ID);
  error = Wire.endTransmission();
#ifdef debug  //debug
  Serial.print("wakeup error code:");
  Serial.println(error);
#endif
  Wire.requestFrom((int)PAJ7620_ID, 1);
  while (Wire.available()) {
     data = Wire.read();  
  }
  if(data != 0x20) {
    return 0xff;
  }

  delay(1);
  return 0;
}
/********************************************************
Description:PAJ7620 select register bank
Input:BANK0, BANK1
Return: none
********************************************************/
__inline void pajSelectBank(uint8_t bank) {
  pajWriteCmd(0xEF, bank);
}

/********************************************************
Description:PAJ7620 Write reg cmd
Input:addr:reg address; cmd:function data
Return: error code; no error return 0
********************************************************/
uint8_t pajWriteCmd(uint8_t addr, uint8_t cmd) {
  Wire.beginTransmission(PAJ7620_ID);
  //write cmd
  Wire.write(addr);
  Wire.write(cmd);
  //return error code
  return Wire.endTransmission();
}

/********************************************************
Description:PAJ7620 read reg data
Input:addr:reg address;
      qty:number of data to read, addr continuously increase;
      data[]:storage memory start address
Return:error code; no error return 0
********************************************************/
uint8_t pajReadData(uint8_t addr, uint8_t qty, uint8_t data[]) {
  uint8_t error;
  Wire.beginTransmission(PAJ7620_ID);
  Wire.write(addr);
  error = Wire.endTransmission();

  if (error) return error; //return error code

  Wire.requestFrom((int)PAJ7620_ID, (int)qty);

  while (Wire.available()) {
    *data = Wire.read();

#ifdef debug    //debug
    Serial.print("addr:");
    Serial.print(addr++, HEX);
    Serial.print("  data:");
    Serial.println(*data, HEX);
#endif

    data++;
  }
  return 0;
}