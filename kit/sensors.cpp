#include "ardufisica.h"


void microfono(int SIG, int Timedelay){
	/*
		SIG -> Voltage input
	*/
	analogReference(DEFAULT);
	firstLine = "Sound: A" + String(SIG); 
  	long Intensity = 0;
  
	lcd_mesagge(firstLine);
  	Ardu_mesagge("Grove - Sound Sensor");
	Serial.println("Intensidad");
	Serial1.println("Intensidad");
	
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		for(int i = 0; i < 1000; i++)
		  Intensity += analogRead(SIG);
		
		Serial.println(Intensity);	// pin A12 --default
		Serial1.println(Intensity);
    secondLine = String(Intensity) + "    dt=" + String(Timedelay)+ " ms";
    lcd_mesagge(firstLine, secondLine);
    
    Intensity = 0;
     
    delay(Timedelay); // tiempo de espera en ms -- 100 ms default
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void hall_magnetico(int AO, int Timedelay){ 
	/*
		AO -> Input Voltage
	 */
	analogReference(DEFAULT);
	float factor = 5000.0/(1023.0*1.3); //1.3mV/Gauss , 5V->1023
  	firstLine = "Hall: A" + String(AO);
  	secondLine = "Calib.campo ...";
  
	lcd_mesagge(firstLine, secondLine);
	delay(1000);
	lcd.clear();
	lcd_mesagge(firstLine);
 
	int V_0 = analogRead(AO), campo;
	unsigned long t_inicial = millis(), t; 
	Ardu_mesagge("Hall Sensor Magnetic M44");
  	Serial.println("tiempo(s)  Campo(G)");
  	Serial1.println("tiempo(s)  Campo(G)");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		t = (millis() - t_inicial);
		campo = (analogRead(AO) - V_0) * factor; // pin A14 --default
		Serial.print(t/1000.0); 
		Serial.print('\t');
		Serial.println(campo);
		Serial1.print(t/1000.0); 
		Serial1.print('\t');
		Serial1.println(campo);

    	secondLine = String(campo) + "           G ";
		lcd_mesagge(firstLine, secondLine);
		delay(Timedelay); // tiempo de espera en ms --500 ms default
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void generador_sonido(int SIG){
	/*
		SIG -> Enable sensor
	*/
	int tono = 0;
	String freq = "";
	bool continar = false;
  	String Lista_freq [12] ={"261", "277", "293", "311", "329", "349", "369", 
  							 "391", "415", "440", "466", "493"};
	//time in microsecond to generate frequency tone (261 Hz, 277Hz, 293 Hz,...)
	int Tiempo_Tono[12] = {1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275,
	                     1203, 1136, 1072 ,1012}; 

	firstLine = "Speaker : D" + String(SIG);
	secondLine = String(tono ) + "         "+ String(500000/Tiempo_Tono[tono]) +
				 " Hz "; 

	lcd_mesagge(firstLine, secondLine);
  	Ardu_mesagge("Grove - Speaker ");
	int encoder_inicio = myEnc.read();

	while(digitalRead(pinsw) == 1 and not(continar)){
		freq = serial_readPhrase();
		for (int i= 0; i < 12; i++){
                if(freq == "freq "+ Lista_freq[i]){
                    tono = i;
                    break;
                }
        
	  }
   	Serial.println(tono);
   	delay(500);
	} 

	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
      	Encoder_menu(0, 11, &tono);
    	
		secondLine = String(tono + 1)+"        "+String(500000/Tiempo_Tono[tono]) +
					" Hz ";
		lcd_mesagge(firstLine, secondLine);
	
		digitalWrite(SIG,HIGH); // pin A4 -- default
		delayMicroseconds(Tiempo_Tono[tono]);
		digitalWrite(SIG,LOW);
		delayMicroseconds(Tiempo_Tono[tono]);
	}
 
	digitalWrite(SIG,LOW);
	clean_buff();
}


void distancia_ultrasonido(int TRIG, int ECHO){
	/*
		TRIG -> Trigger output
		ECHO -> Echo Input 
	 */

	Ultrasonic ultrasonido(TRIG,ECHO,30000); // pin A6-A7 -- default 
	firstLine = "Ultrasonic:D" + String(TRIG) + "-D" + String(ECHO);
	
	lcd_mesagge(firstLine);
	
	unsigned long t_inicial = millis(), t;
	int distancia;

	Ardu_mesagge("Ultrasonic Sensor HC-SR04");
  	Serial.println("t (s) \t ditancia (cm)");
  	Serial1.println("t (s) \t ditancia (cm)");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		t = millis() - t_inicial;
		distancia = ultrasonido.Ranging(CM);
		Serial.print(t / 1000.0);
		Serial.print('\t');
		Serial.println(distancia);
		Serial1.print(t / 1000.0);
		Serial1.print('\t');
		Serial1.println(distancia);

    	secondLine = String(distancia) + "          cm ";
		lcd_mesagge(firstLine, secondLine);
		delay(500);
	}
	clean_buff();
}

