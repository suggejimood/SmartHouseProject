//-----------------------------------------
//Arduino DHT11 Web Server using AJAX
//HTML code of webpage is stored on SD card
//-----------------------------------------
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <DHT.h>
//------------------------------------------------
DHT dht(23, DHT11);
File HMTL_file;
//------------------------------------------------
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x4A, 0xE0};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);
//==================================================================================
void setup()
{
  Serial.begin(9600);
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
            client.println(dht.readTemperature());
            client.println("°C");
            
          }
          else if(HTTP_req.indexOf("readHum")>-1){
            client.println(int(dht.readHumidity()));
            client.println("&nbsp;%");
          }
          else if(HTTP_req.indexOf("led1ON")>-1){
            Serial.println("Led 1 on");
          }
          else if(HTTP_req.indexOf("led1OFF")>-1){
            Serial.println("Led 1 off");
          }
          else if(HTTP_req.indexOf("led2ON")>-1){
            Serial.println("Led 2 on");
          }
          else if(HTTP_req.indexOf("led2OFF")>-1){
            Serial.println("Led 2 off");
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
