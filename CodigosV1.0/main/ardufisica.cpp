#include "ardufisica.h"

Encoder myEnc(pinclk, pindt);
rgb_lcd lcd;
long encoder_inicio = 0;
int posicion_menu = 0;
String firstLine, secondLine;
String Listado_menu[18] = {"<Microfono     >", "<Sensor Hall   >", "<Gen. sonido   >", 
                           "<Distancia     >", "<Temperatura IR>", "<Sensor color  >", 
                           "<Calidad aire  >", "<Fotorresisten.>", "<Ritmo Cardiaco>", 
                           "<Respuesta Galv>", "<Termocupla K  >", "<Instensid. Luz>", 
                           "<Micro Servo   >", "<Atomizador    >", "<Detector EMG  >", 
                           "<Led R_G_B     >", "<Tacometro     >", "<   Mas Info   >" };

String Listado_sensors[18] = {"microfono", "hall", "gen.sonido", "distancia", "temperaturaIR", 
                              "color", "calidad", "fotorresistencia", "cardiaco", "gsr",
                              "temperaturaK", "luz", "servo", "atomizador", "emg", "ledRGB", 
                              "tacometro", "help"};
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
    Serial.println("Salio del sensor...");
    Serial1.println("Salio del sensor...");
    delay(500);
}

void lcd_mesagge(String firstLine, String secondLine){  
	lcd.setCursor(0,0);
    lcd.print(firstLine);
    lcd.setCursor(0,1);
    lcd.print(secondLine);
}
void Ardu_mesagge(){    

    Serial.println("Universidad de Antioquia - Instituto de Física");
    Serial.println("Interfaz ArduFísica");
    Serial.println("Si desea controlar el modulo de forma remota puede usar alguna de las siguientes instrucciones: ");
    //Bluetooth----------------
    Serial1.println("Universidad de Antioquia - Instituto de Física");
    Serial1.println("Interfaz ArduFísica: ");
    Serial1.println("Si desea controlar el modulo de forma remota puede usar alguna de las siguientes instrucciones: ");
    
    for (int i = 0; i < 18; i++){
    	if (i == 17){
    		Serial.println(Listado_sensors[i]);
    		Serial1.println(Listado_sensors[i]);
    	}
    	else{
    		Serial.print(Listado_sensors[i] + ", ");
    		Serial1.print(Listado_sensors[i] + ", ");
    	}
    }

    Serial.println("Para detener la ejecucion de cada sensor escriba 'stop'. ");
    Serial1.println("Para detener la ejecucion de cada sensor escriba 'stop'. ");
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
    char c;

    //======Mensaje po Serial============
    if(Serial.available()){
        while(Serial.available()){
            c = Serial.read();
            if(c != '\r' and c!= '\n')
                phrase.concat(c);
        }
    }
    //======Mensaje po Bluetooth=========
    else if(Serial1.available()){
        while(Serial1.available()){
            c = Serial1.read();
            if(c != '\r' and c!= '\n')
                phrase.concat(c);
        }
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
            Serial.print("Error... Sensor no existe\n");
            Serial1.print("Error... Sensor no existe\n");
            break;
    }
}

void info(){
    int i = 0;
    firstLine = "Para mas informacion de los sensores, de la programacion y de como funciona el modulo Ardufisica, visite el siguiete repositorio:   https://github.com/DanielEstrada971102/Ardufisica    ";
    Serial.print(firstLine + "\n");
    Serial1.print(firstLine + "\n");

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
