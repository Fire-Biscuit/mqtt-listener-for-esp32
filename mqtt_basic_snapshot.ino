#include <WiFi.h>
#include <PubSubClient.h>
#include <driver/gpio.h>


#define magnet2 GPIO_NUM_11
#define magnet GPIO_NUM_10

//these are the wifi credentials
const char* ssid = "#########";
//NOTE: uncomment the one that is needed
const char* password = "#########";     //Mike's esp
//const char* password = "#########";        //Timo's esp

//these are the mqtt client credentials
const char* mqttServer = "#########";
const int mqttPort = 1883;
const char* mqttUser = "#########";
const char* mqttPassword = "#########";

//these are constant chars which are the mqtt topic we are using
const char* topicEqualsAan = "#########/aan";
const char* topicEqualsUit = "#########/uit";

//these objects will be used to use wifi and mqtt
WiFiClient espClient;
PubSubClient client(espClient);

//this functies is located later in the code but is quickly defined here so that we dont get any errors when we call it before the actual function 
void callback(char* topic, byte* payload, unsigned int length);

//this makes the connection with the mqtt server
void mqttConnect(){
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  //this tries to connect until it has a connection
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
 
      Serial.println("Verbonden met de mqtt server.");  
    } else {
      Serial.print("Foutmeldingstatus:  ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

int btnGPIO = 0;
int btnState = false;
void connect() {
  int tryDelay = 500;
  int numberOfTries = 20;

  //this tries to connect until it has a connection with the wifi network
  //(this happens before the connection with the mqtt server is made)
  while (true) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.print("[WiFi] Failed to connect to WiFi!");
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Mqtt message received: ");
  Serial.println(topic);

  //here the payload array is thrown into a sngle string so that we can work with it
  String incommingPayload;
  Serial.print("message payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    incommingPayload = incommingPayload + (String)payload[i];
  }
  Serial.println();
  
  //here we check if the payload matches the bytearray for "uit" of "aan"
  //we than put the magnet in the matching state
  if(incommingPayload == "9797110"){
    gpio_set_level(magnet,1);
    gpio_set_level(magnet2,1);
    Serial.println("magnet on");
  }else if(incommingPayload == "117105116"){
    gpio_set_level(magnet,0);
    gpio_set_level(magnet2,0);
    Serial.println("magnet off");
  }
  Serial.println("-----------------------");
}

void setup() {
  gpio_set_direction(magnet, GPIO_MODE_OUTPUT);
  gpio_set_direction(magnet2, GPIO_MODE_OUTPUT);
  Serial.begin(115200);
  delay(100);
  //here we start by connecting to a WiFi network

  Serial.println();
  Serial.println("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  connect();

  mqttConnect();

  client.subscribe("#########/aan");
  client.subscribe("#########/uit");
}

//in this loop we check if the wifi is disconnected and reconnect if that happens
void loop() {
  if(WiFi.status() != WL_CONNECTED){
    connect();
  }
  
  
  client.loop();
}