# Projeto13 - ADC Simples 📊

![Dificuldade](https://img.shields.io/badge/Dificuldade-Iniciante-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-ADC1-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-30%20min-orange.svg)

## 📋 Descrição

Este projeto introduz os conceitos fundamentais do **ADC (Analog-to-Digital Converter)** do STM32F407VET6. Demonstra como converter sinais analógicos em valores digitais usando polling, incluindo conversão de valores ADC para tensão real. É o primeiro passo para trabalhar com sensores analógicos.

## 🎯 Objetivos de Aprendizado

- Entender funcionamento básico do ADC
- Configurar ADC em modo single conversion
- Implementar leitura com polling (HAL_ADC_PollForConversion)
- Converter valores ADC para tensão
- Calcular constante de conversão
- Trabalhar com resolução de 10 bits

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Potenciômetro:** 10kΩ (para teste)
- **Multímetro** (para verificação)
- **Fonte de tensão variável** (opcional)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
Potenciômetro     |    STM32F407VET6
==================================
VCC (3.3V) --------> 3.3V
GND ----------------> GND
Wiper (saída) -----> PA0 (ADC1_IN0)

Alimentação:
- Potenciômetro: 0V a 3.3V
- Tensão máxima no pino: 3.3V (NUNCA exceder!)
```

### Configuração de Pinos
- **PA0:** ADC1_IN0 (Canal 0 do ADC1)

## 💻 Principais Conceitos

### 1. Configuração ADC Básica
```c
ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Configuração global do ADC
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_10B;        // 10 bits = 0-1023
    hadc1.Init.ScanConvMode = DISABLE;                 // Single channel
    hadc1.Init.ContinuousConvMode = DISABLE;           // Single conversion
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;  // Software trigger
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;        // Alinhamento direita
    hadc1.Init.NbrOfConversion = 1;                    // Uma conversão
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração do Canal 0
    sConfig.Channel = ADC_CHANNEL_0;                   // PA0
    sConfig.Rank = 1;                                  // Primeira conversão
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;   // Tempo de amostragem
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Leitura ADC com Polling
```c
// Constante de conversão para tensão
#define CONST_TENSAO    0.0032258064516129032258064516129

// Variáveis globais
uint16_t adc;      // Valor ADC (0-1023)
float tensao;      // Tensão convertida (0-3.3V)

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    
    while (1)
    {
        // Iniciar conversão ADC
        HAL_ADC_Start(&hadc1);
        
        // Aguardar conversão completar (timeout 100ms)
        HAL_ADC_PollForConversion(&hadc1, 100);
        
        // Ler valor convertido
        adc = HAL_ADC_GetValue(&hadc1);
        
        // Converter para tensão
        tensao = adc * CONST_TENSAO;
        
        // Parar ADC
        HAL_ADC_Stop(&hadc1);
        
        // Aguardar próxima leitura
        HAL_Delay(500);
    }
}
```

### 3. Cálculo da Constante de Conversão
```c
// Para ADC de 10 bits com Vref = 3.3V:
// Resolução: 2^10 = 1024 níveis (0 a 1023)
// Tensão por LSB: 3.3V / 1024 = 0.003225... V
// 
// CONST_TENSAO = 3.3 / 1024 = 0.0032258064516129032258064516129
//
// Exemplo:
// ADC = 512 → Tensão = 512 * 0.003225... = 1.65V
// ADC = 1023 → Tensão = 1023 * 0.003225... = 3.297V ≈ 3.3V
```

## 🔍 Funcionalidades

### Características do ADC:
- **Resolução:** 10 bits (0-1023)
- **Tensão de referência:** 3.3V (VDD)
- **Canal:** ADC1_IN0 (PA0)
- **Modo:** Single conversion, Software trigger
- **Tempo de amostragem:** 480 cycles (alta precisão)

### Comportamento:
- **Polling-based:** CPU aguarda conversão terminar
- **Frequência:** ~2Hz (conversão a cada 500ms)
- **Precisão:** ±1 LSB (~3.2mV)
- **Range:** 0V a 3.3V

## ⚙️ Configuração STM32CubeMX

### ADC1 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Instance | ADC1 | ADC número 1 |
| Clock Prescaler | PCLK2/2 | Clock do ADC |
| Resolution | 10 bits | 0-1023 valores |
| Data Alignment | Right | Bits menos significativos |
| Scan Conversion | Disabled | Single channel |
| Continuous Conversion | Disabled | Uma conversão por trigger |
| Trigger | Software | Trigger por software |

### Channel Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Channel | IN0 | PA0 (Channel 0) |
| Rank | 1 | Primeira conversão |
| Sampling Time | 480 Cycles | Máxima precisão |

### GPIO Configuration:
| Pin | Mode | Description |
|-----|------|-------------|
| PA0 | Analog | ADC1_IN0 input |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure ADC1 em modo single conversion
   - Set PA0 como ADC1_IN0 (Analog mode)
   - Configure resolução para 10 bits
   - Generate code

2. **Hardware Setup:**
   - Conecte potenciômetro entre GND, 3.3V e PA0
   - Verifique que tensão no PA0 não excede 3.3V

3. **Teste:**
   - Compile e execute projeto
   - Use debugger para monitorar variáveis `adc` e `tensao`
   - Varie potenciômetro e observe mudanças

## 📊 Análise de Resultados

### Tabela de Conversão:
| Tensão Input | ADC Value | Tensão Calculada | Erro |
|--------------|-----------|------------------|------|
| 0.00V | 0 | 0.000V | 0mV |
| 0.82V | 256 | 0.826V | ±6mV |
| 1.65V | 512 | 1.651V | ±1mV |
| 2.48V | 768 | 2.477V | ±3mV |
| 3.30V | 1023 | 3.297V | ±3mV |

### Características de Performance:
```c
// Tempo de conversão (aproximado):
// Clock ADC: PCLK2/2 = 84MHz/2 = 42MHz
// Cycles por conversão: 480 + 12 = 492 cycles
// Tempo conversão: 492/42MHz ≈ 11.7μs
// 
// Frequência máxima teórica: ~85kHz
// Frequência do projeto: 2Hz (limitada pelo delay)
```

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Modificar delay:** Alterar frequência de amostragem
2. **LED indicator:** Piscar LED baseado no valor ADC
3. **Threshold detection:** Detectar se tensão > valor específico

### Para Intermediários:
1. **Multiple readings:** Fazer média de várias leituras
2. **Calibração:** Implementar calibração com valores conhecidos
3. **Display:** Mostrar valores em display LCD

### Para Avançados:
1. **Filtering:** Implementar filtro digital passa-baixa
2. **Statistics:** Calcular min, max, média móvel
3. **Non-blocking:** Converter para modo interrupt

## 🐛 Possíveis Problemas

### Leituras incorretas:
- **Tensão > 3.3V:** NUNCA aplicar tensão superior a VDD
- **Ruído:** Usar capacitor de desacoplamento no sinal
- **Impedância alta:** Fonte deve ter baixa impedância

### Valores instáveis:
- **Referência instável:** Verificar alimentação 3.3V
- **Sampling time baixo:** Aumentar tempo de amostragem
- **EMI:** Afastar de fontes de ruído

### Código não funciona:
- **Clock ADC:** Verificar se PCLK2 está habilitado
- **GPIO mode:** Confirmar PA0 em modo Analog
- **Timeout:** Verificar se conversão não está travando

## 🔍 Entendendo o ADC

### Processo de Conversão:
```c
// 1. Sample & Hold: Captura tensão instantânea
// 2. Quantização: Divide tensão em níveis discretos  
// 3. Codificação: Converte nível para código binário
// 4. Disponibiliza resultado no registrador
```

### Fatores que Afetam Precisão:
```c
// Resolução: Determina menor tensão detectável
// Vref: Tensão de referência (estabilidade crítica)
// Sampling time: Tempo para estabilizar entrada
// Noise: Ruído na alimentação e sinal
// Temperature: Drift térmico dos componentes
```

### Cálculos Importantes:
```c
// LSB (Least Significant Bit):
float lsb_voltage = 3.3f / 1024.0f;  // ~3.225mV

// Conversão ADC para tensão:
float voltage = (float)adc_value * lsb_voltage;

// Conversão tensão para ADC:
uint16_t adc_expected = (uint16_t)(voltage / lsb_voltage);

// Erro percentual:
float error_percent = ((voltage_measured - voltage_actual) / voltage_actual) * 100.0f;
```

## 📈 Aplicações Práticas

### Sensor de Luz:
```c
// LDR (Light Dependent Resistor)
uint16_t light_level = adc;
if (light_level < 200)      // Escuro
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
else                        // Claro
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
```

### Monitor de Bateria:
```c
// Divisor de tensão para monitorar bateria
float battery_voltage = tensao * 2.0f;  // Se usar divisor 1:2
if (battery_voltage < 3.0f)
{
    // Bateria baixa - ação necessária
    battery_low_warning();
}
```

### Controle de Velocidade:
```c
// PWM baseado na leitura ADC
uint16_t pwm_duty = (adc * 1000) / 1023;  // 0-1000 range
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
```

## 📚 Próximos Passos

Continue explorando recursos avançados do ADC:
- **[Projeto14 - ADC Simples 2](../Projeto14%20-%20ADC%20Simples%202/):** Versão aprimorada
- **[Projeto15 - Sensor Temperatura](../Projeto15%20-%20Sensor%20de%20temperatura%20interno/):** Sensor interno
- **[Projeto16 - ADC Interrupção](../Projeto16%20-%20ADC%20Interrupcao/):** Modo interrupt

## 📖 Referências

- [STM32F4 ADC Reference Manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN2834: ADC Best Practices](https://www.st.com/resource/en/application_note/cd00211314-how-to-get-the-best-adc-accuracy-in-stm32-microcontrollers-stmicroelectronics.pdf)
- [Getting Started with ADC](https://www.st.com/resource/en/application_note/an4073-how-to-improve-adc-accuracy-when-using-stm32f2-stm32f4-and-stm32f7-series-microcontrollers-stmicroelectronics.pdf)

---

📊 **Dica:** ADC é fundamental para interagir com o mundo analógico. Comece simples e evoluia para aplicações mais complexas!