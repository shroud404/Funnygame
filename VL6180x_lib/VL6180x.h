/**
* @file VL6180x.h
 * @brief Library for VL6180x time of flight range finder.
 * Casey Kuhns @ SparkFun Electronics
 * 10/29/2014
 * https://github.com/sparkfun/
 * 
 * The VL6180x by ST micro is a time of flight range finder that
 * uses pulsed IR light to determine distances from object at close
 * range.  The average range of a sensor is between 0-200mm
 * 
 * In this file are the function prototypes in the VL6180x class
 * 
 * Resources:
 * This library uses the Arduino Wire.h to complete I2C transactions.
 * 
 * Development environment specifics:
 *  IDE: Arduino 1.0.5
 *  Hardware Platform: Arduino Pro 3.3V/8MHz
 *  VL6180x Breakout Version: 1.0
 * 
 * Some settings and initial values come from code written by Kris Winer
 * VL6180X_t3 Basic Example Code
 * by: Kris Winer
 * date: September 1, 2014
 * license: Beerware - Use this code however you'd like. If you 
 * find it useful you can buy me a beer some time.
 * 
 * This code is beerware. If you see me (or any other SparkFun employee) at the
 * local pub, and you've found our code helpful, please buy us a round!
 * 
 * Distributed as-is; no warranty is given.
*/
#include "mbed.h"
#ifndef VL6180x_h
#define VL6180x_h

#define VL6180x_FAILURE_RESET  -1

#define VL6180X_IDENTIFICATION_MODEL_ID              0x0000
#define VL6180X_IDENTIFICATION_MODEL_REV_MAJOR       0x0001
#define VL6180X_IDENTIFICATION_MODEL_REV_MINOR       0x0002
#define VL6180X_IDENTIFICATION_MODULE_REV_MAJOR      0x0003
#define VL6180X_IDENTIFICATION_MODULE_REV_MINOR      0x0004
#define VL6180X_IDENTIFICATION_DATE                  0x0006 //16bit value
#define VL6180X_IDENTIFICATION_TIME                  0x0008 //16bit value

#define VL6180X_SYSTEM_MODE_GPIO0                    0x0010
#define VL6180X_SYSTEM_MODE_GPIO1                    0x0011
#define VL6180X_SYSTEM_HISTORY_CTRL                  0x0012
#define VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO         0x0014
#define VL6180X_SYSTEM_INTERRUPT_CLEAR               0x0015
#define VL6180X_SYSTEM_FRESH_OUT_OF_RESET            0x0016
#define VL6180X_SYSTEM_GROUPED_PARAMETER_HOLD        0x0017

#define VL6180X_SYSRANGE_START                       0x0018
#define VL6180X_SYSRANGE_THRESH_HIGH                 0x0019
#define VL6180X_SYSRANGE_THRESH_LOW                  0x001A
#define VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD     0x001B
#define VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME        0x001C
#define VL6180X_SYSRANGE_CROSSTALK_COMPENSATION_RATE 0x001E
#define VL6180X_SYSRANGE_CROSSTALK_VALID_HEIGHT      0x0021
#define VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE  0x0022
#define VL6180X_SYSRANGE_PART_TO_PART_RANGE_OFFSET   0x0024
#define VL6180X_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT   0x0025
#define VL6180X_SYSRANGE_RANGE_IGNORE_THRESHOLD      0x0026
#define VL6180X_SYSRANGE_MAX_AMBIENT_LEVEL_MULT      0x002C
#define VL6180X_SYSRANGE_RANGE_CHECK_ENABLES         0x002D
#define VL6180X_SYSRANGE_VHV_RECALIBRATE             0x002E
#define VL6180X_SYSRANGE_VHV_REPEAT_RATE             0x0031

#define VL6180X_SYSALS_START                         0x0038
#define VL6180X_SYSALS_THRESH_HIGH                   0x003A
#define VL6180X_SYSALS_THRESH_LOW                    0x003C
#define VL6180X_SYSALS_INTERMEASUREMENT_PERIOD       0x003E
#define VL6180X_SYSALS_ANALOGUE_GAIN                 0x003F
#define VL6180X_SYSALS_INTEGRATION_PERIOD            0x0040