void temperatura_infrarrojo(int SUR, int OBJ){
	/*
		SUR -> Surround temperature  input
		OBJ -> Objective temperature input
	*/

	float temperaturaOBJ, temperaturaSUR;
	firstLine = "Infra.Temp:A" + String(SUR) + "-A" + String(OBJ);
	lcd_mesagge(firstLine);

	unsigned long t_inicial = millis(), t;
	
	Ardu_mesagge("Grove - Infrared Temperature Sensor");
	Serial.println("Sensor voltaje tiempo(s) TempSUR(°C)  TempOBJ(°C)");
	Serial1.println("Sensor voltaje tiempo(s) TempSUR(°C)  TempOBJ(°C)");
	
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		t = millis() - t_inicial;
		measureSurTemp(&temperaturaSUR, SUR); // pin A0 -- default
		measureObjectTemp(&temperaturaOBJ, OBJ); // Pin A1 -- default
		Serial.print(t / 1000.0);
		Serial.print('\t');
		Serial.print(temperaturaSUR);
		Serial.print('\t');
		Serial.println(temperaturaOBJ);
		Serial1.print(t / 1000.0);
		Serial1.print('\t');
		Serial1.print(temperaturaSUR);
		Serial1.print('\t');
		Serial1.println(temperaturaOBJ);
		
		secondLine = String(temperaturaSUR) + " " + String(temperaturaOBJ) + 
					 "  " + char(223) + "C";
		lcd_mesagge(firstLine, secondLine);

	}
	clean_buff();
}

void color(int S0, int S1, int S2, int S3, int OE, int led, int out){
	/*
		S0, S1 -> Output frequency scaling selection inputs
		S2, S3 -> Photodiode type selection inputs
		OE-> Output enable(active in low)
		
		S0 	S1	SCALING
		-------------------
		L 	L 	Power down 
		L 	H 	2% 
		H 	L 	20% 
		H 	H 	100%

		S2 	S3 	PHOTODIODE TYPE
		----------------------
		L 	L 	Red
		L 	H 	Blue
		H 	L 	Clear (no filter)
		H 	H 	Green
	*/

	int R, G, B;
	int maximum;
	firstLine = "Color(RGB):" + String(S0) + "-" + String(out);
	lcd_mesagge(firstLine);
	
	pinMode(S0,OUTPUT); //pin 39 --default
	pinMode(S1,OUTPUT); //pin 41 --default
	pinMode(S2,OUTPUT);//pin 43 --default
	pinMode(S3,OUTPUT); //pin 45 --default
	pinMode(OE,OUTPUT); //pin D4 --default
	pinMode(led,OUTPUT); //pin D5 --default
	digitalWrite(S1,HIGH); 
	digitalWrite(S0,HIGH);//Frequency scaling 100% 
	digitalWrite(OE,LOW); 
	digitalWrite(led,HIGH); 
	
	
	Ardu_mesagge("Color sensor - TCS3200");
	Serial.println("R\tG\tB");
	Serial1.println("R\tG\tB");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
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
		lcd_mesagge(firstLine, secondLine);
		
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
	
	digitalWrite(led,LOW);
	lcd.setRGB(100,100,100);
	clean_buff();
}

