# Projeto09 - Timer ⏲️

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-TIM2%20%7C%20GPIO-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-35%20min-orange.svg)

## 📋 Descrição

Este projeto implementa uma **função de delay em microssegundos** usando o timer TIM2 como base temporal. Demonstra como criar delays precisos personalizados usando timers hardware, oferecendo resolução de microssegundos em vez de milissegundos do HAL_Delay().

## 🎯 Objetivos de Aprendizado

- Implementar delay customizado com timer
- Trabalhar com timer de 32-bit (TIM2)
- Compreender resolução de microssegundos
- Criar funções de tempo não-bloqueantes
- Configurar timer para máxima resolução
- Diferença entre timers de 16-bit e 32-bit

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 2 LEDs externos (led0, signal)
- **Resistores:** 2x 330Ω (LEDs)
- **Osciloscópio** (recomendado para medição precisa)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
led0_Pin  -----> |+| LED0 --|> GND
                      |
                    330Ω

signal_Pin -----> |+| LED1 --|> GND
                      |
                    330Ω
```

### Configuração de Pinos
- **led0_Pin:** GPIO_Output (LED indicador principal)
- **signal_Pin:** GPIO_Output (LED de sinal/teste)

## 💻 Principais Conceitos

### 1. Configuração do Timer TIM2
```c
TIM_HandleTypeDef htim2;

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 15;               // Para resolução de 1μs
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 4294967295;          // Máximo para 32-bit
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Função delay_us() Customizada
```c
/**
 * @brief Delay em microssegundos usando TIM2
 * @param us: Delay desejado em microssegundos
 * @note Timer já configurado para overflow a cada 1μs
 */
void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);           // Reset contador
    while(__HAL_TIM_GET_COUNTER(&htim2) < us);  // Aguarda tempo desejado
}
```

### 3. Loop Principal com Delay Personalizado
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    
    HAL_TIM_Base_Start(&htim2);  // Iniciar timer
    
    while (1)
    {
        HAL_GPIO_TogglePin(led0_GPIO_Port, led0_Pin);
        HAL_GPIO_TogglePin(signal_GPIO_Port, signal_Pin);
        
        delay_us(1000000);  // Delay de 1 segundo (1,000,000 μs)
    }
}
```

### 4. Cálculo de Resolução
```c
// Clock Configuration:
// APB1 Clock: 4MHz (16MHz / 4)
// Timer Clock: 4MHz (APB1 * 1, pois APB1 ≤ HCLK/2)
// Prescaler: 15 + 1 = 16
// Timer Frequency: 4MHz / 16 = 250kHz
// Period: 1/250kHz = 4μs... 

// Observação: Comentário indica 1μs, precisa verificar configuração
```

## 🔍 Funcionalidades

### Sistema de Delay Preciso:
- **Timer:** TIM2 (32-bit, APB1)
- **Resolução:** 1μs (conforme comentário no código)
- **Range:** 0 a 4,294,967,295 μs (~4294 segundos)
- **Precisão:** Hardware-based

### Comportamento:
- **LEDs:** Ambos piscam simultaneamente
- **Período:** 2 segundos (1s ON + 1s OFF)
- **Delay:** 1,000,000 μs = 1 segundo
- **Sincronização:** Perfeita entre LEDs

## ⚙️ Configuração STM32CubeMX

### TIM2 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Instance | TIM2 | Timer 2 (APB1, 32-bit) |
| Prescaler | 15 | Para resolução de 1μs* |
| Counter Mode | Up | Contagem crescente |
| Period | 4294967295 | Valor máximo (32-bit) |
| Clock Division | Div1 | Sem divisão adicional |

### Clock Configuration:
| Bus | Frequência | Observação |
|-----|------------|------------|
| SYSCLK | 16MHz | HSI interno |
| AHB | 16MHz | HCLK |
| APB1 | 4MHz | HCLK/4 |
| APB2 | 16MHz | HCLK/1 |

### GPIO Configuration:
| Pino | Label | Mode | Pull | Speed |
|------|--------|------|------|-------|
| PAx | led0 | Output Push Pull | No Pull | Low |
| PAy | signal | Output Push Pull | No Pull | Low |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure TIM2 com prescaler adequado
   - Calcule prescaler para resolução desejada
   - Configure APB1 clock

2. **Implementação:**
   - Implemente função delay_us()
   - Inicialize timer antes de usar
   - Teste com diferentes valores de delay

3. **Validação:**
   - Use osciloscópio para medir precisão
   - Compare com HAL_Delay()
   - Verifique resolução real

## 📊 Análise de Timing

### Cálculo Correto do Prescaler:
```c
// Para 1μs de resolução:
// APB1 Clock = 4MHz
// Timer Clock = APB1 Clock = 4MHz
// Desired Resolution = 1μs
// Required Timer Frequency = 1MHz
// Prescaler = (Timer Clock / Desired Frequency) - 1
// Prescaler = (4MHz / 1MHz) - 1 = 3

