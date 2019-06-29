#include "ardufisica.h"

String firstLine, secondLine;

void run_sensor(int posicion_menu){
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
				generador_sonido();//por ahora constante La4
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
				air_quality();
				break;
      		case 7:
       			light_intensity();
        			break;
			case 8:
				ritmo_cardiaco();
				break;
			default:
				Serial.print("Error...\n Sensor no existe");
		}
	}
}

void lee_microfono(int SIG, int Timedelay){
	firstLine = "S.sonido: A" + String(SIG); 
  long Intensity = 0;
  
	lcd_mensage(firstLine);
  	Ardu_mensage("Sound Sensor");
	Serial.println("Intensidad");
	
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		for(int i = 0; i < 1000; i++)
		  Intensity += analogRead(SIG);
		Serial.println(Intensity);	// pin A12 --default
    secondLine = String(Intensity) + "   dt=" + String(Timedelay)+ " ms";
    lcd_mensage(firstLine, secondLine);
    
    Intensity = 0;
     
    delay(Timedelay); // tiempo de espera en ms -- 100 ms default
	}
	clean_buff();
}

void sensor_hall(int AO, int Timedelay){
	float factor = 5000.0/(1023.0*1.3); //1.3mV/Gauss , 5V->1023
  	firstLine = "Sen.Hall: A" + String(AO);
  	secondLine = "Calib.campo 0...";
  
	lcd_mensage(firstLine, secondLine);
	delay(1000);
	lcd.clear();
	lcd_mensage(firstLine);
 
	int V_0 = analogRead(AO), campo;
	unsigned long t_inicial = millis(), t; 
	Ardu_mensage("Hall effect Sensor");
  	Serial.println("tiempo(s)  Campo(G)");
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		t = (millis() - t_inicial);
		campo = (analogRead(AO) - V_0) * factor; // pin A14 --default
		Serial.print(t/1000.0); //FALTA EL FACTOR DE CONVERSIÓN
		Serial.print('\t');
		Serial.println(campo);

    secondLine = String(campo) + "           G ";
		lcd_mensage(firstLine, secondLine);
		delay(Timedelay); // tiempo de espera en ms --500 ms default
	}
	clean_buff();
}

void generador_sonido(int SIG){
	//tiempo en microsegundo para generar tono de frecuencias (261 Hz, 277Hz, 293 Hz, ...)
	int Tiempo_Tono[] = {1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275,
	                   1203, 1136, 1072 ,1012}; 

	int encoder_ground = myEnc.read();
	int tono = 0;
	firstLine = "Speaker : D" + String(SIG);
	secondLine = String(tono + 1) + "         " + String(500000/Tiempo_Tono[tono]) + " Hz "; 

	lcd_mensage(firstLine, secondLine);
	while(digitalRead(pinsw) == 1){ 
		if (abs(myEnc.read() - encoder_ground) > 7){ //si el encoder se giro
			if ((myEnc.read() - encoder_inicio) > 0)
				tono += 1;
			else
				tono -= 1;

			encoder_ground = myEnc.read(); 
			if (tono == 12)
				tono = 0;
			else if(tono == -1)
				tono = 11;
			secondLine = String(tono + 1) + "         " + String(500000/Tiempo_Tono[tono]) + " Hz ";
			lcd_mensage(firstLine, secondLine);
		}
		digitalWrite(SIG,HIGH);
		delayMicroseconds(Tiempo_Tono[tono]);
		digitalWrite(SIG,LOW);
		delayMicroseconds(Tiempo_Tono[tono]);
	}
	digitalWrite(SIG,LOW);
	clean_buff();
}


void distancia_ultrasonido(int Trigger, int Echo){
	Ultrasonic ultrasonido(Trigger,Echo,30000); //(trig, echo, max_distancia)
	firstLine = "Distancia: D" + String(Trigger) + "-D" + String(Echo);
	
	lcd_mensage(firstLine);
	
	unsigned long t_inicial = millis(), t;
	int distancia;
  
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		t = millis() - t_inicial;
		distancia = ultrasonido.Ranging(CM);
		Serial.print(t / 1000.0);
		Serial.print('\t');
		Serial.println(distancia);

    	secondLine = String(distancia) + "          cm ";
		lcd_mensage(firstLine, secondLine);
		delay(500);
	}
	clean_buff();
}

void temperatura_infrarrojo(int SUR, int OBJ){
	analogReference(INTERNAL1V1);
	float temperaturaOBJ, temperaturaSUR;
	firstLine = "Sen.Temp:  -A" + String(SUR) + "- ";
  secondLine;
  	
	lcd_mensage(firstLine);

	unsigned long t_inicial = millis(), t;
  	Ardu_mensage("Sensor de temperatura infrarojo");
  	Serial.println("Sensor voltaje tiempo(s) TempSUR(°C)  TempOBJ(°C)");
  
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		t = millis() - t_inicial;
		measureSurTemp(&temperaturaSUR, SUR);
		measureObjectTemp(&temperaturaOBJ, OBJ);
		Serial.print(t / 1000.0);
    Serial.print('\t');
    Serial.print(temperaturaSUR);
		Serial.print('\t');
		Serial.println(temperaturaOBJ);
    secondLine = String(temperaturaSUR) + "  " + String(temperaturaOBJ) + " C ";

    lcd_mensage(firstLine, secondLine);

	}
	analogReference(DEFAULT);
  clean_buff();
}

