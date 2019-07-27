#include "ardufisica.h"

int calibrar_temp;

void run_sensor(int posicion_menu){
	if(digitalRead(pinsw)==0){
		delay(100);
		while(digitalRead(pinsw)==0);
			lcd.clear();
		switch(posicion_menu){
			case 0:
				microfono();
				break;
			case 1:
				hall_magnetico();
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
				calidad_aire();
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
				electroIman();
				break;
			case 12:
				luminosidad();
				break;
      case 13:
        Servomotor();
        break;
			case 14:
				atomizador();
				break;
			case 15:
				EMG_detector();
				break;
			case 16:
				Led_RGB();
				break;
			case 17:
				Tacometro();
				break;
			default:
				Serial.print("Error...\n Sensor no existe");
		}
	}
}

void microfono(int SIG, int Timedelay){
	analogReference(DEFAULT);
	firstLine = "Sound: A" + String(SIG); 
  	long Intensity = 0;
  
	lcd_mensage(firstLine);
  	Ardu_mensage("Grove - Sound Sensor");
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
	analogReference(INTERNAL1V1);
	clean_buff();
}

void hall_magnetico(int AO, int Timedelay){ //https://www.lelong.com.my/hall-sensor-magnetic-module-board-field-detecting-sensor-arduino-ubitronix-191662331-2020-04-Sale-P.htm
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
	Ardu_mensage("Hall Sensor Magnetic");
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
	firstLine = "Ultrasonic:D" + String(Trigger) + "-D" + String(Echo);
	
	lcd_mensage(firstLine);
	
	unsigned long t_inicial = millis(), t;
	int distancia;

	Ardu_mensage("Ultrasonic Sensor HC-SR04");
  	Serial.println("t (s) \t ditancia (cm)");
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
	firstLine = "Infra.Temp:A" + String(SUR) + "-A" + String(OBJ);
	lcd_mensage(firstLine);

	unsigned long t_inicial = millis(), t;
	
	Ardu_mensage("Infrared Temperature Sensor");
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
		secondLine = String(temperaturaSUR) + " " + String(temperaturaOBJ) + "  " + char(223) + "C";
		lcd_mensage(firstLine, secondLine);

	}
	clean_buff();
}

void color(int S0, int S1, int S2, int S3, int GND2, int led, int out){
	
	int R, G, B;
	int maximum;
	firstLine = "Color(RGB):" + String(S0) + "-" + String(out);
	lcd_mensage(firstLine);
	
	pinMode(S0,OUTPUT); //pin 39 --default
	pinMode(S1,OUTPUT); //pin 41 --default
	pinMode(S2,OUTPUT);//(S2,S3)=(0,0)->R, (0,1)->B, (1,1)->G, (1,0)->clear.  //pin 43 --default
	pinMode(S3,OUTPUT); //pin 45 --default
	pinMode(GND2,OUTPUT); //pin 53 --default
	pinMode(led,OUTPUT); //pin 51 --default
	digitalWrite(S1,HIGH); 
	digitalWrite(S0,HIGH);//Frequency scaling 100% 
	digitalWrite(GND2,LOW); 
	digitalWrite(led,HIGH); 
	
	
	Ardu_mensage("Color sensor - TCS3200");
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
		delay(200);
		maximum = max(R, G);
		maximum = max(maximum, B);
		R = R * 255.0 / maximum; //Renormaliza (R,G,B)
		G = G * 255.0 / maximum;
		B = B * 255.0  /maximum;
		lcd.setRGB(R, G, B);
		
		secondLine = "R:" + String(R) + " G:" + String(G) + " B:" + String(B);
		lcd_mensage(firstLine, secondLine);
		
		Serial.print(R);
		Serial.print('\t');
		Serial.print(G);
		Serial.print('\t');
		Serial.println(B);
	}
	
	digitalWrite(led,LOW);
	lcd.setRGB(100,100,100);
	clean_buff();
}