// Configuração atual (Prescaler = 15):
// Timer Frequency = 4MHz / 16 = 250kHz
// Resolution = 1/250kHz = 4μs
```

### Comparação de Performance:

| Método | Resolução | Precisão | CPU Usage | Range |
|--------|-----------|----------|-----------|-------|
| HAL_Delay() | 1ms | ±1% | Bloqueante | 0-2³² ms |
| delay_us() | 4μs* | ±0.01% | Bloqueante | 0-2³² μs |
| Timer polling | 4μs* | ±0.01% | Não-bloqueante | Contínuo |

*Com configuração atual

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Corrigir prescaler:** Ajustar para resolução real de 1μs
2. **Delays diferentes:** Testar com vários valores de delay
3. **LED único:** Usar apenas um LED para simplicidade

### Para Intermediários:
1. **Delay não-bloqueante:** Implementar delay com retorno imediato
2. **Múltiplos delays:** Sistema com vários delays simultâneos
3. **Medição de performance:** Comparar métodos diferentes

### Para Avançados:
1. **Delay interrompível:** Delay que pode ser cancelado
2. **Scheduler simples:** Sistema de tarefas baseado em delay
3. **Calibração automática:** Auto-ajuste baseado em referência

## 🐛 Possíveis Problemas

### Delay não preciso:
- **Prescaler incorreto:** Verificar cálculo matemático
- **Clock errado:** Confirmar frequência APB1
- **Overhead:** Considerar tempo de execução da função

### Travamento no while():
- **Timer não iniciado:** Verificar HAL_TIM_Base_Start()
- **Overflow:** Valor muito grande para counter
- **Clock disabled:** Timer sem clock source

### Performance ruim:
- **Delay muito curto:** Overhead da função é maior que delay
- **Interrupções:** Podem afetar precisão
- **Otimização compilador:** Pode alterar timing

## 🔍 Debug e Otimização

### Versão Otimizada da delay_us():
```c
void delay_us(uint32_t us)
{
    if (us == 0) return;  // Evitar loops infinitos
    
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    uint32_t target = us;
    
    // Compensar overhead (calibração experimental)
    if (us > 10) target -= 2;
    
    while(__HAL_TIM_GET_COUNTER(&htim2) < target);
}
```

### Função de Medição:
```c
uint32_t measure_delay_accuracy(uint32_t expected_us)
{
    uint32_t start = __HAL_TIM_GET_COUNTER(&htim2);
    delay_us(expected_us);
    uint32_t end = __HAL_TIM_GET_COUNTER(&htim2);
    
    return (end - start);  // Delay real medido
}
```

### Delay Não-Bloqueante:
```c
typedef struct {
    uint32_t start_time;
    uint32_t duration;
    uint8_t active;
} delay_timer_t;

delay_timer_t my_delay;

void start_delay_us(delay_timer_t* timer, uint32_t us)
{
    timer->start_time = __HAL_TIM_GET_COUNTER(&htim2);
    timer->duration = us;
    timer->active = 1;
}

uint8_t delay_expired(delay_timer_t* timer)
{
    if (!timer->active) return 1;
    
    uint32_t current = __HAL_TIM_GET_COUNTER(&htim2);
    if ((current - timer->start_time) >= timer->duration)
    {
        timer->active = 0;
        return 1;
    }
    return 0;
}
```

## 📈 Aplicações Práticas

### Casos de Uso:
- **Protocolos timing-críticos:** 1-Wire, DHT22
- **Geração de pulsos:** PWM software
- **Medição de tempo:** Cronômetros precisos
- **Interfaces analógicas:** ADC sampling preciso

## 📚 Próximos Passos

Continue explorando timers avançados:
- **[Projeto10 - Timer Interrupt](../Projeto10%20-%20Timer%20Interrupt/):** Timer com interrupções
- **[Projeto11 - Output Compare](../Projeto11%20-%20Output%20Compare%20OC/):** Geração de sinais
- **[Projeto12 - Input Capture](../Projeto12%20-%20Input%20Capture%20IC/):** Medição de sinais

## 📖 Referências

- [STM32F4 Timer Reference](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Precision Timing with STM32](https://www.st.com/resource/en/application_note/an4776-general-purpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)
- [Microsecond Delays Best Practices](https://interrupt.memfault.com/blog/stm32-timer-peripheral)

---

⏲️ **Dica:** Delays de microssegundos são essenciais para protocolos de comunicação rápidos e controle preciso de tempo!