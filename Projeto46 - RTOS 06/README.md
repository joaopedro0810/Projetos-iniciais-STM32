# Projeto46 - RTOS 06

## Descrição
Projeto que demonstra o uso de **filas de mensagens** (message queues) no FreeRTOS para comunicação entre tarefas. Implementa um padrão produtor-consumidor onde uma task envia dados e outra processa e exibe os resultados.

## Conceitos Implementados

### Message Queue
- **Tipo:** Fila FIFO para comunicação inter-task
- **Capacidade:** 8 mensagens de 1 byte cada
- **API:** `osMessageQueueNew(8, sizeof(uint8_t), &attributes)`

### Comunicação Assíncrona
- **Put:** `osMessageQueuePut(queue, data, priority, timeout)`
- **Get:** `osMessageQueueGet(queue, data, priority, timeout)`
- **Timeout:** 100ms para envio, bloqueio infinito para recebimento

## Arquitetura

### Tarefas Criadas
1. **Envia (Produtor):** Envia valor fixo (100) para a fila a cada 1s
2. **Recebe (Consumidor):** Recebe dados, processa (+5) e transmite via UART

### Fluxo de Dados
```
Envia Task → [Fila1] → Recebe Task → UART
   (100)        (FIFO)     (+5=105)    ("0105")
```

### Comportamento
- Produtor envia dados constantemente
- Consumidor processa quando dados disponíveis
- Fila desacopla as velocidades das tarefas

## Hardware
- **MCU:** STM32F407VET6
- **Comunicação:** UART1 (9600 baud)
- **Clock:** HSI (16MHz)

## Análise do Código

### Criação da Fila
```c
// Fila: 8 elementos de 1 byte cada
Fila1Handle = osMessageQueueNew(8, sizeof(uint8_t), &Fila1_attributes);
```

### Produtor (Envia)
```c
uint8_t dados = 100;
osMessageQueuePut(Fila1Handle, &dados, 0, 100);  // Timeout: 100ms
osDelay(1000);  // Envia a cada 1s
```

### Consumidor (Recebe)
```c
uint8_t recebeDados;
osMessageQueueGet(Fila1Handle, &recebeDados, 0, osWaitForever);
recebeDados += 5;  // Processa dados
snprintf(buffer, sizeof(buffer), "%04u", recebeDados);
HAL_UART_Transmit(&huart1, buffer, sizeof(buffer), 100);
```

## Vantagens das Filas
- **Desacoplamento:** Tasks operam em velocidades diferentes
- **Buffering:** Armazena até 8 mensagens pendentes
- **Thread-safe:** Operações atômicas garantidas pelo RTOS
- **Prioridade:** Suporte a diferentes prioridades de mensagem

## Observações
- Resultado esperado: "0105" transmitido via UART a cada 1s
- Fila permite comunicação assíncrona entre tasks
- Ideal para sistemas pipeline e processamento de dados