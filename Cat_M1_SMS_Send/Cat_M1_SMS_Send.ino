/*
    This example demon
    Written by Matthew Yu, July 2023.
    Copyright 2023 by Matthew Yu
    MIT license, check LICENSE for more information  
 */

#include "Arduino.h"
#include "at_cmd_parser.h"

#define RESP_OK                            "OK\r\n"
#define RET_OK                             1
#define RET_NOK                            -1
#define DEBUG_ENABLE                       1
#define DEBUG_DISABLE                      0
#define CATM1_DEVICE_DEBUG                 DEBUG_ENABLE

#define USIM_STATE_ERROR                   -3

#define SMS_EOF                            0x1A          //Ctrl + 'Z'
#define MAX_SMS_SIZE                       100

char phone_number[] = "010xxxxxxxx";
char send_message[] = "Hello World";

ATCmdParser m_parser = ATCmdParser(&Serial2);

int Modem_Status = 0;

void setup() {
  Serial.begin(115200);       // W5100S-EVB-Pico USB
  Serial2.begin(115200);      // W5100S-EVB-Pico to WD-N522S

  Modem_Status = Check_Modem_Init();
}

void loop() {
  SMS_recv();
/*
  if(SMS_send(phone_number, send_message) == RET_OK)
  {
    Serial.printf("SEND [OK]");
    delay(50000);
  }
  */
}

int Check_Modem_Init()
{
  char CEREG_Result[15] = {0,};

  m_parser.flush();
  m_parser.set_timeout(1000);

  for(int i=0; i<20; i++)
  {
    if(( m_parser.send(F("AT")) && m_parser.recv(F("OK\r\n")))) break;
    delay(500);    
  }
  delay(1000);
  delay(1000);

  m_parser.flush();

  for(int j=0; j<20; j++)
  {
    if(( m_parser.send(F("AT+CEREG?")) && m_parser.recv(F("+CEREG: 0,%[^\n]\n"), CEREG_Result) && m_parser.recv(F("OK\r\n")) ))
    {
      Serial.printf("CEREG_Result = %s", CEREG_Result);

      if(CEREG_Result[0] == '1')
      {
        Serial.printf("Modem Init Fin\r\n");
        Serial.printf("USIM network is OK\r\n");
      }
      else if(CEREG_Result[0] == '2')
      {
        Serial.printf("Modem Init Fin\r\n");
        Serial.printf("USIM network is Limitted\r\n");
        return 2;        
      }
      else if(CEREG_Result[0] == '0')
      {
        Serial.printf("Modem Init Fin\r\n");
        Serial.printf("CEREG 0,0 please check network\r\n");
        return 0;
      }
      else 
      {
        Serial.printf("T");
      }
    }
    Serial.printf("*");
    delay(1000);
  }

  delay(1000);
  delay(1000);
  return USIM_STATE_ERROR;
}

int8_t SMS_send(char * da, char * msg)
{
  char szField0[50] = {0,};
  int8_t ret = RET_NOK;
  int msg_idx = 0;
  bool done = false;

  m_parser.set_timeout(1000);

  m_parser.send("AT+CMGS=\"%s\"",da);       //DA(Destination Phone number)
  if(m_parser.recv(">"))
  {
    m_parser.send(msg);
    m_parser.send("%c", SMS_EOF);
  }

  if( m_parser.recv("+CMGS: %d", &msg_idx) && m_parser.recv("OK"))
  {
    sprintf((char *)szField0, ">> SMS send success: index %d\r\n", msg_idx);
    ret = RET_OK;
  }  
  m_parser.set_timeout(1000);
  return ret;
}

void SMS_recv()
{
  char szField0[50] = {0,};
  char szField1[50] = {0,};
  
  char Phone_Number[20] = {0,};
/*
  String SMS_Command = "AT+CMGF=1";   //TEXT Mode
  String SMS_Recv_Command = "AT+CNMI=1,2,0,0,0";  //Auto Recieve Command (UTF-8 Code)
  String SMS_SKT_NEWMSG = "AT*SKT*NEWMSG=4098";  //Auto Recieve Command (3 times, ANSI Code)
*/
  m_parser.flush();

  m_parser.send(F("AT+CNUM"));
  
  while(1)
  {
    if(m_parser.recv(F("+CNUM:\"\",\"%[^\"]\""), Phone_Number) && m_parser.recv(F("OK\r\n")))
    break;
  }

  m_parser.flush();
  m_parser.set_timeout(1000);
  m_parser.send(F("AT+CMGF=1"));
  delay(50);
  m_parser.send(F("AT+CNMI=1,2,0,0,0"));
  delay(50);

  m_parser.flush();
  if(m_parser.recv(F("+CMT"))&& m_parser.recv(F("%[^\n]\n"), szField0) && m_parser.recv(F("%[^\n]\n"), szField1))
  {
    Serial.println(szField0);
    Serial.println(szField1);
  }  
}
