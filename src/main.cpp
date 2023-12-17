#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
using namespace std;
#define MOTOR_PIN 25
//buzzer pin
#define BUZZ_PIN 14
//motor temp pin
#define ThermistorPin 34
int Vo;
//ultrasonic pin
#define TONE_PIN 12
#define trigPin 18
#define echoPin 19

// defines variables for ultrasonic
long duration;
int distance1 = 0;
bool distanceTooClose = LOW;

// Update these with values suitable for your network.
const char* ssid = "Mikey_Ilya";
const char* password = "mikeythegoat";
const char* mqtt_server = "test.mosquitto.org";
#define mqtt_port 1883
//#define MQTT_USER "james"
//#define MQTT_PASSWORD "wesr431bghj"
#define MQTT_SERIAL_PUBLISH_CH "csc113/serialdata/tx"
#define MQTT_SERIAL_SUBSCRIBE_CH "csc113/serialdata/tx"

#define MQTT_VELOCITY "csc113/controller/vel"
#define MQTT_STEER "csc113/controller/st"

#define MQTT_PROX "csc113/controller/prox"

#define VRX_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRX pin
#define VRY_PIN  39 // ESP32 pin GPIO39 (ADC0) connected to VRY pin

double dataFromClbk;

String valueX = ""; // to store the X-axis value
String valueY = ""; // to store the Y-axis value

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network // Test commit
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-Shamash-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) 
    if (client.connect(clientId.c_str()) )
    {
      Serial.println("connected");
      // subscribe
      //client.subscribe(MQTT_SERIAL_SUBSCRIBE_CH);
      client.subscribe(MQTT_VELOCITY);
      //client.subscribe(MQTT_STEER);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    //Serial.println("-------new message from broker-----");
    //Serial.print("channel:");
    //Serial.println(topic);
    //Serial.print("data:");  
    String messageTemp;

    for (int i = 0; i < length; i++) 
    { 
      //Serial.print((char)payload[i]);
      messageTemp += ((char)payload[i]);
    }
    //Serial.write(payload, length);
    dataFromClbk = stod(messageTemp.c_str());
    //Serial.print(dataFromClbk);

  //stod to turn string to double   
}

void ultrasonicSense()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance1 = duration * 0.034 / 48;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance1);
  if(distance1 <= 2)
    distanceTooClose = HIGH;
  else
    distanceTooClose = LOW;
  Serial.print(" TOO CLOSE? ");
  Serial.println(distanceTooClose);
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  Serial.setTimeout(5000);
  pinMode(MOTOR_PIN, OUTPUT);
  while (! Serial);
    Serial.println("Speed 0 to 255");
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void publishSerialData(const char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  Serial.println(String("publishing: ") + serialData);

  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}

void loop() {
  client.loop();


  //valueX = analogRead(VRX_PIN);
  //valueY = analogRead(VRY_PIN);

  // read X and Y analog values
  int speed = dataFromClbk/8;
  if (speed >= 0 && speed <= 255) {
      if (speed !=0)
        {Serial.println(speed);}
      analogWrite(MOTOR_PIN, speed);
  }
  ultrasonicSense();
  client.publish(MQTT_PROX, to_string(distance1).c_str());
  if(distanceTooClose)
  {
    tone(BUZZ_PIN, 256);
  }
  else
    noTone(BUZZ_PIN);
  
  
  /*
  Vo = analogRead(ThermistorPin)-3600;


  Serial.print("Temperature: "); 
  Serial.print(Vo);
  Serial.println(" F"); 
  */
  
   

  //scale value of joystick so that it's between 0 and 255

  // print data to 
  /**
  Serial.print("x = ");
  Serial.print(valueX);
  Serial.print(", y = ");
  Serial.println(valueY); 
  */
 }


