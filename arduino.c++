#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "NOME_DA_SUA_REDE_WIFI";
const char* password = "SENHA_DO_SUA_REDE_WIFI";
const char* serverUrl = "http://IP_DO_SEU_COMPUTADOR:3000/sensor";

const int sensorPin = 4;
int estadoAnterior = HIGH; // Começa em HIGH (considerando pull-up ou sem objeto)

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT); // Altere para INPUT_PULLUP se o sensor chavear no GND

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
}

void loop() {
  int estadoAtual = digitalRead(sensorPin);

  // Detecta o momento exato da transição (Objeto acabou de entrar na frente do sensor)
  // Se seu sensor ativa em HIGH, mude para: (estadoAtual == HIGH && estadoAnterior == LOW)
  if (estadoAtual == LOW && estadoAnterior == HIGH) {
   
    // Filtro anti-ruído rápido (debouncing)
    delay(40);
    if (digitalRead(sensorPin) == LOW) {
     
      Serial.println("Passagem detectada! Enviando ao banco...");

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.addHeader("Connection", "close");

        // Envia uma string de evento ou timestamp para registrar a passagem
        String dadosPost = "evento=PASSAGEM";
        int httpResponseCode = http.POST(dadosPost);

        if (httpResponseCode > 0) {
          Serial.println("Passagem registrada com sucesso no servidor.");
        } else {
          Serial.print("Erro ao enviar passagem: ");
          Serial.println(httpResponseCode);
        }
        http.end();
      } else {
        Serial.println("Erro: Wi-Fi desconectado.");
      }
    }
  }

  // Atualiza o estado anterior para a próxima leitura do loop
  estadoAnterior = estadoAtual;
 
  // Pequena pausa para estabilidade do processador
  delay(1000);
}