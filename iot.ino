#include <WiFi.h>
#include "secrets.h"

#include <Firebase.h>

Firebase fb(REFERENCE_URL);

// Definir pines
const int trigPin = 12;    // Pin para el Trigger del sensor ultrasónico
const int echoPin = 13;    // Pin para el Echo del sensor ultrasónico
const int potPin1 = 32;    // Pin para el primer potenciómetro
const int potPin2 = 33;    // Pin para el segundo potenciómetro


void setup() {

  Serial.begin(115200);

  #if !defined(ESP32)
    WiFi.mode(WIFI_STA);
  #else
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  #endif
  WiFi.disconnect();
  delay(1000);
 
  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println();

  #if defined(ESP32)
      digitalWrite(LED_BUILTIN, HIGH); 
  #endif



  // Configurar pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configuración de los pines analógicos para los potenciometros
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);


}
float distanceToVolume(float distance) {
  float m = -32.24;   // Pendiente
  float b = 248.22;   // Intersección
  float volume = m * distance + b;

  // Asegurarnos de que el volumen no sea negativo
  if (volume < 0) {
    volume = 0;
  }
  return volume;
}

void loop() {
   // Leer valores de los potenciómetros
  int potValue1 = analogRead(potPin1);
  int potValue2 = analogRead(potPin2);

  // Leer distancia del sensor ultrasónico
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Medir duración del pulso
  duration = pulseIn(echoPin, HIGH);

  // Convertir duración a distancia en cm
  distance = (duration * 0.0343) / 2;
  float volume = distanceToVolume(distance);

  // Mostrar valores en Serial Monitor
  Serial.print("Potenciómetro 1: ");
  Serial.println(potValue1);
  Serial.print("Potenciómetro 2: ");
  Serial.println(potValue2);
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Volumen: ");
  Serial.print(volume);
  Serial.println(" ml");

  // Enviar datos a Firebase
  fb.setInt("sensors/1/pot1", potValue1);
  fb.setInt("sensors/2/pot2", potValue2);
  fb.setFloat("sensors/waterVolume", volume);

  // Esperar 1 segundo antes de la siguiente lectura
  delay(1000);
}
