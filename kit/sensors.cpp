#include "ardufisica.h"

String firstLine, secondLine;
int calibrar_temp;

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
       			photoResitor();
        		break;
			case 8:
				ritmo_cardiaco();
				break;
			case 9:
				conductancia();
				break;
			case 10:
				Termocupla_k();
				break;
			case 11:
				luminosidad();
				break;
			case 12:
				Water_Atomization();
				break;
			default:
				Serial.print("Error...\n Sensor no existe");
		}
	}
}

void lee_microfono(int SIG, int Timedelay){
	firstLine = "Sound: A" + String(SIG); 
  	long Intensity = 0;
  
	lcd_mensage(firstLine);
  	Ardu_mensage("Sound Sensor");
	Serial.println("Intensidad");
	
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		for(int i = 0; i < 1000; i++)
		  Intensity += analogRead(SIG);
		Serial.println(Intensity);	// pin A12 --default
    secondLine = String(Intensity) + "    dt=" + String(Timedelay)+ " ms";
    lcd_mensage(firstLine, secondLine);
    
    Intensity = 0;
     
    delay(Timedelay); // tiempo de espera en ms -- 100 ms default
	}
	clean_buff();
}

void sensor_hall(int AO, int Timedelay){
	analogReference(DEFAULT);
	float factor = 5000.0/(1023.0*1.3); //1.3mV/Gauss , 5V->1023
  	firstLine = "Hall: A" + String(AO);
  	secondLine = "Calib.campo ...";
  
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
	analogReference(INTERNAL1V1);
	clean_buff();
}

