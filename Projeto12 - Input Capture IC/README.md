# Projeto12 - Input Capture IC 📡

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-TIM2%20Input%20Capture-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-50%20min-orange.svg)

## 📋 Descrição

Este projeto demonstra o modo **Input Capture** dos timers STM32, funcionalidade complementar ao Output Compare que permite medir com precisão características de sinais externos (frequência, período, duty cycle). Implementa medição de frequência usando detecção de bordas e cálculo automático via interrupções.

## 🎯 Objetivos de Aprendizado

- Compreender modo Input Capture de timers
- Implementar medição de frequência digital
- Configurar detecção de bordas (rising/falling/both)
- Trabalhar com interrupções de capture
- Calcular período e frequência de sinais
- Diferença entre polling e interrupt-driven measurement

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Gerador de sinal:** Função generator ou saída do Projeto11
- **Osciloscópio** (para verificação)
- **Breadboard e jumpers**

## 📐 Esquema de Ligação

```
Gerador de Sinal  |    STM32F407VET6    |    Equipamentos
======================================================
Signal Output ---------> PA0 (TIM2_CH1)
                         |
                         +------------> Osciloscópio CH1
                         
GND ---------------> GND <------------- GND (Osciloscópio)
```

### Configuração de Pinos
- **PA0:** TIM2_CH1 (Input Capture Channel 1)

## 💻 Principais Conceitos

### 1. Configuração Input Capture
```c
TIM_HandleTypeDef htim2;

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};
    
    // Configuração base do timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 8-1;               // Prescaler = 8
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xffff;               // Máximo para TIM2 32-bit
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração Input Capture
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;  // Ambas bordas
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;          // Direct input
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;                   // Sem prescaler
    sConfigIC.ICFilter = 0;                                   // Sem filtro
    
    if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Algoritmo de Medição de Frequência
```c
// Variáveis globais para medição
int32_t leitura[3];    // Array para armazenar captures
int32_t frequencia;    // Frequência calculada
uint8_t set = 0;       // Estado da máquina de estados

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    
    // Iniciar Input Capture com interrupção
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    
    while (1)
    {
        // Frequência calculada em background pela ISR
        // Usar 'frequencia' para display ou processar
    }
}
```

### 3. Callback de Input Capture
```c
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2)
    {
        if (set == 2)  // Terceira capture: calcular período
        {
            leitura[2] = leitura[1] - leitura[0];  // Período em timer ticks
            frequencia = (2000000 / leitura[2]) / 2;  // Hz
            set = 0;  // Reiniciar ciclo
        }
        else if (set == 1)  // Segunda capture
        {
            leitura[1] = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
            set = 2;
        }
        else if (set == 0)  // Primeira capture
        {
            leitura[0] = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
            set = 1;
        }
    }
}
```

## 🔍 Funcionalidades

### Input Capture Configuration:
- **Timer:** TIM2 (32-bit, alta resolução)
- **Channel:** CH1 (PA0) 
- **Edge:** Both edges (rising + falling)
- **Prescaler:** 8 (clock = 2MHz assumindo HSI 16MHz)

### Algoritmo de Medição:
- **Estado 0:** Primeira captura (referência)
- **Estado 1:** Segunda captura (metade do período)
- **Estado 2:** Cálculo de período e frequência

### Cálculo de Frequência:
```c
// Timer frequency = HSI(16MHz) / Prescaler(8) = 2MHz
// Period (ticks) = leitura[1] - leitura[0] 
// Period (seconds) = Period_ticks / Timer_frequency
// Frequency = 1 / Period = Timer_frequency / Period_ticks
// 
// No código: frequencia = (2000000 / leitura[2]) / 2
// Divisão por 2 porque mede half-period (both edges)
```

## ⚙️ Configuração STM32CubeMX

### TIM2 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Instance | TIM2 | Timer 2 (32-bit) |
| Prescaler | 7 | Divide por 8 (clock = 2MHz) |
| Period | 0xFFFFFFFF | Máximo (32-bit) |
| Clock Division | Div1 | Sem divisão adicional |

### Input Capture Settings:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Channel | CH1 | Canal 1 (PA0) |
| Polarity | Both Edge | Rising + Falling |
| Selection | Direct TI | Input direto |
| Prescaler | Div1 | Sem prescaler |
| Filter | 0 | Sem filtro |

### GPIO Configuration:
| Pin | Function | Mode | Pull |
|-----|----------|------|------|
| PA0 | TIM2_CH1 | AF Input | No Pull |

### NVIC Settings:
- **TIM2 global interrupt:** Enabled

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure TIM2 em modo "Input Capture"
   - Set channel CH1 para both edges
   - Configure PA0 como Alternate Function
   - Enable TIM2 global interrupt
   - Generate code

2. **Hardware Setup:**
   - Conecte gerador de sinal ao pino PA0
   - Configure sinal: 1kHz, 3.3V amplitude
   - Connect GND comum

3. **Teste:**
   - Compile e execute
   - Use debugger para monitorar variável `frequencia`
   - Varie frequência do gerador e observe resultado

## 📊 Análise de Performance

### Range de Frequência:
```c
// Timer clock: 2MHz
// Resolução: 0.5μs por tick
// 
// Frequência mínima: ~30Hz (overflow protection needed)
// Frequência máxima: ~500kHz (Nyquist limit)
// 
// Precisão: ±1 tick = ±0.5μs
// Para 1kHz: ±0.05% error
// Para 100kHz: ±5% error
```

### Exemplo de Cálculo:
```c
// Sinal de entrada: 1kHz (período = 1ms)
// Timer clock: 2MHz (0.5μs por tick)
// 
// Half-period = 0.5ms = 1000 timer ticks
// Measurement: leitura[2] = 1000
// Calculated freq = (2000000 / 1000) / 2 = 1000Hz ✓
```

### Limitações:
- **Jitter:** ±1 timer tick
- **Range:** Limitado por timer overflow
- **Sampling:** Minimum 3 edges para medição
- **Duty cycle:** Não medido diretamente

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Single edge:** Usar apenas rising edge
2. **LED indicator:** Piscar LED a cada medição
3. **Simple counter:** Contar number of pulses

### Para Intermediários:
1. **Duty cycle measurement:** Usar dois canais
2. **Frequency range:** Implementar auto-ranging
3. **Digital filter:** Adicionar filtro digital

### Para Avançados:
1. **DMA capture:** Usar DMA para continuous capture
2. **Statistical analysis:** Calcular jitter, média, desvio
3. **Multi-channel:** Medir multiple signals

## 🐛 Possíveis Problemas

### Não há interrupções:
- Verificar NVIC habilitado para TIM2
- Confirmar HAL_TIM_IC_Start_IT() chamado
- Verificar sinal conectado ao pino correto

### Frequência incorreta:
- Revisar cálculo de timer clock
- Verificar prescaler configuration
- Confirmar both-edge configuration

### Overflow/instabilidade:
- **Low frequencies:** Timer pode overflow
- **High frequencies:** Perda de interrupts
- **Noise:** Usar input filter

### Debugging Tips:
```c
// Adicionar monitoramento
volatile uint32_t capture_count = 0;
volatile uint32_t last_capture = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    capture_count++;  // Count interrupts
    last_capture = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
    
    // Original code...
}
```

## 🔍 Modos Input Capture

### TIM_INPUTCHANNELPOLARITY_RISING:
```c
// Apenas bordas de subida
sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
// Uso: Medição de período completo
```

### TIM_INPUTCHANNELPOLARITY_FALLING:
```c
// Apenas bordas de descida  
sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
// Uso: Sincronização com sinal específico
```

### TIM_INPUTCHANNELPOLARITY_BOTHEDGE:
```c
// Ambas as bordas
sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
// Uso: Medição de frequência rápida (half-period)
```

## 📈 Aplicações Avançadas

### Medição de Duty Cycle:
```c
// Usar dois canais para medir high/low time
void setup_duty_cycle_measurement(void)
{
    // CH1: Rising edge
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1);
    
    // CH2: Falling edge (mesmo sinal)
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2);
    
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}
```

### Frequency Counter:
```c
// Contar pulses em janela de tempo fixa
volatile uint32_t pulse_count = 0;
volatile bool gate_open = false;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (gate_open)
    {
        pulse_count++;
    }
}

