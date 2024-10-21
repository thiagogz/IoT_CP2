#include <WiFi.h>
#include <PubSubClient.h>

// Conexão com o Wi-Fi do Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Conexão com o servidor do Broker
const char* mqtt_server = "191.234.211.26";
const char* mqtt_topic_temperatura = "CPTemperatura";
const char* mqtt_topic_umidade = "CPUmidade";

WiFiClient espClient;
PubSubClient client(espClient);

// Configuração dos pinos referentes aos LEDs
const int redPin = 2;    // Pino do LED vermelho
const int yellowPin = 4; // Pino do LED amarelo
const int bluePin = 5;   // Pino do LED azul
const int greenPin = 18; // Pino do LED verde

// Valores globais de temperatura e umidade como float
float valorTemperatura = 0.0;
float valorUmidade = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}

void setup_wifi() {
  delay(10);
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP32ClientReceiver")) {
      Serial.println("conectado");
      client.subscribe(mqtt_topic_temperatura);
      client.subscribe(mqtt_topic_umidade);
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tente novamente em 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Callback acionado");
  
  // Converte o payload para uma string
  Serial.print("Payload recebido (bruto): ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
    Serial.print((char)payload[i]);
  }

  // Processa o tópico de temperatura
  if (String(topic) == mqtt_topic_temperatura) {
    valorTemperatura = message.toFloat();
    Serial.println("Temperatura recebida: " + String(valorTemperatura) + "ºC");
  }
  // Processa o tópico de umidade
  else if (String(topic) == mqtt_topic_umidade) {
    valorUmidade = message.toFloat();
    Serial.println("Umidade recebida: " + String(valorUmidade) + "%");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println("Valor umidade:");
  Serial.println(valorUmidade);
  Serial.println("Valor temperatura:");
  Serial.println(valorTemperatura);
  // Verifica e controla os LEDs com base nos valores recebidos
  if (valorTemperatura >= 20.0 && valorUmidade >= 50.0) {
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(bluePin, LOW);
    Serial.println("LED VERDE LIGADO");
  } else if (valorTemperatura < 20.0 && valorUmidade >= 50.0) {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(bluePin, HIGH);
    Serial.println("LED AZUL LIGADO");
  } else if (valorTemperatura >= 20.0 && valorUmidade < 50.0) {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(bluePin, LOW);
    Serial.println("LED AMARELO LIGADO");
  } else if (valorTemperatura < 20.0 && valorUmidade < 50.0) {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(bluePin, LOW);
    Serial.println("LED VERMELHO LIGADO");
  }

  delay(1000); // Delay para reduzir a frequência de atualização
}