#define VL6180X_RESULT_RANGE_STATUS                  0x004D
#define VL6180X_RESULT_ALS_STATUS                    0x004E
#define VL6180X_RESULT_INTERRUPT_STATUS_GPIO         0x004F
#define VL6180X_RESULT_ALS_VAL                       0x0050
#define VL6180X_RESULT_HISTORY_BUFFER                0x0052 
#define VL6180X_RESULT_RANGE_VAL                     0x0062
#define VL6180X_RESULT_RANGE_RAW                     0x0064
#define VL6180X_RESULT_RANGE_RETURN_RATE             0x0066
#define VL6180X_RESULT_RANGE_REFERENCE_RATE          0x0068
#define VL6180X_RESULT_RANGE_RETURN_SIGNAL_COUNT     0x006C
#define VL6180X_RESULT_RANGE_REFERENCE_SIGNAL_COUNT  0x0070
#define VL6180X_RESULT_RANGE_RETURN_AMB_COUNT        0x0074
#define VL6180X_RESULT_RANGE_REFERENCE_AMB_COUNT     0x0078
#define VL6180X_RESULT_RANGE_RETURN_CONV_TIME        0x007C
#define VL6180X_RESULT_RANGE_REFERENCE_CONV_TIME     0x0080

#define VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD      0x010A
#define VL6180X_FIRMWARE_BOOTUP                      0x0119
#define VL6180X_FIRMWARE_RESULT_SCALER               0x0120
#define VL6180X_I2C_SLAVE_DEVICE_ADDRESS             0x0212
#define VL6180X_INTERLEAVED_MODE_ENABLE              0x02A3

