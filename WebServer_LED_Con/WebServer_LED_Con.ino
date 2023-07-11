/**
 * @project WizArduino WebServer (LED control)
 * @brief   LED control using Ethernet (Web Server)
 * @detail  refer to http://wiznetacademy.com
 * @author  Kei (http://openmaker.tistory.com/)
 * @date    2017-03-21
 * @version 0.0.2
 * @MCU     ARM Cortex-M0+ (Arduino M0)
 */

#include <SPI.h>
#include <Ethernet.h>    // W5500부터 Ethernet2 라이브러리를 사용.
// http://www.arduino.org/learning/reference/Ethernet-two-Library 참조

#define SOCK_PORT        80
#define PIN_LED_R        12
#define PIN_LED_B        13
#define PIN_LED_G        14

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };    // MAC ADDRESS.

IPAddress local_ip(192,168, 11, 177);    // IP Address
IPAddress gateway(192,168, 11, 1);    // Gateway Address
IPAddress subnet(255, 255, 255, 0);    // Subnet Mask
IPAddress dns_addr(8, 8, 8, 8);    // DNS Address

// TCP/IP 주소 지정 및 서브넷 구성 기본 사항의 이해 - MS공식 Site 
// https://support.microsoft.com/ko-kr/kb/164015

EthernetServer server(SOCK_PORT);    // 소켓 생성할 때 80(Http)번 포트로 생성
// 잘 알려진 포트번호 정보 : https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers

byte linefeed_cnt;

const byte LED_pins[] = {PIN_LED_R, PIN_LED_B, PIN_LED_G};          // Arduino pins used as ouputs for LEDs
byte LED[sizeof(LED_pins)] = {0};        // the state of each LED


void setup()
{
  Ethernet.init(17);
  Serial.begin(9600);      // Serial통신 baudrate 9600로 시작
  pinMode(PIN_LED_B, OUTPUT);    
  pinMode(PIN_LED_R, OUTPUT);    
  
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

  Serial.println("***** W5100S-EVB-Pico WebServer (LED Control) *****");
  Serial.print("Device IP address : ");
  Serial.print(Ethernet.localIP());    // IP주소 출력 
  Serial.print(" : ");    
  Serial.println(SOCK_PORT);    // Port 번호 출력 
  
  server.begin();    // 서버 Listen 시작 (Socket open -> Listen 상태 돌입)
}


void loop()
{ 
  EthernetClient client = server.available();    // 클라이언트가 서버에 연결되었다면 
  
  if (client.connected()) 
  { 
    String http_request = "";
    while (client.available())
    {
       char request_char = client.read(); // 데이터를 1byte 읽어서 request_char 변수에 저장
       http_request += request_char;
       Serial.print(request_char);   // 수신한 데이터를 1byte씩 시리얼 메세지로 출력

      if(checkRequestEnd(request_char))
      {  
        parsingHTTPRequest(http_request);              // get state of LED checkboxes from web page
        sendHTTPResponse();     // display checkboxes on web & write to LEDs
        http_request = "";
        client.stop();
      }
    }
  }

}


// get the state of the LED checkboxes from the HTTP request
void parsingHTTPRequest(String t_req)
{
    for (byte led_num = 0; led_num < sizeof(LED_pins); led_num++)
    {
        if ((t_req.charAt(9) == (LED_pins[led_num]-11 + '0')) &&
                    (t_req.charAt(16) == (LED_pins[led_num]-11 + '0')))
        {  // LED box is checked
            Serial.println("Box checked");
            LED[led_num] = 1;
        }
        else if (t_req.charAt(9) == (LED_pins[led_num]-11 + '0')) 
        {   // LED box is unchecked
            Serial.println("Box unchecked");
            LED[led_num] = 0;
        }
    }
}


bool checkRequestEnd(char request_ch)
{
      bool ret = 0;
      if(linefeed_cnt == 1){
        if((request_ch == '\r')||(request_ch == '\n')) ;
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


// write the HTML that includes the state of the LED checkboxes for displaying on the web browser
void sendHTTPResponse()
{
    // respond to HTTP request
    server.println("HTTP/1.1 200 OK");
    server.println("Content-Type: text/html");
    server.println();
  
    // some CSS in the HTML to change colours and position of the box containing the LED checkboxes
    server.print("<div style=\"background:white; color:green; position: absolute; margin:20px; border: grey solid 2px; padding:0 10px 4px 10px;\">");
    server.print("<h1 style=\"font: normal 20px/150% Verdana, Arial, Sans-serif;\">LED Outputs</h1>");
    
    // send each LED checkbox in turn
    for (byte led_num = 0; led_num < sizeof(LED_pins); led_num++)
    {
        // use hidden checkbox so that unchecking a checkbox sends a value to the Arduino
        // if only a normal checkbox is used, nothing is sent when it is unchecked
        // both hidden and normal checkboxes are produced here for each LED
        server.print("<form> <input type=\"hidden\" name=\"LED");
        server.print(LED_pins[led_num]-11, DEC);
        server.print("\" value=\"0\"> <input type=\"checkbox\" name=\"LED");
        server.print(LED_pins[led_num]-11, DEC);
        server.print("\" value=\"");
        server.print(LED_pins[led_num]-11, DEC);
        server.print("\"");
        
        // write to the LED connected to the Arduino board
        if (LED[led_num])
        {
            server.print(" checked ");             // part of HTML if checkbox is to be displayed checked
            digitalWrite(LED_pins[led_num], HIGH);  // switch LED on
        }
        else
        {
            digitalWrite(LED_pins[led_num], LOW);  // switch LED off
        }
        server.print(" onclick=\"submit();\">LED");
        if(LED_pins[led_num] == PIN_LED_B)      server.print(" BLUE");   // 버튼 이름
        else if(LED_pins[led_num] == PIN_LED_R)      server.print(" RED");    
        else if(LED_pins[led_num] == PIN_LED_G)      server.print(" GREEN");   
        server.print(" </form>");                 // end of HTML for 1 LED's form
    }
    server.print("</div>");                       // end of box containing LEDs
}
