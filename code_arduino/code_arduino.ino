#include <HCSR04.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_Fingerprint.h>


/*
Conexion:
Proximidad:
HC1 -> Nivel alcohol Gel //este no lo usaremos en esta ocacion
HC2 -> Para tomar alcohol Gel
5V
HC1 TRG=12 ECHO=13
HC2 TRG=5 ECHO=11
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
-------------
agregar persona
Boton 5v, Pin 5
Led pin 10
*/

SoftwareSerial NodeMCU(2,3); //pin 2 y 3, comunicacion serial con el nodeMcu, verificar si hay que cambiar los pines

Adafruit_MLX90614 termometroIR = Adafruit_MLX90614();

//HCSR04 hc(13, new int[2]{2,3}, 2); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
//HCSR04 hc1(11, 5);
HCSR04 hc2(12, 13);

const int ledrojo = 6;
const int ledverde = 7;
const int ledadd = 10;
const int buttonPin  = 5;



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
uint8_t id;
//SERIALMCU

int Reset = 11;

void setup(){ 

  digitalWrite(Reset, HIGH);
  delay(200); 
  pinMode(Reset, OUTPUT);  
  
  // Iniciar comunicación serie
  Serial.begin(9600); //comunicacion del
  
  NodeMCU.begin(9600); //comunicacion del nodemcu
 
  // Iniciar termómetro infrarrojo con Arduino
  termometroIR.begin();

  //Iniciar pinMode rele
  pinMode(rele,OUTPUT);

  //leds
  pinMode(ledrojo,OUTPUT);
  pinMode(ledverde,OUTPUT);
  pinMode(ledadd,OUTPUT);
  pinMode(buttonPin, INPUT);

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
  //float distancia_1 = hc1.dist();
  float distancia_2 = hc2.dist(); // para sacar alcohol
  
  //Serial.println("hc1: " + String(distancia_1,3));
  Serial.println("hc2: " + String(distancia_2,3));

  if(distancia_2 < 5.0 ){
    digitalWrite(rele, HIGH);
    delay(2000);
    digitalWrite(rele, LOW);
    delay(3000);
  }else{
    digitalWrite(rele, LOW);
   }

   /*if(distancia_1 < 3.0){
    Serial.println("Boton Apretado");
    }*/

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

  NodeMCU.println("t " + String(temperaturaObjeto));   //mandamos la temp de la persona al node mcu, que el node en el code lea la letra t
  }

void leer_huella(){
    getFingerprintID();
  }

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


void add_huella(){
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = finger.templateCount+1;
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  digitalWrite(ledadd,HIGH);
  while (!  getFingerprintEnroll() );
  digitalWrite(ledadd,LOW);
  delay(500);
  digitalWrite(ledadd,HIGH);
  delay(500);
  digitalWrite(ledadd,LOW);
  //digitalWrite(Reset, LOW);
  };



bool button_debounce = false;

void loop(){
  distancias();
  leer_huella();

  if (digitalRead(buttonPin) == LOW && button_debounce == false) {
        Serial.println("Boton Apretado");
        button_debounce = true; // toggle button debounce flag
        digitalWrite(ledadd,HIGH);
        delay(500);
        digitalWrite(ledadd,LOW);
        delay(500);
        digitalWrite(ledadd,HIGH);
        delay(500);
        digitalWrite(ledadd,LOW);
        add_huella();
    }

    if (digitalRead(buttonPin) == HIGH && button_debounce == true) { // this will prevent subsequent presses
        button_debounce = false; 
    }

    


    
  //temperatura_persona();
  
  //delay(1000);                        // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.


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
  Serial.print("olu");
  
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
  Serial.print("oli");
  
  return finger.fingerID;
}

//add finger

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
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

  Serial.println("Remove finger");

  digitalWrite(ledadd,HIGH);
  delay(500);
  digitalWrite(ledadd,LOW);
  delay(500);
  digitalWrite(ledadd,HIGH);
  delay(500);
  digitalWrite(ledadd,LOW);
  delay(500);
  digitalWrite(ledadd,HIGH);
  delay(500);
  digitalWrite(ledadd,LOW);
  
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
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
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
