# Projeto14 - ADC Simples 2 📊📊

![Dificuldade](https://img.shields.io/badge/Dificuldade-Iniciante-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-ADC1%20Multi--Channel-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-35%20min-orange.svg)

## 📋 Descrição

Este projeto evolui do **Projeto13** introduzindo conceitos de **ADC multi-canal** e **scan mode**. Demonstra como ler múltiplas entradas analógicas sequencialmente usando o mesmo ADC, com resolução aumentada para 12 bits e modo contínuo habilitado. Ideal para aplicações que precisam monitorar vários sensores analógicos.

## 🎯 Objetivos de Aprendizado

- Configurar ADC em modo Scan (multi-canal)
- Trabalhar com múltiplos canais ADC simultaneamente
- Usar resolução de 12 bits (maior precisão)
- Implementar leitura sequencial de canais
- Entender diferenças entre single e continuous mode
- Gerenciar arrays de dados ADC

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **2x Potenciômetros:** 10kΩ (para teste)
- **Multímetro** (para verificação)
- **Breadboard grande** (para dois circuitos)
- **Jumpers**

## 📐 Esquema de Ligação

```
Potenciômetro 1   |    STM32F407VET6    |    Potenciômetro 2
============================================================
VCC (3.3V) ------------> 3.3V <----------- VCC (3.3V)
GND --------------------> GND <------------ GND
Wiper (saída) ---------> PA3 (ADC1_IN3)
                                           Wiper (saída) -> PC0 (ADC1_IN10)

Configuração:
- Channel 3 (PA3): Rank 1 - Primeira leitura
- Channel 10 (PC0): Rank 2 - Segunda leitura
```

### Configuração de Pinos
- **PA3:** ADC1_IN3 (Channel 3, Rank 1)
- **PC0:** ADC1_IN10 (Channel 10, Rank 2)

## 💻 Principais Conceitos

### 1. Configuração ADC Multi-Canal
```c
ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Configuração global - DIFERENÇAS do Projeto13
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;        // 12 bits = 0-4095
    hadc1.Init.ScanConvMode = ENABLE;                  // Multi-channel scan
    hadc1.Init.ContinuousConvMode = ENABLE;            // Continuous conversion
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 2;                    // DUAS conversões
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração Canal 3 (PA3) - Rank 1
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;                                  // Primeira conversão
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;   // Sampling time reduzido
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração Canal 10 (PC0) - Rank 2
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 2;                                  // Segunda conversão
    // sConfig.SamplingTime mantém 144 cycles
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Leitura Multi-Canal
```c
// Constante atualizada para 12 bits
#define CONST_TENSAO    0.0032258064516129032258064516129

// Arrays para múltiplos canais
float adc[2];        // Valores ADC dos dois canais
float tensao[2];     // Tensões convertidas

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    
    while (1)
    {
        // Iniciar conversão ADC (continuous mode)
        HAL_ADC_Start(&hadc1);
        
        // Leitura do Canal 3 (PA3) - Rank 1
        HAL_ADC_PollForConversion(&hadc1, 100);
        adc[0] = HAL_ADC_GetValue(&hadc1);
        tensao[0] = adc[0] * CONST_TENSAO;
        
        // Leitura do Canal 10 (PC0) - Rank 2
        HAL_ADC_PollForConversion(&hadc1, 100);
        adc[1] = HAL_ADC_GetValue(&hadc1);
        tensao[1] = adc[1] * CONST_TENSAO;
        
        // Parar ADC
        HAL_ADC_Stop(&hadc1);
        
        // Aguardar próximo ciclo
        HAL_Delay(500);
    }
}
```

### 3. Diferenças do Projeto13
```c
// PROJETO 13 vs PROJETO 14
//
// Resolução:
// P13: ADC_RESOLUTION_10B (0-1023)
// P14: ADC_RESOLUTION_12B (0-4095)
//
// Scan Mode:
// P13: ScanConvMode = DISABLE (single channel)
// P14: ScanConvMode = ENABLE (multi-channel)
//
// Continuous Mode:
// P13: ContinuousConvMode = DISABLE
// P14: ContinuousConvMode = ENABLE
//
// Number of Conversions:
// P13: NbrOfConversion = 1
// P14: NbrOfConversion = 2
//
// Channels:
// P13: ADC_CHANNEL_0 (PA0)
// P14: ADC_CHANNEL_3 (PA3) + ADC_CHANNEL_10 (PC0)
//
// Sampling Time:
// P13: ADC_SAMPLETIME_480CYCLES (high precision)
// P14: ADC_SAMPLETIME_144CYCLES (faster conversion)
```

## 🔍 Funcionalidades

### Características Melhoradas:
- **Resolução:** 12 bits (0-4095) - 4x mais preciso
- **Multi-canal:** 2 canais simultâneos
- **Continuous mode:** Conversão contínua
- **Scan mode:** Varredura automática de canais

### Sequência de Conversão:
1. **Start ADC:** Inicia modo contínuo
2. **Channel 3 (Rank 1):** PA3 → adc[0]
3. **Channel 10 (Rank 2):** PC0 → adc[1]  
4. **Stop ADC:** Para conversões
5. **Repeat:** Novo ciclo após delay

## ⚙️ Configuração STM32CubeMX

### ADC1 Configuration:
| Parâmetro | Projeto13 | Projeto14 | Diferença |
|-----------|-----------|-----------|-----------|
| Resolution | 10 bits | **12 bits** | +4x precisão |
| Scan Conversion | Disabled | **Enabled** | Multi-canal |
| Continuous Conversion | Disabled | **Enabled** | Modo contínuo |
| Number of Conversions | 1 | **2** | Dois canais |
| Sampling Time | 480 cycles | **144 cycles** | Mais rápido |

### Channel Configuration:
| Channel | Pin | Rank | Sampling Time |
|---------|-----|------|---------------|
| IN3 | PA3 | 1 | 144 Cycles |
| IN10 | PC0 | 2 | 144 Cycles |

### GPIO Configuration:
| Pin | Mode | Description |
|-----|------|-------------|
| PA3 | Analog | ADC1_IN3 |
| PC0 | Analog | ADC1_IN10 |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure ADC1 em modo Scan
   - Add channels IN3 e IN10
   - Set resolução para 12 bits
   - Enable continuous conversion
   - Generate code

2. **Hardware Setup:**
   - Conecte dois potenciômetros aos pinos PA3 e PC0
   - Verifique alimentação comum (GND, 3.3V)

3. **Teste:**
   - Compile e execute
   - Use debugger para monitorar arrays `adc[]` e `tensao[]`
   - Varie independentemente cada potenciômetro

## 📊 Análise de Performance

### Comparação de Precisão:
| Resolução | Range | LSB | Precisão |
|-----------|-------|-----|----------|
| 10 bits | 0-1023 | 3.225mV | ±1.6mV |
| **12 bits** | **0-4095** | **0.806mV** | **±0.4mV** |

### Tempo de Conversão:
```c
// Sampling time: 144 cycles (vs 480 no Projeto13)
// Conversion time: 144 + 12 = 156 cycles per channel
// Total time: 156 × 2 = 312 cycles for both channels
// At 42MHz ADC clock: 312/42MHz ≈ 7.4μs total
// 
// Frequência teórica: ~135kHz (muito mais rápido que Projeto13)
```

### Constante de Conversão:
```c
// Para 12 bits:
// CONST_TENSAO = 3.3V / 4096 = 0.0008056640625
// 
// NOTA: O código usa a constante do Projeto13!
// Isso está INCORRETO para 12 bits
// 
// Constante correta seria:
#define CONST_TENSAO_12BIT    0.0008056640625
```

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Corrigir constante:** Usar constante correta para 12 bits
2. **Display values:** Mostrar valores de ambos canais
3. **LED indicators:** LEDs diferentes para cada canal

### Para Intermediários:
1. **More channels:** Adicionar mais canais (até 16)
2. **Calibration:** Implementar calibração individual por canal
3. **Averaging:** Média móvel para cada canal

### Para Avançados:
1. **DMA mode:** Converter para uso com DMA
2. **Interrupt mode:** Usar interrupções ao invés de polling
3. **Differential measurement:** Medição diferencial entre canais

## 🐛 Possíveis Problemas

### Valores incorretos:
- **Constante errada:** Código usa constante do Projeto13 (10 bits)
- **Channel mapping:** Verificar pinos PA3 e PC0 configurados
- **Clock configuration:** Confirmar clocks habilitados

### Performance issues:
- **Conversion time:** Continuous mode pode causar overhead
- **Cross-talk:** Canais podem interferir entre si
- **Sampling rate:** 144 cycles pode ser insuficiente para alta impedância

### Debugging:
```c
// Corrigir constante de conversão:
#define CONST_TENSAO_10BIT    0.0032258064516129032258064516129  // Atual
#define CONST_TENSAO_12BIT    0.0008056640625                    // Correto

