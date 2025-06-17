#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Endereço de broadcast (todos os dispositivos)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup()
{
  Serial.begin(115200);

  // Inicializa WiFi no modo estação (necessário para ESP-NOW)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Inicializa o ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Erro ao inicializar o ESP-NOW");
    return;
  }

  // Adiciona peer de broadcast
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Erro ao adicionar peer");
    return;
  }

  Serial.println("Digite um comando na serial: <direcao><velocidade>");
  Serial.println("Exemplo: F100 (F = frente, 100 = velocidade)");
}

void loop()
{
  // Verifica se há dados disponíveis na Serial
  if (Serial.available() >= 2)
  {
    char direction = Serial.read();
    uint8_t speed = Serial.parseInt(); // lê como inteiro

    // Estrutura da mensagem compatível
    struct struct_message
    {
      int8_t speed;
      int8_t steering;
    };
    struct_message msg;
    msg.speed = (int8_t)(direction == 'B' ? -speed : speed); // B para backward opcional
    // leitura de steering após velocidade, ex: F100,-30 ou F100L30: use Serial.parseInt para steering
    int8_t steering = 0;
    if (Serial.peek() == ',' || Serial.peek() == 'L' || Serial.peek() == 'R')
    {
      Serial.read();
      steering = (int8_t)Serial.parseInt();
    }
    msg.steering = steering;
    // Envia a mensagem via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&msg, sizeof(msg));

    if (result == ESP_OK)
      Serial.println("Comando enviado via ESP-NOW");
    else
      Serial.printf("Erro ao enviar comando: %d\n", result);
  }
}
