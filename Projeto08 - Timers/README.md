# Projeto08 - Timers ⏱️

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-TIM9%20%7C%20GPIO-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-40%20min-orange.svg)

## 📋 Descrição

Este projeto introduz o uso de **timers hardware** do STM32, especificamente o TIM9. Demonstra como usar um timer para medir tempo de forma precisa, substituindo delays por polling baseado em contadores hardware. É uma evolução importante para sistemas que precisam de timing preciso.

## 🎯 Objetivos de Aprendizado

- Compreender timers hardware do STM32
- Configurar timer básico (Basic Timer)
- Usar contadores para controle temporal
- Diferença entre HAL_Delay() e timer polling
- Trabalhar com registradores de contador
- Implementar timing não-bloqueante

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 1 LED externo (led1)
- **Resistores:** 1x 330Ω (LED)
- **Osciloscópio** (opcional, para medição de precisão)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
led1_Pin  -----> |+| LED1 --|> GND
                      |
                    330Ω
```

### Configuração de Pinos
- **led1_Pin:** GPIO_Output (LED controlado por timer)

## 💻 Principais Conceitos

### 1. Configuração do Timer TIM9
```c
TIM_HandleTypeDef htim9;

static void MX_TIM9_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    
    htim9.Instance = TIM9;
    htim9.Init.Prescaler = 15999;        // Prescaler para 1kHz
    htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim9.Init.Period = 65535;           // Valor máximo para 16-bit timer
    htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
    {
        Error_Handler();
    }
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Lógica Principal com Timer
```c
uint16_t tempo;  // Variável para armazenar contador

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM9_Init();
    
    // Iniciar o timer
    HAL_TIM_Base_Start(&htim9);
    
    while (1)
    {
        // Ler contador atual do timer
        tempo = __HAL_TIM_GET_COUNTER(&htim9);
        
        // Verificar se completou 1000 counts (1 segundo)
        if ((tempo % 1000) == 0)
        {
            HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
        }
    }
}
```

### 3. Cálculo de Timing
```c
// Clock do TIM9: 16MHz (APB2)
// Prescaler: 15999 + 1 = 16000
// Frequência resultante: 16MHz / 16000 = 1kHz
// Cada count = 1ms
// 1000 counts = 1000ms = 1 segundo
```

## 🔍 Funcionalidades

### Sistema de Timing Baseado em Hardware:
- **Timer:** TIM9 (16-bit, APB2)
- **Resolução:** 1ms por count
- **Frequência:** 1kHz após prescaler
- **Controle:** LED pisca a cada segundo

### Comportamento:
- **Precisão:** Hardware, não afetado por código
- **Non-blocking:** CPU livre para outras tarefas
- **Contínuo:** Timer roda independentemente

## ⚙️ Configuração STM32CubeMX

### TIM9 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Instance | TIM9 | Timer 9 (APB2, 16-bit) |
| Prescaler | 15999 | 16MHz → 1kHz |
| Counter Mode | Up | Contagem crescente |
| Period | 65535 | Valor máximo (16-bit) |
| Clock Division | Div1 | Sem divisão adicional |

### Clock Calculation:
```
APB2 Clock: 16MHz
Timer Clock = APB2 Clock / (Prescaler + 1)
Timer Clock = 16MHz / (15999 + 1) = 1kHz
Period = 1/1kHz = 1ms per count
```

### GPIO Configuration:
| Pino | Label | Mode | Pull | Speed |
|------|--------|------|------|-------|
| PAx | led1 | Output Push Pull | No Pull | Low |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Enable TIM9 em modo "Internal Clock"
   - Configure Prescaler para obter frequência desejada
   - Generate code

2. **Programação:**
   - Inicialize timer com HAL_TIM_Base_Start()
   - Use __HAL_TIM_GET_COUNTER() para ler
   - Implemente lógica baseada no contador

3. **Teste:**
   - LED deve piscar exatamente a cada 1 segundo
   - Timing deve ser preciso e consistente

## 📊 Análise de Precisão

### Comparação HAL_Delay vs Timer:

