//EnteralAlert APP
//Criado por:
//Gabriel Henrique Rocha de Carvalho
//Thiago Rayzing França de Farias

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

char *ssid = "";     // SSID da rede Wi-Fi
char *password = ""; // Senha da rede Wi-Fi

const int ldrPin = 36;      // Pino do LDR
const int buzzerPin = 4;   // Pino do Buzzer
const int ledCalibration = 2; // Led indicador de calibração

const int calibrationTime = 10000;  // Tempo de calibração em milissegundos
const int calibrationInterval = 1000;  // Intervalo de amostragem durante a calibração em milissegundos
const float detectionThreshold = 0.8;  // Limite de detecção em relação ao valor de referência

int ldrValue = 0;           // Valor lido do LDR
int threshold = 500;        // Limiar de luminosidade para acionar o buzzer
int referenceValue;         // Valor que o sistema vai comparar

const char *host = "10.77.14.242"; // IP do servidor web local
const int port = 80; // Porta do servidor web (geralmente 80)

WiFiClient client;
AsyncWebServer server(80);

bool alarmeAtivado = false;

void setup() {
  pinMode(ldrPin, INPUT);
  pinMode(ledCalibration, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  initialy();

  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.println(WiFi.localIP());

  // Define a rota para exibir os dados do LDR
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head>";
    html += "<title>EnteralAPP</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; background-color: #f0f0f0; }";
    html += "h1 { color: #333; text-align: center; }";
    html += ".container { max-width: 400px; margin: 0 auto; padding: 20px;";
    html += "background-color: #fff; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.2); }";
    html += "p { font-size: 24px; text-align: center; color: #555; }";
    html += "</style></head><body>";
    html += "<div class=\"container\">";
    html += "<h1>EnteralAPP</h1>";
    html += "<p>Status: Conectado</p>";
    html += "<p>Referencia de calibracao: " + String(referenceValue) + "</p>";

    // Adicione este script JavaScript para mostrar um alerta se o alarme estiver ativado
    html += "<script>";
    html += "setInterval(function() {";
    html += "  location.reload();"; // Atualiza a página a cada 5 segundos
    html += "}, 2000);";

    // Adicione este código para mostrar um alerta se o alarme estiver ativado
    html += "if (" + String(alarmeAtivado) + ") {";
    html += "  alert('O alimento acabou! Tome as medidas necessarias.');";
    html += "}";

    html += "</script>";

    request->send(200, "text/html", html);
  });

  server.begin();
  calibrateSystem(); // Realiza a calibração ao iniciar
}

void loop() {
  checkSystemState();
  calibrationCompleted();
  digitalWrite(ledCalibration, HIGH);
}

void calibrateSystem() {
  Serial.println("Calibrando...");
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

void initialy(){
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  }

void calibrationCompleted() {
  for (int repetition = 0; repetition < 3; repetition++){
  digitalWrite(ledCalibration, LOW);
  delay(500);
  digitalWrite(ledCalibration, HIGH);
  delay(500);
  }
}

void checkSystemState() {
  // Fazer a leitura atual do LDR
  int currentValue = analogRead(ldrPin);

  // Comparar com o valor de referência
  if (currentValue < referenceValue * detectionThreshold) {
    // Ativar o alarme
    Serial.println("O alimento acabou! Tome as medidas necessarias.");
    alarmeAtivado = true; // Ativa o alarme
    digitalWrite(buzzerPin, HIGH); // Ativa o Buzzer
  } else {
    alarmeAtivado = false; // Desativa o alarme
    digitalWrite(buzzerPin, LOW);  // Desativa o Buzzer
  }
}