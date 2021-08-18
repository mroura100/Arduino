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

// Datos MQTT
const char* mqtt_server = "192.168.1.97";
const char* mqtt_topic_bomba = "casa/bomba";
const char* mqtt_topic_estado_bomba = "casa/bomba/estadbomba";

// Conexión a la red WiFi y al Broker
WiFiClient espClient;
PubSubClient clientMqtt(espClient);

// Pin del relé y del LED
const byte pinRele = 14;
const byte pinLed = 13;

// Interrupcion
const byte pulsador = 0;
boolean estadoRele = false;
volatile boolean enviarEstado = false;

void setup() {
  // Iniciamos comunicación serie
  Serial.begin(115200);

  // Modo del pin LED y del relé
  pinMode(pinRele, OUTPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(pulsador, INPUT);

  // Apagamos los dos pines
  digitalWrite(pinRele, LOW);
  digitalWrite(pinLed, HIGH);

  // Iniciamos WiFiManager
  WiFiManager wifiManager;

  // Configuración del punto de acceso
  wifiManager.autoConnect("bombaffAP");

  // Configuración MQTT
  clientMqtt.setServer(mqtt_server, 1883);
  clientMqtt.setCallback(callback);

  // Configuración de interrupción
  attachInterrupt(digitalPinToInterrupt(pulsador), enviarEstadoMQTT, FALLING);
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
  if (strcmp(topic, mqtt_topic_bomba) == 0) {
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
      digitalWrite(pinRele, LOW);
      // Cambiamos estado del relé
      estadoRele = false;
    }
  }
}

void loop() {

  // Comprobación si hemos perdido conexión
  if (!clientMqtt.connected()) {
    reconnect();
  }

  // Procesamos los mensajes entrantes
  clientMqtt.loop();

  // Si tenemos que enviar el estado a Node-RED lo enviamos
  if (enviarEstado) {
    // Cambiamos para que no vuelva a enviar
    enviarEstado = false;

    // Cambiamos estado del relé
    estadoRele = !estadoRele;

    // Encendemos o apagamos el relé y LED
    digitalWrite(pinLed, !estadoRele);
    digitalWrite(pinRele, estadoRele);

    // Enviamos el mensaje
    char msg[1] = {'1'};

    // Si ha sido apagado actualizamos el mensaje
    if (!estadoRele) {
      msg[0] = '0';
    }

    // Mostramos información por el monitor serie
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);

    // Publicación del mensaje
    clientMqtt.publish(mqtt_topic_estado_bomba, msg);
  }
}

void reconnect() {
  // Repetimos hasta que se conecte con el Broker MQTT
  while (!clientMqtt.connected()) {
    Serial.print("Esperando conexión con MQTT...");
    // Intentamos conectar
    // El parámetro es el nombre que tiene este cliente
    if (clientMqtt.connect("bombaClient")) {
      Serial.println("Conectado");
      // Nos suscribimos al topic de la luz
      clientMqtt.subscribe(mqtt_topic_bomba);
    } else {
      Serial.print("Fallo al conectar, rc=");
      Serial.print(clientMqtt.state());
      Serial.println(" Intentando conectar en 5 segundos");
      // Esperamos 5 segundos
      delay(5000);
    }
  }
}

void enviarEstadoMQTT() {
  enviarEstado = true;
}
