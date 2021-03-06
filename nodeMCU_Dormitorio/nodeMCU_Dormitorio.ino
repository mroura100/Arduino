/*
   Programa para controlar el LED integrado en NodeMCU
   desde una Raspberry Pi con Node-RED y MQTT.
*/

// Librerías
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"


// Información para conexión a la red WiFi
const char* ssid = "Lowi3F70";
const char* password = "G2E2W4D6QWEEMU";

// Datos MQTT
const char* mqtt_server = "192.168.1.97";

const char* mqtt_topic_temp = "casa/dormitorio/temperatura";


const char* mqtt_topic_hum = "casa/dormitorio/humedad";

// Configuración DHT11
const byte dhtPin = 5;



DHTesp dht;

// Conexión a la red WiFi y al Broker
WiFiClient espClient;
PubSubClient clientMqtt(espClient);

// Tiempos para enviar los mensajes
long lastMsg = 0;
const int tiempoEnvioMsg = 5000;

void setup() {
  // Iniciamos comunicación serie
  Serial.begin(115200);

  dht.setup(5, DHTesp::DHT11); // Connect DHT sensor to GPIO 5

  // Conexión con la red Wifi
  setup_wifi();

  // Configuración MQTT
  clientMqtt.setServer(mqtt_server, 1883);
  //clientMqtt.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // Iniciamos la conexión con la red WiFi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  // Intentamos conectar
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectada");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

/*void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido[");
  Serial.print(topic);
  Serial.print("] ");
  // Mostramos el mensaje en el monitor serie
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Si el topic coincide con el de la luz apagamos o encendemos
  if (strcmp(topic, mqtt_topic_luz) == 0) {
    // Si el parámetro recibido es un 1 apagamos el LED
    // Esto es debido a que los LEDs integrados en la placa
    // funcionan de forma invertida en NodeMCU.
    if ((char)payload[0] == '1') {
      // Encendemos el LED
      digitalWrite(BUILTIN_LED, LOW);
      // En caso contrario
    } else {
      // Apagamos el LED
      digitalWrite(BUILTIN_LED, HIGH);
    }
  }
}*/

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

    char temperature[50];
    snprintf (temperature, 50, "%2.1f", dht.getTemperature());
    Serial.print("Publicando mensaje: ");
    Serial.println(temperature);

   
  //Serial.print(temperature, 1);
    clientMqtt.publish(mqtt_topic_temp, temperature);

     char humedad[50];
    snprintf (humedad, 50, "%2.1f", dht.getHumidity());
    Serial.print("Publicando mensaje: ");
    Serial.println(humedad);

   
  //Serial.print(temperature, 1);
    clientMqtt.publish(mqtt_topic_hum, humedad);
  }


  

   
  


  
}

void reconnect() {
  // Repetimos hasta que se conecte con el Broker MQTT
  while (!clientMqtt.connected()) {
    Serial.print("Esperando conexión con MQTT...");
    // Intentamos conectar
    // El parámetro es el nombre que tiene este cliente
    if (clientMqtt.connect("NodeMCUDormitorio")) {
      Serial.println("Conectdo");
      // Nos suscribimos al topic de la luz
      //clientMqtt.subscribe(mqtt_topic_luz);
      
    } else {
      Serial.print("Fallo al conectar, rc=");
      Serial.print(clientMqtt.state());
      Serial.println(" Intentando conectar en 5 segundos");
      // Esperamos 5 segundos
      delay(5000);
    }
  }
}
