#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#define DHTPIN 4
#define DHTTYPE DHT11

const char *ssid = "david12345";
const char *pass = "juan12345";
const char *host = "dweet.io";
const int port = 80;

DHT dht(DHTPIN, DHTTYPE); //Construyo un objeto de la clase DHT que representa el sensor

void setup() {
  Serial.begin(115200);
  Serial.println("Prueba del DHT11!");
  dht.begin();  //Inicializa el sensor
  WiFi.begin(ssid, pass); // Inicializar modulo WIFI y que se vincule a la red indicada en el ssid
  Serial.print("Intentando Conexion");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado");
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  pinMode(2, OUTPUT);    // Configurar pin 2 como salida
  digitalWrite(2, HIGH); // Encender Led
}

void loop() {
  delay(2000);
  float humedad = dht.readHumidity(); //Leo la humedad relativa
  float temperatura = dht.readTemperature();  //Leo la temperatura en grados celcius
  WiFiClient cliente; 

  if (!cliente.connect(host, port))
  {
    Serial.println("Conexion Fallida al servidor");
    delay(1000);
    return;
  }

  if(isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT11");
    return;
  }

  // hubo conexion con el servidor
  // flujo de transmision y otro de recepcion

  String request = String("GET ") +"/dweet/for/david?"+"Humedad=" + String(humedad) +"&"+"Temperature=" + String(temperatura) + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

  // Enviamos la peticion en  protocolo http al servidor
  cliente.print(request);
  unsigned long inicio = millis();
  while (cliente.available() == 0)
  {
    if (millis() - inicio > 8000)
    {
      Serial.println("Tiempo de espera del servidor agotado");
      cliente.stop();
      return;
    }
  }

  // se recibieron datos del servidor (cliente.available() !=0)
  while (cliente.available()){
    String linea = cliente.readStringUntil('\r'); // Lea un string hasta que encuentre el caracter
    Serial.println(linea);
  }

  Serial.println("Fin de conexion al servidor");
  cliente.stop();
  delay(5000);
}
