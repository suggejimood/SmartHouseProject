#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <dht.h>
#include <Servo.h>
//------------------------------------------------
dht DHT;
#define DHT11_PIN 7

File HMTL_file;
Servo bedroom;
Servo livingroom;

int bed = 9;
int liv = 10;
int led1 = 31;
int led2 = 33;
//------------------------------------------------
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x4A, 0xE0};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);
//==================================================================================
void setup()
{
  Serial.begin(9600);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  bedroom.attach(9);
  livingroom.attach(10);
  
  dht.begin();
  pinMode(53, OUTPUT);
  //-----------------------------------------------------
  Serial.println(F("Initializing SD card..."));
  if(!SD.begin(4))
  {
    Serial.println(F("Initialization failed!")); return;
  }
  Serial.println(F("Initialization OK"));
  //-----------------------------------------------------
  if(!SD.exists("index.htm"))
  {
    Serial.println(F("index.htm not found!")); return;
  }
  Serial.println(F("index.htm found"));
  //-----------------------------------------------------
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print(F("Server Started...\nLocal IP: "));
  Serial.println(Ethernet.localIP());
}
//==================================================================================
void loop()
{
  String HTTP_req;
  int chk = DHT.read11(DHT11_PIN);
  EthernetClient client = server.available();
  //---------------------------------------------------------------------------
  if(client)
  {
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if(client.available())
      { 
        char c = client.read();
        HTTP_req += c;
        if(c == '\n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r");
          //-------------------------------------------------------------------
          if(HTTP_req.indexOf("readTemp")>-1)
          {
            client.println(DHT.temperature);
            client.println("°C");
            
          }
          else if(HTTP_req.indexOf("readHum")>-1){
            client.println(int(DHT.humidity));
            client.println("&nbsp;%");
          }
          else if(HTTP_req.indexOf("led1ON")>-1){
            Serial.println("Led 1 on");
            digitalWrite(led1, HIGH);
          }
          else if(HTTP_req.indexOf("led1OFF")>-1){
            Serial.println("Led 1 off");
            digitalWrite(led1, LOW);
          }
          else if(HTTP_req.indexOf("led2ON")>-1){
            Serial.println("Led 2 on");
            digitalWrite(led2, HIGH);
          }
          else if(HTTP_req.indexOf("led2OFF")>-1){
            Serial.println("Led 2 off");
            digitalWrite(led2, LOW);
          }
          else if(HTTP_req.indexOf("bedroomOpen")>-1){
            Serial.println("bedroom on");
            bedroom.write(170);
            
          }
          else if(HTTP_req.indexOf("bedroomHalf")>-1){
            Serial.println("bedroom half");
            bedroom.write(90);
            
          }
          else if(HTTP_req.indexOf("bedroomClose")>-1){
            Serial.println("bedroom Close");
            bedroom.write(10);
            
          }
          else if(HTTP_req.indexOf("livingroomOpen")>-1){
            Serial.println("livingroom on");
            livingroom.write(170);
 
          }
          else if(HTTP_req.indexOf("livingroomHalf")>-1){
            Serial.println("livingroom Half");
            livingroom.write(90);
 
          }
          else if(HTTP_req.indexOf("livingroomClose")>-1){
            Serial.println("livingroom Close");
            livingroom.write(10);
 
          }
          else
          {
            HMTL_file = SD.open("index.htm");
            if(HMTL_file)
            {
              while(HMTL_file.available()) client.write(HMTL_file.read());
              HMTL_file.close();
            }
          }
          //-------------------------------------------------------------------
          //HTTP_req = "";
          break;
        }
        //---------------------------------------------------------------------
        if(c == '\n') currentLineIsBlank = true;
        else if(c != '\r') currentLineIsBlank = false;
      }
    }
    delay(10);
    client.stop();
  }
}