/**
* gain settings for ALS
*Data sheet shows gain values as binary list
*/
enum vl6180x_als_gain { 

GAIN_20 = 0, // Actual ALS Gain of 20
GAIN_10,     // Actual ALS Gain of 10.32
GAIN_5,      // Actual ALS Gain of 5.21
GAIN_2_5,    // Actual ALS Gain of 2.60
GAIN_1_67,   // Actual ALS Gain of 1.72
GAIN_1_25,   // Actual ALS Gain of 1.28
GAIN_1 ,     // Actual ALS Gain of 1.01
GAIN_40,     // Actual ALS Gain of 40

};
/**
* VL6180xIdentification
* @brief structure to hold module identification
*
* @param idModel Model number
* @param idModelRevMajor Major revision
* @param idModelRevMinor Minor revision
* @param idModuleRevMajor Module Major revision
* @param idModuleRevMinor Module Minor revision
* @param idDate Date of manufacture
* @param idTime Seconds after midnight manufacture
*/
struct VL6180xIdentification {
  uint8_t idModel;
  uint8_t idModelRevMajor;
  uint8_t idModelRevMinor;
  uint8_t idModuleRevMajor;
  uint8_t idModuleRevMinor;
  uint16_t idDate;
  uint16_t idTime;
};
/**
* VL6180x tof/als sensor example
*
* @code
#include "mbed.h"
#include <VL6180x.h>

const float GAIN_1    = 1.01;  // Actual ALS Gain of 1.01
const float GAIN_1_25 = 1.28;  // Actual ALS Gain of 1.28
const float GAIN_1_67 = 1.72;  // Actual ALS Gain of 1.72
const float GAIN_2_5  = 2.6;   // Actual ALS Gain of 2.60
const float GAIN_5    = 5.21;  // Actual ALS Gain of 5.21
const float GAIN_10   = 10.32; // Actual ALS Gain of 10.32
const float GAIN_20   = 20;    // Actual ALS Gain of 20
const float GAIN_40   = 40;    // Actual ALS Gain of 40


#define VL6180X_ADDRESS 0x29

VL6180xIdentification identification;
// mbed uses 8bit addresses shift address by 1 bit left
VL6180x sensor(D14, D15, VL6180X_ADDRESS<<1);

void printIdentification(struct VL6180xIdentification *temp){
  printf("Model ID = ");
  printf("%d\n",temp->idModel);

  printf("Model Rev = ");
  printf("%d",temp->idModelRevMajor);
  printf(".");
  printf("%d\n",temp->idModelRevMinor);

  printf("Module Rev = ");
  printf("%d",temp->idModuleRevMajor);
  printf(".");
  printf("%d\n",temp->idModuleRevMinor);  

  printf("Manufacture Date = ");
  printf("%d",((temp->idDate >> 3) & 0x001F));
  printf("/");
  printf("%d",((temp->idDate >> 8) & 0x000F));
  printf("/1");
  printf("%d\n",((temp->idDate >> 12) & 0x000F));
  printf(" Phase: ");
  printf("%d\n",(temp->idDate & 0x0007));

  printf("Manufacture Time (s)= ");
  printf("%d\n",(temp->idTime * 2));
  printf("\n\n");
}
int main() {

  wait_ms(100); // delay .1s

  sensor.getIdentification(&identification); // Retrieve manufacture info from device memory
  printIdentification(&identification); // Helper function to print all the Module information

  if(sensor.VL6180xInit() != 0){
        printf("FAILED TO INITALIZE\n"); //Initialize device and check for errors
  }; 

  sensor.VL6180xDefautSettings(); //Load default settings to get started.
  
    wait_ms(1000); // delay 1s

    
    
    while(1) {
  //Get Ambient Light level and report in LUX
      printf("Ambient Light Level (Lux) = ");
  
  //Input GAIN for light levels, 
  // GAIN_20     // Actual ALS Gain of 20
  // GAIN_10     // Actual ALS Gain of 10.32
  // GAIN_5      // Actual ALS Gain of 5.21
  // GAIN_2_5    // Actual ALS Gain of 2.60
  // GAIN_1_67   // Actual ALS Gain of 1.72
  // GAIN_1_25   // Actual ALS Gain of 1.28
  // GAIN_1      // Actual ALS Gain of 1.01
  // GAIN_40     // Actual ALS Gain of 40
  
      printf("%f\n",sensor.getAmbientLight(GAIN_1) );

  //Get Distance and report in mm
      printf("Distance measured (mm) = ");
      printf("%d\n", sensor.getDistance() ); 

      wait_ms(500);  
    }
}
* @endcode
*/
class VL6180x
{
public: 
/**
* VL6180x constructor
*
* @param sda SDA pin
* @param sdl SCL pin
* @param addr addr of the I2C peripheral
*/
  VL6180x(PinName sda, PinName scl, uint8_t addr);
/**
* VL6180x destructor
*/
  ~VL6180x();
/**
* Send mandatory settings as stated in ST datasheet.
* http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf (Section 1.3)
* @returns 0 if ok 
* @returns -1 on error*/
  int VL6180xInit(void);
/**
* Use default settings from ST data sheet section 9.
*
* http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
*/
  void VL6180xDefautSettings(void);
/**
* Get Range distance in (mm)
* @returns TOF distance in mm  
*/
  uint8_t getDistance();
/**
* Get ALS level in Lux
* @param vl6180x_als_gain gain setting for sensor
* @param sdl SCL pin
* @param addr addr of the I2C peripheral
* @returns light level in Lux 
*/
  float getAmbientLight(vl6180x_als_gain VL6180X_ALS_GAIN);
/**
* Load structure provided by the user with identification info
* Structure example:
* struct VL6180xIdentification
*/
  void getIdentification(struct VL6180xIdentification *temp);

/**
* Change the default address of the device to allow multiple
* sensors on the bus.  Can use up to 127 sensors. New address
* is saved in non-volatile device memory.
* @param old_address current address
* @param new_address desired new address
* @returns new address
*/
  uint8_t changeAddress(uint8_t old_address, uint8_t new_address);


private:
  //Store address given when the class is initialized.
  //This value can be changed by the changeAddress() function
  I2C m_i2c;
  int m_addr;
/**
* read 8 bit register
* @param registerAddr address for register
* @returns contents of register
*/
  uint8_t VL6180x_getRegister(uint16_t registerAddr);
/**
* read 16 bit register
* @param registerAddr address for register
* @param returns contents of register
*/
uint16_t VL6180x_getRegister16bit(uint16_t registerAddr);
/**
* write 8 bit register
* @param registerAddr address for register
* @returns ERROR
*/
  void VL6180x_setRegister(uint16_t registerAddr, uint8_t data);
/**
* write 16 bit register
* @param registerAddr address for register
* @returns ERROR
*/
  void VL6180x_setRegister16bit(uint16_t registerAddr, uint16_t data);
};

#endif
