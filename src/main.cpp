#include <Arduino.h>
#include <esp_task_wdt.h> // <-- 1. Importamos a biblioteca do Watchdog do ESP32

void TaskEgoista(void *pv){
  esp_task_wdt_add(NULL); //WatchDog para vigiar a tarefa
  while (1)
  {
    /* code */
    Serial.println("\n [TASK] Começando processamento de 8 segundo ....");
    uint32_t tempoInicio = millis();
    while ((millis()-tempoInicio)<8000)
    {
      // função para ocupar o processador por 8 seg
    }
    
    Serial.println("[TASK] Terminei com Sucesso ! (Isso nunca vai aparecer)");
    esp_task_wdt_reset();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup()
{
  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(3000));
  Serial.println("\n--- Teste do Cão de Guarda (Watchdog) ---");

  xTaskCreatePinnedToCore(TaskEgoista,"task", 2048,NULL,3,NULL,1);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