void color(int S0, int S1, int S2, int S3, int SW,int led, int out){
	
	int R, G, B;
	int maximum;

	pinMode(S0,OUTPUT); //pin 39 --default
	pinMode(S1,OUTPUT); //pin 41 --default
	pinMode(S2,OUTPUT);//(S2,S3)=(0,0)->R, (0,1)->B, (1,1)->G, (1,0)->clear.  //pin 43 --default
	pinMode(S3,OUTPUT); //pin 45 --default
	pinMode(SW,OUTPUT); //pin 53 --default
	pinMode(led,OUTPUT); //pin 51 --default
	digitalWrite(S1,HIGH); 
	digitalWrite(S0,HIGH);//Frequency scaling 100% 
	digitalWrite(SW,HIGH); 
	digitalWrite(led,HIGH); 
	
	firstLine = "S.Color:-" + String(S0) + "->" + String(out) + "-";
	Ardu_mensage("Sensor de color -TCS3200");
	Serial.println("R\tG\tB");
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		for (int i = 0; i < 3; i++){
			if(i == 0){  //RED
				digitalWrite(S2,0);
				digitalWrite(S3,0);
				delay(200);
				R = (1.0 / pulseIn(out, HIGH)) * 100000; //pin 47 --default
			}
			else if(i == 1){  //GREEN
				digitalWrite(S2,1);
				digitalWrite(S3,1);
				delay(200);
				G = (1.0 / pulseIn(out, HIGH)) * 100000;
			}
			else if(i == 2){  //BLUE
				 digitalWrite(S2,0);
				 digitalWrite(S3,1);
				 delay(200);
				 B = (1.0 / pulseIn(out,HIGH)) * 100000;
			} 
		}
		
		maximum = max(R, G);
		maximum = max(maximum, B);
		R  =R * 255.0 / maximum; //Renormaliza (R,G,B)
		G = G * 255.0 / maximum;
		B = B * 255.0  /maximum;
		lcd.setRGB(R, G, B);
		
		secondLine = String(R) + "  ," + String(G) + "  ," + String(B) + " ";
		lcd_mensage(firstLine, secondLine);
		
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
	lcd.setRGB(100,100,100);
	clean_buff();
}

void air_quality(int SIG, int Timedelay){
	firstLine = "Calid.aire:-A" + String(SIG) + "- ";
	secondLine = "Calibrando...   ";
	
	lcd_mensage(firstLine, secondLine);
	delay(1000);
	lcd.clear();
	lcd_mensage(firstLine);
	
	int V_0 = analogRead(SIG); // pin A14 --default
	unsigned long t_inicial = millis() , t;
	int sensor_val;
	String quality;
	Ardu_mensage("Sensor de calidad de aire");
  	Serial.println("t\tS.val\tCalidad");
	
	while(Serial.available()== 0 && digitalRead(pinsw) == 1){
		t = millis() - t_inicial;
		sensor_val = (analogRead(SIG) - V_0);

		if(sensor_val <= 200)
			quality = "normal";
		else if(sensor_val > 200 && sensor_val < 400)
			quality = "medio";
		else if(sensor_val >= 400 && sensor_val < 700)
			quality = "alto ";
		else if(sensor_val >= 700)
		 quality="peligro";//Los intervalos están basados en la librería AirQuality_Sensor.zip
		
		Serial.print(t / 1000.0);
		Serial.print('\t');
		Serial.print(sensor_val);
		Serial.print('\t');
		Serial.println(quality);
		secondLine = String(sensor_val) + "     " + quality + " ";
    lcd_mensage(firstLine, secondLine);
		delay(Timedelay);  // tiempo de espera en ms --500 ms default
	}
	clean_buff();
}

void light_intensity(int S, int Timedelay){
  int intensity = 0;
  firstLine = "Light.sensor: A" + String(S);
  
  lcd_mensage(firstLine);
  Ardu_mensage("Fotorresistencia ky-018 - Sensor de intensidad de luz");
  Serial.println("Intensidad");
 
  while ( Serial.available() == 0 && digitalRead(pinsw) == 1){
    intensity = analogRead(S);
    
    Serial.println(intensity);
    secondLine = String(intensity) + "    dt=" + String(Timedelay); 

    lcd_mensage(firstLine, secondLine);
    delay(Timedelay);
  }
    
  clean_buff();
}

void ritmo_cardiaco(int Timedelay){
	unsigned char c;
	firstLine = "Heart.s:  IIC";
	Ardu_mensage("heart rate sensor");
	
	while(Wire.available() == 0 && digitalRead(pinsw) == 1){ // slave may send less than requested
	    Wire.requestFrom( 0xA0  >>  1 ,  1 );     // solicitar 1 bytes desde el dispositivo esclavo         
      c = Wire.read();   // receive heart rate value (a byte)
      Serial.println(c, DEC);         // print heart rate value
	    secondLine = String(c) + "      BPM";
	    lcd_mensage(firstLine, secondLine);
	
	  	delay(Timedelay); 
	}
  clean_buff();
}
