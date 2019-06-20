/*Sensores faltantes:
GSR, EMG, pulso, voltmeter, pwm_otput (sirve para electroiman y nebulizador), servo.
Esta versión incluye un led RGB para complememtar el sensor de color y generar el color detectado en la pantalla.
*/


#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <Ultrasonic.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>
//#include <FreqCount.h>//No se deja usar con la funcion Tone
//PINES
//Ultrasonido
#define pintrigger 6
#define pinecho 7

//Encoder
#define pinclk 17
#define pindt 16
#define pinsw 12

//Sensor de color
int S0=39,S1=41,S2=43,S3=45,SW=53,led=51,out=47;

Encoder myEnc(pinclk, pindt);
Ultrasonic ultrasonido(pintrigger,pinecho,30000); //(trig, echo, max_distancia)
rgb_lcd lcd;

long encoder_inicio = -999;

int posicion_menu=0;
String Listado_menu[8]={"<Microfono     >","<Sensor Hall   >","<Gen. sonido   >","<Trans. sonido >", "<Distancia     >", "<Temperatura IR>", "<Sensor color  >", "<Calidad aire  >"};

///PARA LA MEDICIÓN DE TEMPERATURA CON SENSOR INFRARROJO. ESTÁ BASADO EN EL EJEMPLO SUMINISTRADO POR SEESTUDIO. EL OBJETO DEBE ESTAR A 9CM DEL SENSOR.
#define SUR_TEMP_PIN A0 // Analog input pin connect to temperature sensor SUR pin
#define OBJ_TEMP_PIN A1 // Analog input pin connect to temperature sensor OBJ pin
float temp_calibration=0;       //this parameter was used to calibrate the temperature
//float objt_calibration=0.000; //this parameter was used to calibrate the object temperature
float temperature_range=10;    //we make a map of temperature-voltage according to sensor datasheet. 10 is the temperature step when sensor and 
                               //object distance is 9CM.
float offset_vol= 0.000;       //this parameter was used to set the mid level voltage,when put the sensor in normal environment after 10 min,
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
const float reference_vol=0.500;
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

float binSearch(long x)// this function used for measure the surrounding temperature
{
  int low,mid,high;
  low=0;
  //mid=0;
  high=100;
  while (low<=high)
  {
    mid=(low+high)/2;
    if(x<res[mid])
      low= mid+1;
    else//(x>res[mid])
      high=mid-1;
  }
  return mid;
}

float arraysearch(float x,float y)//x is the surrounding temperature,y is the object temperature
{
  int i=0;
  float tem_coefficient=100;//Magnification of 100 times  
  i=(x/10)+1;//Ambient temperature      
  voltage=(float)y/tem_coefficient;//the original voltage   
  //Serial.print("sensor voltage:\t");    
  //Serial.print(voltage,5);  
  //Serial.print("V");      
  for(temp3=0;temp3<13;temp3++)   
  {     
    if((voltage>obj[temp3][i])&&(voltage<obj[temp3+1][i]))        
    {     
      return temp3;         
    }     
  }
}
float measureSurTemp()
{  
  unsigned char i=0;
  float current_temp1=0;    
  int signal=0;   
  tempValue=0;

  for(i=0;i<10;i++)       //    
  {     
    tempValue+= analogRead(SUR_TEMP_PIN);       
    delay(10);    
  }   
  tempValue=tempValue/10;   
  temp = tempValue*1.1/1023;    
  R=2000000*temp/(2.50-temp);   
  signal=binSearch(R);    
  current_temp=signal-1+temp_calibration+(res[signal-1]-R)/(res[signal-1]-res[signal]);
  return current_temp;
}

float measureObjectTemp(float &final_temp)
{
  unsigned char i=0;  
  unsigned char j=0;  
  float sur_temp=0;  
  unsigned int array_temp=0;  
  float temp1,temp2; 
  objtValue=0;  
  for(i=0;i<10;i++)
  {
    objtValue+= analogRead(OBJ_TEMP_PIN); 
    delay(10); 
    }       
  objtValue=objtValue/10;//Averaging processing     
  temp1=objtValue*1.1/1023;//+objt_calibration; 
  sur_temp=temp1-(reference_vol+offset_vol);             
  Serial.print("\t Sensor voltage:");   
  Serial.print(sur_temp,3); 
  Serial.print("V");  
  array_temp=arraysearch(current_temp,sur_temp*1000);        
  temp2=current_temp;        
  temp1=(temperature_range*voltage)/(obj[array_temp+1][(int)(temp2/10)+1]-obj[array_temp][(int)(temp2/10)+1]);        
  final_temp=temp2+temp1;        
}
////////

