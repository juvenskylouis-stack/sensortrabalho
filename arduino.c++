#include <WiFi.h> // Se usar ESP8266, mude para: #include <ESP8266WiFi.h>
#include <HTTPClient.h> // Se usar ESP8266, mude para: #include <ESP8266HTTPClient.h>

// Configurações do seu Wi-Fi
const char* ssid = "NOME_DA_SUA_REDE_WIFI";
const char* password = "SENHA_DO_SUA_REDE_WIFI";

// IP do computador onde o Node.js está rodando (Ex: 192.168.1.50)
// Não use "localhost" aqui, deve ser o IP real do PC na rede local
const char* serverUrl = "http://IP_DO_SEU_COMPUTADOR:3000/sensor";

const int sensorPin = 2; 
int ultimoEstado = -1;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);

  // Conectando ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!");
  Serial.print("IP do ESP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int valorSensor = digitalRead(sensorPin);

  // Envia apenas quando o status muda (evita sobrecarregar a rede e o banco)
  if (valorSensor != ultimoEstado) {
    String statusEnvio = (valorSensor == LOW) ? "PROXIMO" : "AFASTADO";

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Monta o corpo da requisição: status=PROXIMO ou status=AFASTADO
      String dadosPost = "status=" + statusEnvio;
      int httpResponseCode = http.POST(dadosPost);

      if (httpResponseCode > 0) {
        Serial.println("Dados enviados via Wi-Fi: " + statusEnvio);
      } else {
        Serial.print("Erro no envio HTTP: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("Erro: Wi-Fi desconectado.");
    }
    
    ultimoEstado = valorSensor;
  }
  
  delay(200); 
}