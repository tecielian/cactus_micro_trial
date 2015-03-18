#include <SoftwareSerial.h>

int sensor_temp = A2;
int value_temp;
int sensor_light = A1;
int value_light;
int sensor_water = A0;
int value_water;
int red = 9;
int green = 8;
int blue = 7;
int status;
int wifiConnected = 0;

#define DEBUG 1

#define _baudrate 9600
#define wifi_pin 13


#define _rxpin 11
#define _txpin 12

SoftwareSerial debug( _rxpin, _txpin );
#define SSID "xxxx"
#define PASS "xxxx"
#define IP "184.106.153.149"
String GET = "GET /update?key=9TYUWTFXTYC9RKT8";

void setup() {
  pinMode (13, OUTPUT);
  digitalWrite (13, HIGH);
  delay(1000);

  pinMode (red, OUTPUT);
  pinMode (blue, OUTPUT); 
  pinMode (green, OUTPUT); 
  Serial.begin( _baudrate );
  debug.begin( _baudrate );
  
  if(DEBUG) { // Please set DEBUG = 0 when USB not connected
    while(!Serial);
  }

  //set mode needed for new boards
  debug.println("AT+RST");
  delay(3000);//delay after mode change       
  debug.println("AT+CWMODE=1");
  delay(300);
  debug.println("AT+RST");
  delay(500);

}

void loop() {
  
  if(!wifiConnected) {
    debug.println("AT");
    delay(1000);
    if(debug.find("OK")){
      Serial.println("Module Test: OK");
      connectWifi();

      if (wifiConnected) {
          String cmd = "AT+CIPMUX=0";
          sendDebug( cmd );
          if( debug.find( "Error") )
          {
               Serial.print( "RECEIVED: Error" );
               return;
          }
      }
    } 
  }

  if(!wifiConnected) {
    delay(500);
    return;
  }

  status=0;

  value_temp = analogRead(sensor_temp);
  Serial.print("temperature");
  Serial.println( value_temp );
  value_light = analogRead(sensor_light);
  Serial.print("light");
  Serial.println( value_light );
  value_water = analogRead(sensor_water);
  Serial.print("water");
  Serial.println( value_water );
  
  String temp =String(value_temp);// turn integer to string
  String light= String(value_light);// turn integer to string
  String water=String(value_water);// turn integer to string
  
  updateTS(temp,light, water);
  delay(5000); //added delay here

  if (value_temp < 290){
  digitalWrite (red, LOW); // plant too cold - red led on
  delay(300);               // wait for a second
  digitalWrite(red, HIGH);    // turn the LED off by making the voltage LOW
  delay(300); 
status=1;
}else{
  digitalWrite (red, HIGH); // normal temp - red led off
}
  if (value_light < 300) {
  digitalWrite (blue, LOW); // not enough light - yellow led on
  digitalWrite (red, LOW);
  digitalWrite (green, LOW);
  delay(300);               // wait for a second
  digitalWrite(blue, HIGH);  // turn the LED off by making the voltage LOW
  digitalWrite (red, HIGH);
  digitalWrite (green, HIGH);
  delay(300); 
status=1;  
}else{
  digitalWrite (blue, HIGH); // enough light - yellow led off
}
  if (value_water < 300) {  
  digitalWrite (blue, LOW); // plant thirsty - blue led on
  delay(300);               // wait for a second
  digitalWrite(blue, HIGH);    // turn the LED off by making the voltage LOW
  delay(300); 
status=1;
}else{
  digitalWrite (blue, HIGH); // soil is moist - blue led off
}

if(status==0) {
  digitalWrite (green, LOW);
  delay(300);               // wait for a second
  digitalWrite(green, HIGH);    // turn the LED off by making the voltage LOW
  delay(300); 
 }else{
  digitalWrite (green, HIGH);
}

}

//----- update the Thingspeak string with 3 values
void updateTS( String T, String H , String W)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  sendDebug(cmd);
  delay(2000);
  if( debug.find( "Error" ) )
  {
    debug.print( "RECEIVED: Error\nExit1" );
    return;
  }

  cmd = GET + "&field1=" + T +"&field2=" + H +"&field3=" + W +"\r\n";
  debug.print( "AT+CIPSEND=" );
  debug.println( cmd.length() );

  //display command in serial monitor
  Serial.print("AT+CIPSEND=");
  Serial.println( cmd.length() );

  if(debug.find(">")) {
    // The line is useless
    //debug.print(">");
    debug.print(cmd);
    Serial.print(cmd);
    delay(1000);
    sendDebug( "AT+CIPCLOSE" );
  } else {
    sendDebug( "AT+CIPCLOSE" );
    return;
  }


  if( debug.find("OK") )
  {
    debug.println( "RECEIVED: OK" );
  }
  else
  {
    debug.println( "RECEIVED: Error\nExit2" );
  }
}

void sendDebug(String cmd)
{
  Serial.print("SEND: ");
  Serial.println(cmd);
  debug.println(cmd);
}

boolean connectWifi() {     
 String cmd="AT+CWJAP=\"";
 cmd+=SSID;
 cmd+="\",\"";
 cmd+=PASS;
 cmd+="\"";
 Serial.println(cmd);
 debug.println(cmd);
           
 for(int i = 0; i < 20; i++) {
   Serial.print(".");
   if(debug.find("OK"))
   {
     wifiConnected = 1;

     break;
   }
   
   delay(50);
 }
 
 Serial.println(
   wifiConnected ? 
   "OK, Connected to WiFi." :
   "Can not connect to the WiFi."
 );
 
 return wifiConnected;
}

