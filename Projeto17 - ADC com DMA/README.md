# Projeto17 - ADC com DMA 🚀

![Dificuldade](https://img.shields.io/badge/Dificuldade-Avançado-red.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-ADC1%20+%20DMA2-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-45%20min-orange.svg)

## 📋 Descrição

Este projeto demonstra o **ADC com DMA (Direct Memory Access)** para aquisição de dados de alta velocidade sem sobrecarregar a CPU. Utiliza DMA2 para transferir automaticamente dados do ADC para memória, permitindo sampling contínuo de múltiplos canais com zero intervenção da CPU durante as transferências.

## 🎯 Objetivos de Aprendizado

- Configurar DMA para ADC (Direct Memory Access)
- Implementar aquisição de dados de alta velocidade
- Usar buffer circular para continuous sampling
- Entender DMA channels e streams
- Trabalhar com multi-channel DMA transfer
- Sistema completamente não-bloqueante

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **2x Potenciômetros:** 10kΩ (para teste)
- **2x LEDs** (indicação visual)
- **2x Resistores:** 330Ω (para LEDs)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
Potenciômetro 1   |    STM32F407VET6    |    Potenciômetro 2
============================================================
VCC (3.3V) ------------> 3.3V <----------- VCC (3.3V)
GND --------------------> GND <------------ GND
Wiper (saída) ---------> PA3 (ADC1_IN3)
                                           Wiper (saída) -> PC0 (ADC1_IN10)

LEDs:
LED1 Anodo ------------> PA4 (LED1_Pin)
LED1 Catodo -----------> GND (via 330Ω)
LED2 Anodo ------------> PA5 (LED2_Pin)  
LED2 Catodo -----------> GND (via 330Ω)
```

### Configuração de Pinos
- **PA3:** ADC1_IN3 (Channel 3, Rank 1)
- **PC0:** ADC1_IN10 (Channel 10, Rank 2)
- **PA4:** LED1 (output)
- **PA5:** LED2 (output)

## 💻 Principais Conceitos

### 1. Configuração ADC + DMA
```c
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Configuração ADC
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;        // 12 bits = 0-4095
    hadc1.Init.ScanConvMode = ENABLE;                  // Multi-channel scan
    hadc1.Init.ContinuousConvMode = ENABLE;            // Continuous conversion
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 2;                    // 2 channels
    hadc1.Init.DMAContinuousRequests = ENABLE;         // !! DMA HABILITADO !!
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Canal 3 (PA3) - Rank 1
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_112CYCLES;   // Sampling otimizado
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Canal 10 (PC0) - Rank 2
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 2;
    // Mantém mesmo sampling time
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Configuração DMA
```c
static void MX_DMA_Init(void)
{
    // Habilitar clock do DMA2
    __HAL_RCC_DMA2_CLK_ENABLE();
    
    // Configurar interrupt do DMA
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

// Configuração DMA feita automaticamente pelo CubeMX:
// - DMA2 Stream 0
// - Channel 0 
// - Peripheral to Memory
// - Circular mode
// - Half word (16-bit) data size
```

### 3. Inicialização e Uso
```c
// Buffer para dados ADC (2 canais)
uint32_t adc[2];  // adc[0] = Channel 3, adc[1] = Channel 10

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();      // !! INICIALIZAR DMA ANTES DO ADC !!
    MX_ADC1_Init();
    
    // Iniciar ADC com DMA - TOTALMENTE NÃO-BLOQUEANTE!
    HAL_ADC_Start_DMA(&hadc1, adc, sizeof(adc));
    
    while (1)
    {
        // CPU COMPLETAMENTE LIVRE!
        // DMA transfere dados automaticamente para array 'adc'
        // Dados sempre atualizados em background
        
        // Pode processar outras tarefas sem afetar aquisição ADC
        HAL_Delay(100);
        // Processar outros algoritmos
        // Comunicação serial
        // Interface gráfica
        // etc...
    }
}
```

### 4. Callback de Conversão Completa
```c
// Callback executado quando DMA completa transferência
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // Threshold: 1250 ≈ 1V (1250/4095 * 3.3V ≈ 1.0V)
    if (adc[0] >= 1250)  // Se canal 3 > 1V
    {
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
    }
    
    // Dados em adc[0] e adc[1] sempre atualizados pelo DMA
}
```

## 🔍 Funcionalidades

### Características do DMA:
- **DMA2 Stream 0:** Dedicado para ADC1
- **Circular mode:** Buffer atualizado continuamente
- **Multi-channel:** Transfere dados de 2 canais sequencialmente
- **Zero CPU overhead:** CPU não participa das transferências

### Vantagens sobre Interrupt:
- **CPU 100% livre:** Zero overhead durante transferências
- **Alta velocidade:** Limitada apenas pela velocidade do ADC
- **Buffer automático:** Dados sempre disponíveis na memória
- **Escalabilidade:** Pode facilmente expandir para mais canais

## ⚙️ Configuração STM32CubeMX

### ADC1 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Resolution | 12 bits | Máxima precisão |
| Scan Conversion | Enabled | Multi-channel |
| Continuous Conversion | Enabled | Conversão contínua |
| **DMA Continuous Requests** | **Enabled** | Chave para DMA |
| Number of Conversions | 2 | Dois canais |

### DMA Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| DMA | DMA2 | Controller para ADC1 |
| Stream | Stream 0 | Stream dedicado |
| Channel | Channel 0 | Canal ADC1 |
| Direction | Peripheral to Memory | ADC → RAM |
| Mode | Circular | Buffer circular |
| Data Width | Half Word (16-bit) | Para valores 12-bit |

### NVIC Settings:
| Interrupt | Priority | Description |
|-----------|----------|-------------|
| DMA2 stream0 global interrupt | Enabled | Para callbacks |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure ADC1 com 2 canais
   - **Enable DMA Continuous Requests** (crítico!)
   - Add DMA: DMA2 Stream 0 Channel 0
   - Set DMA to Circular mode
   - Generate code

2. **Hardware Setup:**
   - Conecte dois potenciômetros aos pinos PA3 e PC0
   - Conecte LEDs aos pinos PA4 e PA5

3. **Teste:**
   - Compile e execute
   - Varie potenciômetro no PA3
   - LEDs acendem quando tensão > ~1V

## 📊 Análise de Performance

### DMA vs Outras Técnicas:
| Método | CPU Usage | Max Sampling Rate | Complexity |
|--------|-----------|-------------------|------------|
| Polling | 100% | ~10kHz | Baixa |
| Interrupt | ~20% | ~50kHz | Média |
| **DMA** | **~0%** | **~85kHz** | **Alta** |

### Timing Analysis:
```c
// ADC Configuration:
// - Clock: 42MHz (PCLK2/2)  
// - Sampling: 112 cycles
// - Conversion: 12 cycles
// - Total per channel: 124 cycles
// - 2 channels: 248 cycles total
// - Sampling rate: 42MHz / 248 ≈ 169kHz per sequence
// - Effective rate per channel: 169kHz / 2 ≈ 84.5kHz
```

### Memory Usage:
```c
// Buffer ADC: 2 × 4 bytes = 8 bytes (desprezível)
// DMA overhead: ~100 bytes de configuração
// Economia: Não precisa de buffers intermediários ou ISRs pesadas
```

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Mais canais:** Expandir para 4-6 canais
2. **Threshold ajustável:** Usar segundo canal para ajustar threshold
3. **Data visualization:** Enviar dados via UART para plotagem

### Para Intermediários:
1. **Circular buffer:** Implementar buffer histórico
2. **Signal processing:** FFT, filtros digitais em tempo real
3. **Multiple ADCs:** Usar ADC1 + ADC2 simultaneamente

### Para Avançados:
1. **Double buffering:** Usar meio-buffer callbacks
2. **Timer-triggered:** Usar timer para trigger ao invés de continuous
3. **Memory-to-peripheral:** DMA para DAC simultaneamente

## 🐛 Possíveis Problemas

### DMA não funciona:
- **DMA Continuous Requests = DISABLE:** Deve estar ENABLED
- **DMA init antes ADC:** MX_DMA_Init() deve vir antes de MX_ADC1_Init()
- **Buffer size incorreto:** sizeof(adc) deve corresponder ao número de canais

### Performance issues:
- **Cache coherency:** Em MCUs com cache, invalidar cache do buffer
- **Memory alignment:** Buffer deve estar alinhado para DMA
- **Overrun:** Se processamento não acompanha aquisição

### Debugging Tips:
```c
// Verificar se DMA está transferindo
volatile uint32_t dma_transfer_count = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    dma_transfer_count++;  // Incrementa a cada transfer completa
    
    // Debug: verificar valores
    printf("CH3: %lu, CH10: %lu\n", adc[0], adc[1]);
}

// No main loop, verificar se counter aumenta
```

## 🔍 Configuração Avançada

### Buffer Circular Duplo:
```c
#define BUFFER_SIZE 100
uint32_t adc_buffer_a[2 * BUFFER_SIZE];  // Double buffer
uint32_t adc_buffer_b[2 * BUFFER_SIZE];
volatile uint8_t active_buffer = 0;

// Callback de meio-buffer
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    // Processar primeira metade enquanto DMA preenche segunda
    process_adc_data(&adc_buffer_a[0], BUFFER_SIZE);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // Processar segunda metade enquanto DMA preenche primeira
    process_adc_data(&adc_buffer_a[BUFFER_SIZE], BUFFER_SIZE);
}
```

### Multi-ADC com DMA:
```c
// Configurar ADC1 e ADC2 simultaneamente
HAL_ADC_Start_DMA(&hadc1, adc1_buffer, BUFFER_SIZE);
HAL_ADC_Start_DMA(&hadc2, adc2_buffer, BUFFER_SIZE);

// Total throughput: 2 × 84.5kHz = 169kHz samples/sec
```

## 📈 Aplicações Práticas

### Data Acquisition System:
```c
#define SAMPLE_RATE_HZ      10000
#define SAMPLES_PER_SEC     (SAMPLE_RATE_HZ * 2)  // 2 channels
uint32_t acquisition_buffer[SAMPLES_PER_SEC];

void start_data_acquisition(void)
{
    HAL_ADC_Start_DMA(&hadc1, acquisition_buffer, SAMPLES_PER_SEC);
}
```

### Real-time Signal Processing:
```c
// Processar sinais em tempo real
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    float ch1_voltage = (float)adc[0] * 3.3f / 4095.0f;
    float ch2_voltage = (float)adc[1] * 3.3f / 4095.0f;
    
    // Filtro passa-baixa em tempo real
    static float filtered_ch1 = 0.0f;
    filtered_ch1 = filtered_ch1 * 0.9f + ch1_voltage * 0.1f;
    
    // Detectar cruzamento de threshold
    if (filtered_ch1 > 1.5f && ch2_voltage < 0.5f)
    {
        trigger_alarm();
    }
}
```

### Oscilloscope Function:
```c
#define OSC_BUFFER_SIZE 1000
uint32_t oscilloscope_data[OSC_BUFFER_SIZE * 2];  // 2 channels

void start_oscilloscope_capture(void)
{
    HAL_ADC_Start_DMA(&hadc1, oscilloscope_data, OSC_BUFFER_SIZE * 2);
}

void send_oscilloscope_data_to_pc(void)
{
    // Enviar dados via USB ou UART para PC
    for (int i = 0; i < OSC_BUFFER_SIZE; i++)
    {
        printf("%lu,%lu\n", oscilloscope_data[i*2], oscilloscope_data[i*2+1]);
    }
}
```

## 📚 Próximos Passos

Continue explorando técnicas avançadas de aquisição:
- **[Projeto18 - Frequência Amostragem](../Projeto18%20-%20Frequencia%20de%20amostrage%201/):** Timer-triggered sampling
- **[Projeto19 - ADC Simultâneo](../Projeto19%20-%20Simult%20ADC/):** Multi-ADC synchronization
- **Projetos DAC:** Combine ADC+DMA com DAC para processamento completo

## 📖 Referências

- [STM32F4 DMA Controller](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4031: DMA Best Practices](https://www.st.com/resource/en/application_note/an4031-using-the-dma-controller-on-stm32f2-stm32f4-and-stm32f7-series-microcontrollers-stmicroelectronics.pdf)
- [ADC+DMA High Speed Sampling](https://www.st.com/resource/en/application_note/an3116-stm32s-adc-modes-and-their-applications-stmicroelectronics.pdf)

---

🚀 **Dica:** DMA é o método mais eficiente para aquisição de dados de alta velocidade. Libera totalmente a CPU para outras tarefas!