| Aspecto | HAL_Delay | Timer Hardware |
|---------|-----------|----------------|
| **Precisão** | ±1% (SysTick) | ±0.01% (Crystal) |
| **Jitter** | Variável | Mínimo |
| **CPU Load** | Bloqueante | Não-bloqueante |
| **Interrupções** | Podem atrasar | Não afetam |
| **Multi-timing** | Difícil | Fácil |

### Medições Esperadas:
- **Período LED:** 2000ms (1s ON + 1s OFF)
- **Duty cycle:** 50%
- **Precisão:** <10ppm com cristal externo
- **Resolução:** 1ms

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Frequência diferente:** Alterar prescaler para outras frequências
2. **Múltiplos LEDs:** Usar diferentes divisores do contador
3. **Contagem regressiva:** Usar contador decrescente

### Para Intermediários:
1. **PWM:** Configurar timer para gerar PWM
2. **Múltiplos timers:** Usar TIM10, TIM11 também
3. **Overflow interrupt:** Gerar interrupção no overflow

### Para Avançados:
1. **Input capture:** Medir frequências externas
2. **Output compare:** Gerar eventos em tempos específicos
3. **Encoder mode:** Ler encoders rotativos

## 🐛 Possíveis Problemas

### LED não pisca na frequência correta:
- Verificar cálculo do prescaler
- Confirmar clock do APB2
- Revisar lógica do módulo 1000

### Timer não funciona:
- Verificar se HAL_TIM_Base_Start() foi chamado
- Confirmar configuração do clock no CubeMX
- Testar com prescaler menor

### Drift temporal:
- Usar cristal externo para maior precisão
- Verificar temperatura de operação
- Calibrar com referência externa

## 🔍 Debug e Medições

### Variáveis para Monitorar:
```c
uint16_t tempo;                    // Contador atual
uint32_t tim_frequency;            // Frequência calculada
uint32_t overflow_count = 0;       // Contador de overflows
```

### Cálculos Úteis:
```c
// Frequência do timer após prescaler
uint32_t timer_freq = HAL_RCC_GetPCLK2Freq() / (htim9.Init.Prescaler + 1);

// Tempo em segundos desde reset
float time_seconds = (float)tempo / 1000.0f;

// Número de overflows (para contagem longa)
if (__HAL_TIM_GET_FLAG(&htim9, TIM_FLAG_UPDATE))
{
    __HAL_TIM_CLEAR_FLAG(&htim9, TIM_FLAG_UPDATE);
    overflow_count++;
}
```

## 📈 Aplicações Avançadas

### Timer como Base de Tempo:
```c
// Sistema de múltiplos timings
uint16_t last_time_1s = 0;
uint16_t last_time_500ms = 0;

while(1)
{
    tempo = __HAL_TIM_GET_COUNTER(&htim9);
    
    // Evento a cada 1 segundo
    if ((tempo - last_time_1s) >= 1000)
    {
        HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
        last_time_1s = tempo;
    }
    
    // Evento a cada 500ms
    if ((tempo - last_time_500ms) >= 500)
    {
        // Outra ação
        last_time_500ms = tempo;
    }
}
```

### Medição de Performance:
```c
uint16_t start_time, end_time, execution_time;

start_time = __HAL_TIM_GET_COUNTER(&htim9);
// Código a ser medido
function_to_measure();
end_time = __HAL_TIM_GET_COUNTER(&htim9);

execution_time = end_time - start_time;  // Tempo em ms
```

## 📚 Próximos Passos

Continue explorando timers:
- **[Projeto09 - Timer](../Projeto09%20-%20Timer/):** Timer com configurações avançadas
- **[Projeto10 - Timer Interrupt](../Projeto10%20-%20Timer%20Interrupt/):** Timer com interrupções
- **[Projeto11 - Output Compare](../Projeto11%20-%20Output%20Compare%20OC/):** Geração de pulsos precisos

## 📖 Referências

- [STM32F4 Timer Reference](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4013: STM32 Timer Overview](https://www.st.com/resource/en/application_note/an4013-stm32-cross-series-timer-overview-stmicroelectronics.pdf)
- [Timer Calculator Tool](https://deepbluembedded.com/stm32-timer-calculator-tool/)

---

⏱️ **Dica:** Timers hardware são a base de sistemas temporais precisos! Domine este conceito para aplicações que exigem timing exato.