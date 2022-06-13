#include <ESP8266WiFi.h>
#include "web.h"
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

#define RELAY1 D1 //D0
#define RELAY2 D2 //D1
#define LED1 2
#define NUT1 D0 //D2
#define NUT2 D3
#define NUT3 D5
float nhietdo=0, doam=0;
unsigned long TTTB1=0, TTTB2=0, bienEF=0, bienGH=0, bienJK=0;
const char* ssid = "Keobongmay";
const char* password = "123123123123";
String chuoiSendWebJson ="";
unsigned long last;
void sendWebPage(){
  server.send(200, "text/html", webpage);
}
void DataJson(String sensor1, String sensor2, String sensor3, String sensor4, String sensor5, String sensor6, String sensor7){
  chuoiSendWebJson = "{\"ND\":\""+ String(sensor1) + "\","+
                      "\"DA\":\""+ String(sensor2) + "\"," +
                      "\"TB1\":\""+ String(sensor3) + "\"," +
                      "\"TB2\":\""+ String(sensor4) + "\"," +
                      "\"EF\":\""+ String(sensor5) + "\"," +
                      "\"GH\":\""+ String(sensor6) + "\"," +
                      "\"JK\":\""+ String(sensor7) + "\"}";
}
void sendUpDate(){
  DataJson(String(nhietdo), String(doam), String(TTTB1), String(TTTB2), String(bienEF), String(bienGH), String(bienJK));
  server.send(200, "text/html", String(chuoiSendWebJson));
}
void chuongTrinhCamBien()
{
  nhietdo = nhietdo + 1.1;
  doam= doam + 1.2;
  if (nhietdo>= 151)
  {
    nhietdo = 0;
  }
  if(doam >=101)
  {
    doam=0;
  }
}

void duyTri()
{
  server.handleClient();
  if(millis()-last >=0)
  {
    chuongTrinhCamBien();
    last = millis();
  }
}
void button()
{
  // Nut 2
  if (digitalRead(NUT1) ==0)
  {
    Serial.println("Nut 1 da duoc nhan");
    delay(300);
    while(1)
    {
      duyTri();
      if(digitalRead(NUT1)==1)
      {
        if (TTTB1 ==1)
        {
          digitalWrite(RELAY1,LOW);
          TTTB1 = 0; 
        }
        else if  (TTTB1 == 0)
        {
          digitalWrite(RELAY1, HIGH);
          TTTB1 = 1;
        }
        last = millis();
        break;
      }
    }
  }

  // Nut 2
  if (digitalRead(NUT2) ==0)
  {
    Serial.println("Nut 2 da duoc nhan");
    delay(300);
    while(1)
    {
      duyTri();
      if(digitalRead(NUT2)==1)
      {
        if (TTTB2 ==1)
        {
          digitalWrite(RELAY2,LOW);
          TTTB2 = 0; 
        }
        else if  (TTTB2 == 0)
        {
          digitalWrite(RELAY2, HIGH);
          TTTB2 = 1;
        }
        last = millis();
        break;
      }
    }
  }
  
  
}
void connectWifi();
void onclickButton();
void xyLyOnOff(String button);
void sendInput();
void xyLyData(String input);

void setup() {
  // Thiết lập truyền dữ liệu nối tiếp ở tốc độ 115200 bits/s
  Serial.begin(115200);
  while(!Serial);
  
  delay(10);
 
  last = millis();
  pinMode(LED1, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(NUT1, INPUT_PULLUP);
  pinMode(NUT2, INPUT_PULLUP);
  pinMode(NUT3, INPUT_PULLUP);

  digitalWrite(RELAY1,LOW);
  digitalWrite(RELAY2, LOW);
  
  Serial.println();
  delay(5000);

  connectWifi();
 
  server.on("/",[]{
    sendWebPage();
  });
  server.on("/Update",[]{
    sendUpDate();
  });
  server.on("/Button",[]{
    onclickButton();
  });
  server.on("/Input",[]{
    sendInput();
  });
}
void loop() {
  server.handleClient(); // duy trì server
  button();
  if(millis()-last >=0)
  {
    chuongTrinhCamBien();
    last = millis();
  }
}
void connectWifi()
{
  Serial.println();
  Serial.print("Connecting to ");

  // In ra tên mạng wifi sẽ kết nối đến
  Serial.println(ssid);
  // Thiết lập ESP8266 ở chế độ station và kết nối đến mạng wifi đã chỉ định
  WiFi.begin(ssid, password);
  // Đoạn code in ra dấu . nếu ESP8266 chưa được kết nối
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // In ra dòng "WiFi connected" và địa chỉ IP của ESP8266
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("Server start");
}
void onclickButton()
{
  Serial.println(">>>onclick Button");
  Serial.println(server.arg("dataOfButton"));
  String button12 = "";
  button12 = server.arg("dataOfButton");
  xyLyOnOff(String(button12));
  server.send(200, "text/html", webpage);
  
}
void xyLyOnOff(String button)
{
  if (button.indexOf("AOB") >= 0)
  {
    Serial.println("Off Den");
    digitalWrite(RELAY1,LOW);
    TTTB1 = 0;
  }
  else if(button.indexOf("A1B") >= 0)
  {
    Serial.println("On Den");
    digitalWrite(RELAY1,HIGH);
    TTTB1 = 1;
  }
  if (button.indexOf("COD") >= 0)
  {
    Serial.println("Off Quat");
    digitalWrite(RELAY2,LOW);
    TTTB2 = 0;
  }
  else if(button.indexOf("C1D") >= 0)
  {
    Serial.println("On Quat");
    digitalWrite(RELAY2,HIGH);
    TTTB2 = 1;
  }
  
}
void sendInput()
{
  Serial.println(">>>Send input ");
  Serial.println(server.arg("_input"));
  String input = "";
  input = server.arg("_input");
  xyLyData(input);
  server.send(200, "text/html", webpage);
  
}
void xyLyData(String input)
{
  int timE, timG, timF, timH;
  timE = input.indexOf("E");
  timF = input.indexOf("F");
  timG = input.indexOf("G");
  timH = input.indexOf("H");
  if (timE >=0 && timF >=0)
  {
    String dataEF= "";
    dataEF = input.substring(timE+1, timF);
    bienEF =dataEF.toInt();
  }
  if (timG >=0 && timH >=0)
  {
    String dataGH= "";
    dataGH = input.substring(timG+1, timH);
    bienGH =dataGH.toInt();
  }
}
