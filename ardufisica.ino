#include <Encoder.h>
#include "rgb_lcd.h"

// pines encoder
#define pinclk 16
#define pindt 17
#define pinsw 12

Encoder myEnc(pinclk, pindt);
rgb_lcd lcd;

long enc_inicio = 0;
int posicion_menu = 0;

String Listado_menu[8]={"<Microfono     >","<Sensor Hall   >","<Gen. sonido   >","<Trans. sonido >",
                        "<Distancia     >", "<Temperatura IR>", "<Sensor color  >", "<Calidad aire  >"};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pinsw, INPUT_PULLUP);
  lcd.begin(16,2); //16 columns and 2 rows
  lcd.setRGB(100,100,100);
}

void loop() {
  // put your main code here, to run repeatedly:
  // MENU==========================
  if(abs(myEnc.read() - enc_inicio)> 7){
    if(myEnc.read() - enc_inicio > 0)
      posicion_menu++;
    else
      posicion_menu--;
      
    if (posicion_menu > 7)
      posicion_menu = 0;
    if (posicion_menu < 0)
      posicion_menu = 7;
      
    enc_inicio = myEnc.read(); 
  }
  //==============================
  //Mostrar en LCD
  lcd.setCursor(0,0);
  lcd.print("Funciones:   ");
  lcd.setCursor(11,0);
  lcd.print(posicion_menu);
  lcd.setCursor(0,1);
  lcd.print(Listado_menu[posicion_menu]);

}
