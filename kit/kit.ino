 #include "ardufisica.h"

void setup(){
  Serial.begin(115200);
  Serial1.begin(115200); // para el bluetooth

  analogReference(INTERNAL1V1);
  pinMode(pinsw, INPUT_PULLUP);
  lcd.begin(16,2); //16 columns and 2 rows
  lcd.setRGB(100,100,100);

}

void loop(){
    //=========Control Encoder ===========
    Encoder_menu(0, 17, &posicion_menu);
    
    //=========Mostrar en LCD=============
    
    firstLine = "Funciones:    " + String(posicion_menu + 1) + "  ";   
    secondLine = Listado_menu[posicion_menu];

    lcd_mesagge(firstLine, secondLine);

    if(Serial.available() or Serial1.available() or digitalRead(pinsw)==0){        
        // =========Control Serial=========
        if(Serial.available() or Serial1.available()){
            String sensor = serial_readPhrase();
            
            for (int i= 0; i < 18; i++){
                if(sensor == Listado_sensors[i]){
                    posicion_menu = i;
                    break;
                }     
            }
        }
        //===================================
        
        run_sensor(posicion_menu);
    } 
}


 
