# Projeto24 - DAC 05

## Descrição do Projeto
Este projeto demonstra o **gerador de onda triangular** por hardware do DAC STM32F407VET6. O sistema utiliza o recurso interno **Triangle Wave Generator** para produzir automaticamente uma onda triangular de alta frequência sem necessidade de buffers ou DMA.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 80MHz (PLL)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino DAC:** PA4 (DAC_OUT1)
- **Recursos:** Timer 6 + Hardware Triangle Generator

## Triangle Wave Generator

### Características do Hardware
- **Geração automática:** Circuito interno do DAC
- **Amplitude configurável:** 12 níveis diferentes
- **Resolução:** 12 bits (4096 níveis)
- **Trigger externo:** Timer 6 para controle de frequência
- **Zero CPU overhead:** Operação completamente independente

### Vantagens do Hardware Generator
- **Alta performance:** Sem uso de CPU ou memória
- **Precisão temporal:** Sincronização por hardware
- **Baixo jitter:** Timing determinístico
- **Simplicidade:** Configuração direta, sem buffers

## Configuração do Triangle Generator

### Inicialização Específica
```c
static void MX_DAC_Init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração básica do canal
    sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;     // Timer 6 trigger
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configurar Triangle Wave Generator
    if (HAL_DACEx_TriangleWaveGenerate(&hdac, DAC_CHANNEL_1, 
                                       DAC_TRIANGLEAMPLITUDE_4095) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### Função HAL_DACEx_TriangleWaveGenerate
```c
HAL_StatusTypeDef HAL_DACEx_TriangleWaveGenerate(DAC_HandleTypeDef *hdac,
                                                 uint32_t Channel,
                                                 uint32_t Amplitude);
```

**Parâmetros:**
- **hdac:** Handle do DAC
- **Channel:** DAC_CHANNEL_1 ou DAC_CHANNEL_2
- **Amplitude:** Amplitude máxima da onda triangular

## Amplitudes Disponíveis

### Opções de Amplitude
```c
DAC_TRIANGLEAMPLITUDE_1     // Amplitude = 1 LSB
DAC_TRIANGLEAMPLITUDE_3     // Amplitude = 3 LSB
DAC_TRIANGLEAMPLITUDE_7     // Amplitude = 7 LSB
DAC_TRIANGLEAMPLITUDE_15    // Amplitude = 15 LSB
DAC_TRIANGLEAMPLITUDE_31    // Amplitude = 31 LSB
DAC_TRIANGLEAMPLITUDE_63    // Amplitude = 63 LSB
DAC_TRIANGLEAMPLITUDE_127   // Amplitude = 127 LSB
DAC_TRIANGLEAMPLITUDE_255   // Amplitude = 255 LSB
DAC_TRIANGLEAMPLITUDE_511   // Amplitude = 511 LSB
DAC_TRIANGLEAMPLITUDE_1023  // Amplitude = 1023 LSB
DAC_TRIANGLEAMPLITUDE_2047  // Amplitude = 2047 LSB
DAC_TRIANGLEAMPLITUDE_4095  // Amplitude = 4095 LSB (máxima)
```

### Projeto Atual: Amplitude Máxima
```c
DAC_TRIANGLEAMPLITUDE_4095  // 4095 LSB = Range completo (0V a 3.3V)
```

### Conversão para Tensão
```c
Amplitude 4095 LSB = 4095 / 4095 × 3.3V = 3.3V (peak-to-peak)
Range de saída: 0V a 3.3V
Ponto médio: 1.65V
```

## Configuração do Timer 6

### Parâmetros Alta Velocidade
```c
static void MX_TIM6_Init(void)
{
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 8-1;         // Prescaler = 8
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 10;             // Período = 10
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    // Master Output Trigger
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
}
```

### Cálculo da Frequência de Trigger
**Com PLL @ 80MHz:**
```
Timer Clock = APB1 × 2 = 40MHz × 2 = 80MHz
Trigger Frequency = 80MHz / (8 × 11) = 909.09kHz
Período do trigger = ~1.1µs
```

### Frequência da Onda Triangular
A frequência da onda triangular depende de como o hardware interno processa os triggers:
```c
Cada trigger incrementa/decrementa o contador interno
Para amplitude 4095: precisa de 2 × 4095 = 8190 triggers para um ciclo completo
Frequência triangular = 909.09kHz / 8190 ≈ 111Hz
Período da triangular ≈ 9ms
```

## Clock Configuration 80MHz

### PLL Settings
```c
void SystemClock_Config(void)
{
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;      // HSI/8 = 2MHz
    RCC_OscInitStruct.PLL.PLLN = 80;     // 2MHz × 80 = 160MHz VCO
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // 160MHz/2 = 80MHz SYSCLK
    RCC_OscInitStruct.PLL.PLLQ = 4;      // 160MHz/4 = 40MHz USB clock
}
```

### Bus Clocks
```c
SYSCLK = 80MHz
HCLK = 80MHz     (AHB)
APB1 = 40MHz     (APB1CLKDivider = RCC_HCLK_DIV2)
APB2 = 80MHz     (APB2CLKDivider = RCC_HCLK_DIV1)

