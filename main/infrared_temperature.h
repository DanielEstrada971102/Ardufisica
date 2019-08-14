#ifndef _INFRARED_TEMPERATURE_
#define _INFRARED_TEMPERATURE_

//====================Prototipes =========================
float binSearch(long x);
float arraysearch(float x, float y);
void measureSurTemp(float *final_temp, int SUR);
void measureObjectTemp(float *final_temp1, int OBJ);

#endif