void calidad_aire(int SIG, int Timedelay){
	/*
		SIG -> Input voltage
	*/
	analogReference(DEFAULT);
	int sensor_val, i = 0;
	String quality;
	String mesagge = {"El sensor debe llevar conectado al menos 3 min para que pueda funcionar adecuadamente. Ponga el sensor en un lugar con aire de buena calidad para la calibracion inicial. Luego, oprima el boton para continuar. "};
	int V_0 = 0;
 
  	delay(1000);
	while(digitalRead(pinsw) == 1){
		if(continuar = 1) break;
		else if (i + 16 < mesagge.length())
			lcd_mesagge(mesagge.substring(i, i + 16));
		else
			i = 0;
		i++;
		delay(100);
	}
	delay(100);
	while(digitalRead(pinsw) == 0 and serial_readPhrase() != "stop");
  	firstLine = "Cal. Aire: A" + String(SIG);
	lcd_mesagge(firstLine);

	V_0 = analogRead(SIG); // pin A6 -- default
	lcd.clear();
	lcd_mesagge(firstLine,"Sensor calibrado");
	delay(2000);
	lcd_mesagge(firstLine,"Enviando al PC...");
	delay(1000);

	Ardu_mesagge("Air Quality sensor");
    Serial.println("Valor");
    Serial1.println("Valor");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		sensor_val = abs(analogRead(SIG)-V_0);
		
		if(sensor_val <= 200)
			quality = "Cal.buena ->";
		else if(sensor_val > 200 && sensor_val < 400)
		 	quality = "Cal.media-> ";
		else if(sensor_val >= 400 && sensor_val < 700)
		 	quality = "Cal.baja-> ";
		else if(sensor_val >= 700)
		 	quality = "Peligro->";
		//Los intervalos están basados en la librería AirQuality_Sensor.zip
		 
		secondLine = quality + String(sensor_val) +"  ";
		lcd_mesagge(firstLine , secondLine);
		
		Serial.println(sensor_val);
		Serial1.println(sensor_val);
		delay(Timedelay);
	}

	analogReference(INTERNAL1V1);
	clean_buff();
}

void photoResitor(int S, int Timedelay){
	/*
		S -> Input voltage
	 */
	analogReference(DEFAULT);
	int value = 0;
	firstLine = "Fotorres: A" + String(S);

	lcd_mesagge(firstLine, "Enviando al PC...");
	delay(500);

	Ardu_mesagge("Photoresistor module ky-018");
	Serial.println("Valor");
	Serial1.println("Valor");

	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		value = analogRead(S); // pin A2 -- default

		Serial.println(value);
		Serial1.println(value);
		
		secondLine = String(value) + "      dt =" + String(Timedelay); 

		lcd_mesagge(firstLine, secondLine);
		delay(Timedelay);
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void ritmo_cardiaco(int Timedelay){
  	Wire.begin();
	
	unsigned char c;
	firstLine = "Ritmo Cardi: IIC";
  
	lcd_mesagge(firstLine, "Enviando al PC...");
    delay(500);
    lcd.clear();
	Ardu_mesagge("finger clip heart rate sensor");
	Serial.println("frecuencia  (BPM)");
	Serial1.println("frecuencia  (BPM)");
	while(Wire.available() == 0 and digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){ 
	    Wire.requestFrom( 0xA0  >>  1 ,  1 );// solicitar 1 bytes desde el dispositivo esclavo         
      	c = Wire.read();   // receive heart rate value (a byte)
      	Serial.println(c, DEC);         // print heart rate value
      	Serial1.println(c, DEC);
	    secondLine = String(c) + "        BPM";
	    lcd_mesagge(firstLine, secondLine);
	
	  	delay(Timedelay); 
	}
    clean_buff();
}

void conductancia(int PIN){
	/*
		PIN -> Input Voltage
	 */
	analogReference(DEFAULT);
	firstLine = "GSR:   A" + String(PIN);
	int sensorValue = 0;
	int gsr_average = 0;
	long sum = 0;

	lcd_mesagge(firstLine);
	delay(500);
	lcd_mesagge(firstLine,"Enviando al PC...");
	delay(500);
	
	Ardu_mesagge("Galvanic Skin Response (GSR)");
	Serial.println("GSR (Promedio)");
	Serial1.println("GSR (Promedio)");
	while(digitalRead(pinsw)==1 and serial_readPhrase() != "stop"){
		sum = 0;
		for(int i = 0; i < 10; i++){
			sensorValue = analogRead(PIN); // pin A8 -- default
			sum += sensorValue;
			delay(5);
		}
		gsr_average = sum / 10;
		secondLine = " GSR -> " +  String(gsr_average) + "          ";
		
		lcd_mesagge(firstLine, secondLine);
		Serial.println(gsr_average);
		Serial1.println(gsr_average);
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void Termocupla_k( int CS, int SCK, int SO, int Timedelay){
	/*
		CS -> Chip select (active in low)
		SCK -> Serial clock input
		SO -> Serial Data output
	 */
	MAX6675 thermocouple(SCK, CS, SO); //Pin D11, D10, D53 -- default
	double temp = thermocouple.readCelsius();

	firstLine = "Termocupla: D" + String(CS);
	
	delay(500);
	lcd_mesagge(firstLine, "Enviando al PC...");
	delay(500);

	Ardu_mesagge("Termocupla tipo K - MAX6675K");	
	Serial.println("Temp (°C) ");
	Serial1.println("Temp (°C) ");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		temp = thermocouple.readCelsius(); 
		secondLine = " " + String(temp) + "    " + char(223) + "C         ";
	    
	    lcd_mesagge(firstLine,secondLine);
	    Serial.println(temp);
	    Serial1.println(temp);
	    delay(Timedelay);
    }
	clean_buff();  
}

// void electroIman(int PIN){
// 	/*
// 		PIN -> Enable Electromagnet
// 	 */
//     encoder_inicio = myEnc.read();
// 	firstLine = "Electromag: D" + String(PIN); // Pin D2 -- default
// 	int state = 1;

// 	digitalWrite(PIN,LOW);
// 	lcd_mesagge(firstLine," Electroiman Off  ");
// 	delay(100);
 
// 	while(digitalRead(pinsw)== 1){ 
// 		Encoder_menu(0, 1, &state);
// 		if (state == 0){
// 			digitalWrite(PIN,HIGH);
// 			secondLine = " Electroiman on  ";
// 		}
// 		else if(state == 1){
// 			secondLine = " Electroiman off  ";
// 			digitalWrite(PIN,LOW);
// 		}
// 		else{
// 			digitalWrite(PIN,LOW);
// 			secondLine = " Electroiman Off  ";
// 		}

// 		lcd_mesagge(firstLine, secondLine);
// 	}
//     digitalWrite(PIN,LOW);
// 	clean_buff();
// }

void luminosidad(){
	uint16_t lux = 0;
	BH1750 luxometro;
	firstLine = "Intensidad: IIC";
	luxometro.begin(BH1750::CONTINUOUS_HIGH_RES_MODE); //Inicializar BH1750 sensor de Luminosidad
	
	lcd_mesagge(firstLine);
	delay(500);
	lcd_mesagge(firstLine, "Enviando al PC...");
	delay(500);
	Ardu_mesagge("Light Intensity Sensor - GY302");
	Serial.println("Luminosidad (lx)");
	Serial1.println("Luminosidad (lx)");

	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){  
		lux = luxometro.readLightLevel(); // Lectura del BH1750
		Serial.println(lux);
		Serial1.println(lux);

		secondLine = " " + String(lux) + "       lx    ";
		lcd_mesagge(firstLine, secondLine);
		delay(500);
	}
	clean_buff();
}

void Servomotor(int PIN){
	/*
		PIN -> PWM output
	 */
    Servo servoMotor;
	servoMotor.attach(PIN); //Microservo naranja

	encoder_inicio = myEnc.read();
	int ang = 0;
	
	firstLine = "Servo:  D" + String(PIN);
	secondLine = "   Angulo  0";

	servoMotor.write(180);
	lcd_mesagge(firstLine, secondLine);

	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){ 
		Encoder_menu(0, 180, &ang, 3);
		servoMotor.write(180-ang);

		secondLine = "   Angulo  " + String(ang); 
		lcd_mesagge(firstLine, secondLine);
	}
    clean_buff();
}