Timer 6 Clock = APB1 × 2 = 80MHz (quando APB1 prescaler ≠ 1)
```

## Funcionamento Interno

### Hardware Triangle Generator
O STM32 possui um **counter interno** que:
1. **Conta crescente:** 0 → amplitude máxima
2. **Conta decrescente:** amplitude máxima → 0
3. **Repete ciclicamente:** Formando onda triangular
4. **Trigger controlado:** Timer define velocidade de contagem

### Diagrama de Funcionamento
```
Timer 6 TRGO ──→ DAC Triangle Counter ──→ DAC Output
     ↓                    ↓                    ↓
   909kHz            Up/Down Counter        Triangle Wave
                    (0 ↔ 4095)              (~111Hz)
```

### Processo Detalhado
1. **Timer 6 gera trigger** a cada 1.1µs
2. **Counter interno** incrementa/decrementa
3. **Valor do counter** é aplicado ao DAC
4. **Saída analógica** segue o valor digital
5. **Ciclo completo** quando counter volta ao início

## Forma de Onda Resultante

### Características da Onda Triangular
```
Tensão (V)
3.3 |    /\     /\     /\
    |   /  \   /  \   /  \
1.65|  /    \ /    \ /    \
    | /      \      \      \
0.0 |/        \      \     \
    +----------+-----+-----+---- Tempo
    0         9ms   18ms   27ms
```

### Parâmetros da Onda
- **Amplitude:** 3.3V peak-to-peak
- **Offset:** 1.65V (centro da escala)
- **Frequência:** ~111Hz
- **Período:** ~9ms
- **Rise time:** ~4.5ms
- **Fall time:** ~4.5ms
- **Duty cycle:** 50% (simétrica)

## Applications Práticas

### 1. Gerador de Rampa para ADC
```c
// Testar linearidade de ADC com rampa triangular precisa
void test_adc_linearity_with_triangle(void)
{
    // Triangle DAC em PA4 → ADC input
    // Comparar valores lidos vs esperados
    // Detectar non-linearity do ADC
}
```

### 2. Sweep Generator
```c
// Alterar frequência dinamicamente
void set_triangle_frequency(uint32_t frequency_hz)
{
    // Calcular período do timer para frequência desejada
    uint32_t triggers_per_cycle = 2 * 4095;  // Up + Down
    uint32_t trigger_freq = frequency_hz * triggers_per_cycle;
    uint32_t timer_period = 80000000 / (8 * trigger_freq);
    
    if (timer_period > 0 && timer_period < 65536)
    {
        __HAL_TIM_SET_AUTORELOAD(&htim6, timer_period);
    }
}
```

### 3. Controle de Amplitude
```c
// Diferentes amplitudes para diferentes aplicações
typedef enum {
    AMPLITUDE_SMALL = DAC_TRIANGLEAMPLITUDE_255,    // ~0.2V pp
    AMPLITUDE_MEDIUM = DAC_TRIANGLEAMPLITUDE_1023,  // ~0.8V pp
    AMPLITUDE_LARGE = DAC_TRIANGLEAMPLITUDE_4095    // 3.3V pp
} triangle_amplitude_t;

void set_triangle_amplitude(triangle_amplitude_t amplitude)
{
    HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
    HAL_DACEx_TriangleWaveGenerate(&hdac, DAC_CHANNEL_1, amplitude);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
}
```

### 4. Calibração de Instrumentos
```c
// Gerar sinal de referência para calibração
void generate_calibration_triangle(float peak_voltage)
{
    // Calcular amplitude necessária
    uint32_t amplitude_code = (uint32_t)(peak_voltage * 4095 / 3.3);
    
    // Selecionar amplitude mais próxima disponível
    uint32_t hw_amplitude = select_closest_amplitude(amplitude_code);
    
    HAL_DACEx_TriangleWaveGenerate(&hdac, DAC_CHANNEL_1, hw_amplitude);
}
```

## Noise Generator

### Triangle + Noise Mode
O STM32 também suporta **Noise Generator**:
```c
// Em vez de triangle, usar noise generator
HAL_StatusTypeDef HAL_DACEx_NoiseWaveGenerate(DAC_HandleTypeDef *hdac,
                                              uint32_t Channel,
                                              uint32_t Amplitude);

