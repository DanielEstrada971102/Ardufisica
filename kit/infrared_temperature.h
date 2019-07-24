#ifndef _INFRARED_TEMPERATURE_
#define _INFRARED_TEMPERATURE_

float binSearch(long x);
float arraysearch(float x, float y);
void measureSurTemp(float *final_temp, int PIN_SUR);
void measureObjectTemp(float *final_temp, int PIN_OBJ);

#endif
