#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecureBearSSL.h>

TFT_eSPI tft = TFT_eSPI();

#define TFT_GREY 0x5AEB 
const char *ssid         = ""; //Your Wifi SSID
const char *password     = ""; //Your Wifi Password
String payload = "";


void connect() {
  tft.setCursor(70, 20, 2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.print("Connecting to Wifi ");
  WiFi.begin ( ssid, password );
  while ( WiFi.status() != WL_CONNECTED ) {
    tft.print(".");
    delay (500);
  }
  tft.print(" Connnected!");
  delay(700);

}

void reset_sc(){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.setTextFont(2);
}


void read_price(int x_i,int y_i,String stock_name){
  float previousClosePrice;
    float currentPrice;
    float differenceInPrice;
    String  httpRequestAddress; 
    BearSSL::WiFiClientSecure client;
    HTTPClient http; //Declare an object of class HTTPClient
    httpRequestAddress = "https://finnhub.io/api/v1/quote?symbol="+stock_name+"&token=c1tjb52ad3ia4h4uee9g"; 
    client.setInsecure();

    http.begin(client,httpRequestAddress); //Specify request destination
    DynamicJsonBuffer doc;
    int httpCode = http.GET(); //Send the request
    if (httpCode > 0)
    { //Check the returning code

      String payload = http.getString(); //Get the request response payload
      Serial.println(payload);           //Print the response payload
      JsonObject& obj = doc.parseObject(payload);
      
      previousClosePrice = obj[String("pc")];
      currentPrice = obj[String("c")];
      
      float diff=currentPrice-previousClosePrice;
      differenceInPrice = ((currentPrice-previousClosePrice)/previousClosePrice)*100.0;
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(25+x_i,10+y_i,4);
      tft.print(stock_name); 
      
       
       if(differenceInPrice<0.0){
        tft.setTextColor(TFT_RED);
        tft.setCursor(25+x_i, 45+y_i, 4);
        tft.drawFloat(currentPrice, 2, 25+x_i, 45+y_i, 4); 
        tft.setCursor(5+x_i,82+y_i, 3);
        tft.fillTriangle(5+x_i,95+y_i,20+x_i,95+y_i,12+x_i,110+y_i,TFT_RED);
        tft.drawFloat(differenceInPrice , 2, 25+x_i,82+y_i, 2);
        tft.setCursor(65+x_i,82+y_i, 2);
        tft.print("% ("); 
        tft.drawFloat(diff , 2, 90+x_i,82+y_i, 2);
        tft.setCursor(135+x_i,82+y_i, 2);
        tft.print(")"); 
       }
       else{
          tft.setTextColor(TFT_GREEN);
          tft.setCursor(25+x_i, 45+y_i, 4);
          tft.drawFloat(currentPrice, 2, 25+x_i, 45+y_i, 4); 
          tft.setCursor(5+x_i,82+y_i,2);
          tft.fillTriangle(5+x_i,105+y_i,20+x_i,105+y_i,12+x_i,90+y_i,TFT_GREEN);
          tft.drawFloat(differenceInPrice , 2, 25+x_i,82+y_i, 2);
          tft.setCursor(65+x_i,82+y_i, 2);
          tft.print("% ("); 
          tft.drawFloat(diff , 2, 90+x_i,82+y_i, 2);
          tft.setCursor(135+x_i,82+y_i, 2);
          tft.print(")"); 
       }
       
    }
    http.end(); //Close connection
  
}

void main_page(){
  tft.setCursor(20, 75, 4);
  tft.fillRect(0, 0, 320, 240, TFT_WHITE);
  tft.setTextColor(TFT_BLACK); 
  tft.println("24x7 Stock Prices Tracker");
}

void stock_page(String s1,String s2,String s3,String s4){
  read_price(0,0,s1);
  read_price(160,0,s2);
  read_price(0,120,s3);
  read_price(160,120,s4);
  tft.fillTriangle(305,100,305,140,319,120,TFT_WHITE);
}

void setup(void) {
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3); // horizontal display
  main_page();
  delay(1000);
  reset_sc();
  connect();  // call connect to wifi
}

void loop() {
  reset_sc();
  stock_page("AAPL","AMZN","TSLA","MSFT");
  delay(3000);
  reset_sc();
  stock_page("PFE","OXY","EBAY","FDX");
  delay(3000);
}
