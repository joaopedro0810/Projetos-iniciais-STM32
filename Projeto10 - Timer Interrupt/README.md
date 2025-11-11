# Projeto10 - Timer Interrupt ⚡⏱️

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-TIM2%20%7C%20TIM3%20%7C%20Interrupts-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-40%20min-orange.svg)

## 📋 Descrição

Este projeto combina **timers hardware com interrupções**, demonstrando o uso de múltiplos timers (TIM2 e TIM3) operando simultaneamente com diferentes frequências. Cada timer gera interrupções independentes que controlam LEDs específicos, ilustrando programação multitarefa baseada em eventos temporais.

## 🎯 Objetivos de Aprendizado

- Configurar múltiplos timers com interrupções
- Implementar callbacks de timer período elapsed
- Trabalhar com timers simultâneos
- Compreender prioridades de interrupção de timer
- Programação orientada a eventos temporais
- Diferença entre polling e interrupt-driven timers

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 2 LEDs externos (signal0, signal1)
- **Resistores:** 2x 330Ω (LEDs)
- **Osciloscópio** (recomendado para análise de timing)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
signal0_Pin ----> |+| LED0 --|> GND
                       |
                     330Ω

signal1_Pin ----> |+| LED1 --|> GND
                       |
                     330Ω
```

### Configuração de Pinos
- **signal0_Pin:** GPIO_Output (LED controlado por TIM2)
- **signal1_Pin:** GPIO_Output (LED controlado por TIM3)

## 💻 Principais Conceitos

### 1. Configuração de Múltiplos Timers
```c
TIM_HandleTypeDef htim2;  // Timer 2 (32-bit, APB1)
TIM_HandleTypeDef htim3;  // Timer 3 (16-bit, APB1)

