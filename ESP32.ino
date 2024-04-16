#include <WiFi.h>

const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";

WiFiServer server(12345);

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); // Suponiendo que el LED está en el pin 2

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a la red WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Escucha a los clientes entrantes

  if (client) {
    Serial.println("Nuevo cliente");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == 'Alerta de movimiento') {
          digitalWrite(2, HIGH); // Encender LED
          delay(1000);           // LED encendido por 1 segundo
          digitalWrite(2, LOW);  // Apagar LED
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
