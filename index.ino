#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "SEU_SSID";     // SSID da rede Wi-Fi
const char *password = "SUA_SENHA"; // Senha da rede Wi-Fi

const int ldrPin = 36;      // Pino do LDR
const int buzzerPin = 4;   // Pino do Buzzer
const int ledCalibration = 2; // Led indicador de calibração

const int calibrationTime = 10000;  // Tempo de calibração em milissegundos
const int calibrationInterval = 1000;  // Intervalo de amostragem durante a calibração em milissegundos
const float detectionThreshold = 0.9;  // Limite de detecção em relação ao valor de referência

int ldrValue = 0;           // Valor lido do LDR
int threshold = 500;        // Limiar de luminosidade para acionar o buzzer
int referenceValue;         // Valor que o sistema vai comparar

const char *host = "IP_DO_SERVIDOR"; // IP do servidor web local
const int port = 80; // Porta do servidor web (geralmente 80)

WiFiClient client;
AsyncWebServer server(80);

void setup() {
  pinMode(ldrPin, INPUT);
  pinMode(ledCalibration, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Define a rota para exibir os dados do LDR
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>Leitura do LDR</h1>";
    html += "<p>Valor do LDR: " + String(ldrValue) + "</p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.begin();
  calibrateSystem(); // Realiza a calibração ao iniciar
}

void loop() {
  checkSystemState();
  digitalWrite(ledCalibration, HIGH);
  delay(1000); // Intervalo entre leituras
}

void calibrateSystem() {
  Serial.println("Calibrating...");
  digitalWrite(ledCalibration, HIGH);

  // Inicializar variáveis
  long startTime = millis();
  int totalSamples = 0;
  long calibrationEndTime = startTime + calibrationTime;

  // Realizar aquisição de amostras durante o tempo de calibração
  while (millis() < calibrationEndTime) {
    ldrValue += analogRead(ldrPin);
    totalSamples++;
    delay(calibrationInterval);
  } 

  // Calcular valor de referência
  referenceValue = ldrValue / (float)totalSamples;
  Serial.print("Calibração Completa. Valor de referência: ");
  Serial.println(referenceValue);

  // Reinicializar variáveis para próxima calibração
  ldrValue = 0;
}

void checkSystemState() {
  // Fazer a leitura atual do LDR
  int currentValue = analogRead(ldrPin);

  // Comparar com o valor de referência
  if (currentValue < referenceValue * detectionThreshold) {
    // Ativar o alarme
    Serial.println("Food running low! Take necessary measures.");
    // TODO: Acionar o alarme, por exemplo, acender um LED ou emitir um som
    digitalWrite(buzzerPin, HIGH); // Ativa o Buzzer
  } else {
    digitalWrite(buzzerPin, LOW);  // Desativa o Buzzer
  }
}
