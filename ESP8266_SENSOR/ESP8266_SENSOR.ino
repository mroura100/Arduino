#include <DHT.h>

#pragma region Globals
 #include <ESP8266WiFi.h>
 #include "Gsender.h"
#include <WiFiClientSecure.h>



#define DHTPIN D6//Sensor DHT11
#define DHTTYPE DHT22 
#define DHTPIN2 D7 //Sensor DHT11
#define DHTTYPE2 DHT11

//variable mensaje correo



//variable sensor luz
int ledPinSensor = 14; //D5
int sensorLuz = 16;
const int nivelLuz = 100; //Nivel de luz

//variable sensorGas
int sensorGas=A0; //D0

String TramaMensajeGmail = "";


//const char* ssid = "TP-LINK_51172A_51172A";
//const char* password = "casacasa";
//wifi connect
const char* ssid = "MOVISTAR_48D5";
const char* password = "SolyVida1@";

// Create an instance of the server
// specify the port to listen on as an argument

DHT dht2(DHTPIN2, DHTTYPE2);
DHT dht(DHTPIN, DHTTYPE);

IPAddress ip(192,168,1,180);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);
//WiFiServer server(80);

/*uint8_t connection_state = 0;                    // Connected to WIFI or not
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
    //Serial.println(WiFi.localIP);
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
}*/



void setup() {
  Serial.begin(115200);
  delay(10);

  /*connection_state = WiFiConnect();
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect

    Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
    String subject = "Subject is optional!";
    if(gsender->Subject(subject)->Send("marc_roura10@hotmail.com", "hi here is testing email")) {
        Serial.println("Message send.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender->getError());
    }*/
  
  dht.begin();
  dht2.begin();
  
  //pinMode(ledPin, OUTPUT);
// digitalWrite(ledPin, LOW);


 pinMode(ledPinSensor, OUTPUT);
   pinMode(sensorLuz, INPUT);

   pinMode(sensorGas,INPUT);

   
  // Connect to WiFi network
  
  
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  WiFi.config(ip,gateway,subnet);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  //serverThingSpeak.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  Serial.println("Sensor Luz: "+sensorLuz);
}


void loop() {



  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }




//encendemos el LED segun la luz que haya en el entorno
 
 int input = digitalRead(sensorLuz);

 int inputGas = analogRead(sensorGas);
 


 


 
 /*if(input > nivelLuz){
  digitalWrite(ledPinSensor, LOW);
 }else{
  digitalWrite(ledPinSensor, HIGH);
 }*/
  
  // Read the first line of the request
  float h2 = dht2.readHumidity();
  float h = dht.readHumidity();
  
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  float t2 = dht2.readTemperature();


  




  

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
   //leds

 
  /*client.stop();
  Serial.println("Waiting…");
  delay(20000);*/


  /*int value = digitalRead(ledPin);
  if (req.indexOf("on") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (req.indexOf("off") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
 }*/


 //led de sensor de luz amarillo
 int value = digitalRead(ledPinSensor);
 if (req.indexOf("ledAmarillo") != -1)  {
    digitalWrite(ledPinSensor, HIGH);
    value = HIGH;
 }

//valor sensor de luz
 if (req.indexOf("/sensorLuz") != -1)  {
    client.print("Sensor de luz"); 
    client.print(input); 
    //value = HIGH;
 }
 

  if (req.indexOf("/data") != -1){
    client.flush();
    client.println("HTTP/1.1 200 OK");           // This tells the browser that the request to provide data was accepted
    client.println("Access-Control-Allow-Origin: *");  //Tells the browser it has accepted its request for data from a different domain (origin).
    client.println("Content-Type: application/json;charset=utf-8");  //Lets the browser know that the data will be in a JSON format
    //client.print("GET 192.168.1.37:8080/WebDomotica/entrada_datos.php?sensorId=1111111&temperatura=");
    client.print(t);
    client.println("Server: Arduino");           // The data is coming from an Arduino Web Server (this line can be omitted)
    client.println("Connection: close");         // Will close the connection at the end of data transmission.
    client.println();                            // You need to include this blank line - it tells the browser that it has reached the end of the Server reponse header.
                               // This is tha starting bracket of the JSON data
    client.print("{\"temperExterior\": \"");
    client.print(t);                           
    client.print("\", \"humedadExterior\": \"");
    client.print(h);
    client.print("\", \"luz\": \"");
    client.print(input);
    client.print("\", \"temperInterior\": \"");
    client.print(t2);
    client.print("\", \"humedadInterior\": \"");
    client.print(h2);                  
    client.print("\", \"calidadAire\": \"");
    client.print(inputGas);                  
    client.print("\"}");  
    
    //client.print(String("GET ")+ "192.168.1.37:8080/WebDomotica/entrada_datos.php?sensorId=1111111&temperatura=" + t);                    
                      
  }
  else {
    client.flush();
    client.println("HTTP/1.1 200 OK");           // This tells the browser that the request to provide data was accepted
    client.println("Access-Control-Allow-Origin: *");  //Tells the browser it has accepted its request for data from a different domain (origin).
    client.println("Content-Type: application/json;charset=utf-8");  //Lets the browser know that the data will be in a JSON format
    
    client.println("Server: Arduino");           // The data is coming from an Arduino Web Server (this line can be omitted)
    client.println("Connection: close");         // Will close the connection at the end of data transmission.
    client.println();                            // You need to include this blank line - it tells the browser that it has reached the end of the Server reponse header.
                          // This is tha starting bracket of the JSON data
    client.print("{\"Response\": ");
    client.print("Invalid");                          
    client.print("}");                     
                       // This is the final bracket of the JSON data
  }

    delay(1);
    Serial.println("Client disonnected");

    /*//concexion php
    String urlPhp="http://192.168.1.37:8080/WebDomotica/entrada_datos.php";
    String data="sensorId=1&temperatura=";
    String host="192.168.1.37";

    //solicitud tipo POST que enviamos al servidor
    client.print(String("GET ") + urlPhp + " HTTP/1.0\r\n" + 
           "Host: " + host + "\r\n" + 
           "Accept: *" + "/" + "*\r\n" +
           "Content-Lengh: " + data.length() + "\r\n" +
           "Content-Type: application/x-www-form-urlencoded\r\n" +
           "\r\n" + data);
       delay(10);*/

       if(input<=40){
        digitalWrite(ledPinSensor, LOW);
       }else{
        digitalWrite(ledPinSensor, HIGH);
       }

      /* if(inputGas>180){
        EnviarMensajeGMAIL();
        
       }*/

  
 

}

/*void EnviarMensajeGMAIL(){
  
  Gsender *gsender1 = Gsender::Instance();    // Getting pointer to class instance
    String subject = "Cocina";
    if(gsender1->Subject(subject)->Send("marc_roura10@hotmail.com", "Hay gas en la cocina")) {
        Serial.println("Message send.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender1->getError());
    }
    
}*/
