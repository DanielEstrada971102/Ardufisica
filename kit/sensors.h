#ifndef _SENSORS_
#define _SENSORS_

void run_sensor(int posicion_menu);
void lee_microfono(int SIG = 12, int Timedelay = 10);
void sensor_hall(int AO = 14, int Timedelay = 500);
void generador_sonido(int SIG = 4);
void distancia_ultrasonido(int Trigger = 6, int Echo = 7);
void temperatura_infrarrojo(int SUR = 0, int OBJ = 1);
void color(int S0 = 39, int S1 = 41, int S2 = 43, int S3 = 45,
		   int SW = 53,int led = 51, int out = 47);
void air_quality(int SIG = 14, int Timedelay = 500);
void photoResitor(int S = 0, int Timedelay = 100);
void ritmo_cardiaco(int Timedelay = 500);
void conductancia(int PIN = 8);
void Termocupla_k(int CONFIG_TCSCK = 11, int CONFIG_TCCS = 10, int CONFIG_TCDO = 53, int Timedelay = 1000);
void Electromagn(int PIN = 2);
void luminosidad();
void Servomotor(int PIN = 2);
void Water_Atomization(int PIN = 8);
#endif