// Opções de noise amplitude (mesmo que triangle)
DAC_LFSRUNMASK_BIT0      // 1-bit noise
DAC_LFSRUNMASK_BITS1_0   // 2-bit noise
// ... até
DAC_LFSRUNMASK_BITS11_0  // 12-bit noise
```

## Comparação com Outros Métodos

### Hardware Triangle vs Software
| Aspecto | Hardware Triangle | Software (DMA+Buffer) |
|---------|-------------------|----------------------|
| **CPU Usage** | 0% | 0% (após setup) |
| **Memory** | 0 bytes | Vários KB |
| **Flexibility** | Limitada | Total |
| **Precision** | Alta | Dependente da resolução |
| **Setup** | Simples | Complexo |
| **Frequency Range** | Alta | Limitada pelo DMA |

### Hardware Triangle vs Direct Control
| Aspecto | Hardware Triangle | Direct Control |
|---------|-------------------|----------------|
| **CPU Usage** | 0% | Alta |
| **Timing** | Perfeito | Com jitter |
| **Frequency** | Muito alta | Limitada |
| **Control** | Limitado | Total |
| **Reliability** | Alta | Dependente do código |

## Troubleshooting

### Problemas Comuns

#### 1. Frequência Muito Alta/Baixa
**Sintomas:** Onda muito rápida ou muito lenta
**Solução:** Ajustar prescaler e período do Timer 6
```c
// Para frequências mais baixas: aumentar prescaler ou período
htim6.Init.Prescaler = 80-1;  // Prescaler maior
htim6.Init.Period = 100;      // Período maior
```

#### 2. Amplitude Incorreta
**Sintomas:** Amplitude menor que esperada
**Solução:** Verificar configuração de amplitude
```c
// Certificar que está usando amplitude correta
HAL_DACEx_TriangleWaveGenerate(&hdac, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_4095);
```

#### 3. Onda Não Aparece
**Sintomas:** Saída sempre zero ou constante
**Solução:** Verificar inicialização
```c
// Ordem correta de inicialização
HAL_TIM_Base_Start(&htim6);           // Timer primeiro
HAL_DAC_Start(&hdac, DAC_CHANNEL_1);  // DAC depois
```

### Debug e Verificação
```c
// Verificar se timer está rodando
uint32_t timer_count = __HAL_TIM_GET_COUNTER(&htim6);

// Verificar configuração DAC
DAC_HandleTypeDef *hdac_check = &hdac;

// Medir frequência com osciloscópio
// Verificar amplitude com multímetro RMS
```

## Medição e Caracterização

### Equipamentos Necessários
- **Osciloscópio:** Para visualizar forma de onda
- **Frequency counter:** Para medir frequência precisa
- **Multímetro RMS:** Para amplitude e offset
- **Spectrum analyzer:** Para análise harmônica
- **Function generator:** Para comparação

### Parâmetros de Medição
```c
Frequência: ~111Hz ±5%
Amplitude: 3.3V pp ±2%
Offset: 1.65V DC ±1%
Rise time: Linear (não exponencial)
Linearidade: <1% típico
THD: <0.1% (excelente linearidade)
```

## Conceitos Aprendidos
1. **Hardware waveform generation** - Geração por circuito interno
2. **Triangle wave synthesis** - Síntese de onda triangular
3. **Autonomous operation** - Operação independente da CPU
4. **High-frequency generation** - Geração de alta frequência
5. **Timer-DAC synchronization** - Sincronização timer-DAC
6. **Zero-overhead signals** - Sinais sem overhead de processamento
7. **Hardware vs software trade-offs** - Comparação de implementações

## Melhorias Sugeridas
1. **Frequency control** - Interface para controle de frequência
2. **Amplitude selection** - Seleção dinâmica de amplitude
3. **Dual channel** - Usar ambos os canais DAC
4. **External trigger** - Trigger por sinal externo
5. **Noise mode** - Alternar entre triangle e noise
6. **Offset control** - Controle independente de offset

## Hardware Recomendado
- **High-speed scope:** Para frequências altas
- **Precision DVM:** Para medições DC precisas
- **Low-pass filter:** Para remover componentes de alta frequência
- **Buffer amplifier:** Para drive de cargas pesadas
- **Impedance matching:** Para cargas específicas

## Observações Importantes
- **Hardware limitation:** Apenas 12 opções de amplitude
- **Frequency calculation:** Complexa devido ao algoritmo interno
- **Temperature stability:** Hardware é mais estável que software
- **Power consumption:** Baixo consumo comparado a métodos software
- **Synchronization:** Perfeita sincronização por hardware
- **Predictable timing:** Sem jitter ou variações temporais