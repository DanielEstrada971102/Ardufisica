//Version actualmente implementada en sistemas ardufisica

#define ENCODER_DO_NOT_USE_INTERRUPTS//Para que libreria no use interrupciones
#include "rgb_lcd.h" //incluye libreria para manejo de LCD
#include <Encoder.h> //libreria encoder
#include <Ultrasonic.h> //Para sensor de ultrasonido
#include <math.h>
#include "max6675.h" //para uso de termocupla tipo K
#include <Wire.h>
#include <BH1750.h>
#include <Servo.h> //libreria para servomotor
#include"Arduino.h"

/*
D2-D3   -> Electroiman, RGB(D3), Microservo(GND,VCC,D2)
D4-D5   -> Grove Speaker,Color Sensor
D6-D7   -> Ultrasonido, RGB
D8-D9   -> Atomizador
D10-D11 -> Termocupla tipo K
D12-D13 -> Encoder
IIC     -> Intensidad de luz, Ritmo cardiaco

A0-A1   -> Temperatura infr
A2-A3   -> Fotorresistencia
A4-A5   -> Tacometro
A6-A7   -> Calidad de Aire
A8-A9   -> Resp.Galv(GSR)
A10-A11 -> EMG Sensor
A12-A13 -> Microfono
A14-A15 -> Hall
*/

//Grove Speaker
#define Speaker 4

//Sensor de temperatura infrarrojo
#define SUR_TEMP_PIN A0 // SUR sensor de temperatura
#define OBJ_TEMP_PIN A1 // OBJ sensor de temperatura

//Ultrasonido
#define pintrigger 6
#define pinecho 7

//Pines Encoder
#define pin_clk 16 //Pin A encoder
#define pin_dt 17 //Pin B encoder 
#define pin_sw 12 //suiche encoder

//Sensor de color
int S0=39,S1=41,S2=43,S3=45,out=47,GND2=4,led=5;

//GSR Sensor: Galvanic Skin Response
int sensorValue=0;
int gsr_average=0;

//Termocupla Tipo k
#define CONFIG_TCSCK_PIN      11
#define CONFIG_TCCS_PIN       10
#define CONFIG_TCDO_PIN       53

//Electromagnet
#define Electromagnet 2

//Solenoide pequeño 5V
#define Solenoide 53

rgb_lcd lcd; //Define a lcd como una variable del tipo rgb_lcd 
Encoder mi_encoder(pin_clk, pin_dt); 
Ultrasonic ultrasonido(pintrigger,pinecho,30000); //(trig, echo, max_distancia)
MAX6675 thermocouple(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN);
BH1750 luxometro;
Servo servoMotor;

//GY-302 Luminosidad
//const byte luxMode = BH1750_CONTINUOUS_HIGH_RES_MODE;
// BH1750_CONTINUOUS_HIGH_RES_MODE
// BH1750_CONTINUOUS_HIGH_RES_MODE_2
// BH1750_CONTINUOUS_LOW_RES_MODE
// BH1750_ONE_TIME_HIGH_RES_MODE
// BH1750_ONE_TIME_HIGH_RES_MODE_2
// BH1750_ONE_TIME_LOW_RES_MODE

long encoder_inicio=0; //inicio posicion encoder 0
int posicion_menu=0; //posicion inicial menu
int Tiempo_Tono[]={1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072 ,1012};//tiempo en microsegundo para generar tono de frecuencias (261 Hz, 277Hz, 293 Hz, ...) 
int maximo_dato_analogo = 300;
int minimo_dato_analogo = 100;
int dato_analogo_estatico = 0;
String Array_menu[18]={"<Microfono     >", "<Sensor Hall   >", "<Gen. Sonido   >", "<Distancia     >", "<Temperatura IR>", "<Sensor Color  >", "<Calidad Aire  >", "<Respuesta Galv>", "<Termocupla K  >", "<Electromagnet >", "<Instensid. Luz>", "<Micro Servo   >", "<Ritmo Cardiaco>", "<Water Atomizat>", "<EMG Detector  >", "<Fotorresisten.>", "<Led R_G_B>", "<Tachometer>" };
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------Para Sensor de Temperatura------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
float temp_calibration=0;    //this parameter was used to calibrate the temperature
//float objt_calibration=0.000;//this parameter was used to calibrate the object temperature
float temperature_range=10;    //we make a map of temperature-voltage according to sensor datasheet. 10 is the temperature step when sensor and 
                               //object distance is 9CM.
float offset_vol=-0.005;       //this parameter was used to set the mid level voltage,when put the sensor in normal environment after 10 min,
                               //the sensor output 0.For example,the surrounding temperature is 29℃，but the result is 27℃ via the sensor,
                               //you should set the reerence to 0.520 or more,according to your sensor to change.
                               //the unit is V
