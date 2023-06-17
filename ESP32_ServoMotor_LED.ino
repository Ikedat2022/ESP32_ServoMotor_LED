// 『ESP32でのサーボモーターの使い方』
// 角度をPWM信号化するライブラリを使用
// https://wak-tech.com/archives/1534

// 『【超簡単】 ESP32 を使って携帯でLEDを点灯する(Wifi使用）』
// https://enjoy-life-fullest.com/2022/02/05/%E3%80%90%E8%B6%85%E7%B0%A1%E5%8D%98%E3%80%91-esp32-%E3%82%92%E4%BD%BF%E3%81%A3%E3%81%A6%E6%90%BA%E5%B8%AF%E3%81%A7led%E3%82%92%E7%82%B9%E7%81%AF%E3%81%99%E3%82%8Bwifi%E4%BD%BF%E7%94%A8%EF%BC%89/

#include <WiFi.h>
#include <Servo.h>

const char *ssid = "HUMAX-64606"; //SSID
const char *password = "NEdJdTX5LENad"; //password

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String servoMotorOutput18State = "off";

// Assign output variables to GPIO pins
const int servoMotorOutput18 = 18;
const int ledOutput32 = 32;

boolean motorMoveFlag = true;

Servo myservo; //Servoオブジェクトを作成

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {

  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  myservo.attach(servoMotorOutput18);
  pinMode(ledOutput32, OUTPUT);
}

void loop(){
  if(motorMoveFlag){
    Serial.println("動くゾ〜");
    // サーボモーターを0度に戻す
    myservo.write(0); 
    delay(500);
    // サーボモーターを180度まで動かす
    myservo.write(180); 
    delay(500);  
  }
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if(client){                               // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
        
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /18/on") >= 0) {
              Serial.println("GPIO 18 on");
              servoMotorOutput18State = "on";
              // サーボモーターを0に戻す
              delay(700);
              myservo.write(90); 
              Serial.println("onが押されました");
              // 3度LEDを点灯させる
              digitalWrite(ledOutput32, LOW);
              delay(500);
              digitalWrite(ledOutput32, HIGH);
              delay(700);
              digitalWrite(ledOutput32, LOW);
              delay(300);
              digitalWrite(ledOutput32, HIGH);
              delay(700);
              digitalWrite(ledOutput32, LOW);
              delay(300);
              digitalWrite(ledOutput32, HIGH);
              delay(700);
              digitalWrite(ledOutput32, LOW);
              motorMoveFlag = false;
            } else if (header.indexOf("GET /18/off") >= 0) {
              Serial.println("GPIO 18 off");
              servoMotorOutput18State = "off";
              // サーボモーターを0に戻す
              Serial.println("offが押されました");
              motorMoveFlag = true;
              delay(300);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Servo Motor LED</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 18  
            client.println("<p>GPIO 18 - State " + servoMotorOutput18State + "</p>");
            // If the servoMotorOutput18State is off, it displays the ON button       
            if (servoMotorOutput18State=="off") {
              client.println("<p><a href=\"/18/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/18/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
                       
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
