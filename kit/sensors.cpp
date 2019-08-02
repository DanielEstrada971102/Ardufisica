#include "ardufisica.h"

//=========================Sensores disponibles=================================
void microfono(int SIG, int Timedelay){
	/*
		SIG -> Voltage input
	*/
	analogReference(DEFAULT);
	firstLine = "Sound: A" + String(SIG); 
  	long Intensity = 0;
  
	lcd_mesagge(firstLine);
  	Serial.println("Sensor : Grove - Sound Sensor");
  	Serial1.println("Sensor : Grove - Sound Sensor");
	Serial.println("Intensidad");
	Serial1.println("Intensidad");
	
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		for(int i = 0; i < 1000; i++)
		  Intensity += analogRead(SIG);

    Intensity >>=5;
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
	float factor = 5000.0/(1023.0 * 1.3); //1.3mV/Gauss , 5V->1023--> no se implementa pues no hay certeza.
  	firstLine = "Hall: A" + String(AO);
  	secondLine = "Calib.campo ...";
  
	lcd_mesagge(firstLine, secondLine);
	delay(1000);
	lcd.clear();
	lcd_mesagge(firstLine);
 
	int V_0 = analogRead(AO), campo;
	unsigned long t_inicial = millis(), t; 
	Serial.println("Sensor : Hall Sensor Magnetic M44");
	Serial1.println("Sensor : Hall Sensor Magnetic M44");
  	Serial.println("tiempo(s)  Valor");
  	Serial1.println("tiempo(s)  Valor");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		t = (millis() - t_inicial);
		campo = (analogRead(AO) - V_0); // pin A14 --default
		Serial.print(t/1000.0); 
		Serial.print('\t');
		Serial.println(campo);
		Serial1.print(t/1000.0); 
		Serial1.print("    ");
		Serial1.println(campo);

    	secondLine = "     "+ String(campo) + "           ";
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
	int tono_temp = tono;
	String freq = "";
	bool continuar = false;
  	String Lista_freq [12] ={"261", "277", "293", "311", "329", "349", "369", 
  							 "391", "415", "440", "466", "493"};
	//time in microsecond to generate frequency tone (261 Hz, 277Hz, 293 Hz,...)
	int Tiempo_Tono[12] = {1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275,
	                     1203, 1136, 1072 ,1012}; 

	firstLine = "Speaker : D" + String(SIG) + "    ";
	secondLine = String(tono ) + "         "+ Lista_freq[tono] +
				 " Hz "; 

	lcd_mesagge(firstLine, secondLine);
  	Serial.println("Sensor : Grove - Speaker ");
  	Serial1.println("Sensor : Grove - Speaker ");
	int encoder_inicio = myEnc.read();

	Serial.println("Introduzca una de las siguientes frecuencia o escojala manualmente con el encoder");
	Serial1.println("Introduzca una de las siguientes frecuencia o escojala manualmente con el encoder");

	for(int i = 0; i < 12; i++){
	  	Serial.println(Lista_freq[i]);
	  	Serial1.println(Lista_freq[i]);
	}
  
  	delay(100);
    secondLine = " "+String(tono+1)+"   "+String(500000/Tiempo_Tono[tono])+" Hz  ";
	lcd_mesagge(firstLine, secondLine);
	
	while(digitalRead(pinsw) == 1 and freq != "stop"){
       	if(Serial.available() or Serial1.available()){
	       	freq = serial_readPhrase();
		 	for (int i= 0; i < 12; i++){
	             if(freq == Lista_freq[i]){
	                 tono = i;
	                 continuar = true;
	                 Serial.println("Usted escogio " + Lista_freq[i] + " Hz");
	                 Serial1.println("Usted escogio " + Lista_freq[i] + " Hz");
	                 break;
	             }
		     }
		 }
      	Encoder_menu(0, 11, &tono);
      
      	if(tono_temp != tono){
          	tono_temp = tono;
          	secondLine = " "+String(tono+1)+"   "+String(500000/Tiempo_Tono[tono])+" Hz  ";
    		lcd_mesagge(firstLine, secondLine);
     	}
      
      	digitalWrite(SIG,HIGH);
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

	Ultrasonic ultrasonido(TRIG,ECHO,30000); // pin D6-D7 -- default 
	firstLine = "Ultrasonic:D" + String(TRIG) + "-D" + String(ECHO);
	
	lcd_mesagge(firstLine);
	
	unsigned long t_inicial = millis(), t;
	int distancia;

	Serial.println("Sensor : Ultrasonic Sensor HC-SR04");
	Serial1.println("Sensor : Ultrasonic Sensor HC-SR04");
  	Serial.println("t (s) \t ditancia (cm)");
  	Serial1.println("t (s)      ditancia (cm)");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		t = millis() - t_inicial;
		distancia = ultrasonido.Ranging(CM);
		Serial.print(t / 1000.0);
		Serial.print('\t');
		Serial.println(distancia);
		Serial1.print(t / 1000.0);
		Serial1.print("    ");
		Serial1.println(distancia);

    	secondLine = String(distancia) + "          cm   ";
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

	Serial.println("Sensor : Grove - Infrared Temperature Sensor");
	Serial1.println("Sensor : Grove - Infrared Temperature Sensor");
	Serial.println("Sensor voltaje tiempo(s) TempSUR(°C)  TempOBJ(°C)");
	Serial1.println("Sensor voltaje tiempo(s) TempSUR(°C)  TempOBJ(°C)");

	unsigned long t_inicial = millis(), t;
	
	while(digitalRead(pinsw) == 1 and serial_readPhrase()!= "stop"){
		t = millis() - t_inicial;  
		measureSurTemp(&temperaturaSUR, SUR);//measure the Surrounding temperature around the sensor
		measureObjectTemp(&temperaturaOBJ ,OBJ);

		Serial.print(t / 1000.0);
		Serial.print('\t');
		Serial.print(temperaturaSUR);
		Serial.print('\t');
		Serial.println(temperaturaOBJ);
		Serial1.print(t / 1000.0);
		Serial1.print("    ");
		Serial1.print(temperaturaSUR);
		Serial1.print("    ");
		Serial1.println(temperaturaOBJ);

		secondLine = String(temperaturaSUR) + " " + String(temperaturaOBJ) + 
		       "  " + char(223) + "C    ";
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
	
	
	Serial.println("Sensor : Color sensor - TCS3200");
	Serial1.println("Sensor : Color sensor - TCS3200");
	Serial.println("R\tG\tB");
	Serial1.println("R    G    B");
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
		Serial1.print("    ");
		Serial1.print(G);
		Serial1.print("    ");
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
  	bool continuar = false;
	String quality;
	String mesagge = {"El sensor debe llevar conectado al menos 3 min para que pueda funcionar adecuadamente. Ponga el sensor en un lugar con aire de buena calidad para la calibracion inicial. Luego, oprima el encoder o escriba 'continue' para continuar. "};
	int V_0 = 0;
 
  	delay(1000);
  	Serial.println("Sensor : Air Quality sensor");
  	Serial1.println("Sensor : Air Quality sensor");
  	Serial.println(mesagge);
  	Serial1.println(mesagge);
	while(digitalRead(pinsw) == 1 and not(continuar)){
		if(serial_readPhrase() == "continue")
			continuar = true;

		else if (i + 16 < mesagge.length())
			lcd_mesagge(mesagge.substring(i, i + 16));
		
		else
			i = 0;
		i++;
		
		delay(100);
	}

	delay(100);
	
	while(digitalRead(pinsw) == 0);
  	firstLine = "Cal. Aire: A" + String(SIG);
	lcd_mesagge(firstLine);

	V_0 = analogRead(SIG); // pin A6 -- default
	lcd.clear();
	lcd_mesagge(firstLine,"Sensor calibrado");
	delay(2000);
	lcd_mesagge(firstLine,"Enviando al PC...");
	delay(1000);

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

	Serial.println("Sensor : Photoresistor module ky-018");
	Serial1.println("Sensor : Photoresistor module ky-018");
	Serial.println("Valor");
	Serial1.println("Valor");

	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		value = analogRead(S); // pin A2 -- default

		Serial.println(value);
		Serial1.println(value);
		
		secondLine = String(value) + "      dt =" + String(Timedelay)+ "   "; 

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
	Serial.println("Sensor : finger clip heart rate sensor");
	Serial1.println("Sensor : finger clip heart rate sensor");
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
	
	Serial.println("Sensor : Galvanic Skin Response (GSR)");
	Serial1.println("Sensor : Galvanic Skin Response (GSR)");
	Serial.println("GSR Valor");
	Serial1.println("GSR Valor");
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

	Serial.println("Sensor : Termocupla tipo K - MAX6675K");
	Serial1.println("Sensor : Termocupla tipo K - MAX6675K");	
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
// Este sensor se deshabilito por que requiere de mucha corriente para funcionar

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
	Serial.println("Sensor : Light Intensity Sensor - GY302");
	Serial1.println("Sensor : Light Intensity Sensor - GY302");
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
  	int ang_temp = ang;
  	String angulo= "";
	
	firstLine = "Servo:  D" + String(PIN);
	secondLine = "   Angulo  0  ";

	servoMotor.write(180);
	lcd_mesagge(firstLine, secondLine);
  	Serial.println("Sensor : Mico Servo SG90");
  	Serial1.println("Sensor : Mico Servo SG90");
    Serial.println("Ingrese un angulo entre 0 y 180 o escojalo manualmente con el encoder. ");
    Serial1.println("Ingrese un angulo entre 0 y 180 o escojalo manualmente con el encoder. ");
  	Serial.println("Angulo");
  	Serial1.println("Angulo");
  
	while(digitalRead(pinsw) == 1 and angulo != "stop"){ 
		if (Serial.available() or Serial1.available()){
			angulo = serial_readPhrase();
			if(angulo.toInt() < 180 and angulo.toInt() > 0)
				ang = angulo.toInt(); 
		}
		
		Encoder_menu(0, 180, &ang,2);
		if( ang != ang_temp){
		    servoMotor.write(180-ang);
		    secondLine = "   Angulo  " + String(ang) + "     "; 
        lcd_mesagge(firstLine, secondLine);
		    Serial.println(ang);
        Serial1.println(ang);
		    ang_temp = ang;
		}
    	delay(10);
	}
    servoMotor.write(180);
    clean_buff();
}

void atomizador(int PIN){
	firstLine = "Atomizador: D" + String(PIN);
	encoder_inicio = myEnc.read();

	int option = 0;
	int option_temp = option;
	String state = "";
	digitalWrite(PIN,LOW);
	lcd_mesagge(firstLine, "Atomizacion Off");
	Serial.println("Sensor : Grove - Water Atomization");
	Serial1.println("Sensor : Grove - Water Atomization");
  Serial.println("Puede establecer el estado del sensor como 'on' o 'off'. ");
  Serial1.println("Puede establecer el estado del sensor como 'on' o 'off'. ");

	while(digitalRead(pinsw)== 1 and state  != "stop"){
		if (Serial.available() or Serial1.available()){
			state = serial_readPhrase();
			if (state == "on"){
				option = 1;
			}
			else if (state == "off"){
				option = 0;
			}
		}
   
		Encoder_menu(0, 1, &option);
		
		if (option_temp != option){
			if (option == 1){
				digitalWrite(PIN, HIGH);
				lcd_mesagge(firstLine, "Atomizacion on  ");
				Serial.println("Atomizacion on  ");
				Serial1.println("Atomizacion on  ");
			}
			else{
				digitalWrite(PIN, LOW);
				lcd_mesagge(firstLine, "Atomizacion off  ");
				Serial.println("Atomizacion off  ");
				Serial1.println("Atomizacion off  ");
			}
      		option_temp = option;
		}
		delay(10);
	}
 	clean_buff();
}

void EMG_detector(int VOUT, int Timedelay){
	analogReference(DEFAULT);
	int valor =0, valor_final=0;
	long suma = 0;

	firstLine = "EMG: A" + String(VOUT);
  	Serial.println("Sensor : Grov - EMG detector");
  	Serial1.println("Sensor : Grov - EMG detector");
  	Serial.println("Calibrando ...");
  	Serial1.println("Calibrando ...");
  
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

	Serial.println("V_out");
	Serial1.println("V_out");
	while(digitalRead(pinsw) == 1 and serial_readPhrase() != "stop"){
		valor = getAnalog(VOUT);
		secondLine = "     " + String(valor) + "         ";

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
	int color = 1, Pin = R, Valor = 0;
  	String RGB[3] = {"0", "0", "0"};
	String palabra = "", intensity = "";
	bool Salir =false, Salir_color =false;
	String opciones[5] = {"Volver al Menu   ", "Intensidad Rojo   ", 
						  "Intensidad Verde   ", "Intensidad Azul   "};

	analogWrite(R,LOW);//Red Pin D2 -- default
	analogWrite(G,LOW);//Green Pin D3 -- default
	analogWrite(B,LOW);//Blue Pin D4 -- default

	firstLine = "RGB: D" + String(R) +"-D"+ String(G) + "-D" + String(B);
	
	lcd_mesagge(firstLine, opciones[color]);
	Serial.println("Sensor : Led RGB");
	Serial1.println("Sensor : Led RGB");
	Serial.println("Introduzca la intensidad de cada color seguido de la palabra 'apply'. ");
	Serial.println("Ejem:  enviar primero 'R53', Luego 'apply'. Esto establecera la intensidad del rojo en 53. ");
	Serial1.println("Introduzca la intensidad de cada color seguido de la palabra 'apply'. ");
	Serial1.println("Ejem:  enviar primero 'R53', Luego 'apply'. Esto establecera la intensidad del rojo en 53. ");
	while(not(Salir) and serial_readPhrase() != "stop"){

	 	while(digitalRead(pinsw) == 1 and not(Salir_color)){

    		if(Serial.available() or Serial1.available()){

      			palabra = serial_readPhrase();
      			intensity = palabra.substring(1); 
      			if(palabra == "stop"){
          			color = 0;
          			Salir_color = true;
      			}
      			else if(intensity.toInt() < 255 and intensity.toInt() >0){
          			
          			if( palabra[0] == 'R'){
                    color = 1;
                    RGB[0] = intensity; 
          			}
          			else if( palabra[0] == 'G'){
          			    color = 2;
          			    RGB[1] = intensity;
          			}
          			
          			else if( palabra[0] == 'B'){
          			    color = 3;
          			    RGB[2] = intensity;
          			}
					
					Valor = intensity.toInt();
					Salir_color = true;   
     			}
   			}

			Encoder_menu(0, 3, &color);
			
			if (color == 1){
				Pin = R;
				lcd_mesagge(firstLine,opciones[color]);
			}
			else if (color == 2){
				Pin = G;
				lcd_mesagge(firstLine,opciones[color]);
			}
			else if (color == 3){
				Pin = B;
				lcd_mesagge(firstLine,opciones[color]);
			}
			else if (color == 0)
				lcd_mesagge(firstLine,opciones[color]);
		}

		delay(100);
		
		if (color == 0) Salir = true;
		
		else{
			while(digitalRead(pinsw) == 0);
			encoder_inicio = myEnc.read();
			
			while(digitalRead(pinsw) == 1 and palabra != "apply" and palabra != "stop"){
        		if (Serial.available() or Serial1.available())
        			palabra = serial_readPhrase();
        		
				Encoder_menu(0, 255, &Valor);
				lcd_mesagge(firstLine,"Intensidad: "+String(Valor) + "      ");
			}

			analogWrite(Pin, Valor);

			Serial.println('(' + RGB[0] + ',' + RGB[1] + ',' + RGB[2] + ')');
			Serial1.println('(' + RGB[0] + ',' + RGB[1] + ',' + RGB[2] + ')');

			lcd_mesagge(firstLine,opciones[color]);
    		delay(500);
		}

		Salir_color = false;
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
	int tiempo1 = 0, tiempo2= 0, T_Osc = 0;
	int Salir = 0;
	
	firstLine = "Tacometro: A" + String(OUT); // Pin D4 --default

	Serial.println("Sensor : Tachometer");
	Serial1.println("Sensor : Tachometer");
	lcd_mesagge(firstLine,"Envia al PC");
	delay(500);
	Serial.println("Tiemp.Oscuridad (ms)");
	Serial1.println("Tiemp.Oscuridad (ms)");

	while(Salir == 0 and (Serial1.available() == 0 and Serial.available() == 0)){
	    valor= ((analogRead(4))*5)/1023;
	    while(valor > 2 and Salir == 0){
		    if (digitalRead(pinsw) == 0){
		        Salir =1;
		    }
		    valor= ((analogRead(4))*5)/1023;
	    }
	    lcd_mesagge("T.Oscuri.: A4-A5","Esperando");
	    while(valor < 3 and Salir==0 and (Serial1.available() == 0 and Serial.available() == 0)){
			if (digitalRead(pinsw) == 0){
				Salir =1;
			}
			valor= ((analogRead(4))*5)/1023;
		}
	    tiempo1=millis();
	    lcd_mesagge("T.Oscuri.: A4-A5","Tomando tiempo");
	    while(valor > 2 and Salir==0 and (Serial1.available() == 0 and Serial.available() == 0)){
			if (digitalRead(pinsw) == 0){
				Salir =1;
			}
			valor= ((analogRead(4))*5)/1023;
		}
	    
	    tiempo2=millis();
	    T_Osc =tiempo2-tiempo1;
	    if (Salir == 0 and (Serial1.available() == 0 and Serial.available() == 0)){
			lcd_mesagge("T.Oscuri.: A4-A5","T.Osc.->"+String(T_Osc)+" ms");
			Serial.println(T_Osc);
			Serial1.println(T_Osc);
		}
    }
    posicion_menu = 17;
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