float tempValue = 0; 
float objtValue= 0;  
float current_temp=0;
float temp=0;
float temp1=0;
float temp2=0;
unsigned int temp3=0;
const float reference_vol=0.5;
unsigned char clear_num=0;//when use lcd to display
float R=0;
float voltage=0;


long res[100]={
                 318300,302903,288329,274533,261471,249100,237381,226276,215750,205768,
                 196300,187316,178788,170691,163002,155700,148766,142183,135936,130012,
                 124400,119038,113928,109059,104420,100000,95788,91775,87950,84305,
                 80830,77517,74357,71342,68466,65720,63098,60595,58202,55916,
                 53730,51645,49652,47746,45924,44180,42511,40912,39380,37910,
                 36500,35155,33866,32631,31446,30311,29222,28177,27175,26213,
                 25290,24403,23554,22738,21955,21202,20479,19783,19115,18472,
                 17260,16688,16138,15608,15098,14608,14135,13680,13242,12819,
                 12412,12020,11642,11278,10926,10587,10260,9945,9641,9347,
                 9063,8789,8525,8270,8023,7785,7555,7333,7118,6911};
                 
float obj [13][12]={
/*0*/             { 0,-0.274,-0.58,-0.922,-1.301,-1.721,-2.183,-2.691,-3.247,-3.854,-4.516,-5.236}, //
/*1*/             { 0.271,0,-0.303,-0.642,-1.018,-1.434,-1.894,-2.398,-2.951,-3.556,-4.215,-4.931},  //→surrounding temperature,from -10,0,10,...100
/*2*/             { 0.567,0.3,0,-0.335,-0.708,-1.121,-1.577,-2.078,-2.628,-3.229,-3.884,-4.597},   //↓object temperature,from -10,0,10,...110
/*3*/             { 0.891,0.628,0.331,0,-0.369,-0.778,-1.23,-1.728,-2.274,-2.871,-3.523,-4.232},
/*4*/             { 1.244,0.985,0.692,0.365,0,-0.405,-0.853,-1.347,-1.889,-2.482,-3.13,-3.835},
/*5*/             { 1.628,1.372,1.084,0.761,0.401,0,-0.444,-0.933,-1.47,-2.059,-2.702,-3.403},
/*6*/             { 2.043,1.792,1.509,1.191,0.835,0.439,0,-0.484,-1.017,-1.601,-2.24,-2.936},
/*7*/             { 2.491,2.246,1.968,1.655,1.304,0.913,0.479,0,-0.528,-1.107,-1.74,-2.431},
/*8*/             { 2.975,2.735,2.462,2.155,1.809,1.424,0.996,0.522,0,-0.573,-1.201,-1.887},
/*9*/             { 3.495,3.261,2.994,2.692,2.353,1.974,1.552,1.084,0.568,0,-0.622,-1.301},
/*10*/            { 4.053,3.825,3.565,3.27,2.937,2.564,2.148,1.687,1.177,0.616,0,-0.673},
/*11*/            { 4.651,4.43,4.177,3.888,3.562,3.196,2.787,2.332,1.829,1.275,0.666,0},
/*12*/            { 5.29,5.076,4.83,4.549,4.231,3.872,3.47,3.023,2.527,1.98,1.379,0.72}
};
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void setup(){
  Serial.begin(115200); //Comunicacion serial a 9600 baudios 
  Serial1.begin(115200); //Comunicacion serial a 9600 baudios con Bluetooth
  lcd.begin(16,2); //Inicia LCD 16 columnas, 2 filas.  
  lcd.setRGB(100,100,100); //Colores Red Green Blue en la pantalla
  pinMode(pin_sw, INPUT_PULLUP); //Boton del encoder 
  pinMode(Speaker,OUTPUT); //Pin Grove Speaker
  digitalWrite(Speaker, LOW);
  pinMode(Electromagnet, OUTPUT);//Pin Electromagnet
  digitalWrite(Electromagnet, LOW);
  pinMode(Solenoide, OUTPUT);
  digitalWrite(Solenoide, LOW);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  pinMode(45, OUTPUT); 
  digitalWrite(45, LOW);
  analogReference(INTERNAL1V1);//(Solo para arduino Mega)Establece el nivel de referencia en 1.1V,la distinguibilidad va hasta 1mV.
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  servoMotor.attach(2); //Microservo naranja
  }

