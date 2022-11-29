//=======================================
// PASTILLERO ELECTRÓNICO
// Asignatura: IoT
// Titulo: Funcionamiento del reloj
//=====================================

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h> // Inclusión de Libreria Grafica
#include <Adafruit_SSD1306.h> // Inclusión de Libreria de la pantalla
#include "Ubidots.h"
#include <WiFi.h> 
#include <WiFiClientSecure.h>
#include <PubSubClient.h> 
#include <HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include "logo.h"
//SDA 21 ; SCL 22 ; VCC 3.3

// Definiciones para la Pantalla
#define ANCHOPANTALLA 128 //  Ancho de la Pantalla en Pixeles
#define ALTOPANTALLA 64 // Alto de la Pantalla en Pixeles
#define OLED_RESET     4 // ASignación del Pin de Reset para la pantalla
#define SCREEN_ADDRESS 0x3C //Segun el datasheet de la pantalla la direccion es
                            //0x3D para 128x64, 0x3C para 128x32
Adafruit_SSD1306 display(ANCHOPANTALLA, ALTOPANTALLA, &Wire, OLED_RESET);
//***********************************************************************

const char* UBIDOTS_TOKEN = "BBFF-Q6ZM02UgdW2ia2Wn8LUUBDXAorwd1e";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "INFINITUME09C_2.4"; // Put here your Wi-Fi SSID
const char* WIFI_PASS = "3pmWh68V63"; // Put here your Wi-Fi password
#define PIN_LED 33
#define PIN_TTP223B 14   //SIG de sensor en pin digital 4
#define PIN_BUZZER 32
Ubidots ubidots(UBIDOTS_TOKEN, UBI_TCP);

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

int ahorahora;           // Guarda la hora del reloj RTC
int ahoraminuto;         // Guarda los minutos del reloj RTC
int ahorasegundo;
int ahorames;
int ahorayear;
char dia_semana;
int dia;                 // Guarda el dia del reloj RTC
int setfecha = 2;        // para cargar la hora en el reloj
int bandera = 0;
//12
int LED1 = 25;
int LED2 = 15;
int LED3 = 27; //martes1
int LED4 = 34;
int LED5 = 26;  //jueves1
int LED6 = 23;
//int LED7 = 13;  //faltan por definir los pines de los sig leds
int LED8 = 19;  
int LED9 = 15;
int LED10 = 5;  //martes2
int LED11 = 35; 
int LED12 = 25; //jueves2
int LED13 = 18;
int LED14 = 17;

void setup() {

  pinMode(setfecha, INPUT);                  // Declaracion de pin de salida
	Serial.begin(115200);
	Serial.println("Serial OK");
 
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  Serial.println("Serial OK");

if (! rtc.begin()) {   //En caso de que no se encuentre el RTC
Serial.println("Couldn't find RTC");
while (1);
}

// Inicio de Pantalla
  Wire.begin();               // Inicialización del Protocolo I2C para el manejo de pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 Fallo la Localización de Pantalla"));
    for(;;);                  // Si no la encuentra se bloquea en un ciclo infinito 
  }
  delay(500);
  display.clearDisplay();     // Función de borrado de Pantalla
  display.display();

// Inicio del reloj
// valores a introducir para configurar el RTC
// ===========================
// int ano = 2022; // 0-2099
// int mes = 11;   // 0-12
// int dia = 16;   // 0-31
// int hora= 20;   // 0-23
// int minu = 20;   // 0-59
// int seg = 00;   // 0-59
// // ===========================

display.display();
delay(2);
display.clearDisplay();
 