void atomizador(int PIN){
	firstLine = "Atomizador: D" + String(PIN);
	encoder_inicio = myEnc.read();

	int option = 0;
	digitalWrite(PIN,LOW);
	lcd_mesagge(firstLine, "Atomizacion Off");
	
	while(digitalRead(pinsw)== 1 and serial_readPhrase() != "stop"){ 
		Encoder_menu(0, 1, &option);

		if (option == 1){
			digitalWrite(PIN, HIGH);
			lcd_mesagge(firstLine, "Atomizacion on  ");
		}
		else{
			digitalWrite(PIN, LOW);
			lcd_mesagge(firstLine, "Atomizacion off  ");
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
		lcd_mesagge(firstLine, secondLine);
	}

	suma /= 1100;

	lcd_mesagge(firstLine,"Sistema calibrado");
	delay(2000);
	lcd_mesagge(firstLine,"Enviando al PC...");

	Ardu_mesagge("Grov - EMG detector");
	Serial.println("V_out");
	Serial1.println("V_out");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		valor = getAnalog(VOUT);
		secondLine = "VOUT -> " + String(valor) + "       ";

		lcd_mesagge(firstLine, secondLine);
		Serial.println(valor);
		Serial1.println(valor);
		delay(Timedelay);
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

void Led_RGB(int R, int G, int B){
	/*
		R -> Digital output
		G -> Digital output
		B -> Digital output
	 */

	int encoder_inicio = myEnc.read();
	int opcion = 1;
	int Pin = R;
	int Salir = 0, Salir_color = 0;
	int Valor = 0;
	String opciones[5] = {"Volver al Menu  ", "Intensidad Rojo  ", 
						  "Intensidad Verde  ", "Intensidad Azul  "};
	analogWrite(R,LOW);//Red Pin D2 -- default
	analogWrite(G,LOW);//Green Pin D3 -- default
	analogWrite(B,LOW);//Blue Pin D4 -- default

	firstLine = "RGB: D" + String(R) +"-D"+ String(G) + "-D" + String(B);
	
	lcd_mesagge(firstLine, opciones[opcion]);
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		while(Salir == 0){
			while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
				Encoder_menu(0, 3, &opcion);
				if (opcion == 1){
					Pin = R;
					lcd_mesagge(firstLine,opciones[opcion]);
				}
				else if (opcion == 2){
					Pin = G;
					lcd_mesagge(firstLine,opciones[opcion]);
				}
				else if (opcion == 3){
					Pin = B;
					lcd_mesagge(firstLine,opciones[opcion]);
				}
				else if (opcion == 0)
					lcd_mesagge(firstLine,opciones[opcion]);
			}	

			if (opcion == 0 and digitalRead(pinsw) == 0)
			    Salir = 1;

			else if (digitalRead(pinsw) == 0){
				delay(100);
				while(digitalRead(pinsw) == 0);
				lcd_mesagge(firstLine,"Gire la perilla  ");

				encoder_inicio = myEnc.read();
				while(Salir_color == 0){
					while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
						Encoder_menu(0, 255, &Valor);
						lcd_mesagge(firstLine,"Intensidad: "+String(Valor) + "  ");
					 }

					analogWrite(Pin, Valor);

					if (digitalRead(pinsw) == 0){
					  Salir_color = 1;
					  delay(100);
					  while(digitalRead(pinsw) == 0);
					}
				}

				Salir_color = 0;
				lcd_mesagge(firstLine,opciones[opcion]);
			}
		}
	}
  analogWrite(R,LOW);//Red Pin D2 -- default
  analogWrite(G,LOW);//Green Pin D3 -- default
  analogWrite(B,LOW);//Blue Pin D4 -- default
  
	clean_buff();
}