void loop(){
  //MENU ENCODER
  if (abs(mi_encoder.read()-encoder_inicio)>7){ //si el encoder se giro
    if ((mi_encoder.read()-encoder_inicio)>0){
      posicion_menu+=1;
      }
    else{
      posicion_menu-=1;
      }
    encoder_inicio= mi_encoder.read(); 
    
    if (posicion_menu==18){
      posicion_menu=0;
      }
    else if(posicion_menu==-1){
      posicion_menu=17;
      }

    print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
    }

    //MENU SERIAL
    if (digitalRead(pin_sw)==0){ //si el suiche del encoder es oprimido
      delay(100); //100 ms hasta que se estabilice la 
      while(digitalRead(pin_sw)==0); //espere hasta que se suelte el boton
      lcd.clear(); //limpia la pantalla
      switch (posicion_menu){
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
        conductancia();
        break;
        case 8:
        Termocupla_k(); //GND -> GND,VCC -> VCC, SCK -> D11, CS -> D10, SO -> D53
        break;
        case 9:
        Electromagn(); //GND -> GND,VCC -> VCC, NC->
        break;
        case 10:
        Luminosidad(); //GND -> GND,VCC -> VCC, SCL -> SCL, SDA -> SDA, ADDR -> NC
        break;
        case 11:
        Servomotor();
        break;
        case 12:
        Ritmo_cardiaco();
        break;
        case 13:
        Water_Atomization();
        break;
        case 14:
        EMG_detector();
        break;
        case 15:
        Fotorresistencia();
        break;
        case 16:
        Led_RGB(); // V -> GND, R -> D7, B -> D6, G -> 53; 
        break;
        case 17:
        Tacometro();
        break;
        }
      }
    if(Serial1.available()){
      char caracter=Serial1.read();
      if(caracter=='0'){
        while(Serial1.available()==0);
        char caracter=Serial1.read();
        if (caracter=='1'){
          Serial1.println("");
          Serial1.println("Microfono");
          microfono();
          }
        else if (caracter=='2'){
          Serial1.println("");
          Serial1.println("Campo Magnetico");
          hall_magnetico();
          }
        else if (caracter=='3'){
          Serial1.println("");
          Serial1.println("Grove Speaker");
          generador_sonido();
          }
        else if (caracter=='4'){
          Serial1.println("");
          Serial1.println("Distancia - ultrasonido");
          distancia_ultrasonido();
          }
        else if (caracter=='5'){
          Serial1.println("");
          Serial1.println("Temperatura Infrarrojo");
          temperatura_infrarrojo();
          }
        else if (caracter=='6'){
          Serial1.println("");
          Serial1.println("Color");
          color();
          }
        else if (caracter=='7'){
          Serial1.println("");
          Serial1.println("Calidad de Aire");
          calidad_aire();
          }
        else if (caracter=='8'){
          Serial1.println("");
          Serial1.println("Galvanic Skin Response (GSR)");
          conductancia();
          }
        else if (caracter=='9'){
          Serial1.println("");
          Serial1.println("Termocupla K");
          Termocupla_k();
          }
        else{
          }
        }
      else if(caracter=='1'){
        while(Serial1.available()==0);
        char caracter=Serial1.read();
        if (caracter=='0'){
          Serial1.println("");
          Serial1.println("Electromagnet");
          Electromagn();
          }
        else if (caracter=='1'){
          Serial1.println("");
          Serial1.println("Luminosidad");
          Luminosidad();
          }
        else if (caracter=='2'){
          Serial1.println("");
          Serial1.println("Servomotor");
          Servomotor();
          }
        else if (caracter=='3'){
          Serial1.println("");
          Serial1.println("Ritmo Cardiaco");
          Ritmo_cardiaco();
          }
        else if (caracter=='4'){
          Serial1.println("");
          Serial1.println("Water Atomizator");
          Water_Atomization();
          }
        else if (caracter=='5'){
          Serial1.println("");
          Serial1.println("EMG Sensor");
          EMG_detector();
          }
        else if (caracter=='6'){
          Serial1.println("");
          Serial1.println("Fotorresistencia");
          Fotorresistencia();
          }
        else if (caracter=='7'){
          Serial1.println("");
          Serial1.println("Led RGB");
          Led_RGB();
          }
        else if (caracter=='8'){
          Serial1.println("");
          Serial1.println("Tacometro");
          Tacometro();
          }
        else{
          }
        }
      }  
  }


