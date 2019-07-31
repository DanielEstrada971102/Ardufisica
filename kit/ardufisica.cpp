#include "ardufisica.h"

Encoder myEnc(pinclk, pindt);
rgb_lcd lcd;
int continuar = 0;
long encoder_inicio = 0;
int posicion_menu = 0;
String firstLine, secondLine;
String Listado_menu[18] = {"<Microfono     >", "<Sensor Hall   >", "<Gen. sonido   >", "<Distancia     >", 
                           "<Temperatura IR>", "<Sensor color  >", "<Calidad aire  >", "<Fotorresisten.>", 
                           "<Ritmo Cardiaco>", "<Respuesta Galv>", "<Termocupla K  >", "<Instensid. Luz>", 
                           "<Micro Servo   >", "<Atomizador    >", "<Detector EMG  >", "<Led R_G_B     >", 
                           "<Tacometro     >", "<   Mas Info   >" };

String Listado_sensors[18] = {"microf", "hall", "gen.son", "distan", 
                           "tempIR", "color", "calidad", "fotorres", 
                           "cardiaco", "gsr", "tempK", "luz", "servo", "atomiza", "emg", 
                           "ledRGB", "tacomet", "help"};
void clean_buff(void){
    lcd.clear();
    Serial.read();//Limpia el buffer serial
    Serial1.read();
    delay(100);
    
    if (digitalRead(pinsw) == 0){   
        lcd_mesagge(" Suelte el boton  "); 
        while(digitalRead(pinsw) == 0);//Hasta que no deje de presionar el encoder no sale de la función
        lcd.clear();
    }
    delay(500);
}

void lcd_mesagge(String firstLine, String secondLine){  
	lcd.setCursor(0,0);
    lcd.print(firstLine);
    lcd.setCursor(0,1);
    lcd.print(secondLine);
}
void Ardu_mesagge(String sensor){    

    Serial.println("Universidad de Antioquia - Instituto de Física");
    Serial.print("Interfaz ArduFísica: ");
    Serial.println(sensor);
    //Bluetooth----------------
    Serial1.println("Universidad de Antioquia - Instituto de Física");
    Serial1.print("Interfaz ArduFísica: ");
    Serial1.println(sensor);
}

void Encoder_menu(int infLim, int supLim, int *option, int step){
		
	if(abs(myEnc.read() - encoder_inicio)> 3){
		if(myEnc.read() - encoder_inicio > 0)
			*option = *option + step;
		else
			*option = *option - step;

		if (*option > supLim)
			*option = infLim;
		if (*option < infLim)
			*option = supLim;
     
	  encoder_inicio = myEnc.read();
	}
}

String serial_readPhrase(){
    String phrase = "";
    int i = 0;
    char c;
   
    while(Serial.available()){
        c = Serial.read();
        if(c != '\n')
            phrase.concat(c);
    }
    return phrase;       
}

void run_sensor(int posicion_menu){
    
    while(digitalRead(pinsw)==0);
    lcd.clear();
    
    switch(posicion_menu){
        case 0:
            microfono();
            break;
        case 1:
            hall_magnetico();
            break;
        case 2:
            generador_sonido();
            break;
        case 3:
            distancia_ultrasonido();
            break;
        case 4:
            temperatura_infrarrojo();
            break;
        case 5:
            color();
            break;
        case 6:
            calidad_aire();
            break;
        case 7:
            photoResitor();
            break;
        case 8:
            ritmo_cardiaco();
            break;
        case 9:
            conductancia();
            break;
        case 10:
            Termocupla_k();
            break;
        case 11:
            luminosidad();
            break;
        case 12:
            Servomotor();
            break;
        case 13:
            atomizador();
            break;
        case 14:
            EMG_detector();
            break;
        case 15:
            Led_RGB();
            break;
        case 16:
            Tacometro();
            break;
        case 17:
            info();
            break;
        default:
            Serial.print("Error...\n Sensor no existe");
            Serial1.print("Error...\n Sensor no existe");
    }
}

void info(){
    int i = 0;
    firstLine = "Para mas informacion de los sensores, de la programacion y de como funciona el modulo Ardufisica, visite el siguiete repositorio:   https://github.com/DanielEstrada971102/Ardufisica    ";
    delay(1000);
    while(digitalRead(pinsw) == 1){
        if (i + 16 < firstLine.length())
            lcd_mesagge(firstLine.substring(i, i + 16));
        else
            i = 0;
        i++;
        delay(150);
    }
    clean_buff();
}