void calidad_aire(int SIG, int Timedelay){
	analogReference(DEFAULT);
	int sensor_val, i = 0;
	String quality;
	String mensage = {"El sensor debe llevar conectado al menos 3 min para que pueda funcionar adecuadamente. Ponga el sensor en un lugar con aire de buena calidad para la calibracion inicial. Luego, oprima el boton para continuar. "};
	int V_0 = 0;
 
  	delay(1000);
	while(digitalRead(pinsw) == 1){
		if (i + 16 < mensage.length())
			lcd_mensage(mensage.substring(i, i + 16));
		else
			i = 0;
		i++;
		delay(100);
	}
	delay(100);
	while(digitalRead(pinsw) == 0);
  	firstLine = "Cal. Aire: A" + String(SIG);
	lcd_mensage(firstLine);

	V_0 = analogRead(SIG);
	lcd.clear();
	lcd_mensage(firstLine,"Sensor calibrado");
	delay(2000);
	lcd_mensage(firstLine,"Enviando al PC...");
	delay(1000);

	Ardu_mensage("Air Quality sensor");
    Serial.println("Valor");
	while (digitalRead(pinsw) == 1){
		sensor_val = abs(analogRead(SIG)-V_0);
		
		if(sensor_val <= 200)
			quality = "Cal.buena ->";
		else if(sensor_val > 200 && sensor_val < 400)
		 	quality = "Cal.media-> ";
		else if(sensor_val >= 400 && sensor_val < 700)
		 	quality = "Cal.baja-> ";
		else if(sensor_val >= 700)
		 	quality = "Peligro->";//Los intervalos están basados en la librería AirQuality_Sensor.zip
		 
		secondLine = quality + String(sensor_val) +"  ";
		lcd_mensage(firstLine , secondLine);
		
		Serial.println(sensor_val);
		delay(Timedelay);
	}

	analogReference(INTERNAL1V1);
	clean_buff();
}

void photoResitor(int S, int Timedelay){
  analogReference(DEFAULT);
  int value = 0;
  firstLine = "Fotorres: A" + String(S);
  
  lcd_mensage(firstLine, "Enviando al PC...");
  delay(500);

  Ardu_mensage("Fotorresistencia ky-018");
  Serial.println("Valor");
 
  while ( Serial.available() == 0 && digitalRead(pinsw) == 1){
    value = analogRead(S);
    
    Serial.println(value);
    secondLine = String(value) + "      dt =" + String(Timedelay); 

    lcd_mensage(firstLine, secondLine);
    delay(Timedelay);
  }
  analogReference(INTERNAL1V1);
  clean_buff();
}

void ritmo_cardiaco(int Timedelay){
  
	unsigned char c;
	firstLine = "Ritmo Cardi: IIC";
  
	lcd_mensage(firstLine, "Enviando al PC...");
    delay(500);
    lcd.clear();
	Ardu_mensage("finger clip heart rate sensor");
	Serial.println("frecuencia  (BPM)");
	while(Wire.available() == 0 && digitalRead(pinsw) == 1){ // slave may send less than requested
	    Wire.requestFrom( 0xA0  >>  1 ,  1 );     // solicitar 1 bytes desde el dispositivo esclavo         
      	c = Wire.read();   // receive heart rate value (a byte)
      	Serial.println(c, DEC);         // print heart rate value
	    secondLine = String(c) + "        BPM";
	    lcd_mensage(firstLine, secondLine);
	
	  	delay(Timedelay); 
	}
    clean_buff();
}

