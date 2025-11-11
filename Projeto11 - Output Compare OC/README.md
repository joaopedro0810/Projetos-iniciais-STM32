# Projeto11 - Output Compare OC 🎯

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-TIM2%20Output%20Compare-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-45%20min-orange.svg)

## 📋 Descrição

Este projeto introduz o modo **Output Compare** dos timers STM32, uma funcionalidade avançada que permite gerar sinais precisos em pinos específicos. Demonstra como usar dois canais do TIM2 (CH1 e CH2) para gerar ondas quadradas com toggle automático, eliminando a necessidade de intervenção do software.

## 🎯 Objetivos de Aprendizado

- Compreender modo Output Compare de timers
- Configurar múltiplos canais de timer
- Trabalhar com pinos de saída alternativos (AF)
- Gerar sinais precisos sem intervenção de software
- Diferença entre GPIO toggle e OC toggle
- Monitorar registradores de Compare

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Osciloscópio** (essencial para visualizar sinais)
- **LEDs:** 2 LEDs externos (opcional, para indicação visual)
- **Resistores:** 2x 330Ω (se usar LEDs)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
PA0 (TIM2_CH1) ---> Osciloscópio CH1
                    |+| LED1 --|> GND (opcional)
                         |
                       330Ω

PA1 (TIM2_CH2) ---> Osciloscópio CH2  
                    |+| LED2 --|> GND (opcional)
                         |
                       330Ω
```

### Configuração de Pinos
- **PA0:** TIM2_CH1 (Output Compare Channel 1)
- **PA1:** TIM2_CH2 (Output Compare Channel 2)

## 💻 Principais Conceitos

### 1. Configuração Output Compare
```c
TIM_HandleTypeDef htim2;

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    
    // Configuração base do timer
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 15999;        // Para frequência base
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;             // Define frequência final
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração Output Compare
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;      // Toggle no match
    sConfigOC.Pulse = 500;                     // Compare value
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    
    // Configurar ambos os canais
    if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    
    if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Inicialização dos Canais
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    
    // Iniciar Output Compare nos dois canais
    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
    
    while (1)
    {
        // Monitorar valor de compare do canal 2
        uint32_t count = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_2);
        // Sinais são gerados automaticamente pelo hardware!
    }
}
```

### 3. Funcionamento do Output Compare
```c
// Quando Timer Counter == Compare Register:
// 1. Hardware automaticamente alterna o pino (TOGGLE mode)
// 2. Gera flag/interrupção (se habilitada)
// 3. Continue contando até Period, depois reinicia

// Frequência de saída = Timer Frequency / (2 * Compare Value)
// Pois cada toggle acontece no compare match
```

## 🔍 Funcionalidades

### Modo Output Compare Toggle:
- **Timer:** TIM2 (32-bit, APB1)
- **Canais:** CH1 (PA0) e CH2 (PA1)
- **Modo:** TOGGLE - alterna automaticamente no match
- **Valor Compare:** 500 (configurable)

### Comportamento:
- **Hardware-driven:** Sinais gerados sem intervenção do CPU
- **Precisão:** Jitter mínimo (hardware timing)
- **Simultaneidade:** Ambos canais operam independentemente
- **Monitoramento:** Possível ler registradores em tempo real

## ⚙️ Configuração STM32CubeMX

### TIM2 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Instance | TIM2 | Timer 2 (32-bit) |
| Prescaler | 15999 | Reduz frequência base |
| Period | 999 | Define período do timer |
| Clock Division | Div1 | Sem divisão adicional |

### Output Compare Settings:
| Channel | Mode | Pulse | Polarity | Fast Mode |
|---------|------|-------|----------|-----------|
| CH1 | Toggle | 500 | High | Disable |
| CH2 | Toggle | 500 | High | Disable |

### GPIO Alternate Function:
| Pin | Function | Mode | Speed |
|-----|----------|------|-------|
| PA0 | TIM2_CH1 | AF Push-Pull | High |
| PA1 | TIM2_CH2 | AF Push-Pull | High |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure TIM2 em modo "Output Compare"
   - Set channels CH1 e CH2 para TOGGLE mode
   - Configure pinos PA0/PA1 como Alternate Function
   - Generate code

2. **Hardware Setup:**
   - Conecte PA0 ao canal 1 do osciloscópio  
   - Conecte PA1 ao canal 2 do osciloscópio
   - Configure GND comum

3. **Análise:**
   - Compile e execute projeto
   - Observe formas de onda no osciloscópio
   - Meça frequência e duty cycle

## 📊 Análise de Sinais

### Cálculo de Frequência:
```c
// Exemplo com configuração:
// APB1 Clock: 4MHz
// Prescaler: 15999 + 1 = 16000
// Timer Freq: 4MHz / 16000 = 250Hz
// Compare: 500
// Output Freq: 250Hz / (2 * 500) = 0.25Hz

