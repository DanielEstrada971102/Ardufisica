#include "ardufisica.h"

void setup(){
  Serial.begin(115200);
  analogReference(INTERNAL1V1);
  pinMode(pinsw, INPUT_PULLUP);
  lcd.begin(16,2); //16 columns and 2 rows
  lcd.setRGB(100,100,100);
  Wire.begin();
}

void loop(){	
    //========ENCODER=====================

    Encoder_menu(0, 18, &posicion_menu);

    //====================================
    //Mostrar en LCD
    
    if(posicion_menu == 18)
      lcd_mesagge("    Más Info   ");
    else{
      firstLine = "Funciones:    " + String(posicion_menu);   
      secondLine = Listado_menu[posicion_menu];

      lcd_mesagge(firstLine, secondLine);
    }

 // =========SERIAL=======================
    if(Serial.available()){
      String caracter = Serial.read();
      
      if(caracter == 'microfono')
        posicion_menu = 0;
      
      else if(caracter == 'hall')
        posicion_menu = 1;
      
      else if(caracter == 'gen.son')
        posicion_menu = 2;
      
      else if(caracter == 'distan'){
        posicion_menu = 3;
      }
      else if(caracter == 'tempIR'){
        posicion_menu = 4;
      }
      else if(caracter=='color')
        posicion_menu = 5;
      
      else if(caracter=='calidad')
        posicion_menu = 6;
      
      else if(caracter=='fotoresis')
        posicion_menu = 7;

      else if(caracter=='cardiaco')
        posicion_menu = 8;
      
      else if(caracter=='resp.gal')
        posicion_menu = 9;

      else if(caracter=='tempk')
        posicion_menu = 10;

      else if(caracter=='iman')
        posicion_menu = 11;

      else if(caracter=='luz')
        posicion_menu = 12;

      else if(caracter=='servo')
        posicion_menu = 13;

      else if(caracter=='Atomiz')
        posicion_menu = 14;

      else if(caracter=='emg')
        posicion_menu = 15;

      else if(caracter=='ledRGB')
        posicion_menu = 16;

      else if(caracter=='tacomet')
        posicion_menu = 17;

      else if(caracter=='help')
        posicion_menu = 18;

    }

    run_sensor(posicion_menu);
}


 