static void MX_TIM2_Init(void)
{
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 15999;        // Para 1kHz base
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;             // 1 segundo (1000 counts)
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_TIM3_Init(void)
{
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 15999;        // Para 1kHz base
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 499;             // 0.5 segundo (500 counts)
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Inicialização com Interrupções
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    
    // Iniciar timers com interrupções habilitadas
    HAL_TIM_Base_Start_IT(&htim2);  // TIM2 com interrupt
    HAL_TIM_Base_Start_IT(&htim3);  // TIM3 com interrupt
    
    // Loop principal vazio - tudo controlado por interrupções
    while (1)
    {
        // CPU em idle, aguardando interrupções de timer
        // Pode implementar outras tarefas aqui
    }
}
```

### 3. Callback de Interrupção de Timer
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2)
    {
        // TIM2 overflow - a cada 1 segundo
        HAL_GPIO_TogglePin(signal0_GPIO_Port, signal0_Pin);
    }
    else if (htim == &htim3)
    {
        // TIM3 overflow - a cada 0.5 segundo
        HAL_GPIO_TogglePin(signal1_GPIO_Port, signal1_Pin);
    }
}
```

### 4. Handlers de Interrupção (em stm32f4xx_it.c)
```c
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}
```

## 🔍 Funcionalidades

### Sistema Multi-Timer:
- **TIM2:** Período de 1 segundo → LED signal0 pisca a 0.5Hz
- **TIM3:** Período de 0.5 segundo → LED signal1 pisca a 1Hz
- **Execução simultânea:** Ambos operam independentemente
- **Baixo overhead:** CPU livre entre interrupções

### Comportamento Temporal:
```
Tempo:    0s    0.5s   1.0s   1.5s   2.0s   2.5s   3.0s
TIM2:     ▲      -     ▲      -      ▲      -      ▲
TIM3:     ▲      ▲     ▲      ▲      ▲      ▲      ▲
signal0:  ↑      -     ↓      -      ↑      -      ↓
signal1:  ↑      ↓     ↑      ↓      ↑      ↓      ↑
```

## ⚙️ Configuração STM32CubeMX

### Timer Configuration:
| Timer | Instance | Prescaler | Period | Frequency | IRQ |
|-------|----------|-----------|---------|-----------|-----|
| TIM2 | TIM2 | 15999 | 999 | 1Hz | TIM2_IRQn |
| TIM3 | TIM3 | 15999 | 499 | 2Hz | TIM3_IRQn |

### NVIC Configuration:
| IRQ | Priority | Sub Priority | Enable |
|-----|----------|--------------|--------|
| TIM2_IRQn | 0 | 0 | ✓ |
| TIM3_IRQn | 0 | 0 | ✓ |

### Clock Calculation:
```
APB1 Clock: 4MHz (HCLK/4)
Timer Clock: 4MHz (APB1 não dobra pois ≤ HCLK/2)
After Prescaler: 4MHz / 16000 = 250Hz
TIM2 Period: 250Hz / 1000 = 0.25Hz (4s total, 2s per LED state)
TIM3 Period: 250Hz / 500 = 0.5Hz (2s total, 1s per LED state)

// Observação: Verificar cálculos com configuração real
```

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure TIM2 e TIM3 em modo "Internal Clock"
   - Enable interrupts para ambos os timers
   - Configure NVIC priorities
   - Generate code

2. **Implementação:**
   - Implemente HAL_TIM_PeriodElapsedCallback()
   - Use HAL_TIM_Base_Start_IT() para iniciar
   - Compile e flash

3. **Teste:**
   - signal0 deve piscar mais lentamente
   - signal1 deve piscar mais rapidamente
   - Ambos devem operar simultaneamente

## 📊 Análise de Performance

### Comparação de Métodos:

| Método | CPU Usage | Precisão | Flexibilidade | Complexidade |
|--------|-----------|----------|---------------|--------------|
| HAL_Delay() | 100% | ±1% | Baixa | Simples |
| Timer Polling | 90% | ±0.01% | Média | Média |
| Timer Interrupt | <1% | ±0.01% | Alta | Média |

### Overhead de Interrupção:
- **Latência:** ~2-5μs (entrada na ISR)
- **Execução:** ~1-2μs (callback simples)
- **Total:** ~3-7μs por interrupção
- **Impact:** Desprezível para frequências baixas

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Frequências diferentes:** Alterar periods para outros valores
2. **Mais LEDs:** Adicionar TIM4, TIM5 com mais LEDs
3. **Contadores:** Contar quantas interrupções ocorreram

### Para Intermediários:
1. **Prioridades:** Configurar diferentes prioridades
2. **Nested interrupts:** Permitir interrupções aninhadas
3. **PWM:** Usar timers para gerar PWM

### Para Avançados:
1. **Scheduler:** Sistema de tarefas baseado em timer
2. **Dynamic period:** Alterar período em runtime
3. **One-shot mode:** Timer de disparo único

## 🐛 Possíveis Problemas

### Interrupções não disparam:
- Verificar NVIC configuration no CubeMX
- Confirmar HAL_TIM_Base_Start_IT() chamado
- Verificar clock do timer

### Timing incorreto:
- Verificar cálculo de prescaler e period
- Confirmar clock source do timer
- Medir com osciloscópio

### Jitter ou irregularidade:
- **Outras interrupções:** Podem causar atraso
- **Callback muito longo:** Manter callback curto
- **Priority conflicts:** Ajustar prioridades

## 🔍 Debug e Otimização

### Variáveis de Debug:
```c
volatile uint32_t tim2_count = 0;
volatile uint32_t tim3_count = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2)
    {
        tim2_count++;
        HAL_GPIO_TogglePin(signal0_GPIO_Port, signal0_Pin);
    }
    else if (htim == &htim3)
    {
        tim3_count++;
        HAL_GPIO_TogglePin(signal1_GPIO_Port, signal1_Pin);
    }
}
```

### Medição de Performance:
```c
// Medir tempo de execução do callback
uint32_t callback_start, callback_duration;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    callback_start = DWT->CYCCNT;  // ARM Cortex-M cycle counter
    
    // Código do callback...
    
    callback_duration = DWT->CYCCNT - callback_start;
}
```

### Callback Otimizado:
```c
// Versão otimizada com lookup table
static const struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} timer_outputs[] = {
    [2] = {signal0_GPIO_Port, signal0_Pin},  // TIM2
    [3] = {signal1_GPIO_Port, signal1_Pin}   // TIM3
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint8_t timer_num = 0;
    
    if (htim->Instance == TIM2) timer_num = 2;
    else if (htim->Instance == TIM3) timer_num = 3;
    
    if (timer_num != 0)
    {
        HAL_GPIO_TogglePin(timer_outputs[timer_num].port, 
                          timer_outputs[timer_num].pin);
    }
}
```

## 📈 Aplicações Práticas

### Casos de Uso:
- **Sistemas de controle:** Múltiplas tarefas periódicas
- **Aquisição de dados:** Sampling em diferentes taxas
- **Comunicação:** Timeouts e protocolos
- **Interface usuário:** Blink patterns, indicators

### Sistema de Tarefas Simples:
```c
typedef struct {
    void (*function)(void);
    uint32_t period_ms;
    uint32_t counter;
} task_t;

task_t tasks[] = {
    {task_led_blink, 1000, 0},
    {task_sensor_read, 100, 0},
    {task_data_send, 5000, 0}
};

// No callback do timer (1ms base):
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 0; i < sizeof(tasks)/sizeof(task_t); i++)
    {
        if (++tasks[i].counter >= tasks[i].period_ms)
        {
            tasks[i].counter = 0;
            tasks[i].function();
        }
    }
}
```

## 📚 Próximos Passos

Continue explorando recursos avançados de timer:
- **[Projeto11 - Output Compare](../Projeto11%20-%20Output%20Compare%20OC/):** Geração de pulsos precisos
- **[Projeto12 - Input Capture](../Projeto12%20-%20Input%20Capture%20IC/):** Medição de sinais
- **[Projeto41 - RTOS 01](../Projeto41%20-%20RTOS%2001/):** Sistemas operacionais em tempo real

## 📖 Referências

- [STM32F4 Timer Interrupts](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4776: Timer Cookbook](https://www.st.com/resource/en/application_note/an4776-general-purpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)
- [Interrupt-Driven Programming Best Practices](https://interrupt.memfault.com/blog/cortex-m-rtos-context-switching)

---

⚡⏱️ **Dica:** Timer interrupts são a base de sistemas multitarefa! Domine este conceito para criar sistemas complexos e responsivos.