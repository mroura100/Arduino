// Neopixel
#include <Adafruit_NeoPixel.h>

// ESP 8266
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// MQTT Subscribes
const char* SubscribeTo = "esp/1/out";
const char* SubscribeFrom = "esp/1/in";

// WiFi & MQTT Server
const char* ssid = "Lowi3F70";
const char* password = "G2E2W4D6QWEEMU";
const char* mqtt_server = "192.168.1.97";

WiFiClient espClient;
PubSubClient pubClient(espClient);
long lastMsg = 0;
char msg[50];
String message("#000000");
String lastMessage("#000000");

// Neopixel Config
#define NeoPIN D3
#define NUM_LEDS 30
int brightness = 150;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NeoPIN, NEO_RGB + NEO_KHZ800);

void setup() {
  Serial.begin ( 115200 );
  
  // ##############
  // NeoPixel start
  Serial.println();
  strip.setBrightness(brightness);
  strip.begin();
  strip.show(); 
  delay(50);
  
  // WiFi
  setup_wifi();
  
  // MQTT
  Serial.print("Connecting to MQTT...");
  // connecting to the mqtt server
  pubClient.setServer(mqtt_server, 1883);
  pubClient.setCallback(callback);
  Serial.println("done!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String color("#");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    if(i > 0){
      color = color + String((char)payload[i]);  
    }  
  }
  
  Serial.println();


  // finding payload
  if((char)payload[0] == '#'){
    // setting color
     setNeoColor(color);
  }
  
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!pubClient.connected()) {
    delay(100);
    reconnect();
  }
  pubClient.loop();
  
  long now = millis();
  if (now - lastMsg > 60000 || lastMessage.indexOf(message) < 0 ) {
    
    lastMsg = now;
    Serial.print("Publish message: ");
    Serial.println(message);
    char msg[7];
    message.toCharArray(msg, message.length());
    Serial.print("Publishing...");
    pubClient.publish(SubscribeTo, msg);
    Serial.println("Done!!!");
    lastMessage = message;
  }
  delay(10);
}


void setNeoColor(String value){
    Serial.println("Setting Neopixel..." + value);
    message = value;
    int number = (int) strtol( &value[1], NULL, 16);
    
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

    

     Serial.print("RGB: ");
    Serial.print(r, DEC);
    Serial.print(" ");
    Serial.print(g, DEC);
    Serial.print(" ");
    Serial.print(b, DEC);
    Serial.println(" ");
  
   for(int i=0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color( g, r, b ) );
    }
    strip.show();
    
    Serial.println("on.");
}


void reconnect() {
  // Loop until we're reconnected
  while (!pubClient.connected()) {
    // Attempt to connect
    if (pubClient.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      pubClient.publish(SubscribeTo, "Restart");
      // ... and resubscribe
      pubClient.subscribe(SubscribeFrom);
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