void generador_sonido(int SIG){
	//tiempo en microsegundo para generar tono de frecuencias (261 Hz, 277Hz, 293 Hz, ...)
	int Tiempo_Tono[] = {1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275,
	                   1203, 1136, 1072 ,1012}; 

	int encoder_inicio = myEnc.read();
	int tono = 0;
	firstLine = "Speaker : D" + String(SIG);
	secondLine = String(tono ) + "         " + String(500000/Tiempo_Tono[tono]) + " Hz "; 

	lcd_mensage(firstLine, secondLine);
	while(digitalRead(pinsw) == 1){ 
    	Encoder_menu(0, 11, &tono);
      	
		secondLine = String(tono) + "        " + String(500000/Tiempo_Tono[tono]) + " Hz ";
		lcd_mensage(firstLine, secondLine);
	
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
	firstLine = "Ultrasonic: D" + String(Trigger) + "-D" + String(Echo);
	
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
	float temperaturaOBJ, temperaturaSUR;
	firstLine = "Infra.Temp: A" + String(SUR) + "-A" + String(OBJ);
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
		secondLine = String(temperaturaSUR) + " " + String(temperaturaOBJ) + " " + char(223) + "C";

		lcd_mensage(firstLine, secondLine);

	}
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
	
	firstLine = "Color (RGB): " + String(S0) + "->" + String(out);
	Ardu_mensage("Sensor de color -TCS3200");
	Serial.println("R\tG\tB");
	while(Serial.available() == 0 && digitalRead(pinsw) == 1){
		for (int i = 0; i < 3; i++){
			if(i == 0){  //RED
				digitalWrite(S2,0);
				digitalWrite(S3,0);
				delay(20);
				R = abs(100000.0/pulseIn(out,HIGH)); //pin 47 --default
			}
			else if(i == 1){  //GREEN
				digitalWrite(S2,1);
				digitalWrite(S3,1);
				delay(20);
				G = abs(100000.0/pulseIn(out,HIGH));
			}
			else if(i == 2){  //BLUE
				 digitalWrite(S2,0);
				 digitalWrite(S3,1);
				 delay(20);
				 B =abs(100000.0/pulseIn(out,HIGH));
			} 
		}
		delay(600);
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
	firstLine = "Air.qual: A" + String(SIG);
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

void photoResitor(int S, int Timedelay){
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

void conductancia(int PIN){
	analogReference(DEFAULT);
	firstLine = "GSR: A" + String(PIN);
	int sensorValue = 0;
	int gsr_average = 0;
	long sum = 0;

	lcd_mensage(firstLine);
	delay(500);
	print_pantalla(firstLine,"Enviando al PC...");
	delay(500);
	
	Ardu_mensage("Galvanometro");
	// poner encabezado....
	while (digitalRead(pin_sw)==1){
	sum = 0;
	for(int i = 0; i < 10; i++){
	  sensorValue = analogRead(PIN);
	  sum += sensorValue;
	  delay(5);
	  }
	gsr_average = sum / 10;
	secondLine = "GSR_Avg    "+ String(gsr_average);
	lcd_mensage(firstLine,secondLine);
	Serial.println(gsr_average);
	}
		analogReference(INTERNAL1V1);
		clean_buff();
	}

void Termocupla_k(int CONFIG_TCSCK, int CONFIG_TCCS, int CONFIG_TCDO, int Timedelay){
	MAX6675 thermocouple(CONFIG_TCSCK, CONFIG_TCCS, CONFIG_TCDO);
	double temp = thermocouple.readCelsius();

	firstLine = "Termocupla: D" + String(CONFIG_TCCS);
	secondLine = "      " + String(temp) + " " + char(223) + "C";
	
	lcd_mensage(firstLine, secondLine);
	delay(500);
	lcd_mensage(firstLine, "Enviando al PC...");
	delay(500);

	Ardu_mensage("Termocupla tipo K")
	Serial.println("Temp (°C) ")
	while(digitalRead(pin_sw) == 1){
		temp = thermocouple.readCelsius(); 
		secondLine = "      " + String(temp) + " " + char(223) + "C";
	    
	    lcd_mensage(firstLine,secondLine);
	    Serial.println(temp);
	    // ESPERAR UN SEGUNDO ENTRE LAS LECTURAS
	    delay(Timedelay);
    }
	clean_buff();  
}

void Electromagn(int PIN){
	firstLine = "Electromag: D" + String(PIN);
	int state = 0;

	digitalWrite(PIN,LOW);
	lcd_mensage(firstLine," Electroiman Off ");
	
	encoder_inicio = myEnc.read();
	while(digitalRead(pin_sw)== 1){ 
		Encoder_menu(0,1, &state);
		
		if (state = 0){
			digitalWrite(PIN,LOW);
			secondLine = " Electroiman Off";
		}
		else if(state = 1){
			secondLine = " Electroiman On";
			digitalWrite(PIN,LOW);
		}
		else{
			digitalWrite(PIN,LOW);
			secondLine = " Electroiman Off";
		}

		lcd_mensage(firstLine, secondLine);
	}
	clean_buff();
}

void luminosidad(){
	uint16_t lux = 0;
	BH1750 luxometro;
	firstLine = "Intensidad: IIC";
	luxometro.begin(BH1750::CONTINUOUS_HIGH_RES_MODE); //Inicializar BH1750 sensor de Luminosidad
	
	lcd_mensage(firstLine);
	delay(500);
	lcd_mensage(firstLine, "Enviando al PC...")
	delay(500);

	Serial.println("Luminosidad (lx)");
	while(digitalRead(pin_sw) == 1){  
		lux = luxometro.readLightLevel(); // Lectura del BH1750
		Serial.println(lux);
		secondLine = "      " + String(lux) + " lx";
		lcd_mensage(firstLine, secondLine);
		delay(500);
	}
	clean_buff();
}

void Servomotor(int PIN){
	servoMotor.attach(PIN); //Microservo naranja
	Servo servoMotor;

	encoder_inicio = myEnc.read();
	int ang = 0;
	
	firstLine = "Servo: D" + String(PIN);
	secondLine = "   Angulo  0";

	servoMotor.write(180);
	lcd_mensage(firstLine, secondLine);

	while(digitalRead(pin_sw)== 1){ 
		Encoder_menu(0, 180, &ang, 3);
		servoMotor.write(180-ang);

		secondLine = "   Angulo  " + String(ang); 
		lcd_mensage(firstLine, secondLine);
	}
    clean_buff();
}

void Water_Atomization(int PIN){
	firstLine = "Atomizer: D" + String(PIN);
	encoder_inicio = myEnc.read();

	int option = 0;
	digitalWrite(PIN,LOW);
	lcd_mensage(firstLine, "Atomizacion Off");
	
	while(digitalRead(pin_sw)== 1){ 
		Encoder_menu(0, 1, &option)

		if (option = 1){
			digitalWrite(PIN, HIGH);
			lcd_mensage(firstLine, "Atomizacion on");
		}
		else{
			digitalWrite(PIN, LOW);
			lcd_mensage(firstLine, "Atomizacion off");
		}
	clean_buff();	
}