// Verificar valores esperados:
// 12 bits: ADC = 2048 → Tensão = 2048 * 0.000806... ≈ 1.65V
// 10 bits: ADC = 512  → Tensão = 512  * 0.003226... ≈ 1.65V
```

## 🔍 Análise do Código

### Problemas Identificados:
1. **Constante incorreta:** Usando constante de 10 bits para dados de 12 bits
2. **Continuous mode desperdiçado:** Para depois de cada leitura
3. **Variáveis float:** Para ADC integer seria mais eficiente

### Versão Corrigida:
```c
// Constante correta para 12 bits
#define CONST_TENSAO_12BIT    (3.3f / 4096.0f)

// Variáveis corretas
uint16_t adc[2];        // ADC values (0-4095)
float tensao[2];        // Voltages (0-3.3V)

// Loop principal corrigido
while (1)
{
    HAL_ADC_Start(&hadc1);
    
    // Canal 1
    HAL_ADC_PollForConversion(&hadc1, 100);
    adc[0] = HAL_ADC_GetValue(&hadc1);
    tensao[0] = (float)adc[0] * CONST_TENSAO_12BIT;
    
    // Canal 2  
    HAL_ADC_PollForConversion(&hadc1, 100);
    adc[1] = HAL_ADC_GetValue(&hadc1);
    tensao[1] = (float)adc[1] * CONST_TENSAO_12BIT;
    
    HAL_ADC_Stop(&hadc1);
    HAL_Delay(500);
}
```

## 📈 Aplicações Práticas

### Sistema de Monitoramento:
```c
// Monitor dois parâmetros simultaneamente
if (tensao[0] > 2.5f)      // Sensor 1: temperatura alta
    temperature_alarm();
    
