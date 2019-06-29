#ifndef _ARDU_PHYSICS_
#define _ARDU_PHYSICS_

#include <Encoder.h>
#include "rgb_lcd.h"
#include <Ultrasonic.h>
#include <Wire.h>
#include "sensors.h"
#include <math.h>
#include <Time.h>
#include <TimeLib.h>
#include "infrared_temperature.h"

//Encoder
#define pinclk 16
#define pindt 17
#define pinsw 12

//====Variables globales para el sensor de temperatura infrarojo==== //
extern float temp_calibration;                                       //
//float objt_calibration=0.000;                                      //   
extern float temperature_range;                                      //
extern float offset_vol;                                             //
                                                                     //
extern float current_temp;                                           //
extern float temp, temp1, temp2;                                     //
extern int temp3;                                                    // 
extern const float reference_vol;                                    // 
extern float voltage;                                                //
extern long res[100];                                                //       
extern float obj[13][12];                                            //
//====================================================================
extern String firstLine, secondLine; // se usan para enviar los mensajes al LCD
extern rgb_lcd lcd;
extern long encoder_inicio;
extern int posicion_menu;
extern String Listado_menu[9];
extern Encoder myEnc;


void clean_buff(void);
void lcd_mensage(String firstLine = "         ", String secondLine = "         ");
void Ardu_mensage(String sensor);

#endif
