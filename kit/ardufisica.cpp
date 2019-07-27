#include "ardufisica.h"

Encoder myEnc(pinclk, pindt);
rgb_lcd lcd;
long encoder_inicio = 0;
int posicion_menu = 0;
String firstLine, secondLine;
String Listado_menu[18] = {"<Microfono     >", "<Sensor Hall   >", "<Gen. sonido   >", "<Distancia     >", 
                          "<Temperatura IR>", "<Sensor color  >", "<Calidad aire  >", "<Fotorresisten.>", 
                          "<Ritmo Cardiaco>", "<Respuesta Galv>", "<Termocupla K  >", "<Electromagnet >", 
                          "<Instensid. Luz>", "<Micro Servo   >", "<Water Atomizat>", "<EMG Detector  >", 
                          "<Led R_G_B     >", "<Tachometer    >" };

void clean_buff(void){
    encoder_inicio = 0;
    lcd.clear();
    Serial.read();//Limpia el buffer serial
    delay(100);
    
    if (digitalRead(pinsw) == 0){   
        lcd_mensage(" Suelte el boton  "); 
        while(digitalRead(pinsw) == 0);//Hasta que no deje de presionar el encoder no sale de la función
        lcd.clear();
    }
    delay(500);
}

void lcd_mensage(String firstLine, String secondLine){  
	lcd.setCursor(0,0);
    lcd.print(firstLine);
    lcd.setCursor(0,1);
    lcd.print(secondLine);
}
void Ardu_mensage(String sensor){    
    //char fecha[25];
    
    //sprintf(fecha, "%.2d/%.2d/%.4d-%.2d:%.2d:%.2d", day(dateNow), month(dateNow), year(dateNow),
    //                                                hour(dateNow), minute(dateNow), second(dateNow));
    Serial.println("Universidad de Antioquia - Instituto de Física");
    Serial.print("Interfaz ArduFísica: ");
    Serial.println(sensor);
    Serial.print("Fecha: ");
    Serial.print("\t\n");
    //Serial.println(fecha);
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

int getAnalog(int pin){
    int maximo_dato_analogo = 300;
    int minimo_dato_analogo = 100;
    long suma = 0;
    
    for(int i = 0; i < 32; i++)
        suma += analogRead(pin);

    int dato = suma / 32;
    
    maximo_dato_analogo = dato > maximo_dato_analogo ? dato : maximo_dato_analogo;         // if max data
    minimo_dato_analogo = minimo_dato_analogo > dato ? dato : minimo_dato_analogo;         // if min data
    
    return dato;
}
