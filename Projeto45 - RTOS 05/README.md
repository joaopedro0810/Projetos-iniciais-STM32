# Projeto45 - RTOS 05

## Descrição
Projeto que demonstra o uso de **mutex** no FreeRTOS para proteção de recursos compartilhados. O mutex garante acesso mutuamente exclusivo ao recurso UART, evitando conflitos quando múltiplas tarefas tentam transmitir dados simultaneamente.

## Conceitos Implementados

### Mutex (Mutual Exclusion)
- **Tipo:** Binary mutex com propriedade de ownership
- **API:** `osMutexNew(&attributes)`
- **Acquire:** `osMutexAcquire(mutex, timeout)`
- **Release:** `osMutexRelease(mutex)`

### Proteção de Recurso
- **Recurso protegido:** UART1 para transmissão serial
- **Timeout:** 1000ms para aquisição
- **Padrão:** Acquire → Use Resource → Release

## Arquitetura

### Tarefas Criadas
1. **task1:** Prioridade Normal, transmite "t1" via UART protegida por mutex
2. **task2:** Prioridade Normal, transmite "t2" via UART protegida por mutex

### Comportamento
- Apenas uma task pode usar UART por vez
- Se mutex está ocupado, task aguarda até 1000ms
- Após timeout, task continua (sem usar UART)
- Cada task libera mutex após uso

## Hardware
- **MCU:** STM32F407VET6
- **Comunicação:** UART1 (9600 baud)
- **Clock:** HSI (16MHz)

## Análise do Código

### Criação do Mutex
```c
// Mutex para proteção do recurso UART
myMutexHandle = osMutexNew(&myMutex_attributes);
```

### Uso nas Tarefas
```c
// Padrão de uso do mutex
osMutexAcquire(myMutexHandle, 1000);  // Timeout: 1s
HAL_UART_Transmit(&huart1, data, len, 100);
osMutexRelease(myMutexHandle);
osDelay(1000);
```

## Diferenças: Mutex vs Semáforo
- **Mutex:** Ownership (só quem adquire pode liberar)
- **Semáforo:** Contador (qualquer task pode liberar)
- **Uso:** Mutex para recursos exclusivos, semáforo para contagem

## Observações
- Evita corrupção de dados na transmissão UART
- Timeout previne deadlock em caso de problema
- Essencial para recursos não reentrantes