display.clearDisplay();
display.setTextColor(WHITE);
//display.startscrollright(0x00, 0x0F);
display.setTextSize(2);
display.setCursor(0,5);
display.print("  Reloj");
display.display();
delay(3000);
pinMode (LED1, OUTPUT); //domingo 1
pinMode (LED2, OUTPUT); //lunes 1
pinMode (LED3, OUTPUT); //martes 1
pinMode (LED4, OUTPUT); //miercoles 1
pinMode (LED5, OUTPUT); //jueves 1
pinMode (LED6, OUTPUT); //viernes 1
//pinMode (LED7, OUTPUT); //sabado1
pinMode (LED8, OUTPUT); //domingo 2
pinMode (LED9, OUTPUT); //lunes 2
pinMode (LED10, OUTPUT);  //martes 2
pinMode (LED11, OUTPUT);  //miercoles 2
pinMode (LED12, OUTPUT);  //  jueves 2
pinMode (LED13, OUTPUT);  //viernes 2
pinMode (LED14, OUTPUT);  //sabado 2
pinMode(PIN_LED, OUTPUT);     // pin 33 como salida
//Sensor y Buzzer
pinMode(PIN_TTP223B, INPUT);  // pin 4 como entrada
pinMode(PIN_BUZZER, OUTPUT);  // pin 32 como salida
}
char* getDay1(int dia){
  switch(dia){
    case 0:
     return "domingo1";
    case 1:
     return "lunes1";
    case 2:
     return "martes1";
    case 3:
     return "miercoles1";
    case 4:
     return "jueves1";
    case 5:
     return "viernes1";
    case 6:
     return "sabado1";
  }
}
char* getDay2(int dia){
    switch(dia){
    case 0:
      return "domingo2";
    case 1:
      return "lunes2";
    case 2:
      return "martes2";
    case 3:
      return "miercoles2";
    case 4:
      return "jueves2";
    case 5:
      return "viernes2";
    case 6:
      return "sabado2";
  }
}
//FUNCIONES DE LA ALARMA
void sonarAlarma(){
    //digitalWrite(PIN_TTP223B, HIGH);  // si se presiono el sensor
    Serial.println("entra sonar alarma");
    digitalWrite(PIN_LED, HIGH);        // enciende LED
    digitalWrite(PIN_BUZZER, HIGH);
}
void apagarAlarma(){
    Serial.println("apaga alarma");
    digitalWrite(PIN_TTP223B,LOW);
    digitalWrite(PIN_LED, LOW);   // sino apaga LED
    digitalWrite(PIN_BUZZER, LOW); // sino apaga LED
    delay(60000);
}

