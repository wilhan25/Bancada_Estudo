#include <Arduino.h>

void TaskImportante(void *pv)
{
  while (1)
  {
    Serial.println("\n[👑 NOBRE] Acordei! Eu tenho prioridade 3. Saiam da frente!");
    Serial.println("[👑 NOBRE] Executando algo crítico...");

   uint32_t tempoInicio = millis();
        // O ESP32 vai ficar preso neste 'while' fritando o processador por 500ms
        while ((millis() - tempoInicio) < 300) {
            // Prende o processador aqui. A task continua em RUNNING.
            // Nenhuma task de prioridade menor consegue respirar aqui.
        }

    Serial.println("[👑 NOBRE] Terminei, vou dormir por 3 segundos...");
    // vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void TaskComum(void *pv)
{
  uint32_t contador = 0;
  while (1)
  {
    Serial.printf("[🛠️ OPERARIA] Trabalhando... Giro número: %d\n", contador++);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup()
{
  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(3000));
  Serial.println("---Duelo pelo processador começou---");

  xTaskCreatePinnedToCore(TaskImportante,    "Nobre",    2048, NULL, 3, NULL, 1); 
    xTaskCreatePinnedToCore(TaskComum, "Operaria", 2048, NULL, 1, NULL, 1);

}

void loop()
{
  // put your main code here, to run repeatedly:
}
