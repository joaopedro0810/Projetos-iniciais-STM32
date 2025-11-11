# Projeto47 - RTOS 07

## Descrição
Projeto que demonstra o uso de **software timers** no FreeRTOS para execução de funções de callback em intervalos regulares. Implementa dois LEDs com frequências diferentes: um controlado por timer software e outro por task.

## Conceitos Implementados

### Software Timer
- **Tipo:** Timer periódico baseado em software
- **API:** `osTimerNew(callback, osTimerPeriodic, NULL, &attributes)`
- **Callback:** Função executada automaticamente na expiração

### Gerenciamento de Timers
- **Start:** `osTimerStart(timerHandle, period_ms)`
- **Período:** 1000ms (1 segundo)
- **Execução:** No contexto do daemon de timers

## Arquitetura

### Componentes Principais
1. **myTimer01:** Software timer periódico (1s)
2. **PiscaLed Task:** Task para controle independente (2s)
3. **Callback01:** Função executada pelo timer

### Controle de LEDs
- **LED_1:** Controlado pelo timer software (1s)
- **LED_2:** Controlado pela task (2s)

### Comportamento
- Timer executa callback automaticamente a cada 1s
- Task pisca LED independentemente a cada 2s
- Demonstra duas formas de temporização no RTOS

## Hardware
- **MCU:** STM32F407VET6
- **LEDs:** LED_1 e LED_2 em GPIOA
- **Clock:** HSI (16MHz)

## Análise do Código

### Criação do Timer
```c
// Timer periódico com callback
myTimer01Handle = osTimerNew(Callback01, osTimerPeriodic, NULL, &myTimer01_attributes);
```

### Task Principal
```c
void PiscaLed_fun(void *argument) {
    osTimerStart(myTimer01Handle, 1000);  // Inicia timer com 1s
    
    for(;;) {
        HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);  // LED_2
        osDelay(2000);  // Delay de 2s
    }
}
```

### Callback do Timer
```c
void Callback01(void *argument) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);  // LED_1
}
```

### Configuração GPIO
```c
// Inicialização dos LEDs como saída
HAL_GPIO_WritePin(GPIOA, LED_1_Pin|LED_2_Pin, GPIO_PIN_SET);
```

## Vantagens dos Software Timers
- **Precisão:** Executados pelo daemon de timers (alta prioridade)
- **Economia:** Não consomem tasks dedicadas
- **Flexibilidade:** One-shot ou periódicos
- **Eficiência:** Múltiplos timers com overhead mínimo

## Observações
- LED_1 pisca a cada 1s (timer software)
- LED_2 pisca a cada 2s (task com osDelay)
- Demonstra diferentes métodos de temporização no FreeRTOS
- Timer daemon executa callbacks em contexto privilegiado