void loop() {
  
  display.drawBitmap(0, 0,  epd_bitmap_relojLogo, 128, 64, WHITE);
  display.display();
  delay(3000);

if (digitalRead(setfecha)==HIGH){               // Si presiona el pulsador se establece la fecha que se introduzca por variables
     //rtc.adjust(DateTime(2022,11,16,19,00,00)); //Año,mes,dia,hora,minutos,segundos
     rtc.adjust(DateTime(__DATE__, __TIME__));
     Serial.println("Entra al setfecha");
    }  
while(1)
{
while (bandera == 0){
    DateTime now = rtc.now(); // Lee el valor actual de la hora y fecha
                              // lo coloca en una estructura now que es del tipo rtc.
     display.clearDisplay();
    // display.setTextSize(2);
    // display.setCursor(75,0);
    // display.println(now.second(), DEC); // Lee el tiempo en segundos en formato decimal
     
    display.setTextSize(2);
    display.setCursor(25,0);
    display.println(":");
     
    // display.setTextSize(2);
    // display.setCursor(65,0);
    // display.println(":");
     
    display.setTextSize(2);
    display.setCursor(40,0);
    display.println(now.minute()+3, DEC);// Lee el tiempo en Minutos en formato decimal
     
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println(now.hour(), DEC);// Lee el tiempo en Horas en formato decimal
     
    display.setTextSize(2);
    display.setCursor(0,20);
    display.println(now.day(), DEC); // Lee el dia de la fecha
     
    display.setTextSize(2);
    display.setCursor(25,20);
    display.println("-");
     
    display.setTextSize(2);
    display.setCursor(40,20);
    display.println(now.month(), DEC);// Lee el mes de la fecha
     
    display.setTextSize(2);
    display.setCursor(65,20);
    display.println("-");
     
    display.setTextSize(2);
    display.setCursor(78,20);
    display.println(now.year(), DEC);// Lee el Año de la fecha
     
    display.setTextSize(2);
    display.setCursor(0,40);
    display.print(daysOfTheWeek[now.dayOfTheWeek()]); //Busca en la matriz creada
                                                      // donde está la cadena que representa el dia de la semana/de lunes a domingo
    display.display(); 
    ahorahora = now.hour();
    ahoraminuto = now.minute()+3;
    ahorasegundo = now.second();
    dia = now.dayOfTheWeek();

    Serial.println("Fecha RTC: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.println(now.day(), DEC);
    
    Serial.print("Hora: ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute()+3, DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);
    
    Serial.print("Dia: ");
    Serial.println(daysOfTheWeek[now.dayOfTheWeek()]);
    delay(1000);
      int valorManana = ubidots.get("pastillero1", getDay1(dia));
      int valorTarde = ubidots.get("pastillero1", getDay2(dia));

    if ((int(valorManana/100)  == ahorahora) and ((valorManana%100) == ahoraminuto )){    // PRIMER HORARIO
      Serial.println("Es hora de tomar el medicamento 1");
      delay(1000);
      sonarAlarma();
      switch (dia){
        case 0:{ // domingo
          digitalWrite(LED1, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Domingo 1"); 
          // delay(1000); 
          bandera = 1; 
          break;          
        }
        case 1:{ // lunes
          digitalWrite(LED2, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Lunes 1"); 
          // delay(1000); 
          bandera = 1;  
          break; 
          }
        case 2: {// martes
          digitalWrite(LED3, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Martes 1"); 
          // delay(1000); 
          bandera = 1;
          break;
          }
        case 3: {// miercoles
          digitalWrite(LED4, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Miercoles 1"); 
          // delay(1000); 
          bandera = 1;
          break;
          }
        case 4:{// jueves
          digitalWrite(LED5, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Jueves 1"); 
          // delay(1000); 
          bandera = 1;  
          break;
          }
        case 5:{ // viernes
          digitalWrite(LED6, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Viernes 1"); 
          // delay(1000); 
          bandera = 1;
          break;
          }    
        case 6:{// sabado
          // digitalWrite(LED7, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Sabado 1"); 
          // delay(1000); 
          // bandera = 1; 
          break;
          }
         }// cierra switch dia
      }// cierra primer horario1
    else if ((int(valorTarde/100) == ahorahora) and ((valorTarde%100)== ahoraminuto)){        
      Serial.println("Es hora de tomar el medicamento 2");
      sonarAlarma();
      switch (dia){
          case 0:{ // domingo
          digitalWrite(LED8, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Domingo 2"); 
          // delay(1000); 
          bandera = 1;  
          break;         
        }
        case 1:{ // lunes
           digitalWrite(LED9, HIGH);     
          //  delay(1000);
           Serial.println("Abra el compartimiento");
           Serial.println("Lunes 2"); 
          //  delay(1000); 
          // bandera = 1; 
          break;  
          }
        case 2: {// martes
          digitalWrite(LED10, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Martes 2"); 
          // delay(1000); 
          bandera = 1;
          break;
          }
        case 3: {// miercoles
          digitalWrite(LED11, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Miercoles 2"); 
          // delay(1000); 
          bandera = 1;
          break;
          }
        case 4:{// jueves
          digitalWrite(LED12, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Jueves 2"); 
          // delay(1000); 
          bandera = 1; 
          break; 
          }
        case 5:{ // viernes
          digitalWrite(LED13, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Viernes 2"); 
          // delay(1000); 
          bandera = 1;
          break;
          }    
        case 6:{// sabado
          digitalWrite(LED14, HIGH);     
          // delay(1000);
          Serial.println("Abra el compartimiento");
          Serial.println("Sabado 2"); 
        // delay(1000); 
          bandera = 1; 
          break;
          }
      }// cierra switch
    }// cierra hora 2
        
    bandera = 0;
    
    if(digitalRead(PIN_TTP223B)==HIGH){
      //ubidots.add("alarma",1);
      //ubidots.send();    
      //apagarTodosLosLeds;
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, LOW);
      digitalWrite(LED5, LOW);
      digitalWrite(LED6, LOW);
      //digitalWrite(LED7, LOW);
      digitalWrite(LED8, LOW);
      digitalWrite(LED9, LOW);
      digitalWrite(LED10, LOW);
      digitalWrite(LED11, LOW);
      digitalWrite(LED12, LOW);
      digitalWrite(LED13, LOW);
      digitalWrite(LED14, LOW);
      apagarAlarma();
   }   
}// cierra bandera == 0    
}// cierra while 1
}// cierra el void loop
