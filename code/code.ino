#include <HCSR04.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 termometroIR = Adafruit_MLX90614();
//HCSR04 hc(13, new int[2]{2,3}, 2); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
HCSR04 hc1(12, 2);
HCSR04 hc2(13, 3);
void setup()
{ Serial.begin(9600); 
  // Iniciar comunicación serie
  Serial.begin(9600);
 
  // Iniciar termómetro infrarrojo con Arduino
  termometroIR.begin();}

void loop()
{
  Serial.print("-------------------------\n");
   Serial.println(hc1.dist());
    Serial.println(hc2.dist());
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

  // Si quieres mostrar la información en grados Fahrenheit utiliza las funciones
  // readAmbientTempF() para temperatura ambiente
  // readObjectTempF() para temperatura del objeto

    delay(5000);                        // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.


}