/*//Declaracion del objeto para el puerto serial, inicialmente Serial. Sera asignado a Serial o a Serial1(modulo Bluettooth) dependiendo de como lo escoja el usuario.
Stream &myPort= (Stream &)Serial1; Aun no se ha implementado.
*/
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
lcd.begin(16, 2);
lcd.setRGB(100, 100, 100);
pinMode(pinsw,INPUT_PULLUP);
}

void loop() {
 //MENÚ ENCODER
 if(abs(myEnc.read()-encoder_inicio)>3){
  if((myEnc.read()-encoder_inicio)>0){
    posicion_menu++;
  }else{
    posicion_menu--;
  }
  encoder_inicio=myEnc.read();
  if(posicion_menu>7){
   posicion_menu=0;
  }else if(posicion_menu<0){
   posicion_menu=7;
  }
  lcd.setCursor(0,0);
  lcd.print("Funciones:      ");
  lcd.setCursor(11,0);
  lcd.print(posicion_menu+1);
  lcd.setCursor(0,1);
  lcd.print(Listado_menu[posicion_menu]);
 }
 if(digitalRead(pinsw)==0){
  delay(100);
  while(digitalRead(pinsw)==0);
  lcd.clear();
  switch(posicion_menu){
   case 0:
    lee_microfono();
    break;
   case 1:
    sensor_hall();
    break;
   case 2:
    generador_sonido(880);//por ahora constante La4
    break;
   case 3:
    variacion_sonido(880);//por ahora constante La4
    break;
   case 4:
    distancia_ultrasonido();
    break;
   case 5:
    temperatura_infrarrojo();
    break;
   case 6:
    color();
    break;
   case 7:
    air_quality();
	break;
  }
 }
 //MENÚ SERIAL
 if(Serial.available()){
  char caracter=Serial.read();
  if(caracter=='m'){
    lee_microfono();
  }else if(caracter=='h'){
    sensor_hall();
  }else if(caracter=='d'){
    distancia_ultrasonido();
  }else if(caracter=='g'){
    while(Serial.available()==0);
    int frecuencia=Serial.parseInt();
    generador_sonido(frecuencia);
  }else if(caracter=='s'){
    while(Serial.available()==0);
    int frecuencia=Serial.parseInt();
    variacion_sonido(frecuencia);
  }else if(caracter=='t'){
    temperatura_infrarrojo();
 }else if(caracter=='c'){
     color();
 }else if(caracter=='a'){
     air_quality();
 }
}
}
///FUNCIONES

