#include "ardufisica.h"

Encoder myEnc(pinclk, pindt);
rgb_lcd lcd;
long encoder_inicio = 0;
int posicion_menu = 0;
String firstLine, secondLine;
String Listado_menu[18] = {"<Microfono     >", "<Sensor Hall   >", "<Gen. sonido   >", "<Distancia     >", 
                           "<Temperatura IR>", "<Sensor color  >", "<Calidad aire  >", "<Fotorresisten.>", 
                           "<Ritmo Cardiaco>", "<Respuesta Galv>", "<Termocupla K  >", "<Electroimán   >", 
                           "<Instensid. Luz>", "<Micro Servo   >", "<Atomizador    >", "<Detector EMG  >", 
                           "<Led R_G_B     >", "<Tacómetro     >" };

void clean_buff(void){
    encoder_inicio = 0;
    lcd.clear();
    Serial.read();//Limpia el buffer serial
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
}

void Encoder_menu(int infLim, int supLim,int *option, int step){
		
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

