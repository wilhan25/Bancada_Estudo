#include <Arduino.h>

void TaskMemoria(void *pv){
  while (1)
  {
    /* code */
    UBaseType_t livreAntes = uxTaskGetStackHighWaterMark(NULL);
    Serial.print("[STACK] Espaço livre iniciamlente: ");
    Serial.println(livreAntes);

    char meu_buffer[1800];

    snprintf(meu_buffer, sizeof(meu_buffer),"Consumindo memória de forma controlada...");

    UBaseType_t livreDepois = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("[STACK] Espaço livre com buffer ativo: %d bytes \n", livreDepois);
    Serial.println("--------------------------");

    vTaskDelay(pdMS_TO_TICKS(3000));
  }  
}

void setup()
{
  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(3000));
  Serial.println("---Duelo pelo processador começou---");


  xTaskCreatePinnedToCore(TaskMemoria, "memoria", 2048,NULL,1,NULL,1);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
