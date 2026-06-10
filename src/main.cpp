#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// conexões com internet e servidor mqtt
const char* nomeWifi = "NOME_DA_SUA_REDE";
const char* senhaWifi = "SENHA_DA_SUA_REDE";
const char *mqtt_server = "broker.hivemq.com";
const int Porta_BrokerMQTT = 1883;

// Pinos GPIO
#define LED_PIN 2

// Objetos de Rede
WiFiClient clienteTCP;
PubSubClient clienteMQTT(clienteTCP);

// Fila para transmitir os dados entre tasks
QueueHandle_t filaLED;

// Task para manter as conexões do esp32
void TaskManterConexoes(void *pv)
{
  while (1)
  {
    // checar a conexão com wifi
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("[WIFI] ->Conectando ao WIFI ....");
      WiFi.begin(nomeWifi, senhaWifi);

      while (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(500));
      }
      Serial.println("[WIFI] Conectado Com SUCESSO!");
    }

    // checar a conexão com o broker MQTT
    if (WiFi.status() == WL_CONNECTED && !clienteMQTT.connected())
    {
      Serial.println("[MQTT]Criando ID para Conexão com Broker MQTT...");
      String idCliente = "ESP32_WILHAN_" + String(random(0xffff),HEX);

      if(clienteMQTT.connect(idCliente.c_str())){
        Serial.println("[MQTT]Conexão com Broker feita com sucesso!");
        clienteMQTT.subscribe("casa/quarto/luz");
      }
      else{
        Serial.print("[MQTT] Erro ao conectar ao broker: ");
        Serial.println(clienteMQTT.state());
        vTaskDelay(pdMS_TO_TICKS(2000));
      }
    }

    //Persistencia na conexão mqtt
    if(clienteMQTT.connected()){
      clienteMQTT.loop();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Task para fazer a lógica de ligar e desligar o LED
void TaskLED(void *pv){

  bool estadoLED;
  while (1)
  {
    if(xQueueReceive(filaLED, &estadoLED, portMAX_DELAY)){
      digitalWrite(LED_PIN, estadoLED);

      if(estadoLED) Serial.println("[LED] Estou Ligado");
      else Serial.println("[LED] Estou Desligado");
    }
  }
}

void callbackMQTT(char* topico, byte* dados, unsigned int tamanho){
  String mensagem;

  for(int i =0; i<tamanho; i++){
    mensagem += (char)dados[i];
  }

  Serial.print("[CALLBACK] Mensagem: ");
  Serial.println(mensagem);

  bool estadoLED;
  if(mensagem == "ON"){
    estadoLED = true;
    xQueueSend(filaLED, &estadoLED, portMAX_DELAY);
  }

  if(mensagem == "OFF"){
    estadoLED = false;
    xQueueSend(filaLED, &estadoLED,portMAX_DELAY);
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // config da fila do led
  filaLED = xQueueCreate(5,sizeof(bool));

  // configura o motor MQTT apontando para o servidor
  clienteMQTT.setServer(mqtt_server, Porta_BrokerMQTT);
  // configurando para qual funções o mqtt deve olhar quando chegar mensagem
  clienteMQTT.setCallback(callbackMQTT);

  // Criação das task
  xTaskCreatePinnedToCore(TaskManterConexoes,"conexoes",4096, NULL,2,NULL,1);
  xTaskCreatePinnedToCore(TaskLED, "led",2048,NULL,1,NULL,0);
}

void loop(){}