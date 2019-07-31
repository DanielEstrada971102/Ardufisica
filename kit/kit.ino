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
    Encoder_menu(0, 17, &posicion_menu);
    
    //=========Mostrar en LCD=============
    
    firstLine = "Funciones:    " + String(posicion_menu + 1) + "  ";   
    secondLine = Listado_menu[posicion_menu];

    lcd_mesagge(firstLine, secondLine);

    //=========Control Encoder ===========
    if(digitalRead(pinsw)==0){
        delay(100);
        run_sensor(posicion_menu);
    }

    // =========Control Bluetooht=========
//    if(Serial.available()){
//        String palabra[5];
//        int i = 0;
//        char c;
//
//        while((c = Serial1.read()) != '\n')
//            palabra[i] = c;
//            i++;
//
//        i = 0;
//
//        for (i; i < 18; i++){
//            if(palabra = Listado_sensores[i]){
//                if( i == 18)
//                    Serial1.println("detenido..");
//                else{
//                    posicion_menu = i;
//                    run_sensor(posicion_menu);
//                    break;
//                }
//            }
//        }
//    }    
}


 
