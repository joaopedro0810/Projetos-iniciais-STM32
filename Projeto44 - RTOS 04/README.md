# Projeto44 - RTOS 04

## Descrição
Projeto que demonstra o uso de **semáforos contadores** (counting semaphores) no FreeRTOS para controlar o acesso a recursos limitados. Este projeto implementa um semáforo com contagem 2, permitindo que até duas tarefas acessem um recurso simultaneamente.

## Conceitos Implementados

### Semáforo Contador
- **Tipo:** Counting Semaphore com valor máximo 2
- **Valor inicial:** 2 (ambas as unidades disponíveis)
- **API:** `osSemaphoreNew(2, 2, &attributes)`

### Controle de Acesso
- **Acquire:** `osSemaphoreAcquire(semContHandle, osWaitForever)`
- **Count:** `osSemaphoreGetCount(semContHandle)`
- **Bloqueio indefinido:** `osWaitForever`

## Arquitetura

### Tarefas Criadas
1. **task1:** Consome 1 unidade do semáforo, envia "t1" via UART
2. **task2:** Consome 1 unidade do semáforo, envia "t2" via UART, monitora contador
3. **task3:** Independente, envia "t3" via UART sem usar semáforo

### Comportamento
- Tasks 1 e 2 podem executar simultaneamente (semáforo permite 2 acessos)
- Task 3 executa independentemente
- Quando 2 tasks estão ativas, próxima fica bloqueada até liberação

## Hardware
- **MCU:** STM32F407VET6
- **Comunicação:** UART1 (9600 baud)
- **Clock:** HSI (16MHz)

## Análise do Código

### Criação do Semáforo
```c
// Semáforo contador: max=2, inicial=2
semContHandle = osSemaphoreNew(2, 2, &semCont_attributes);
```

### Uso nas Tarefas
```c
// Tasks 1 e 2: Adquirem semáforo antes de usar UART
osSemaphoreAcquire(semContHandle, osWaitForever);
HAL_UART_Transmit(&huart1, data, len, 100);

// Task 2: Monitora contador atual
cont = osSemaphoreGetCount(semContHandle);
```

## Observações
- Demonstra controle de recursos limitados (max 2 usuários simultâneos)
- Task 3 mostra comportamento sem restrição de semáforo
- Útil para pools de recursos ou limitação de acesso concorrente