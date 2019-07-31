#ifndef _ARDU_PHYSICS_
#define _ARDU_PHYSICS_

//==================Libraries=========================
#include <Encoder.h>
#include "rgb_lcd.h"
#include <Ultrasonic.h>
#include <Wire.h>
#include "sensors.h"
#include <math.h>
#include "infrared_temperature.h"
#include "max6675.h" //para uso de termocupla tipo K
#include <BH1750.h> // Para el sensor de luminosidad
#include <Servo.h> //libreria para servomotor
//====================================================
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
extern int calibrar_temp;                                            //
//====================================================================

extern Encoder myEnc;
extern rgb_lcd lcd;
extern int continuar;
extern long encoder_inicio;
extern int posicion_menu;
extern String Listado_menu[18];
extern String Listado_sensors[18];
extern String firstLine, secondLine; // se usan para enviar los mensajes al LCD

//====================Prototipes =========================
void clean_buff(void);
void lcd_mesagge(String firstLine = "                ", 
				 String secondLine = "                ");
void Ardu_mesagge(String sensor);
void Encoder_menu(int infLim, int supLim, int *option, int step = 1);
String serial_readPhrase();
void run_sensor(int posicion_menu);
void info();
#endif
