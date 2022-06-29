#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//const char* ssid = "Get away from here.";
//const char* password = "Pa206798637";

//wifi nacho sv nacho
const char* ssid = "Redmi Note 8 Pro";
const char* password = "12345678";


//String serverName = "http://192.168.101.54:80"; //cambiar si se cambia de compu donde esta el sv
String serverName = "http://192.168.220.54:80"; //cambiar si se cambia de compu donde esta el sv

//wifi nico
//const char* ssid = "Grumbly Wifi";
//const char* password = "holacomotas"

SoftwareSerial ArduinoUno(D2,D3);

String datos[] = {"0", "0"};

char data;

void setup()
{
  Serial.begin(115200);
  ArduinoUno.begin(9600);

  delay(30);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  //Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop()
{
  String palabra;
  
  if(ArduinoUno.available() > 0)
  
  {
    while(ArduinoUno.available() > 0)
    {

      data = (char)ArduinoUno.read();
      Serial.print(data);
      if(data == '\n') break;
      else palabra += data;
    }
    sendGET(palabra);
  }
  delay(250);
  
}
void sendGET(String dato)
{
  char prueba[50]; 
  
  dato.toCharArray(prueba, 50);
   if('t' == prueba[0])
   {
    datos[1] = String(0);
    datos[0] = String(0);
    datos[1] = dato.substring(3,8);
    
   }
if('h' == prueba[0])
   {
//http://192.168.78.54:80/pagina_web/pagina_tics_alcohol_gel/conexion_arduino.php?huella=1&temperatura=16.01
//http://192.168.78.54:80/pagina_web/pagina_tics_alcohol_gel/conexion_arduino.php?huella=3&temperatura=16.01  
    Serial.println(prueba[0]);
    datos[0] = dato.substring(2,6);
  }

if(datos[0] != "0" and datos[1] != "0"){
  Serial.println(datos[0]);
  Serial.println(datos[1]);


  String datos0 = datos[0];
  String datos1 = datos[1];
  WiFiClient client;
  HTTPClient http;

  //String serverPath = serverName + "/pagina_web/pagina_tics_alcohol_gel/conexion_arduino.php?huella=" + datos0 + "&temperatura=" + datos1; //Este era si funcionara el sensor de temperatura
  String serverPath = serverName + "/pagina_web/pagina_tics_alcohol_gel/conexion_arduino.php?huella=" + datos0 + "&temperatura=" + (String)36.4;
  Serial.println(serverPath.c_str());
    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }


  
}
