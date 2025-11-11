# Projeto48 - RTOS 08

## Descrição
Projeto que demonstra o uso de **event flags** (sinalizadores de eventos) no FreeRTOS para sincronização complexa entre múltiplas tarefas. Implementa um sistema onde duas condições devem ser atendidas simultaneamente para liberar uma ação.

## Conceitos Implementados

### Event Flags
- **Tipo:** Grupo de bits para sinalização de eventos
- **API:** `osEventFlagsNew(&attributes)`
- **Bits:** 0x10 (Tarefa1) e 0x20 (Botão/Interrupt)

### Sincronização Complexa
- **Set:** `osEventFlagsSet(eventHandle, flags)`
- **Wait:** `osEventFlagsWait(eventHandle, mask, options, timeout)`
- **Condição:** Aguarda AMBOS os eventos (0x30 = 0x10 | 0x20)

## Arquitetura

### Fontes de Eventos
1. **Tarefa1:** Seta flag 0x10 a cada 1s + transmite "T1"
2. **Botão (EXTI):** Seta flag 0x20 quando pressionado
3. **Tarefa2:** Aguarda AMBOS os eventos para transmitir "T2"

### Fluxo de Sincronização
```
Tarefa1 (0x10) ────┐
                   ├─→ Event Flags (0x30) ─→ Tarefa2 ("T2")
Botão ISR (0x20) ──┘
```

### Comportamento
- Tarefa2 só executa quando AMBAS as condições estão presentes
- Implementa sincronização AND entre eventos assíncronos
- Botão atua como "habilitador" da execução

## Hardware
- **MCU:** STM32F407VET6
- **Comunicação:** UART1 (9600 baud)
- **Entrada:** Botão em EXTI4 (rising edge)
- **Clock:** HSI (16MHz)

## Análise do Código

### Criação dos Event Flags
```c
// Grupo de event flags
myEvent01Handle = osEventFlagsNew(&myEvent01_attributes);
```

### Tarefa Produtora (Tarefa1)
```c
void Tarefa1_fun(void *argument) {
    for(;;) {
        HAL_UART_Transmit(&huart1, "T1\n", 3, 100);
        osEventFlagsSet(myEvent01Handle, 0x10);  // Seta flag 0x10
        osDelay(1000);
    }
}
```

### Interrupt do Botão
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == BOT_Pin) {
        osEventFlagsSet(myEvent01Handle, 0x20);  // Seta flag 0x20
    }
}
```

### Tarefa Consumidora (Tarefa2)
```c
void Tarefa2_fun(void *argument) {
    for(;;) {
        // Aguarda AMBOS os eventos (0x30 = 0x10 | 0x20)
        osEventFlagsWait(myEvent01Handle, 0x30, osFlagsWaitAll, osWaitForever);
        HAL_UART_Transmit(&huart1, "T2\n", 3, 100);
    }
}
```

### Configuração do Botão
```c
// Botão com interrupt na borda de subida
GPIO_InitStruct.Pin = BOT_Pin;
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
HAL_NVIC_EnableIRQ(EXTI4_IRQn);
```

## Vantagens dos Event Flags
- **Múltiplos eventos:** Até 32 flags independentes
- **Sincronização flexível:** AND, OR ou combinações
- **Eficiência:** Uma única primitiva para múltiplas condições
- **Atomicidade:** Operações thread-safe garantidas

## Observações
- "T1" aparece a cada 1s (Tarefa1 sempre executa)
- "T2" só aparece quando botão é pressionado após "T1"
- Demonstra sincronização complexa entre task periódica e evento externo
- Ideal para sistemas que precisam aguardar múltiplas condições