#include<EasyDDNS.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#pragma region Globals
 #include <ESP8266WiFi.h>
 #include "Gsender.h"
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>



//Declaracion de las variables
#define DHTPIN D6//Sensor DHT22
#define DHTTYPE DHT22 
#define DHTPIN2 D7 //Sensor DHT11
#define DHTTYPE2 DHT11

//variables sensor distancia
#define TRIGGER 16  //D0
#define ECHO    5   //D1

long duracion=0;
int16_t distancia= 0; 
int16_t ultimoValor = 0; 
uint8_t contador = 0;  

//variable sensor luz
int ledCocina = 14; //D5
int ledDespacho = 4; //D2
int ledDormitorio = 15; //D8

//variable sensorGas
int sensorGas=A0; //D0


//Conexión WiFi
const char* ssid = "MOVISTAR_48D5";
const char* password = "SolyVida1@";

// Create an instance of the server
// specify the port to listen on as an argument

DHT dht2(DHTPIN2, DHTTYPE2);
DHT dht(DHTPIN, DHTTYPE);

IPAddress ip(192,168,10,180); //ip del nodemcu
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);
ESP8266WebServer server(90);


uint8_t connection_state = 0;                    // Connected to WIFI or not
uint16_t reconnect_interval = 10000;             // If not connected wait time to try again
#pragma endregion Globals

uint8_t WiFiConnect(const char* nSSID = ssid, const char* nPassword = password)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(200);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}


//función para leer la temperatura
void getTemperature()
{
  
  float t = dht.readTemperature();
  WiFiClient client;
  

  if (isnan(t)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }

  String json = "{\"temperature\":"+String(t)+"}";
  
  //Envia un json para el cliente com el código 200, que es el código cuando una petición fue realizada correctamente
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send (200, "application/json", json);

thinkSpeak();
  
}

