#include <Encoder.h>
#include <Ultrasonic.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>

//Encoder
#define pinclk 16
#define pindt 17
#define pinsw 12


class ArduPhysic
{
    // private: 
    //     long encoder_inicio = 0;
    //     rgb_lcd lcd;
    //     Encoder MyEnc(pinclk, pindt);
    public:
        void clean_buff(void);
        void start_mensage(char firstLine[], char secondLine[]);
        //sensors
        void microphone(void);
        void sensor_hall(void);
        void distancia_ultrasonido();
        void generador_sonido(int frec)
};

void ArduPhysic::clean_buff(void){
    
    encoder_inicio = 0;
    lcd.clear();
    Serial.read();//Limpia el buffer serial
    delay(100);
    
    if (digitalRead(pinsw) == 0){
        while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
    }
    delay(500);
}
void ArduPhysic::start_mensage(char firstLine[], char secondLine[]){
    lcd.setCursor(0,0);
    lcd.print(firstLine);
    lcd.setCursor(0,1);
    lcd.print(secondLine);
}

void ArduPhysic::microphone(void){
    char firstLine[] = {"Enviando datos  "};
    char secondLine[] = {"al PC...        "};

    ArduPhysic::start_mensage(firstLine, secondLine);
    while(Serial.available() == 0 && digitalRead(pinsw) == 1){
        Serial.println(analogRead(A12));
    //Sample rate in Arduino MEGA is 9615 Hz
    }
    ArduPhysic::clean_buff();
}

void ArduPhysic::sensor_hall(void){
    float factor=5000.0/(1023.0*1.3);//1.3mV/Gauss , 5V->1023
    char firstLine[] = {"Campo magnetico:"};
    char secondLine[] = {"Calib. campo 0.."};
    
    ArduPhysic::start_mensage(firstLine, secondLine);
    delay(1000);

    int V_0 = analogRead(A14);
    unsigned long t_inicial=millis();
    unsigned long t;
    int campo;
    
    while(Serial.available()==0&& digitalRead(pinsw)==1){
        t = (millis() - t_inicial) / 1000.0;
        campo = (analogRead(A14) - V_0) * factor;
        Serial.print(t);//FALTA EL FACTOR DE CONVERSIÓN
        Serial.print('\t');
        Serial.println(campo);
        
        lcd.setCursor(0,1);
        lcd.print(campo);
        lcd.print("        ");
        lcd.setCursor(4,1);
        lcd.print("   G   -A14-");
        delay(500);
    }
    ArduPhysic::clean_buff();  
}

void ArduPhysic::distancia_ultrasonido(){

    char firstLine[] = {"Distancia: "};
    char secondLine[] = {"          "};

    ArduPhysic::start_mensage(firstLine, secondLine);
    
    unsigned long t_inicial = millis();
    unsigned long t;
    
    int distancia;
    
    while(Serial.available()==0&& digitalRead(pinsw)==1){
        t = (millis() - t_inicial) / 1000.0;
        distancia = ultrasonido.Ranging(CM);
        Serial.print(t);
        Serial.print('\t');
        Serial.println(distancia);
        lcd.setCursor(0,1);
        lcd.print(distancia);
        lcd.print("        ");
        lcd.setCursor(4,1);
        lcd.print("   cm  -D6 -");
        delay(500);
    }
    ArduPhysic::clean_buff();
}

void ArduPhysic::generador_sonido(int frec){ 
    boolean salir = false;
    int frecuencia = frec;

    char firstLine[] = {"Generador tonos: "};
    char secondLine[16];
    sprintf(secondLine,"%d     Hz    -D4-", frecuencia);

    tone(4, frecuencia);
    ArduPhysic::start_mensage(firstLine, secondLine);
  
    while (salir == false && digitalRead(pinsw)==1){
        if(Serial.available()){
            char caracter = Serial.read();
            if(caracter == 'x'){
                noTone(4);
                lcd.clear();
                salir=true;
            }
            else if (caracter == 'g'){
                while(Serial.available() == 0);
                frecuencia = Serial.parseInt();
                tone(4, frecuencia);
                lcd.setCursor(0,1);
                lcd.print(secondLine);   
            }
        }
    }
    ArduPhysic::clean_buff();
    noTone(4);
}


