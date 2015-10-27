#ifndef PWM_H_
#define PWM_H_



void initPWM(int module, int channel, int centerAligned, int prescaleShift, int lowTrue, int TOIE, int CHIE, uint32_t period, uint32_t pulseWidth);
void setPeriod(int module, uint32_t countTo);
void setPulseWidth(int module, int channel, uint32_t pulseWidth);
void setTone(int module, int channel, int freq);
void enablePWM(int module);
void disablePWM(int module);

#endif
