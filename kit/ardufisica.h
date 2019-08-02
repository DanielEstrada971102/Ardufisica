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
//Pines Encoder
#define ENCODER_DO_NOT_USE_INTERRUPTS//Para que libreria no use interrupciones
#define pinclk 16
#define pindt 17
#define pinsw 12

// Variables globales==================================
extern Encoder myEnc;
extern rgb_lcd lcd;
extern long encoder_inicio;
extern int posicion_menu;
extern String Listado_menu[18];
extern String Listado_sensors[18];
extern String firstLine, secondLine; // se usan para enviar los mensajes al LCD

//====================Prototipos =========================
void clean_buff(void);
void lcd_mesagge(String firstLine = "                ", 
				 String secondLine = "                ");
void Ardu_mesagge();
void Encoder_menu(int infLim, int supLim, int *option, int step = 1);
String serial_readPhrase();
void run_sensor(int posicion_menu);
void info();
#endif
