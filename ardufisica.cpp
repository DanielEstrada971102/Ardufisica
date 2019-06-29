#include "ardufisica.h"

Encoder myEnc(pinclk, pindt);
rgb_lcd lcd;
long encoder_inicio = 0;
int posicion_menu = 0;
String Listado_menu[9] = {"<Microfono     >", "<Sensor Hall   >", "<Gen. sonido   >", "<Distancia     >", 
                          "<Temperatura IR>", "<Sensor color  >", "<Calidad aire  >", "<Light.Sensor >", "<Heart.sensor  >"};

void clean_buff(void){
    encoder_inicio = 0;
    lcd.clear();
    Serial.read();//Limpia el buffer serial
    delay(100);
    
    if (digitalRead(pinsw) == 0){
        while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
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
    Serial.print("Interfaz ArduFísica-");
    Serial.println(sensor);
    Serial.print("Fecha: ");
    Serial.print("\t\n");
    //Serial.println(fecha);
}