void conductancia(int PIN){
	analogReference(DEFAULT);
	firstLine = "GSR:   A" + String(PIN);
	int sensorValue = 0;
	int gsr_average = 0;
	long sum = 0;

	lcd_mensage(firstLine);
	delay(500);
	lcd_mensage(firstLine,"Enviando al PC...");
	delay(500);
	
	Ardu_mensage("Galvanic Skin Response (GSR)");
	Serial.println("GSR (Promedio)");
	while (digitalRead(pinsw)==1){
		sum = 0;
		for(int i = 0; i < 10; i++){
			sensorValue = analogRead(PIN);
			sum += sensorValue;
			delay(5);
		}
		gsr_average = sum / 10;
		secondLine = " GSR -> " +  String(gsr_average) + "          ";
		
		lcd_mensage(firstLine, secondLine);
		Serial.println(gsr_average);
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void Termocupla_k( int CS, int SCK, int SO, int Timedelay){
	MAX6675 thermocouple(SCK, CS, SO);
	double temp = thermocouple.readCelsius();

	firstLine = "Termocupla: D" + String(CS);
	
	delay(500);
	lcd_mensage(firstLine, "Enviando al PC...");
	delay(500);

	Ardu_mensage("Termocupla tipo K - MAX6675K");	
	Serial.println("Temp (°C) ");
	while(digitalRead(pinsw) == 1){
		temp = thermocouple.readCelsius(); 
		secondLine = " " + String(temp) + "    " + char(223) + "C         ";
	    
	    lcd_mensage(firstLine,secondLine);
	    Serial.println(temp);
	    delay(Timedelay);
    }
	clean_buff();  
}

void electroIman(int PIN){
    encoder_inicio = myEnc.read();
	firstLine = "Electromag: D" + String(PIN);
	int state = 1;

	digitalWrite(PIN,LOW);
	lcd_mensage(firstLine," Electroiman Off  ");
	delay(100);
 
	while(digitalRead(pinsw)== 1){ 
		Encoder_menu(0, 1, &state);
		if (state == 0){
			digitalWrite(PIN,HIGH);
			secondLine = " Electroiman on  ";
		}
		else if(state == 1){
			secondLine = " Electroiman off  ";
			digitalWrite(PIN,LOW);
		}
		else{
			digitalWrite(PIN,LOW);
			secondLine = " Electroiman Off  ";
		}

		lcd_mensage(firstLine, secondLine);
	}
    digitalWrite(PIN,LOW);
	clean_buff();
}

void luminosidad(){
	uint16_t lux = 0;
	BH1750 luxometro;
	firstLine = "Intensidad: IIC";
	luxometro.begin(BH1750::CONTINUOUS_HIGH_RES_MODE); //Inicializar BH1750 sensor de Luminosidad
	
	lcd_mensage(firstLine);
	delay(500);
	lcd_mensage(firstLine, "Enviando al PC...");
	delay(500);
	Ardu_mensage("Light Intensity Sensor - GY302");
	Serial.println("Luminosidad (lx)");
	while(digitalRead(pinsw) == 1){  
		lux = luxometro.readLightLevel(); // Lectura del BH1750
		Serial.println(lux);
		secondLine = " " + String(lux) + "       lx    ";
		lcd_mensage(firstLine, secondLine);
		delay(500);
	}
	clean_buff();
}

void Servomotor(int PIN){ //SG90
    Servo servoMotor;
	servoMotor.attach(PIN); //Microservo naranja

	encoder_inicio = myEnc.read();
	int ang = 0;
	
	firstLine = "Servo:  D" + String(PIN);
	secondLine = "   Angulo  0";

	servoMotor.write(180);
	lcd_mensage(firstLine, secondLine);

	while(digitalRead(pinsw) == 1){ 
		Encoder_menu(0, 180, &ang, 3);
		servoMotor.write(180-ang);

		secondLine = "   Angulo  " + String(ang); 
		lcd_mensage(firstLine, secondLine);
	}
    clean_buff();
}

void atomizador(int PIN){
	firstLine = "Atomizador: D" + String(PIN);
	encoder_inicio = myEnc.read();

	int option = 0;
	digitalWrite(PIN,LOW);
	lcd_mensage(firstLine, "Atomizacion Off");
	
	while(digitalRead(pinsw)== 1){ 
		Encoder_menu(0, 1, &option);

		if (option == 1){
			digitalWrite(PIN, HIGH);
			lcd_mensage(firstLine, "Atomizacion on  ");
		}
		else{
			digitalWrite(PIN, LOW);
			lcd_mensage(firstLine, "Atomizacion off  ");
		}	
 	}
 	clean_buff();
}

void EMG_detector(int VOUT, int Timedelay){
	analogReference(DEFAULT);
	int valor =0, valor_final=0;
	long suma = 0;

	firstLine = "EMG: A" + String(VOUT);

	for(int i = 0; i <= 10; i++){
		for(int j = 0; j < 100; j++){
		 	suma += getAnalog(VOUT);
		  	delay(1);
		}
		secondLine = "Calibrando " + String(i+1) ;
		lcd_mensage(firstLine, secondLine);
	}

	suma /= 1100;

	lcd_mensage(firstLine,"Sistema calibrado");
	delay(2000);
	lcd_mensage(firstLine,"Enviando al PC...");

	Ardu_mensage("Grov - EMG detector");
	Serial.println("V_out");
	while(digitalRead(pinsw) == 1){
		valor = getAnalog(VOUT);
		secondLine = "VOUT -> " + String(valor) + "       ";

		lcd_mensage(firstLine, secondLine);
		Serial.println(valor);
		delay(Timedelay);
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void Led_RGB(int R, int G, int B){
	int encoder_inicio = myEnc.read();
	int opcion = 1;
	int Pin = R;
	int Salir = 0, Salir_color = 0;
	int Valor = 0;
	String opciones[5] = {"Salir del Menu", "Intensidad Rojo", 
						  "Intensidad Verde", "Intensidad Azul"};
	analogWrite(R,LOW);//Red
	analogWrite(G,LOW);//Green
	analogWrite(B,LOW);//Blue

	firstLine = "RGB: D" + String(R) +"-D"+ String(G) + "-D" + String(B);
	
	lcd_mensage(firstLine, opciones[opcion]);
	while (digitalRead(pinsw) == 0){
		while(Salir == 0){
			while (digitalRead(pinsw) == 0){
				Encoder_menu(0, 3, &opcion);
				if (opcion == 1){
					Pin = R;
					lcd_mensage(firstLine,opciones[opcion]);
				}
				else if (opcion == 2){
					Pin = G;
					lcd_mensage(firstLine,opciones[opcion]);
				}
				else if (opcion == 3){
					Pin = B;
					lcd_mensage(firstLine,opciones[opcion]);
				}
				else if (opcion == 0)
					lcd_mensage(firstLine,opciones[opcion]);
			}	

			if (opcion == 0 && digitalRead(pinsw) == 0)
			    Salir = 1;

			else if (digitalRead(pinsw) == 0){
				delay(100);
				while(digitalRead(pinsw) == 0);
				lcd_mensage(firstLine,"Gire la perilla ");

				encoder_inicio = myEnc.read();
				while(Salir_color == 0){
					while(digitalRead(pinsw) == 0){
						Encoder_menu(0, 255, &Valor);
						lcd_mensage(firstLine,"Intensidad: "+String(Valor));
					 }

					analogWrite(Pin, Valor);

					if (digitalRead(pinsw) == 0){
					  Salir_color = 1;
					  delay(100);
					  while(digitalRead(pinsw) == 0);
					  delay(100);
					}
				}

				Salir_color = 0;
				opcion = 0;
				lcd_mensage(firstLine,opciones[opcion]);
			}
		}
	}
	clean_buff();
}

void Tacometro(int OUT){
	analogReference(DEFAULT);
	int valor = 0;
	int t1 = 0, t2= 0, T_Osc = 0;
	int Salir = 0;
	
	firstLine = "Tacometro: A" + String(OUT);

	lcd_mensage(firstLine,"Envia al PC");
	delay(500);
	
	while(Salir == 0){
		valor = ( analogRead(OUT) * 5) / 1023;

		while(valor > 2 and Salir == 0){
		  	if (digitalRead(pinsw) == 0)
		  		Salir =1;
		  	valor = (analogRead(OUT) * 5) / 1023;
		}
		lcd_mensage(firstLine,"Esperando");

		while(valor < 3 and Salir == 0){
		  	if (digitalRead(pinsw) == 0)
		    	Salir =1;
		  	valor= (analogRead(OUT) * 5) / 1023;
		}

		t1 = millis();
		lcd_mensage(firstLine, "Tomando tiempo");
		
		while(valor > 2 and Salir==0){
		  	if (digitalRead(pinsw) == 0)
		    	Salir = 1;
		  	valor= (analogRead(OUT) * 5) / 1023;
		}

		t2 = millis();
		T_Osc = t2 - t1;
		
		if (Salir == 0){
			secondLine = "T.Osc.->"+ String(T_Osc) + "  ms";
		  	lcd_mensage(firstLine, secondLine);
		  	Serial.println(T_Osc);
		}
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}
