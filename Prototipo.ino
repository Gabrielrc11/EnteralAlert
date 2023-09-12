#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "SEU_SSID";     // SSID da rede Wi-Fi
const char *password = "SUA_SENHA"; // Senha da rede Wi-Fi

const char *serverIP = "IP_DO_SERVIDOR"; // IP do servidor web local
const int serverPort = 80; // Porta do servidor web (geralmente 80)

const int ldrPin = A0; // Pino analógico ao qual o LDR está conectado

WiFiClient client;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);

  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Define a rota para enviar dados ao servidor
  server.on("/enviar", HTTP_GET, [](AsyncWebServerRequest *request){
    int ldrValue = analogRead(ldrPin);
    String data = String(ldrValue);
    client.connect(serverIP, serverPort);
    client.print("GET /receber?valor=");
    client.print(data);
    client.println(" HTTP/1.1");
    client.println("Host: " + String(serverIP));
    client.println("Connection: close");
    client.println();
    request->send(200, "text/plain", "Dados enviados com sucesso!");
  });

  server.begin();
}

void loop() {
}
