
#ifdef ESP8266
 #include <ESP8266WiFi.h>  // Pins for board ESP8266 Wemos-NodeMCU
 #else
 #include <WiFi.h>  
#endif
 
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Relays
#define RelayPin1 5  //D1
#define RelayPin2 4  //D2
#define RelayPin3 0  //D5
#define RelayPin4 2  //D6

// Switches
#define SwitchPin1 14 
//SD3
#define SwitchPin2 12   //D3 
#define SwitchPin3 13  //D7
#define SwitchPin4 15   //RX

//WiFi Status LED
#define wifiLed    16   //D0


int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 1; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 1; //Define integer to remember the toggle state for relay 4




//---- WiFi settings
const char* ssid = "Deepan";
const char* password = "12345678";

//---- MQTT Broker settings
const char* mqtt_server = "6b5b817567834cb09f57c948075583b3.s1.eu.hivemq.cloud"; // replace with your broker url
const char* mqtt_username = "deepan";
const char* mqtt_password = "deepan2020G";
const int mqtt_port =8883;


#define sub1 "switch1"
#define sub2 "switch2"
#define sub3 "switch3"
#define sub4 "switch4"

#define pub1 "switch1_status"
#define pub2 "switch2_status"
#define pub3 "switch3_status"
#define pub4 "switch4_status"


 

WiFiClientSecure espClient;   // for no secure connection use WiFiClient instead of WiFiClientSecure 
//WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

int value = 0;




static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";


//==========================================
void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}


//=====================================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(sub1);
      client.subscribe(sub2);
      client.subscribe(sub3);
      client.subscribe(sub4);
      // subscribe the topics here
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//================================================ setup
//================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);
  setup_wifi();
   Serial.begin(115200);
    
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  pinMode(wifiLed, OUTPUT);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
  
  //During Starting WiFi LED should TURN OFF
  digitalWrite(wifiLed, HIGH);

  #ifdef ESP8266
    espClient.setInsecure();
  #else   // for the ESP32
    espClient.setCACert(root_ca);      // enable this line and the the "certificate" code for secure connection
  #endif
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


//================================================ loop
//================================================
void loop() {

  if (!client.connected()) {
    digitalWrite(wifiLed, HIGH);
    reconnect();
  }
  else{
    digitalWrite(wifiLed, LOW);
    manual_control();
  }
  client.loop();
}

//=======================================  
// This void is called every time we have a message from the broker

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strstr(topic, sub1))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_1 = 0;
      client.publish(pub1, "0");
    } else {
      digitalWrite(RelayPin1, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_1 = 1;
      client.publish(pub1, "1");
    }    
  }

  else if ( strstr(topic, sub2))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin2, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_2 = 0;
      client.publish(pub2, "0");
    } else {
      digitalWrite(RelayPin2, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_2 = 1;
      client.publish(pub2, "1");
    }
  }
  else if ( strstr(topic, sub3))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin3, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_3 = 0;
      client.publish(pub3, "0");
    } else {
      digitalWrite(RelayPin3, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_3 = 1;
      client.publish(pub3, "1");
    }
  }
  else if ( strstr(topic, sub4))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin4, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_4 = 0;
      client.publish(pub4, "0");
    } else {
      digitalWrite(RelayPin4, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_4 = 1;
      client.publish(pub4, "1");
    }
  }
  else
  {
    Serial.println("unsubscribed topic");
  }
}

void manual_control(){
    //Manual Switch Control
    if (digitalRead(SwitchPin1) == LOW){
      delay(200);
      if(toggleState_1 == 1){
        digitalWrite(RelayPin1, LOW); // turn on relay 1
        toggleState_1 = 0;
        client.publish(pub1, "0");
        Serial.println("Device1 ON");
        }
      else{
        digitalWrite(RelayPin1, HIGH); // turn off relay 1
        toggleState_1 = 1;
        client.publish(pub1, "1");
        Serial.println("Device1 OFF");
        }
     }
    else if (digitalRead(SwitchPin2) == LOW){
      delay(200);
      if(toggleState_2 == 1){
        digitalWrite(RelayPin2, LOW); // turn on relay 2
        toggleState_2 = 0;
        client.publish(pub2, "0");
        Serial.println("Device2 ON");
        }
      else{
        digitalWrite(RelayPin2, HIGH); // turn off relay 2
        toggleState_2 = 1;
        client.publish(pub2, "1");
        Serial.println("Device2 OFF");
        }
    }
    else if (digitalRead(SwitchPin3) == LOW){
      delay(200);
      if(toggleState_3 == 1){
        digitalWrite(RelayPin3, LOW); // turn on relay 3
        toggleState_3 = 0;
        client.publish(pub3, "0");
        Serial.println("Device3 ON");
        }
      else{
        digitalWrite(RelayPin3, HIGH); // turn off relay 3
        toggleState_3 = 1;
        client.publish(pub3, "1");
        Serial.println("Device3 OFF");
        }
    }
    else if (digitalRead(SwitchPin4) == LOW){
      delay(200);
      if(toggleState_4 == 1){
        digitalWrite(RelayPin4, LOW); // turn on relay 4
        toggleState_4 = 0;
        client.publish(pub4, "0");
        Serial.println("Device4 ON");
        }
      else{
        digitalWrite(RelayPin4, HIGH); // turn off relay 4
        toggleState_4 = 1;
        client.publish(pub4, "1");
        Serial.println("Device4 OFF");
        }
    }
    delay(100);
}
