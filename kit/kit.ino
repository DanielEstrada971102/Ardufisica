#include "ardufisica.h"

void setup(){
  Serial.begin(115200);
  analogReference(INTERNAL1V1);
  pinMode(pinsw, INPUT_PULLUP);
  lcd.begin(16,2); //16 columns and 2 rows
  lcd.setRGB(100,100,100);
  Wire.begin();
}

void loop() {	
  //========ENCODER========
    Encoder_menu(0, 13, &posicion_menu);
    //==============================
    //Mostrar en LCD
    
    char firstLine[16];
    sprintf(firstLine, "Funciones:    %d", posicion_menu);    
    String secondLine = Listado_menu[posicion_menu];
    
    lcd_mensage(firstLine, secondLine);

 // =========SERIAL====
    if(Serial.available()){
      char caracter = Serial.read();
      
      if(caracter == 'm')
        posicion_menu = 0;
      
      else if(caracter == 'h')
        posicion_menu = 1;
      
      else if(caracter == 'd')
        posicion_menu = 2;
      
      else if(caracter == 'g'){
        while(Serial.available() == 0);
        int frecuencia = Serial.parseInt();
        //generador_sonido(frecuencia);
      }
      else if(caracter == 's'){
        while(Serial.available()==0);
        int frecuencia=Serial.parseInt();
        //variacion_sonido(frecuencia);
      }
      else if(caracter=='t')
        posicion_menu = 5;
      
      else if(caracter=='c')
        posicion_menu = 6;
      
      else if(caracter=='a')
        posicion_menu = 7;
    }
    run_sensor(posicion_menu);
}


 
