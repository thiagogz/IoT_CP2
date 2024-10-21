#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

// Conexão com o Wi-Fi do Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Conexão com o servidor do Broker
const char* mqtt_server = "191.234.211.26";
const char* mqtt_topic_temperatura = "CPTemperatura";
const char* mqtt_topic_umidade = "CPUmidade";

WiFiClient espClient;
PubSubClient client(espClient);

// Configuração do pino referente ao DHT22
const int dhtPin = 15;

DHTesp dhtSensor;

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(dhtPin, DHTesp::DHT22);  // Configura o DHT22

  // Conectar à rede Wi-Fi
  setup_wifi();
  
  // Configurar o servidor MQTT
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!client.connected()) {
    client.connect("ESP32ClientPublisher"); // Identifique o publisher corretamente
  }
  client.loop();

  TempAndHumidity dados = dhtSensor.getTempAndHumidity();
  String temperatura = String(dados.temperature, 2);
  String umidade = String(dados.humidity, 2);
  Serial.println("Publicando Temperatura: " + temperatura);
  Serial.println("Publicando Umidade: " + umidade);
  
  // Publicar os dados com a conversão para const char*
  client.publish(mqtt_topic_temperatura, temperatura.c_str());
  client.publish(mqtt_topic_umidade, umidade.c_str());

  delay(2000);
}

