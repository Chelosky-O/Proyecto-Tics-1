#include <HCSR04.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_Fingerprint.h>

/*

Conexion:
Proximidad:
HC1 -> Nivel alcohol Gel
HC2 -> Para tomar alcohol Gel
5V
HC1 TRG=12 ECHO=13
HC2 TRG=10 ECHO=11
----------
RELE:
5V
PIN 4 <- IN
DC+ -> 5V
DC- -> GND
NO -> 5V
----------
Temperatura
3.3V
SCL=A4
SDA=A5
--------------
HUELLAS
3.3V
TX -> PIN 9 amarillo
RX -> PIN 8 blanco
-----------
led
6 rojo
7 verde






*/

Adafruit_MLX90614 termometroIR = Adafruit_MLX90614();

//HCSR04 hc(13, new int[2]{2,3}, 2); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
HCSR04 hc1(10, 11);
HCSR04 hc2(12, 13);

const int ledrojo = 6;
const int ledverde = 7;

const int rele = 4;

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(9, 8);
#else 
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup(){ 
  // Iniciar comunicación serie
  Serial.begin(9600);
 
  // Iniciar termómetro infrarrojo con Arduino
  termometroIR.begin();

  //Iniciar pinMode rele
  pinMode(rele,OUTPUT);

  //leds
  pinMode(ledrojo,OUTPUT);
  pinMode(ledverde,OUTPUT);

  //huellas
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }


  
}

void distancias(){
  Serial.print("-------------------------\n");
  float distancia_1 = hc1.dist();
  float distancia_2 = hc2.dist(); // para sacar alcohol
  
  Serial.println("hc1: " + String(distancia_1,3));
  Serial.println("hc2: " + String(distancia_2,3));

  if(distancia_2 < 5.0 ){
    digitalWrite(rele, HIGH);
    delay(1000);
    digitalWrite(rele, LOW);
    delay(3000);
  }else{
    digitalWrite(rele, LOW);
   }

  }

void temperatura_persona(){
  
  // Obtener temperaturas grados Celsius
  float temperaturaAmbiente = termometroIR.readAmbientTempC();
  float temperaturaObjeto = termometroIR.readObjectTempC();
 
  // Mostrar información
  Serial.print("Temp. ambiente => ");
  Serial.print(temperaturaAmbiente);
  Serial.println("ºC");
 
  Serial.print("Temp. objeto => ");
  Serial.print(temperaturaObjeto);
  Serial.println("ºC");
  }

void leer_huella(){
    getFingerprintID();
  }


void loop(){
  distancias();
  leer_huella();
  //temperatura_persona();
  
  delay(1000);                        // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.


}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
      //encender led verde
      digitalWrite(ledverde,HIGH);
      delay(1000);
      digitalWrite(ledverde,LOW);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    //encender led rojo
    digitalWrite(ledrojo,HIGH);
    delay(1000);
    digitalWrite(ledrojo,LOW);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  temperatura_persona();
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
