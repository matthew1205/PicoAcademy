#include <SPI.h>
#include <Ethernet.h> 

#define SOCK_PORT        5000           //PORT

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };    // MAC ADDRESS.

IPAddress local_ip(192,168, 11, 177);    // IP Address
IPAddress gateway(192,168, 11, 1);    // Gateway Address
IPAddress subnet(255, 255, 255, 0);    // Subnet Mask
IPAddress dns_addr(8, 8, 8, 8);    // DNS Address

EthernetServer server(SOCK_PORT);    // 소켓 생성할 때 80(Http)번 포트로 생성

void setup()
{
  Ethernet.init(17);       // CS Pin
  Serial.begin(9600);      // Serial통신 baudrate 9600로 시작

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
      char thisChar = client.read();
      server.write(thisChar);
      Serial.print(thisChar);
    }
  }
}