if (tensao[1] < 0.5f)      // Sensor 2: nível baixo
    level_low_warning();
```

### Controle Dual:
```c
// Controlar dois atuadores baseado em dois sensores
uint16_t pwm1 = (adc[0] * 1000) / 4095;  // PWM1: 0-1000
uint16_t pwm2 = (adc[1] * 1000) / 4095;  // PWM2: 0-1000

__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm1);
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm2);
```

### Data Logger:
```c
// Armazenar dados de múltiplos sensores
typedef struct {
    uint32_t timestamp;
    uint16_t sensor1;
    uint16_t sensor2;
} sensor_data_t;

sensor_data_t data_buffer[100];
uint8_t buffer_index = 0;

// Armazenar leitura
data_buffer[buffer_index].timestamp = HAL_GetTick();
data_buffer[buffer_index].sensor1 = adc[0];
data_buffer[buffer_index].sensor2 = adc[1];
buffer_index = (buffer_index + 1) % 100;
```

## 📚 Próximos Passos

Continue explorando recursos avançados:
- **[Projeto15 - Sensor Temperatura](../Projeto15%20-%20Sensor%20de%20temperatura%20interno/):** Sensor interno STM32
- **[Projeto16 - ADC Interrupção](../Projeto16%20-%20ADC%20Interrupcao/):** Modo interrupt
- **[Projeto17 - ADC com DMA](../Projeto17%20-%20ADC%20com%20DMA/):** High-speed sampling

## 📖 Referências

- [STM32F4 ADC Multi-Channel](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN2834: Multi-Channel ADC](https://www.st.com/resource/en/application_note/cd00211314-how-to-get-the-best-adc-accuracy-in-stm32-microcontrollers-stmicroelectronics.pdf)
- [Scan Mode Configuration](https://www.st.com/resource/en/application_note/an4073-how-to-improve-adc-accuracy-when-using-stm32f2-stm32f4-and-stm32f7-series-microcontrollers-stmicroelectronics.pdf)

---

📊📊 **Dica:** Multi-channel ADC é essencial para sistemas complexos. Atenção às constantes de conversão e timing!