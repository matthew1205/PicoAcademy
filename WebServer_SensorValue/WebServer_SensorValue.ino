#include <SPI.h>
#include <Ethernet.h>    // W5500부터 Ethernet2 라이브러리를 사용.


#define SOCK_PORT        80
#define PIN_LED_R        12
#define PIN_LED_B        13
#define PIN_LED_G        14

#define PIN_LIGHT        26

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };    // MAC ADDRESS.

IPAddress local_ip(192,168, 11, 177);    // IP Address
IPAddress gateway(192,168, 11, 1);    // Gateway Address
IPAddress subnet(255, 255, 255, 0);    // Subnet Mask
IPAddress dns_addr(8, 8, 8, 8);    // DNS Address

EthernetServer server(SOCK_PORT);    // 소켓 생성할 때 80(Http)번 포트로 생성

byte linefeed_cnt;

void setup()
{
  Ethernet.init(17);
  Serial.begin(9600);      // Serial통신 baudrate 9600로 시작
  pinMode(PIN_LED_R, OUTPUT);

  pinMode(PIN_LIGHT, INPUT); 
  
  while (!Serial);    //Serial 모니터를 열때까지 대기
  
      // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  
  if (Ethernet.begin(mac) == 0)    // DHCP서버에 IP할당 요청  
  {
    // IP할당에 실패하면 Static으로 설정 
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("Configure Static Network Information");
    // initialize the ethernet device not using DHCP:
    Ethernet.begin(mac, local_ip, dns_addr, gateway, subnet);
  }
  Serial.println("***** W5100S-EVB-Pico Loopback Server (LED Control) *****");
  Serial.print("Device IP address : ");
  Serial.print(Ethernet.localIP());    // IP주소 출력 
  Serial.print(" : ");    
  Serial.println(SOCK_PORT);    // Port 번호 출력 
  
  server.begin();    // 서버 Listen 시작 (Socket open -> Listen 상태 돌입)
}

void loop()
{ 
  EthernetClient client = server.available();    // 클라이언트가 서버에 연결되었다면 

  if(client.connected())
  {
    while(client.available())
    {
      char request_char = client.read();
      Serial.print(request_char);
      
      if(checkRequestEnd(request_char))
      {
        sendHTTPResponse();
        client.stop();
      }
    }
  }
}

bool checkRequestEnd(char request_ch)
{
      bool ret = 0;
      if(linefeed_cnt == 1)
      {
        if(request_ch == '\r') linefeed_cnt++;
        else linefeed_cnt = 0;
      }
      if(request_ch == '\n') linefeed_cnt++;

      if(linefeed_cnt >= 2)
      {
        linefeed_cnt = 0;
        ret = 1;
      }
      return ret;
}

void sendHTTPResponse(void)
{
  // HTTP Message
  server.println("HTTP/1.1 200 OK");
  server.println("Content-Type: text/html");
  server.println("Connection: close");  // the connection will be closed after completion of the response
  server.println("Refresh: 5");  // refresh the page automatically every 5 sec
  server.println();
  // HTML Message
  server.println("<!DOCTYPE HTML>");
  server.println("<html>");
  server.println("<head>");
  server.println("<title>W5100S-EVB-Pico WebServer (Post Data)</title>");
  server.println("</head>");

  server.println("<body>");
  server.println("<br /> ***  W5100S-EVB-Pico WebServer (Display Sensors Value)  ***  <br /><br />");
  
  server.print("CDS value     : ");
  server.print(analogRead(PIN_LIGHT));
  server.println("&nbsp(0~1023 value)<br />");
  
  server.println("</body>");
  server.println("</html>");
  // HTML Message Done
}
