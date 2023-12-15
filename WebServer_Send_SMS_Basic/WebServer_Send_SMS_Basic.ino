#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 11, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

String messages = "";


void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
  Ethernet.init(17);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  //Serial2 = LTE Cat.M1
  /*
    Serial2.setTX(8);
    Serial2.setRX(9); */
  Serial2.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    String httpMethod, httpPath, postData;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c); // 디버깅을 위해 시리얼 모니터에 출력

        if (httpMethod.length() == 0 || httpPath.length() == 0) {
          if (c == ' ') {
            if (httpMethod.length() == 0) {
              httpMethod = postData;
              postData = "";
            } else if (httpPath.length() == 0) {
              httpPath = postData;
              postData = "";
            }
          } else {
            postData += c;
          }
        } else if (c == '\n' && currentLineIsBlank) {
          if (httpMethod == "POST") {
            Serial.println("POST data received");
            int endIndex;
            while (client.available()) {
              char k = client.read();
              postData += k;
              if (postData.endsWith("\r\n\r\n")) {
                break; // 종료 조건
              }
            }

            // 전화번호와 메시지 파싱
            String phone = parsePostData(postData, "phone=");
            String message = parsePostData(postData, "&message=");
            messages += "Phone: " + phone + ", Message: " + message + "<br>";

            Serial.println(messages); // 디버깅 출력

            //SMS Send
            Serial2.print("AT+CMGF=1\r\n");
            delay(50);
            while (1) {
              if (Serial2.available())
              {
                char RC = Serial2.read();
                Serial.write(RC);
                if (RC == '\n') break;
              }
            }
            Serial2.print("AT+CSCS=\"GSM\"\r\n");
            delay(50);
            while (1) {
              if (Serial2.available())
              {
                char RC = Serial2.read();
                Serial.write(RC);
                if (RC == '\n') break;
              }
            }
            Serial2.print("AT+CMGS=\"");
            Serial2.print(phone);
            Serial2.print("\"\r\n");
            delay(50);
            while (1) {
              if (Serial2.available())
              {
                char RC = Serial2.read();
                Serial.write(RC);
                if (RC == '\n') break;
              }
            }
            Serial2.print(message);
            //Serial2.println();
            delay(50);
            while (1) {
              if (Serial2.available())
              {
                char RC = Serial2.read();
                Serial.write(RC);
                if (RC == '\n') break;
              }
            }
            Serial2.print((char)26);
            delay(50);
            while (1) {
              if (Serial2.available())
              {
                char RC = Serial2.read();
                Serial.write(RC);
                if (RC == '\n') break;
              }
            }
          }

          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // 웹 페이지 콘텐츠
          client.println("<!DOCTYPE html>");
          client.println("<html lang=\"en\">");
          client.println("<head>");
          client.println("<meta charset=\"UTF-8\">");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
          client.println("<title>Raspberry Pi Pico Web to LTE SMS Messenger</title>");
          client.println("<style>");
          client.println("  body { font-family: 'Arial', sans-serif; margin: 0 auto; max-width: 600px; padding: 1rem; }");
          client.println("  h1, h2 { color: #333; }");
          client.println("  form { margin-bottom: 2rem; }");
          client.println("  input[type=text], input[type=submit] { padding: 0.5rem; margin: 0.5rem 0; }");
          client.println("  input[type=submit] { color: white; background-color: #007BFF; border: none; cursor: pointer; }");
          client.println("  input[type=submit]:hover { background-color: #0056b3; }");
          client.println("  .history { background-color: #f2f2f2; padding: 1rem; border-radius: 0.5rem; }");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>LTE SMS Messenger</h1>");
          client.println("<h2>Raspberry Pi Pico Web to SMS</h2>");
          client.println("<form action=\"/\" method=\"POST\">");
          client.println("  <label for=\"phone\">Phone Number:</label>");
          client.println("  <input type=\"text\" id=\"phone\" name=\"phone\"><br>");
          client.println("  <label for=\"message\">Message:</label>");
          client.println("  <input type=\"text\" id=\"message\" name=\"message\"><br>");
          client.println("  <input type=\"submit\" value=\"Send\">");
          client.println("</form>");
          client.println("<div class=\"history\">");
          client.println("  <h2>Message History:</h2>");
          client.println(messages);
          client.println("</div>");
          client.println("</body>");
          client.println("</html>");

          client.stop();
          Serial.println("Client disconnected");
        }

        if (c == '\n') {
          // 새로운 줄이 시작될 때
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
  }
}

// POST 데이터 파싱 함수
String parsePostData(String data, String field) {
  String result = "";
  int startIndex = data.indexOf(field);
  if (startIndex != -1) {
    startIndex += field.length();
    int endIndex = data.indexOf('&', startIndex);
    if (endIndex == -1) {
      endIndex = data.length();
    }
    result = data.substring(startIndex, endIndex);
    result.replace('+', ' '); // 공백 처리
    result.replace("%20", " "); // URL 인코딩된 공백 처리
  }
  return result;
}
