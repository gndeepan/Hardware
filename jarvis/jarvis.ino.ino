#define ENA   0         // Enable/speed motors Right        GPIO14(D5)
#define ENB   15          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  14          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  2          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  12           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  13           // L298N in4 motors Left            GPIO0(D3)




long duration, distance;

int R_S = A0; //sensor Right
int S_1 = 16; //sensor center
int S_2 = 5; //sensor center
int L_S = 4; //sensor Left

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>


String command;
String data;
String ip;


int Cs1 = 0;// ir sensor value
int Cs2 = 0;// ir sensor value
int Rs = 0;
int Ls = 0;



int speedCar = 800;         // 400 - 1023. car speed
int speed_Coeff = 3;



const char* ssid = "Deepan";         // wifi dwtails
const char* password = "12345678";
ESP8266WebServer server(80);

#define FIREBASE_HOST "jarvis-2f668-default-rtdb.firebaseio.com"  // firebase details
#define FIREBASE_AUTH "hIEPKOvpY3n9rB9NpOcvcVXjHl6UBV0eqY06qYEj"





void setup() {

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);


  Serial.begin(115200);

  // Connecting WiFi


  // Connect to Wi-Fi
  Serial.print("Wi-Fi...");
  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP().toString();

  Serial.print("Ip Address: ");
  Serial.println(ip);



  // Starting WEB-server
  server.on ( "/", HTTP_handleRoot );
  server.onNotFound ( HTTP_handleRoot );
  server.begin();





  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // firebase sent ip address
  Firebase.setString("Jarvis/jarvis/ip", ip);




  pinMode(L_S, INPUT);    // set line sensor
  pinMode(S_1, INPUT);
  pinMode(S_2, INPUT);
  pinMode(R_S, INPUT);















}





void goAhead() {


  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goBack() {




  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goRight() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goLeft() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goAheadRight() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar / speed_Coeff);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar);
}

void goAheadLeft() {

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, speedCar / speed_Coeff);
}

void goBackRight() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar / speed_Coeff);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);
}

void goBackLeft() {

  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar / speed_Coeff);
}

void stopRobot() {
  data = "stop";

  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, speedCar);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, speedCar);

}



void line() {

  data = "startl";


  Ls = digitalRead(L_S);
  Cs1 = digitalRead(S_1);
  Cs2 = digitalRead(S_2);
  Rs = analogRead(R_S);



  //<=0
  //>=1


  if ((Ls == 0) && (Cs1 == 1) && (Cs2 == 1) && (Rs < 100)) {
    goBack();
    Serial.print("front");

    //left
  }
  if ((Ls == 0) && (Cs1 == 0) && (Cs2 == 0) && (Rs > 100)) {
    goRight();
    Serial.print("left1");
  }
  if ((Ls == 0) && (Cs1 == 0) && (Cs2 == 1) && (Rs > 100)) {
    goAheadRight();
    Serial.print("left2");
  }
  if ((Ls == 0) && (Cs1 == 1) && (Cs2 == 1) && (Rs > 100)) {
    goRight();
    Serial.print("left3");
  }


  //right

  if ((Ls == 1) && (Cs1 == 0) && (Cs2 == 0) && (Rs < 100)) {
    goLeft();
    Serial.print("right1");
  }
  if ((Ls == 1) && (Cs1 == 1) && (Cs2 == 0) && (Rs < 100)) {
    goAheadLeft();
    Serial.print("right2");
  }
  if ((Ls == 1) && (Cs1 == 1) && (Cs2 == 1) && (Rs < 100)) {
    goLeft();
    Serial.print("right3");
  }


  //stop


  if ((Ls == 1) && (Cs1 == 1) && (Cs2 == 1) && (Rs > 100)) {
    stopRobot();
    Serial.print("stop ");
  }
  if ((Ls == 0) && (Cs1 == 0) && (Cs2 == 0) && (Rs < 100)) {
    stopRobot();
    Serial.print("stop2 ");
  }






}







void loop() {

  server.handleClient();


  command = server.arg("State");

  // Serial.print("Connected data: ");
  // Serial.println(command);





  if (command == "line") {
    line();
  }




  if (data == "startl") {
    line();
  }

  if (command == "stop")stopRobot();



  else if (command == "B") goAhead();
  else if (command == "F") goBack();
  else if (command == "L") goLeft();
  else if (command == "R") goRight();
  else if (command == "I") goAheadRight();
  else if (command == "G") goAheadLeft();
  else if (command == "J") goBackRight();
  else if (command == "H") goBackLeft();
  else if (command == "0") speedCar = 400;
  else if (command == "1") speedCar = 470;
  else if (command == "2") speedCar = 540;
  else if (command == "3") speedCar = 610;
  else if (command == "4") speedCar = 680;
  else if (command == "5") speedCar = 750;
  else if (command == "6") speedCar = 820;
  else if (command == "7") speedCar = 890;
  else if (command == "8") speedCar = 960;
  else if (command == "9") speedCar = 1023;
  else if (command == "10") speedCar = 1023;
  else if (command == "S") stopRobot();


}

void HTTP_handleRoot(void) {

  if ( server.hasArg("State") ) {
    Serial.println(server.arg("State"));
  }
  server.send ( 200, "text/html", "" );
  delay(1);
}