// Nota: Verificar valores reais no projeto
```

### Formas de Onda Esperadas:
```
Timer Counter:  0    500   999   0    500   999   0
CH1 (PA0):      ▲     ▼     -    ▲     ▼     -    ▲
CH2 (PA1):      ▲     ▼     -    ▲     ▼     -    ▲
Period:         |----1/f----|----1/f----|----1/f----|
```

### Características:
- **Duty Cycle:** 50% (simétrico)
- **Phase:** Ambos canais em fase (mesmo compare value)
- **Jitter:** <1 clock cycle
- **Precision:** Hardware timing

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Compare values diferentes:** CH1=250, CH2=750 para sinais defasados
2. **Frequências diferentes:** Alterar prescaler ou period
3. **Single channel:** Usar apenas um canal primeiro

### Para Intermediários:
1. **PWM Mode:** Mudar de TOGGLE para PWM1/PWM2
2. **Phase shift:** Criar sinais com defasagem específica
3. **Duty cycle variável:** Alterar pulse width dinamicamente

### Para Avançados:
1. **DMA-driven:** Usar DMA para alterar compare values
2. **Complementary outputs:** Usar canais complementares
3. **Dead time:** Implementar dead time entre sinais

## 🐛 Possíveis Problemas

### Sem sinal na saída:
- Verificar configuração Alternate Function dos pinos
- Confirmar HAL_TIM_OC_Start() chamado
- Verificar clock do timer habilitado

### Frequência incorreta:
- Revisar cálculo de prescaler e period
- Verificar clock source (APB1 vs APB2)
- Confirmar compare values

### Sinais distorcidos:
- **Load capacitivo:** Usar buffer se driving LEDs
- **Ground loops:** Verificar GND comum
- **Impedância:** Verificar impedância de entrada do scope

## 🔍 Modos Output Compare

### TIM_OCMODE_TOGGLE:
```c
// Alterna pino a cada match
sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
sConfigOC.Pulse = 500;  // Toggle a cada 500 counts
```

### TIM_OCMODE_PWM1:
```c
// PWM com duty cycle configurável
sConfigOC.OCMode = TIM_OCMODE_PWM1;
sConfigOC.Pulse = 300;  // 30% duty cycle (300/1000)
```

### TIM_OCMODE_PWM2:
```c
// PWM invertido
sConfigOC.OCMode = TIM_OCMODE_PWM2;
sConfigOC.Pulse = 300;  // 70% duty cycle (invertido)
```

## 📈 Aplicações Avançadas

### Gerador de Clock:
```c
// Clock preciso para outros periféricos
void setup_clock_output(uint32_t frequency_hz)
{
    uint32_t timer_freq = HAL_RCC_GetPCLK1Freq();
    uint32_t prescaler = (timer_freq / (frequency_hz * 2)) - 1;
    
    htim2.Init.Prescaler = prescaler;
    htim2.Init.Period = 1;  // Toggle a cada count
    
    HAL_TIM_OC_Init(&htim2);
}
```

### Encoder Simulation:
```c
// Simular encoder quadrature
void setup_quadrature_output(void)
{
    // CH1: Phase A
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
    sConfigOC.Pulse = 250;
    HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    
    // CH2: Phase B (90° defasado)
    sConfigOC.Pulse = 125;  // 90° phase shift
    HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
}
```

### Medição de Performance:
```c
// Medir drift de frequência
volatile uint32_t compare_values[1000];
volatile uint16_t sample_index = 0;

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (sample_index < 1000)
    {
        compare_values[sample_index] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
        sample_index++;
    }
}
```

## 📚 Próximos Passos

Continue explorando recursos avançados de timer:
- **[Projeto12 - Input Capture](../Projeto12%20-%20Input%20Capture%20IC/):** Medição de sinais
- **[Projeto25 - DAC PWM](../Projeto25%20-%20DAC_PWM/):** PWM para controle analógico
- **[Projeto08 - Timers](../Projeto08%20-%20Timers/)** (revisitar): Comparar com OC

## 📖 Referências

- [STM32F4 Timer Output Compare](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4776: Timer Cookbook - Output Compare](https://www.st.com/resource/en/application_note/an4776-general-purpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)
- [Signal Generation with STM32 Timers](https://www.st.com/content/ccc/resource/technical/document/application_note/4b/4b/9c/96/54/6d/4c/5e/CD00218824.pdf/files/CD00218824.pdf/jcr:content/translations/en.CD00218824.pdf)

---

🎯 **Dica:** Output Compare é perfeito para gerar sinais precisos sem sobrecarregar a CPU. Essencial para aplicações que precisam de timing exato!