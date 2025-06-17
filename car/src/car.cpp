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
  int8_t speed;    // velocidade (-100 a 100)
  int8_t steering; // ângulo de direção (-100 a 100)
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

// Controle de motores baseado em velocidade e direção (tank steering)
void controlMotors(int8_t speed, int8_t steering)
{
  // calcula velocidade de cada motor
  int leftSpeed = speed + steering;
  int rightSpeed = speed - steering;
  // limita faixa
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);
  // aplica direção e PWM no motor esquerdo
  if (leftSpeed >= 0)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, leftSpeed);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, -leftSpeed);
  }
  // aplica direção e PWM no motor direito
  if (rightSpeed >= 0)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, rightSpeed);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, -rightSpeed);
  }
}

// Callback para quando um dado é recebido via ESP-NOW
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingCommand, incomingData, sizeof(incomingCommand));
  controlMotors(incomingCommand.speed, incomingCommand.steering);
  Serial.print("Comando recebido - Velocidade: ");
  Serial.print(incomingCommand.speed);
  Serial.print(" | Direcao: ");
  Serial.println(incomingCommand.steering);
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
