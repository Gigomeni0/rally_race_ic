#include <Arduino.h> // Necessário para as funções padrão do Arduino
#include <WiFi.h>
#include <esp_now.h>

// Definição dos pinos da ponte H
#define IN1 14
#define IN2 27
#define IN3 26
#define IN4 25
#define ENA 33
#define ENB 32

// Estrutura da mensagem
typedef struct struct_message
{
  char direction;
  uint8_t speed;
} struct_message;

struct_message incomingCommand;
// Estrutura para armazenar o endereço MAC do dispositivo
String macAddress;
// Função para configurar motores
void setupMotors()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

// Controle de motores baseado na direção
void controlMotors(char direction, uint8_t speed)
{
  switch (direction)
  {
  case 'F': // Frente
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    break;
  case 'T': // Trás
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    break;
  case 'L': // Esquerda
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    break;
  case 'R': // Direita
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    break;
  case 'S': // Parar
  default:
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    speed = 0;
    break;
  }
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// Callback para quando um dado é recebido via ESP-NOW
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
  controlMotors(incomingCommand.direction, incomingCommand.speed);
  Serial.print("Comando recebido: ");
  Serial.print(incomingCommand.direction);
  Serial.print(" | Velocidade: ");
  Serial.println(incomingCommand.speed);
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.print("MAC do ESP32: ");
  macAddress = WiFi.macAddress();
  Serial.println(macAddress);
  setupMotors();

  // Inicializa ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  delay(1000); // Aguarda 1 segundo entre os loops
  Serial.println("Endereço MAC desse carro: ");
  Serial.println(macAddress);

  // Nada aqui, o controle é por interrupção de recebimento
}
