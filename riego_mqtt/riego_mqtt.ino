/*
   Programa para controlar un relé SonoFF
   desde una Raspberry Pi con Node-RED y MQTT.
*/

// Librerías
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Librerías WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <DHT.h>

// Datos MQTT
const char* mqtt_server = "192.168.1.97";
const char* mqtt_topic_humedadTierra = "casa/invernadero/humedadTierra";
const char* mqtt_topic_humedad = "casa/invernadero/humedad";
const char* mqtt_topic_temperatura = "casa/invernadero/temperatura";
const char* mqtt_topic_ventilador = "casa/invernadero/ventilador";
const char* mqtt_topic_bombaAgua = "casa/invernadero/bombaAgua";
const char* mqtt_topic_nivelAgua = "casa/invernadero/nivelAgua";

// Conexión a la red WiFi y al Broker
WiFiClient espClient;
PubSubClient clientMqtt(espClient);


// Tiempos para enviar los mensajes
long lastMsg = 0;
const int tiempoEnvioMsg = 2000;
//Declaracion de las variables
#define DHTPIN D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//variables sensor distancia
#define TRIGGER 16  //D0
#define ECHO    5   //D1

//variable sensorTierra
const int humedadTierra = A0;

//variables sensorDistancia (nivel del agua)
long duracion = 0;
int16_t distancia = 0;
int16_t ultimoValor = 0;
uint8_t contador = 0;

//variable agua y ventilador
int BombaActivada = 14; //D5
int ventilador = 13; //D7


//funcion sensor distancia (nivel de agua)
//función para medir la cantidad de agua en la botella(sensor distancia)
void sensorDistancia()
{


  WiFiClient client;
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duracion = pulseIn(ECHO, HIGH);
  distancia = (duracion / 2) / 29.1;
  if (distancia < 0) distancia = 0;
  if (distancia != ultimoValor) {
    String msj = String(distancia);
    ultimoValor = distancia;
  }
}
void setup() {
  // Iniciamos comunicación serie
  Serial.begin(115200);

  // Modo del pin LED y del relé
  /*pinMode(pinRele, OUTPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(pulsador, INPUT);*/
/*
  // Apagamos los dos pines
  digitalWrite(pinRele, LOW);
  digitalWrite(pinLed, HIGH);*/

  // Iniciamos WiFiManager
  WiFiManager wifiManager;

  // Configuración del punto de acceso
  wifiManager.autoConnect("releSonoffAP");

  // Configuración MQTT
  clientMqtt.setServer(mqtt_server, 1883);
  clientMqtt.setCallback(callback);

  // Configuración de interrupción
 // attachInterrupt(digitalPinToInterrupt(pulsador), enviarEstadoMQTT, FALLING);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido[");
  Serial.print(topic);
  Serial.print("] ");
  // Mostramos el mensaje en el monitor serie
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Si el topic coincide con el de la luz apagamos o encendemos
 /* if (strcmp(topic, mqtt_topic_rele) == 0) {
    // Si el parámetro recibido es un 1 apagamos el LED
    // Esto es debido a que los LEDs integrados en la placa
    // funcionan de forma invertida en NodeMCU.
    if ((char)payload[0] == '1') {
      // Encendemos el LED
      digitalWrite(pinLed, LOW);
      // Activamos el relé
      digitalWrite(pinRele, HIGH);
      // Cambiamos estado del relé
      estadoRele = true;
      // En caso contrario
    } else {
      // Apagamos el LED
      digitalWrite(pinLed, HIGH);
      // Desactivamos el relé
     // digitalWrite(pinRele, LOW);
      // Cambiamos estado del relé
      estadoRele = false;
    }
  }*/
}

void loop() {
  // Comprobación si hemos perdido conexión
  if (!clientMqtt.connected()) {
    reconnect();
  }

  // Procesamos los mensajes entrantes
  clientMqtt.loop();

  // Si ha pasado el tiempo marcado enviamos la temperatura
  if (millis() - lastMsg > tiempoEnvioMsg) {
    lastMsg = millis();

    char msg[50];
    snprintf (msg, 50, "%2.1f", dht.readTemperature());
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);
    clientMqtt.publish(mqtt_topic_temperatura, msg);
  }

  // Si ha pasado el tiempo marcado enviamos la HUMEDAD
  if (millis() - lastMsg > tiempoEnvioMsg) {
    lastMsg = millis();

    char msg[50];
    snprintf (msg, 50, "%2.1f", dht.readHumidity());
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);
    clientMqtt.publish(mqtt_topic_humedad, msg);
  }

  // Si ha pasado el tiempo marcado enviamos la temperatura
  int hTierra = analogRead(humedadTierra);
  if (millis() - lastMsg > tiempoEnvioMsg) {
    lastMsg = millis();

    char msg[50];
    snprintf (msg, 50, "%2.1f", hTierra);
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);
    clientMqtt.publish(mqtt_topic_humedadTierra, msg);
  }

  // Si ha pasado el tiempo marcado enviamos la temperatura
  
  if (millis() - lastMsg > tiempoEnvioMsg) {
    lastMsg = millis();

    char msg[50];
    snprintf (msg, 50, "%2.1f", distancia);
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);
    clientMqtt.publish(mqtt_topic_nivelAgua, msg);
  }

  
}

void reconnect() {
  // Repetimos hasta que se conecte con el Broker MQTT
  while (!clientMqtt.connected()) {
    Serial.print("Esperando conexión con MQTT...");
    // Intentamos conectar
    // El parámetro es el nombre que tiene este cliente
    if (clientMqtt.connect("NodeMCUClient")) {
      Serial.println("Conectdo");
      // Nos suscribimos al topic de la luz
      clientMqtt.subscribe(mqtt_topic_temperatura);
      clientMqtt.subscribe(mqtt_topic_humedadTierra);
      clientMqtt.subscribe(mqtt_topic_nivelAgua);
      
    } else {
      Serial.print("Fallo al conectar, rc=");
      Serial.print(clientMqtt.state());
      Serial.println(" Intentando conectar en 5 segundos");
      // Esperamos 5 segundos
      delay(5000);
    }
  }

}