//función para leer la temperatura Interior
void getTemperatureInt()
{
  //Fazemos a leitura da temperatura através do módulo dht
  float t2 = dht2.readTemperature();
  WiFiClient client;
  

  if (isnan(t2)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  String json = "{\"temperatureInt\":"+String(t2)+"}";
  //Envia un json para el cliente com el código 200, que es el código cuando una petición fue realizada correctamente
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send (200, "application/json", json);
}


//Funcón para medir la Humedad Exterior
void getHumidity()
{
  WiFiClient client;
  //Fazemos a leitura da temperatura através do módulo dht
  float h = dht.readHumidity();
  
  if (isnan(h)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  
  String json = "{\"humidity\":"+String(h)+"}";
  
  //Envia un json para el cliente com el código 200, que es el código cuando una petición fue realizada correctamente
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send (200, "application/json", json);
}


//Funcón para medir la Humedad Interior
void getHumidityInt()
{
  WiFiClient client;
  //Fazemos a leitura da temperatura através do módulo dht
  float h2 = dht2.readHumidity();
  //Cria um json com os dados da temperatura
  // Check if any reads failed and exit early (to try again).
  if (isnan(h2)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }
  String json = "{\"humidityInt\":"+String(h2)+"}";
  //Envia un json para el cliente com el código 200, que es el código cuando una petición fue realizada correctamente
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send (200, "application/json", json);
}


//Funcón para medir la Calidad del aire
void getGas()
{
  WiFiClient client;
  int inputGas = analogRead(sensorGas);
 
//Si hay gas en la sala se envia un correo
  if(inputGas>150){
     EnviarMensajeGMAIL();
  }
 
  String json = "{\"gas\":"+String(inputGas)+"}";
  
  //Envia un json para el cliente com el código 200, que es el código cuando una petición fue realizada correctamente
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send (200, "application/json", json);
}



//Funcion para enviar datos a thinkSpeak
void thinkSpeak(){
  WiFiClient client;
  String apiKey = "OFFXHXVURXQN0F1F";
  const char* serverThingSpeak = "api.thingspeak.com";
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  int inputGas = analogRead(sensorGas);

  digitalWrite(TRIGGER, LOW);  
    delayMicroseconds(2);     
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);    
    digitalWrite(TRIGGER, LOW);
    duracion = pulseIn(ECHO, HIGH);
    distancia = (duracion/2) / 29.1;    
    //if (distancia < 0) distancia = 0;
    /*if (distancia != ultimoValor) {
      String msj = String(distancia);
      webSocket.broadcastTXT(msj);
    }*/
    ultimoValor = distancia;

  if (isnan(t)||isnan(h)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

     //Conexion thinkSpeak
    if (client.connect(serverThingSpeak,80)) {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String((int)t);
    postStr +="&field2=";
    postStr += String((int)h);
    postStr +="&field3=";
    postStr += String((int)h2);
    postStr +="&field4=";
    postStr += String((int)t2);
    postStr +="&field5=";
    postStr += String((int)inputGas);
    postStr +="&field6=";
    postStr += String((int)distancia);
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("% send to Thingspeak");
  }
  client.stop();
  Serial.println("Waiting…");
  //delay(20000);



}


//Funciones para encender 3 LEDS
void ledCocinaOn(){
   digitalWrite(ledCocina, HIGH); 
}
void ledCocinaOff(){
   digitalWrite(ledCocina, LOW);
}

void ledDormitorioOn(){
   digitalWrite(ledDormitorio, HIGH); 
}
void ledDormitorioOff(){
   digitalWrite(ledDormitorio, LOW);
}

void ledDespachoOn(){
   digitalWrite(ledDespacho, HIGH); 
}
void ledDespachoOff(){
   digitalWrite(ledDespacho, LOW);
}


//funcion sensor Distancia
void sensorDistancia()
{
  
  
  WiFiClient client;
   digitalWrite(TRIGGER, LOW);  
    delayMicroseconds(2);     
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);    
    digitalWrite(TRIGGER, LOW);
    duracion = pulseIn(ECHO, HIGH);
    distancia = (duracion/2) / 29.1;    
    //if (distancia < 0) distancia = 0;
    /*if (distancia != ultimoValor) {
      String msj = String(distancia);
      webSocket.broadcastTXT(msj);
    }*/
    ultimoValor = distancia;
    delay(2000);
    Serial.println(distancia);

  

  String json = "{\"sensorDistancia\":"+String(distancia)+"}";
  
  //Envia un json para el cliente com el código 200, que es el código cuando una petición fue realizada correctamente
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send (200, "application/json", json);

}


void setup() {
 Serial.begin(115200);
 delay(10);


EasyDDNS.service("noip");
EasyDDNS.client("conforhouse.ddns.net","mroura10","Mariachi10");
//email
connection_state = WiFiConnect();
    if(!connection_state)  
        Awaits();          

    Gsender *gsender = Gsender::Instance();    
    String subject = "Subject is optional!";
    if(gsender->Subject(subject)->Send("marc_roura10@hotmail.com", "hi here is testing email")) {
        Serial.println("Message send.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender->getError());
    }

  //Estado inicial de los leds al encender el 
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  
  pinMode(ledCocina, OUTPUT);
  digitalWrite(ledCocina, LOW);

  pinMode(ledDespacho, OUTPUT);
  digitalWrite(ledDespacho, LOW);

  pinMode(ledDormitorio, OUTPUT);
  digitalWrite(ledDormitorio, LOW);
  
  pinMode(sensorGas,INPUT);
  

 WiFi.begin(ssid, password);
 WiFi.config(ip, gateway, subnet);
 
 server.begin(); 
 delay(4000);

//Llamamos a las funciones para medit las temperaturas, humedades, calidad del aire y controlar los LED
server.on("/humidity", HTTP_GET, getHumidity);
server.on("/temperature", HTTP_GET, getTemperature);
server.on("/humidityInt", HTTP_GET, getHumidityInt);
server.on("/temperatureInt", HTTP_GET, getTemperatureInt);
server.on("/gas", HTTP_GET, getGas);
server.on("/ledCocinaOn", HTTP_GET, ledCocinaOn);
server.on("/ledCocinaOff", HTTP_GET, ledCocinaOff);
server.on("/ledDespachoOn", HTTP_GET, ledDespachoOn);
server.on("/ledDespachoOff", HTTP_GET, ledDespachoOff);
server.on("/ledDormitorioOn", HTTP_GET, ledDormitorioOn);
server.on("/ledDormitorioOff", HTTP_GET, ledDormitorioOff);
server.on("/sensorDistancia", HTTP_GET, sensorDistancia);

}


void loop() {
  server.handleClient();
  EasyDDNS.update(10000);
}


//Funcion para enviar Correos
void EnviarMensajeGMAIL(){
  connection_state = WiFiConnect();
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

  
  Gsender *gsender1 = Gsender::Instance();    // Getting pointer to class instance
    String subject = "Cocina";
    if(gsender1->Subject(subject)->Send("marc_roura10@hotmail.com", "Hay gas en la cocina")) {
        Serial.println("Message send.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender1->getError());
    }
    
}
