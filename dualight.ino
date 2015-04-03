// #define SHIFTPWM_USE_TIMER2  // for Arduino Uno and earlier (Atmega328)
// #define SHIFTPWM_USE_TIMER3  // for Arduino Micro/Leonardo (Atmega32u4)


const int ShiftPWM_latchPin = 9;
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>   // include ShiftPWM.h after setting the pins!
#include <ooPinChangeInt.h>
#include <AdaEncoder.h>
#include <Button.h>

unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 100;
unsigned int numRegisters = 2;

const int encoderAPin = 6;
const int encoderBPin = 5;
const int encoderBtnPin = 4;
AdaEncoder encoder = AdaEncoder('x', encoderAPin, encoderBPin);
Button encoderBtn = Button(encoderBtnPin, LOW);

void setup(){
  pinMode(encoderBtnPin, INPUT_PULLUP);
  Serial.begin(115200);
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(pwmFrequency,maxBrightness);
}

unsigned char mode = -1;
// 0: on/off
// 1: hue 1
// 2: sat 1
// 3: val 1
// 4: hue 2
// 5: sat 2
// 6: val 2
// 7: 2 as 1
// 8: 1 as 2
// 9: swap
// 10: both white
// 11: randomize

int ch0[] = {0, 0, 0};
int ch1[] = {0, 0, 0};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
void loop()
{
  encoderBtn.listen();
  if (encoderBtn.onRelease()) {
    setMode((mode + 1) % 256 % 12);
  }
  
  handleMode();

  AdaEncoder *enc = NULL;
  enc = AdaEncoder::genie();
  if (enc != NULL) {
    int modifier = enc->getClearClicks() * 5;
    Serial.print(mode);
    Serial.print(": ");
    switch (mode) {
      case 1:
        ch0[0] = (ch0[0] + modifier + 360) % 360;
        GL_SetHSV(0, ch0[0], ch0[1], ch0[2]);
        break;
      case 2:
        ch0[1] = (ch0[1] + modifier + 256) % 256;
        GL_SetHSV(0, ch0[0], ch0[1], ch0[2]);
        break;
      case 3:
        ch0[2] = (ch0[2] + modifier + 256) % 256;
        GL_SetHSV(0, ch0[0], ch0[1], ch0[2]);
        break;
      case 4:
        ch1[0] = (ch1[0] + modifier + 360) % 360;
        GL_SetHSV(1, ch1[0], ch1[1], ch1[2]);
        break;
      case 5:
        ch1[1] = (ch1[1] + modifier + 256) % 256;
        GL_SetHSV(1, ch1[0], ch1[1], ch1[2]);
        break;
      case 6:
        ch1[2] = (ch1[2] + modifier + 256) % 256;
        GL_SetHSV(1, ch1[0], ch1[1], ch1[2]);
        break;
      case 7:
        break;
      case 8:
        break;
      case 9:
        break;
      case 10:
        ch0[0] = ch1[0] = 0;
        ch0[1] = ch1[1]= 0;
        ch0[2] = ch1[2]= 255;
        GL_SetHSV(0, ch0[0], ch0[1], ch0[2]);
        GL_SetHSV(1, ch1[0], ch1[1], ch1[2]);
        break;
      case 11:
        ch1[2] = (ch1[2] + modifier + 256) % 256;
        GL_SetHSV(1, ch1[0], ch1[1], ch1[2]);
        break;
    }
  }
}

unsigned long startTime = 0;

void setMode(unsigned char newMode)
{
  mode = newMode;
  GL_SetRGB(3, 0, 0, 0);
  GL_SetRGB(4, 0, 0, 0);
  startTime = millis();
}

unsigned long lastIteration = 999;

void handleMode()
{
  float pos;
  switch (mode) {
    case 1:
      GL_SetHSV(3, getPos(2000) * 360, 255, 255);
      break;
    case 2:
      GL_SetHSV(3, 0, splitPos2(getPos(1000)) * 255, 255);
      break;
    case 3:
      GL_SetHSV(3, 0, 255, splitPos2(getPos(1000)) * 255);
      break;
    case 4:
      GL_SetHSV(4, getPos(2000) * 360, 255, 255);
      break;
    case 5:
      GL_SetHSV(4, 0, splitPos2(getPos(1000)) * 255, 255);
      break;
    case 6:
      GL_SetHSV(4, 0, 255, splitPos2(getPos(1000)) * 255);
      break;
    case 7:
      GL_SetHSV(3, 0, 255, 255);
      GL_SetHSV(4, int(240 + getPos(1000) * 120) % 360, 255, 255);
      break;
    case 8:
      GL_SetHSV(4, 240, 255, 255);
      GL_SetHSV(3, int(360 - getPos(1000) * 120) % 360, 255, 255);
      break;
    case 9:
      pos = splitPos2(getPos(1000));
      GL_SetHSV(3, int(360 - pos * 120) % 360, 255, 255);
      GL_SetHSV(4, int(240 + pos * 120) % 360, 255, 255);
      break;
    case 10:
      GL_SetHSV(3, 0, 0, 255);
      GL_SetHSV(4, 0, 0, 255);
      break;
    case 11:
      unsigned long iteration = getIteration(250);
      if (iteration != lastIteration) {
        GL_SetHSV(3, random(360), random(128, 256), 255);
        GL_SetHSV(4, random(360), random(128, 256), 255);
        lastIteration = iteration;
      }
      break;
  }
}

float splitPos2(float pos)
{
   return 1 - abs(pos * 2 - 1);
}

float getPos(int duration)
{
  return (millis() - startTime) % duration / (float)duration;
}

unsigned long getIteration(int duration)
{
  return (millis() - startTime) / duration;
}

const unsigned char PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

const byte white[5][3] = {
  {255, 170, 140},
  {255, 170, 140},
  {0, 0, 0},
  {255, 210, 170},
  {255, 210, 170},
};

unsigned char GL(unsigned char x) {
  return pgm_read_byte(&gamma[x]);
}

void GL_SetAll(unsigned char x) {
  ShiftPWM.SetAll(GL(x));
}

void GL_SetOne(int pin, unsigned char x) {
  ShiftPWM.SetOne(pin, GL(x));
}

void GL_SetRGB(int led, unsigned char r, unsigned char g, unsigned char b) {
  ShiftPWM.SetRGB(led, GL(map(r, 0, 255, 0, white[led][0])), GL(map(g, 0, 255, 0, white[led][1])), GL(map(b, 0, 255, 0, white[led][2])));
}

void GL_SetAllRGB(unsigned char r, unsigned char g, unsigned char b) {
  for (int led = 0; led++; led < 5) {
    GL_SetRGB(led, r, g, b);
  }
}

void GL_SetAllHSV(unsigned int h, unsigned int s, unsigned int v) {
  int rgb[3];
  getRGB(h, s, v, rgb);
  GL_SetAllRGB(rgb[0], rgb[1], rgb[2]);
}

void GL_SetHSV(int led, unsigned int h, unsigned int s, unsigned int v) {
  int rgb[3];
  getRGB(h, s, v, rgb);
  GL_SetRGB(led, rgb[0], rgb[1], rgb[2]);
}

void getRGB(int hue, int sat, int val, int colors[3]) { 
  int r;
  int g;
  int b;
  int base;
 
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 
    base = ((255 - sat) * val)>>8;
 
    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;
 
    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;
 
    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;
 
    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;
 
    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;
 
    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }
 
    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }   
}