void Tacometro(int OUT){
	/*
		OUT -> Input voltage
	 */
	analogReference(DEFAULT);
	int valor = 0;
	int t1 = 0, t2= 0, T_Osc = 0;
	int Salir = 0;
	
	firstLine = "Tacometro: A" + String(OUT); // Pin D4 --default

	lcd_mesagge(firstLine,"Envia al PC");
	delay(500);
	Serial.println("Tiemp.Oscuridad (ms)");
	Serial1.println("Tiemp.Oscuridad (ms)");

	while(Salir == 0){
		valor = ( analogRead(OUT) * 5) / 1023;

		while(valor > 2 and Salir == 0){
		  	if (digitalRead(pinsw) == 0)
		  		Salir =1;
		  	valor = (analogRead(OUT) * 5) / 1023;
		}
		lcd_mesagge(firstLine,"Esperando     ");

		while(valor < 3 and Salir == 0){
		  	if (digitalRead(pinsw) == 0)
		    	Salir =1;
		  	valor= (analogRead(OUT) * 5) / 1023;
		}

		t1 = millis();
		lcd_mesagge(firstLine, "Tomando tiempo");
		
		while(valor > 2 and Salir==0){
		  	if (digitalRead(pinsw) == 0 or serial_readPhrase() == "stop")
		    	Salir = 1;
		  	valor= (analogRead(OUT) * 5) / 1023;
		}

		t2 = millis();
		T_Osc = t2 - t1;
		
		if (Salir == 0){
			secondLine = "T.Osc.->"+ String(T_Osc) + "  ms";
		  	lcd_mesagge(firstLine, secondLine);
		  	Serial.println(T_Osc);
		  	Serial1.println(T_Osc);	
		}
	}
	analogReference(INTERNAL1V1);
	clean_buff();
}

int getAnalog(int pin){
    int maximo_dato_analogo = 300;
    int minimo_dato_analogo = 100;
    long suma = 0;
    
    for(int i = 0; i < 32; i++)
        suma += analogRead(pin);

    int dato = suma / 32;
    
    maximo_dato_analogo = dato > maximo_dato_analogo ? dato : maximo_dato_analogo;         // if max data
    minimo_dato_analogo = minimo_dato_analogo > dato ? dato : minimo_dato_analogo;         // if min data
    
    return dato;
}
