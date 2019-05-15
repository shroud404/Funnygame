#include "Adafruit_TCS34725.h"

/*example*****************************************************************

#include "mbed.h"
#include "Adafruit_TCS34725.h"

#define commonAnode true

I2C i2c(p28, p27);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(&i2c, TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int main()
{
    char gammatable[256];
    if (tcs.begin())
    {
        printf("Found sensor");
    }
    else
    {
        printf("No TCS34725 found ... check your connections");
        while (1); // halt!
    }
    for (int i=0; i<256; i++)
    {
        float x = i;
        x /= 255;
        x = pow((double)x, 2.5);
        x *= 255;
        if (commonAnode)
        {
            gammatable[i] = 255 - x;
        }
        else
        {
            gammatable[i] = x;      
        }
        printf("%d\r\n", gammatable[i]);
    }
    while(1)
    {
        uint16_t clear, red, green, blue;
        tcs.setInterrupt(false);      // turn on LED
        tcs.getRawData(&red, &green, &blue, &clear);
        tcs.setInterrupt(true);  // turn off LED
        printf("%d, %d, %d, %d\r\n", clear, red, green, blue);
        // Figure out some basic hex code for visualization
        uint32_t sum = clear;
        float r, g, b;
        r = red; r /= sum;
        g = green; g /= sum;
        b = blue; b /= sum;
        r *= 256; g *= 256; b *= 256;
    }
}


*************************************************************************/

void Adafruit_TCS34725::write8 (uint8_t reg, uint32_t value)
{
    char data[2] = {TCS34725_COMMAND_BIT | reg, value & 0xFF};
    _i2c->write(TCS34725_ADDRESS, data, 2);
}

uint8_t Adafruit_TCS34725::read8(uint8_t reg)
{
    char data[2] = {TCS34725_COMMAND_BIT | reg, 0};
    char r_data = 0;
    _i2c->write(TCS34725_ADDRESS, data, 1);
    _i2c->read(TCS34725_ADDRESS, &r_data, 1);
    
    return r_data;
}

uint16_t Adafruit_TCS34725::read16(uint8_t reg)
{
    uint16_t x; uint16_t t;
    char data[2] = {TCS34725_COMMAND_BIT | reg, 0};
    char r_data[2] = {};
    _i2c->write(TCS34725_ADDRESS, data, 1);
    _i2c->read(TCS34725_ADDRESS, r_data, 2);
    t = r_data[0];
    x = r_data[1];
    x <<= 8;
    x |= t;
    return x;
}

void Adafruit_TCS34725::enable(void)
{
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    wait_ms(3);
    write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}

void Adafruit_TCS34725::disable(void)
{
    /* Turn the device off to save power */
    uint8_t reg = 0;
    reg = read8(TCS34725_ENABLE);
    write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

Adafruit_TCS34725::Adafruit_TCS34725(I2C *i2c, tcs34725IntegrationTime_t it, tcs34725Gain_t gain) 
{
    _i2c = i2c;
    _tcs34725Initialised = false;
    _tcs34725IntegrationTime = it;
    _tcs34725Gain = gain;
}

bool Adafruit_TCS34725::begin(void) 
{
    /* Make sure we're actually connected */
    uint8_t x = read8(TCS34725_ID);
    //Serial.println(x, HEX);
    if (x != 0x44)
    {
        return false;
    }
    _tcs34725Initialised = true;
    
    /* Set default integration time and gain */
    setIntegrationTime(_tcs34725IntegrationTime);
    setGain(_tcs34725Gain);
    
    /* Note: by default, the device is in power down mode on bootup */
    enable();
    
    return true;
}

void Adafruit_TCS34725::setIntegrationTime(tcs34725IntegrationTime_t it)
{
    if (!_tcs34725Initialised) begin();
    
    /* Update the timing register */
    write8(TCS34725_ATIME, it);
    
    /* Update value placeholders */
    _tcs34725IntegrationTime = it;
}

void Adafruit_TCS34725::setGain(tcs34725Gain_t gain)
{
    if (!_tcs34725Initialised) begin();
    
    /* Update the timing register */
    write8(TCS34725_CONTROL, gain);
    
    /* Update value placeholders */
    _tcs34725Gain = gain;
}

void Adafruit_TCS34725::getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
    if (!_tcs34725Initialised) begin();
    
    *c = read16(TCS34725_CDATAL);
    *r = read16(TCS34725_RDATAL);
    *g = read16(TCS34725_GDATAL);
    *b = read16(TCS34725_BDATAL);
  
    /* Set a delay for the integration time */
    switch (_tcs34725IntegrationTime)
    {
        case TCS34725_INTEGRATIONTIME_2_4MS:
            wait_ms(3);
            break;
        case TCS34725_INTEGRATIONTIME_24MS:
            wait_ms(24);
            break;
        case TCS34725_INTEGRATIONTIME_50MS:
            wait_ms(50);
            break;
        case TCS34725_INTEGRATIONTIME_101MS:
            wait_ms(101);
            break;
        case TCS34725_INTEGRATIONTIME_154MS:
            wait_ms(154);
            break;
        case TCS34725_INTEGRATIONTIME_700MS:
            wait_ms(700);
            break;
    }
}

uint16_t Adafruit_TCS34725::calculateColorTemperature(uint16_t r, uint16_t g, uint16_t b)
{
    float X, Y, Z;      /* RGB to XYZ correlation      */
    float xc, yc;       /* Chromaticity co-ordinates   */
    float n;            /* McCamy's formula            */
    float cct;
    
    /* 1. Map RGB values to their XYZ counterparts.    */
    /* Based on 6500K fluorescent, 3000K fluorescent   */
    /* and 60W incandescent values for a wide range.   */
    /* Note: Y = Illuminance or lux                    */
    X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
    Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    Z = (-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b);

    /* 2. Calculate the chromaticity co-ordinates      */
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);
    
    /* 3. Use McCamy's formula to determine the CCT    */
    n = (xc - 0.3320F) / (0.1858F - yc);
    
    /* Calculate the final CCT */
    cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;
    
    /* Return the results in degrees Kelvin */
    return (uint16_t)cct;
}

uint16_t Adafruit_TCS34725::calculateLux(uint16_t r, uint16_t g, uint16_t b)
{
    float illuminance;
    
    /* This only uses RGB ... how can we integrate clear or calculate lux */
    /* based exclusively on clear since this might be more reliable?      */
    illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    
    return (uint16_t)illuminance;
}


void Adafruit_TCS34725::setInterrupt(bool i)
{
    uint8_t r = read8(TCS34725_ENABLE);
    if (i)
    {
        r |= TCS34725_ENABLE_AIEN;
    }
    else
    {
        r &= ~TCS34725_ENABLE_AIEN;
    }
    write8(TCS34725_ENABLE, r);
}

void Adafruit_TCS34725::clearInterrupt(void)
{
    char data[2] = {0x66, 0};
    _i2c->write(TCS34725_ADDRESS, data, 1);
}


void Adafruit_TCS34725::setIntLimits(uint16_t low, uint16_t high)
{
    write8(0x04, low & 0xFF);
    write8(0x05, low >> 8);
    write8(0x06, high & 0xFF);
    write8(0x07, high >> 8);
}