/* MICRÓFONO */
void lee_microfono(){//_________________ya_______________
 lcd.setCursor(0,0);
 lcd.print("Enviando datos  ");
 lcd.setCursor(0,1);
 lcd.print("al PC...        ");
 while(Serial.available()==0&& digitalRead(pinsw)==1){
  Serial.println(analogRead(A12));
  //Sample rate in Arduino MEGA is 9615 Hz
 }
 encoder_inicio=-999;
 lcd.clear();
 Serial.read();//Limpia el buffer serial
 delay(100);
 if(digitalRead(pinsw)==0){
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

/*Sensor Hall*/

void sensor_hall(){ //________________ya____________
  float factor=5000.0/(1023.0*1.3);//1.3mV/Gauss , 5V->1023
  lcd.setCursor(0,0);
  lcd.print("Campo magnetico:");
  lcd.setCursor(0,1);
  lcd.print("Calib. campo 0..");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Campo magnetico:");
  int V_0=analogRead(A14);
  unsigned long t_inicial=millis();
  unsigned long t;
  int campo;
  while(Serial.available()==0&& digitalRead(pinsw)==1){
    t=(millis()-t_inicial)/1000.0;
    campo=(analogRead(A14)-V_0)*factor;
    Serial.print(t);//FALTA EL FACTOR DE CONVERSIÓN
    Serial.print('\t');
    Serial.println(campo);
    lcd.setCursor(0,1);
    lcd.print(campo);
    lcd.print("        ");
    lcd.setCursor(4,1);
    lcd.print("   G   -A14-");
    delay(500);
  }
  lcd.clear();
  Serial.read();//Limpia el buffer serial
  encoder_inicio=-999;
  delay(100);
 if(digitalRead(pinsw)==0){
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

/*Distancia*/

void distancia_ultrasonido(){//_____________________ya__________________
  lcd.setCursor(0,0);
  lcd.print("Distancia:");
  unsigned long t_inicial=millis();
  unsigned long t;
  int distancia;
  while(Serial.available()==0&& digitalRead(pinsw)==1){
    t=(millis()-t_inicial)/1000.0;
    distancia=ultrasonido.Ranging(CM);
    Serial.print(t);
    Serial.print('\t');
    Serial.println(distancia);
    lcd.setCursor(0,1);
    lcd.print(distancia);
    lcd.print("        ");
    lcd.setCursor(4,1);
    lcd.print("   cm  -D6 -");
    delay(500);
  }
  Serial.read();
  lcd.clear();
  encoder_inicio=-999;
  delay(100);
 if(digitalRead(pinsw)==0){
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

/*Generador_sonido*/

void generador_sonido(int frec){ //Hacer otra que también lea el valor medio del micrófono.
  boolean salir=false;
  int frecuencia=frec;
  tone(4,frecuencia);
  lcd.setCursor(0,0);
  lcd.print("Generador tonos :");
  lcd.setCursor(0,1);
  lcd.print(frecuencia);
  lcd.print("     ");
  lcd.setCursor(6,1);
  lcd.print("Hz    -D4-");
  while(salir==false&& digitalRead(pinsw)==1){
   if(Serial.available()){
    char caracter=Serial.read();
    if(caracter=='x'){
      noTone(4);
      lcd.clear();
      salir=true;
    }else if(caracter=='g'){
      while(Serial.available()==0);
      int frecuencia=Serial.parseInt();
      tone(4,frecuencia);
      lcd.setCursor(0,1);
      lcd.print(frecuencia);
      lcd.print("     ");
      lcd.setCursor(6,1);
      lcd.print("Hz    -D4-");
    }
   }
  }
  delay(100);
  if(digitalRead(pinsw)==0){
  noTone(4);
  lcd.clear();
  encoder_inicio=-999;
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

/*Variaciones de intensidad sonora
Emplea el parlante y el micrófono. Se comprueba variacion de la intensidad con la distancia usando esta funcion.
*/
void variacion_sonido(int frec){//REVISAR Y COMPARAR CON EL PROGRAMA INDIVIDUAL
  boolean salir=false;
  int frecuencia=frec;
  int intensidad_inicial, intensidad_actual, intensidad_maxima, maximo_actual, maximo_anterior;
  tone(4,frecuencia);
  lcd.setCursor(0,0);
  lcd.print("Inten. sonido:");
  while(salir==false&& digitalRead(pinsw)==1){
   intensidad_inicial=0;
   maximo_anterior=0;
   for(int i=0;i<10000;i++){//Calcula el valor maximo
    intensidad_actual=analogRead(A12);
	if(intensidad_actual>intensidad_inicial){
	 maximo_actual=intensidad_actual;
	 if(maximo_actual>maximo_anterior){
	  intensidad_maxima=intensidad_actual;
	  maximo_anterior=maximo_actual;
	 }
	}
	intensidad_inicial=intensidad_actual;
      //Sample rate in Arduino MEGA is 9615 Hz, so it will take 1.04 s to complete the for loop.
   }
   Serial.println(intensidad_maxima);
   lcd.setCursor(0,1);
   lcd.print(intensidad_maxima);
   lcd.print("     ");
   lcd.setCursor(6,1);
   lcd.print("UA   -A12-");
   if(Serial.available()){
    char caracter=Serial.read();
    if(caracter=='x'){
      noTone(4);
      lcd.clear();
      salir=true;
    }else if(caracter=='s'){
      while(Serial.available()==0);
      int frecuencia=Serial.parseInt();
      tone(4,frecuencia);
    }
   }
  }
  delay(100);
  if(digitalRead(pinsw)==0){
   noTone(4);
   lcd.clear();
   encoder_inicio=-999;
   while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

//Temperatura infrarrojo -EL OBJETO DEBE ESTAR A 9C DEL SENSOR-

void temperatura_infrarrojo(){
  analogReference(INTERNAL1V1);
  lcd.setCursor(0,0);
  lcd.print("Temperatura:  ");
  unsigned long t_inicial=millis();
  unsigned long t;
  float temperatura;
  while(Serial.available()==0&& digitalRead(pinsw)==1){
    t=(millis()-t_inicial)/1000.0;
    measureSurTemp();
    measureObjectTemp(temperatura);
    Serial.print(t);
    Serial.print('\t');
    Serial.println(temperatura);
    lcd.setCursor(0,1);
    lcd.print(temperatura);
    lcd.print("        ");
    lcd.setCursor(4,1);
    lcd.print("   ");
    lcd.write(0xDF);
    lcd.print("C   -A0-");
    delay(500);
  }
  analogReference(DEFAULT);
  Serial.read();//Limpia el buffer serial
  lcd.clear();
  encoder_inicio=-999;
  delay(100);
 if(digitalRead(pinsw)==0){
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

/* Sensor de color*/
void color(){//Sensor TCS3200
  int R, G, B;
  
  pinMode(S0,OUTPUT);
  pinMode(S1,OUTPUT); 
  pinMode(S2,OUTPUT);//(S2,S3)=(0,0)->R, (0,1)->B, (1,1)->G, (1,0)->clear.
  pinMode(S3,OUTPUT);
  pinMode(SW,OUTPUT);
  pinMode(led,OUTPUT);
  digitalWrite(S1,HIGH);
  digitalWrite(S0,HIGH);//Frequency scaling 100%
  digitalWrite(SW,HIGH);
  digitalWrite(led,HIGH);
  lcd.setCursor(0,0);
  lcd.print("Color (RGB):  ");
  while(Serial.available()==0&& digitalRead(pinsw)==1){
    for (int i=0;i<3;i++){
      if(i==0){//RED
        digitalWrite(S2,0);
        digitalWrite(S3,0);
        delay(200);
        R=(1.0/pulseIn(out,HIGH))*100000;
      }else if(i==1){//GREEN
        digitalWrite(S2,1);
        digitalWrite(S3,1);
        delay(200);
        G=(1.0/pulseIn(out,HIGH))*100000;
      }else if(i==2){//BLUE
         digitalWrite(S2,0);
         digitalWrite(S3,1);
         delay(200);
         B=(1.0/pulseIn(out,HIGH))*100000;
      } 
    }
	int maximum=max(R,G);
	maximum=max(maximum,B);
	R=R*255.0/maximum; //Renormaliza (R,G,B)
	G=G*255.0/maximum;
	B=B*255.0/maximum;
	lcd.setRGB(R, G, B);
    lcd.setCursor(0,1);
    lcd.print(R);
    lcd.print("   ");
    lcd.setCursor(4,1);
    lcd.print(",");
    lcd.print(G);
    lcd.print("   ");
    lcd.setCursor(10,1);
    lcd.print(",");
    lcd.print(B);
    lcd.print("   ");
    Serial.print(R);
    Serial.print('\t');
    Serial.print(G);
    Serial.print('\t');
    Serial.println(B);
	analogWrite(11,R);//Genera el color a traves de un led RGB.
	analogWrite(12,G);
	analogWrite(13,B);
  }
  //digitalWrite(SW,LOW);
  //digitalWrite(led,LOW);
  Serial.read();//Limpia el buffer serial
  lcd.setRGB(100,100,100);
  lcd.clear();
  encoder_inicio=-999;
  delay(100);
 if(digitalRead(pinsw)==0){
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

/*Sensor air Quality*/

void air_quality(){
  lcd.setCursor(0,0);
  lcd.print("Calid. del aire:");
  lcd.setCursor(0,1);
  lcd.print("Calibrando......");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Calid. del aire:");
  int V_0=analogRead(A14);
  unsigned long t_inicial=millis();
  unsigned long t;
  int sensor_val;
  String quality;
  while(Serial.available()==0&& digitalRead(pinsw)==1){
    t=(millis()-t_inicial)/1000.0;
    sensor_val=(analogRead(A14)-V_0);
	if(sensor_val<=200){
	 quality="normal";
	}else if(sensor_val>200 && sensor_val<400){
	 quality="medio";
	}else if(sensor_val>=400 && sensor_val<700){
	 quality="alto ";
	}else if(sensor_val>=700){
	 quality="peligro";//Los intervalos están basados en la librería AirQuality_Sensor.zip
	}
    Serial.print(t);
    Serial.print('\t');
	Serial.print(sensor_val);
    Serial.print('\t');
    Serial.println(quality);
    lcd.setCursor(0,1);
    lcd.print(sensor_val);
    lcd.print("        ");
    lcd.setCursor(4,1);
	lcd.print(quality);
	lcd.setCursor(11,1);
    lcd.print("-A14-");
    delay(500);
  }
  Serial.read();//Limpia el buffer serial
  lcd.clear();
  encoder_inicio=-999;
  delay(100);
 if(digitalRead(pinsw)==0){
  while(pinsw==0);//Hasta que no deje de presionar el encoder no sale de la función
 }
 delay(500);
}