// Timer para gate (1 segundo)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim3)  // 1Hz timer
    {
        frequency = pulse_count;  // Hz
        pulse_count = 0;
        gate_open = true;
    }
}
```

### Signal Analyzer:
```c
// Análise estatística de sinal
#define SAMPLE_SIZE 100
volatile uint32_t periods[SAMPLE_SIZE];
volatile uint16_t sample_index = 0;

void calculate_statistics(void)
{
    uint32_t sum = 0, min = 0xFFFFFFFF, max = 0;
    
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        sum += periods[i];
        if (periods[i] < min) min = periods[i];
        if (periods[i] > max) max = periods[i];
    }
    
    uint32_t average = sum / SAMPLE_SIZE;
    uint32_t jitter = max - min;
    
    // Process statistics...
}
```

## 📚 Próximos Passos

Continue explorando recursos avançados de medição:
- **[Projeto13 - ADC Simples](../Projeto13%20-%20ADC%20Simples/):** Medição de sinais analógicos
- **[Projeto49 - CAN Loopback](../Projeto49%20-%20CAN%20Loopback/):** Comunicação digital
- **[Projeto11 - Output Compare OC](../Projeto11%20-%20Output%20Compare%20OC/)** (revisitar): Comparar OC vs IC

## 📖 Referências

- [STM32F4 Timer Input Capture](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4776: Timer Cookbook - Input Capture](https://www.st.com/resource/en/application_note/an4776-general-purpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)
- [Frequency Measurement Techniques](https://www.st.com/content/ccc/resource/technical/document/application_note/4b/4b/9c/96/54/6d/4c/5e/CD00218824.pdf/files/CD00218824.pdf/jcr:content/translations/en.CD00218824.pdf)

---

📡 **Dica:** Input Capture é ideal para medir sinais externos com alta precisão. Combine com Output Compare para criar sistemas de medição e geração!