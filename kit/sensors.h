#ifndef _SENSORS_
#define _SENSORS_

//====================Prototipes =========================
void run_sensor(int posicion_menu);
void microfono(int SIG = 12, int Timedelay = 10);
void hall_magnetico(int AO = 14, int Timedelay = 500);
void generador_sonido(int SIG = 4);
void distancia_ultrasonido(int TRIG = 6, int ECHO = 7);
void temperatura_infrarrojo(int SUR = 0, int OBJ = 1);
void color(int S0 = 39, int S1 = 41, int S2 = 43, int S3 = 45,
		   int OE = 4, int led = 5, int out = 47);
void calidad_aire(int SIG = 6, int Timedelay = 500);
void photoResitor(int S = 2, int Timedelay = 100);
void ritmo_cardiaco(int Timedelay = 500);
void conductancia(int PIN = 8);
void Termocupla_k(int CS = 10, int SCK = 11, int SO = 53, int Timedelay = 500);
void electroIman(int PIN = 2);
void luminosidad();
void Servomotor(int PIN = 2);
void atomizador(int PIN = 8);
void EMG_detector(int VOUT = 10, int Timedelay = 100);
void Led_RGB(int R = 2, int G = 3, int B = 4);
void Tacometro(int OUT = 4);
void info();
int getAnalog(int pin);

#endif