void microfono(){
    analogReference(DEFAULT);
    print_pantalla("Sound: A12-A13","Enviando al PC...");
    long sum = 0;
    while (digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){
      for(int i=0; i<1000; i++){
        sum += analogRead(A12);
        }
      Serial.println(sum);
      Serial1.println(sum);
      sum = 0;
      delay(10);
    }
    analogReference(INTERNAL1V1);
    print_pantalla("Sound: A12-A13","Suelte el boton");
    while(digitalRead(pin_sw)== 0);
    delay(100);
    print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void hall_magnetico(){
    analogReference(DEFAULT);
    int campo=0;
    int campo_0=analogRead(A14);
    print_pantalla("Hall:  A14-A15","Enviando al PC...");
    while (digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){
      campo=(analogRead(A14)-campo_0);  
      print_pantalla("Hall:  A14-A15","   Valor ->"+ String(campo));
      Serial.println(campo);
      Serial1.println(campo);
      delay(100);
    }
    analogReference(INTERNAL1V1);
    print_pantalla("Hall:  A14-A15","Suelte el boton");
    while(digitalRead(pin_sw)== 0);
    delay(100);
    print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void generador_sonido(){
  int encoder_ground= mi_encoder.read();
  int tono = 0;
  print_pantalla("Speaker: D4-D5"," "+String(tono+1)+"   "+String(500000/Tiempo_Tono[tono])+" Hz");
  while(digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){ 
    if (abs(mi_encoder.read()-encoder_ground)>7){ //si el encoder se giro
      if ((mi_encoder.read()-encoder_ground)>0){
        tono+=1;
        }
      else{
        tono-=1;
        }
      encoder_ground= mi_encoder.read(); 
      if (tono==12){
        tono=0;
        }
      else if(tono==-1){
        tono=11;
        }
      print_pantalla("Speaker: D4-D5"," "+String(tono+1)+"   "+String(500000/Tiempo_Tono[tono])+" Hz");
      }
      digitalWrite(Speaker,HIGH);
      delayMicroseconds(Tiempo_Tono[tono]);
      digitalWrite(Speaker,LOW);
      delayMicroseconds(Tiempo_Tono[tono]);
    }
    print_pantalla("Speaker: D4-D5","Suelte el boton");
    while(digitalRead(pin_sw)== 0);
    delay(100);
    print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }


void distancia_ultrasonido(){
  print_pantalla("Ultrasonido: D6"," ");
  unsigned long t_inicial=millis();
  unsigned long t;
  int distancia;
  while(digitalRead(pin_sw)==1 and (Serial1.available()== 0)){
    t=(millis()-t_inicial)/1000.0;
    distancia=ultrasonido.Ranging(CM);
    print_pantalla("Ultrasonido: D6","    "+String(distancia)+"   cm");
    Serial.println(distancia);
    Serial1.println(distancia);
    delay(500);
    }
  print_pantalla("Ultrasonido: D6","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void temperatura_infrarrojo(){
  print_pantalla("Infra.Temp.: A0 "," ");
  while(digitalRead(pin_sw) == 1 and (Serial1.available()== 0)){  
    measureSurTemp();//measure the Surrounding temperature around the sensor
    measureObjectTemp();
    }
  print_pantalla("Infra.Temp.: A0 ","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void color(){
  int R, G, B;
  print_pantalla("Color (RGB):  ","");
  pinMode(S0,OUTPUT);
  pinMode(S1,OUTPUT); 
  pinMode(S2,OUTPUT);//(S2,S3)=(0,0)->R, (0,1)->B, (1,1)->G, (1,0)->clear.
  pinMode(S3,OUTPUT);
  pinMode(GND2,OUTPUT);
  pinMode(led,OUTPUT);
  digitalWrite(S1,LOW);
  digitalWrite(S0,HIGH);//Frequency scaling 100%
  digitalWrite(GND2,LOW);
  digitalWrite(led,HIGH);
  while(digitalRead(pin_sw)==1 and (Serial1.available()== 0)){
    for (int i=0;i<3;i++){
      if(i==0){//RED
        digitalWrite(S2,0);
        digitalWrite(S3,0);
        delay(20);
        R=abs(100000.0/pulseIn(out,HIGH));
      }else if(i==1){//GREEN
        digitalWrite(S2,1);
        digitalWrite(S3,1);
        delay(20);
        G=abs(100000.0/pulseIn(out,HIGH));
      }else if(i==2){//BLUE
         digitalWrite(S2,0);
         digitalWrite(S3,1);
         delay(20);
         B=abs(100000.0/pulseIn(out,HIGH));
      } 
    }
    delay(600);
  int maximum=max(R,G);
  maximum=max(maximum,B);
  R=R*255.0/maximum; //Renormaliza (R,G,B)
  G=G*255.0/maximum;
  B=B*255.0/maximum;
  lcd.setRGB(R, G, B);
  print_pantalla("Color (RGB):  ","R:"+String(R)+" G:"+String(G)+" B:"+String(B));
    Serial.print(R);
    Serial.print('\t');
    Serial.print(G);
    Serial.print('\t');
    Serial.println(B);
    Serial1.print(R);
    Serial1.print('\t');
    Serial1.print(G);
    Serial1.print('\t');
    Serial1.println(B);
  }
  lcd.setRGB(100,100,100);
  print_pantalla("Color (RGB):  ","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void calidad_aire(){
  analogReference(DEFAULT);
  int sensor_val;
  String quality;
  int V_0 = 0;
  print_pantalla("Cal.Aire:  A6-A7"," ");
  delay(100);
  while(digitalRead(pin_sw)==0 and (Serial1.available()== 0));
  delay(100);
    print_pantalla("Cal.Aire:  A6-A7"," ");
    delay(2000);
    print_pantalla("El sensor debe","llevar conectado ");
    delay(2000);
    print_pantalla("al menos 3 min","para que pueda");
    delay(2000);
    print_pantalla("funcionar","adecuadamente.");
    delay(2000);
    print_pantalla("Ponga el sensor","en un lugar con");
    delay(2000);
    print_pantalla("aire de buena","calidad para");
    delay(2000);
    print_pantalla("la calibracion ","inicial. Luego,");
    delay(2000);
    print_pantalla("oprima el boton","para calibrar.");
  while(digitalRead(pin_sw)==1  and (Serial1.available()== 0));
  delay(100);
  while(digitalRead(pin_sw)==0 and (Serial1.available()== 0));
  delay(100);
  V_0=analogRead(A6);
  print_pantalla("Cal.Aire:  A6-A7","Sensor calibrado");
  delay(2000);
  print_pantalla("Cal.Aire:  A6-A7","Enviando al PC...");
  delay(1000);
  while (digitalRead(pin_sw)==1 and (Serial1.available()== 0)){
    sensor_val=abs(analogRead(A6)-V_0);
    if(sensor_val<=200){
     quality="Cal.buena->";
     }
    else if(sensor_val>200 && sensor_val<400){
     quality="Cal.media->";
      }
    else if(sensor_val>=400 && sensor_val<700){
     quality="Cal.baja->";
      }
    else if(sensor_val>=700){
     quality="Peligro->";//Los intervalos están basados en la librería AirQuality_Sensor.zip
      }
    print_pantalla("Cal.Aire:  A6-A7",quality+"  "+String(sensor_val));
    Serial.println(sensor_val);
    Serial1.println(sensor_val);
    delay(500);
    }
  analogReference(INTERNAL1V1);
  print_pantalla("Cal.Aire:  A6-A7","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }
 
void conductancia(){
  analogReference(DEFAULT);
  print_pantalla("GSR:  A8-A9"," ");
  delay(500);
  print_pantalla("GSR:  A8-A9","Enviando al PC...");
  delay(500);
  int sensorValue=0;
  int gsr_average=0;
  long sum=0;
  while (digitalRead(pin_sw)==1 and (Serial1.available()== 0)){
    sum=0;
    for(int i=0;i<10;i++){
      sensorValue=analogRead(A8);
      sum += sensorValue;
      delay(5);
      }
    gsr_average = sum/10;
    print_pantalla("GSR:  A8-A9","GSR_Avg    "+ String(gsr_average));
    Serial.println(gsr_average);
    Serial1.println(gsr_average);
    }
  analogReference(INTERNAL1V1);
  print_pantalla("GSR:  A8-A9","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  
  }

 void Termocupla_k(){
  print_pantalla("Termocupla: D10","      "+String(thermocouple.readCelsius())+" "+char(223)+"C");
  delay(500);
  print_pantalla("Termocupla: D10","Enviando al PC...");
  delay(500);
  while(digitalRead(pin_sw) == 1 and (Serial1.available()== 0)){  
    print_pantalla("Termocupla: D10","      "+String(thermocouple.readCelsius())+" "+char(223)+"C");
    Serial.println(thermocouple.readCelsius());
    Serial1.println(thermocouple.readCelsius());
    // ESPERAR UN SEGUNDO ENTRE LAS LECTURAS
    delay(1000);
    }
  print_pantalla("Infra.Temp.: A0 ","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void Electromagn(){
  int encoder_ground= mi_encoder.read();
  int opcion = 1;
  digitalWrite(2,LOW);
  print_pantalla("Electromag:D2-D3","Electroiman Off");
  while(digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){ 
    if (abs(mi_encoder.read()-encoder_ground)>7){ //si el encoder se giro
      if ((mi_encoder.read()-encoder_ground)>0){
        opcion=1;
        digitalWrite(2,HIGH);
        print_pantalla("Electromag:D2-D3","Electroiman On");
        }
      else{
        opcion=2;
        digitalWrite(2,LOW);
        print_pantalla("Electromag:D2-D3","Electroiman Off");
        }
      encoder_ground= mi_encoder.read();
      }
    }
  print_pantalla("Electromag:D2-D3","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }
 
 void Luminosidad(){
  uint16_t lux = 0;
  luxometro.begin(BH1750::CONTINUOUS_HIGH_RES_MODE); //Inicializar BH1750 sensor de Luminosidad
  print_pantalla("Intensidad: IIC","      ");
  delay(500);
  print_pantalla("Intensidad: IIC","Enviando al PC...");
  delay(500);
  while(digitalRead(pin_sw) == 1 and (Serial1.available()== 0)){  
    lux = luxometro.readLightLevel(); // Lectura del BH1750
    Serial.println(lux);
    Serial1.println(lux);
    print_pantalla("Intensidad: IIC","      "+String(lux)+" lx");
    delay(500);
    }
  print_pantalla("Intensidad: IIC","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void Servomotor(){
  int encoder_ground= mi_encoder.read();
  int ang = 0;
  servoMotor.write(180);
  print_pantalla("Servo:GND-VCC-D2","   Angulo  "+String(0));
  while(digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){ 
    if (abs(mi_encoder.read()-encoder_ground)>3){ //si el encoder se giro
      if ((mi_encoder.read()-encoder_ground)>0){
        ang+=3;
        }
      else{
        ang-=3;
        }
      if (ang>180){
        ang=0;
        }
      else if(ang<0){
        ang=180;
        }
      encoder_ground= mi_encoder.read(); 
      servoMotor.write(180-ang);
      print_pantalla("Servo:GND-VCC-D2","   Angulo  "+String(ang));
      }
    }
    print_pantalla("Servo:GND-VCC-D2","Suelte el boton");
    while(digitalRead(pin_sw)== 0);
    delay(100);
    print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
    
  }

void Ritmo_cardiaco(){
  unsigned char c=0;
  Wire.begin();
  delay(100);
  print_pantalla("Ritmo Cardi: IIC","      ");
  delay(500);
  print_pantalla("Ritmo Cardi: IIC","Enviando al PC...");
  delay(500);
  while(digitalRead(pin_sw) == 1 and (Serial1.available()== 0)){  
    Wire.requestFrom(0xA0 >> 1, 1);    // request 1 bytes from slave device
    while(Wire.available()) {          // slave may send less than requested
      c = Wire.read();   // receive heart rate value (a byte)
      Serial.println(c, DEC);         // print heart rate value
      Serial1.println(c, DEC);
      print_pantalla("Ritmo Cardi: IIC","    c=  "+String(c));
      }
    delay(100);
    }
  print_pantalla("Ritmo Cardi: IIC","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void Water_Atomization(){
  int encoder_ground= mi_encoder.read();
  int opcion = 1;
  digitalWrite(8,LOW);
  print_pantalla("Atomizador:D8-D9","Atomizacion Off");
  while(digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){ 
    if (abs(mi_encoder.read()-encoder_ground)>7){ //si el encoder se giro
      if ((mi_encoder.read()-encoder_ground)>0){
        opcion=1;
        digitalWrite(8,HIGH);
        print_pantalla("Atomizador:D8-D9","Atomizacion On");
        }
      else{
        opcion=2;
        digitalWrite(8,LOW);
        print_pantalla("Atomizador:D8-D9","Atomizacion Off");
        }
      encoder_ground= mi_encoder.read();
      }
    }
  print_pantalla("Atomizador:D8-D9","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void EMG_detector(){
  analogReference(DEFAULT);
  int maximo_dato_analogo = 0;
  int minimo_dato_analogo = 2000;
  int dato_analogo_estatico = 0;
  int valor =0;
  int valor_final=0;
  long suma= 0;

  for(int i=0; i<=10; i++){
    for(int j=0; j<100; j++){
      suma += getAnalog(A10);
      delay(1);
      }
    print_pantalla("EMG: A10-A11","Calibrando"+String(i+1)+"de 11");
    }
  suma/= 1100;
  dato_analogo_estatico =suma;
  print_pantalla("EMG: A10-A11","Sistema calibrado");
  delay(2000);
  print_pantalla("EMG: A10-A11","Enviando al PC...");
  while(digitalRead(pin_sw)== 1 and (Serial1.available()== 0)){
    valor = getAnalog(A10);
    Serial.println(valor);
    Serial1.println(valor);
    delay(10);
    }
  analogReference(INTERNAL1V1);
  print_pantalla("EMG: A10-A11","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }
  
void Fotorresistencia(){
  analogReference(DEFAULT);
  int valor=0;
  print_pantalla("Fotorres.: A2-A3","      ");
  delay(500);
  print_pantalla("Fotorres.: A2-A3","Enviando al PC...");
  delay(500);
  while(digitalRead(pin_sw) == 1 and (Serial1.available()== 0)){  
    valor=analogRead(A3);
    print_pantalla("Fotorres.: A2-A3","  Valor ->  "+String(valor));
    Serial.println(valor);
    Serial1.println(valor);
    delay(100);
    }
  analogReference(INTERNAL1V1);
  print_pantalla("Fotorres.: A2-A3","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }  
void Led_RGB(){
  int encoder_ground= mi_encoder.read();
  int opcion = 1;
  int Pin = 7;
  int Salir = 0;
  int Valor = 0;
  int Salir_color = 0;
  analogWrite(7,LOW);//Red
  analogWrite(3,LOW);//Green
  analogWrite(6,LOW);//Blue
  print_pantalla("RGB: D7-D3-D6","Intensidad Rojo");
  while(Salir == 0 and (Serial1.available()== 0)){ 
    if (abs(mi_encoder.read()-encoder_ground)>3){ //si el encoder se giro
      if ((mi_encoder.read()-encoder_ground)>0){
        opcion+=1;
        }
      else{
        opcion-=1;
        }
      if (opcion == 1){
      Pin = 7;
      print_pantalla("RGB: D7-D3-D6","Intensidad Rojo");
      }
      else if (opcion == 2){
        Pin = 3;
        print_pantalla("RGB: D7-D3-D6","Intensidad Verde");
        }
      else if (opcion == 3){
        Pin = 6;
        print_pantalla("RGB: D7-D3-D6","Intensidad Azul");
        }
      else if (opcion == 4){
        print_pantalla("RGB: D7-D3-D6","Salir del Menu");
        }
      else if (opcion == 5){
        opcion = 1;
        Pin = 7;
        print_pantalla("RGB: D7-D3-D6","Intensidad Rojo");
        }
      else if (opcion == 0){
        opcion = 4;
        print_pantalla("RGB: D7-D3-D6","Salir del Menu");
        }
      encoder_ground= mi_encoder.read();
      }
    if ((opcion == 4) and (digitalRead(pin_sw) == 0)){
        Salir = 1;
        }
    else if((opcion != 4) and (digitalRead(pin_sw) == 0)){
      delay(100);
      while(digitalRead(pin_sw) == 0);
      print_pantalla("RGB: D7-D3-D6","Gire la perilla ");
      while(Salir_color == 0 and (Serial1.available()== 0)){
        if (abs(mi_encoder.read()-encoder_ground)>3){ //si el encoder se giro
          if ((mi_encoder.read()-encoder_ground)>0){
            Valor+=5;
            }
          else{
            Valor-=5;
            }
          if (Valor > 255){
            Valor =0;
            }
          else if (Valor < 0){
            Valor =255;
            }
          analogWrite(Pin, Valor);
          print_pantalla("RGB: D7-D3-D6","Intensidad: "+String(Valor));
          encoder_ground= mi_encoder.read();
          }
        if (digitalRead(pin_sw) == 0){
          Salir_color = 1;
          delay(100);
          while(digitalRead(pin_sw) == 0);
          delay(100);
          }
        }
        Salir_color = 0;
        opcion = 4;
        print_pantalla("RGB: D7-D3-D6","Gire la perilla ");
      }
    }
  print_pantalla("RGB: D7-D3-D6","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]);
  }

void Tacometro(){
  analogReference(DEFAULT);
  int valor=0;
  int tiempo1=0;
  int tiempo2=0;
  int T_Osc=0;
  int Salir =0;
  print_pantalla("Tacometro: A4-A5","Envia al PC");
  delay(500);
  print_pantalla("Tacometro: A4-A5","Tiempo en ms");
  delay(500);
  while(Salir == 0 and (Serial1.available()== 0)){
    valor= ((analogRead(4))*5)/1023;
    while(valor > 2 and Salir==0){
      if (digitalRead(pin_sw) == 0){
        Salir =1;
        }
      valor= ((analogRead(4))*5)/1023;
      };
    print_pantalla("T.Oscuri.: A4-A5","Esperando");
    while(valor < 3 and Salir==0 and (Serial1.available()== 0)){
      if (digitalRead(pin_sw) == 0){
        Salir =1;
        }
      valor= ((analogRead(4))*5)/1023;
      };
    tiempo1=millis();
    print_pantalla("T.Oscuri.: A4-A5","Tomando tiempo");
    while(valor > 2 and Salir==0 and (Serial1.available()== 0)){
      if (digitalRead(pin_sw) == 0){
        Salir =1;
        }
      valor= ((analogRead(4))*5)/1023;
      };
    
    tiempo2=millis();
    T_Osc =tiempo2-tiempo1;
    if (Salir == 0 and (Serial1.available()== 0)){
      print_pantalla("T.Oscuri.: A4-A5","T.Osc.->"+String(T_Osc)+" ms");
      Serial.println(T_Osc);
      Serial1.println(T_Osc);
      }
    }
  analogReference(INTERNAL1V1);
  print_pantalla("Tacometro: A4-A5","Suelte el boton");
  while(digitalRead(pin_sw)== 0);
  delay(100);
  print_pantalla("Funcion:   "+String(posicion_menu+1),Array_menu[posicion_menu]); 
  }
  
void print_pantalla(String linea1, String linea2){
    lcd.clear();
    lcd.setCursor(0,0);   //columna 0 fila 0. No es en formato fila columna
    lcd.print(linea1);
    lcd.setCursor(0,1); //columna 0 fila 1
    lcd.print(linea2);
  }

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------Para Sensor EMG-----------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
int getAnalog(int pin)
{
    long suma = 0;
    
    for(int i=0; i<32; i++){
        suma += analogRead(pin);
    }
    
    int dato = suma/32;
    
    maximo_dato_analogo = dato > maximo_dato_analogo ? dato : maximo_dato_analogo;         // if max data
    minimo_dato_analogo = minimo_dato_analogo > dato ? dato : minimo_dato_analogo;         // if min data
    
    return dato;
}
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------Para Sensor de Temperatura------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
  float binSearch(long x){// this function used for measure the surrounding temperature
  int low,mid,high;
  low=0;
  //mid=0;
  high=100;
  while (low<=high){
    mid=(low+high)/2;
    if(x<res[mid])
      low= mid+1;
    else//(x>res[mid])
      high=mid-1;
    }
  return mid;
  }
float arraysearch(float x,float y){//x is the surrounding temperature,y is the object temperature
  int i=0;
  float tem_coefficient=100;//Magnification of 100 times  
  i=(x/10)+1;//Ambient temperature      
  voltage=(float)y/tem_coefficient;//the original voltage   
  //Serial.print("sensor voltage:\t");    
  //Serial.print(voltage,5);  
  //Serial.print("V");      
  for(temp3=0;temp3<13;temp3++){     
    if((voltage>obj[temp3][i])&&(voltage<obj[temp3+1][i])){     
      return temp3;         
      }     
    }
  }
float measureSurTemp(){  
  unsigned char i=0;
  float current_temp1=0;    
  int signal=0;   
  tempValue=0;

  for(i=0;i<10;i++){     
    tempValue+= analogRead(SUR_TEMP_PIN);       
    delay(10);    
    }   
  tempValue=tempValue/10;   
  temp = tempValue*1.1/1023;    
  R=2000000*temp/(2.50-temp);   
  signal=binSearch(R);    
  current_temp=signal-1+temp_calibration+(res[signal-1]-R)/(res[signal-1]-res[signal]);
  Serial.print("Temperatura Ambiente:");
  Serial.print(current_temp);
  Serial1.print("Temperatura Ambiente:");
  Serial1.print(current_temp);
  return current_temp;
  }
float measureObjectTemp(){
  unsigned char i=0;  
  unsigned char j=0;  
  float sur_temp=0;  
  unsigned int array_temp=0;  
  float temp1,temp2; 
  float final_temp=0;
  objtValue=0;  
  for(i=0;i<10;i++){
    objtValue+= analogRead(OBJ_TEMP_PIN); 
    delay(10); 
    }
  objtValue=objtValue/10;//Averaging processing     
  temp1=objtValue*1.1/1023;//+objt_calibration; 
  sur_temp=temp1-(reference_vol+offset_vol);             
  Serial.print("\t Voltaje del sensor:");   
  Serial.print(sur_temp,3); 
  Serial.print("V");
  Serial1.print("\t Voltaje del sensor:");   
  Serial1.print(sur_temp,3); 
  Serial1.print("V");  
  array_temp=arraysearch(current_temp,sur_temp*1000);        
  temp2=current_temp;        
  temp1=(temperature_range*voltage)/(obj[array_temp+1][(int)(temp2/10)+1]-obj[array_temp][(int)(temp2/10)+1]);        
  final_temp=temp2+temp1;        
  if((final_temp>100)||(final_temp<=-10)){
    Serial.println ("\t Fuera de rango!");
    Serial1.println ("\t Fuera de rango!");
    }
  else{
      Serial.print("\t Temperatura del objeto:");   
      Serial.println(final_temp,2);
      Serial1.print("\t Temperatura del objeto:");   
      Serial1.println(final_temp,2);
      print_pantalla("Infra.Temp.: A0 ","   "+String(final_temp)+" "+char(223)+"C"); 
      }
  }
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
