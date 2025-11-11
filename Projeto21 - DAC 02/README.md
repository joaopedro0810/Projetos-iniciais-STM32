# Projeto21 - DAC 02

## Descrição do Projeto
Este projeto demonstra o uso **dual-channel** do DAC do STM32F407VET6, gerando **rampas sincronizadas** em ambos os canais. O sistema produz sinais dente-de-serra idênticos nas saídas PA4 e PA5, mostrando aplicações avançadas de conversão D/A com múltiplos canais.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 80MHz (PLL)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pinos DAC:** PA4 (DAC_OUT1) e PA5 (DAC_OUT2)

## DAC Dual Channel

### Características do Sistema
- **Resolução:** 12 bits (4096 níveis) por canal
- **Canais simultâneos:** 2 canais independentes
- **Sincronização:** Ambos os canais atualizados simultaneamente
- **Forma de onda:** Rampa dente-de-serra (0V a 3.3V)
- **Frequência:** ~49Hz (período de ~20.48s)

### Pinos Utilizados
```c
DAC_OUT1 → PA4  (Canal 1)
DAC_OUT2 → PA5  (Canal 2)
```

## Cálculo da Frequência da Rampa

### Parâmetros do Sistema
```c
Delay entre amostras: 5ms
Número de amostras: 4096 (0 a 4095)
Período total: 4096 × 5ms = 20.48s
Frequência: 1 / 20.48s ≈ 49Hz
```

### Taxa de Atualização
```c
Sample rate: 1 / 5ms = 200 amostras/segundo
Bandwidth: 200Hz / 2 = 100Hz (Nyquist)
```

## Configuração Dual Channel

### Inicialização dos Dois Canais
```c
static void MX_DAC_Init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração idêntica para ambos os canais
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    
    // Canal 1 (PA4)
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Canal 2 (PA5)
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### Inicialização no Main
```c
int main(void)
{
    // Inicializações...
    
    // Iniciar ambos os canais
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    
    // Loop principal...
}
```

## Geração de Rampa Sincronizada

### Loop Principal
```c
uint16_t dac1 = 0;
uint16_t dac2 = 0;

while (1)
{
    // Atualizar ambos os canais simultaneamente
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac2);
    
    // Incrementar contadores
    dac1++;
    dac2++;
    
    // Reset quando atingir o máximo
    if ((dac1 > 4095) && (dac2 > 4095))
    {
        dac1 = 0;
        dac2 = 0;
    }
    
    HAL_Delay(5);  // 5ms entre amostras
}
```

### Análise do Algoritmo

#### 1. Sincronização Perfeita
- **Mesmo valor:** dac1 = dac2 em todo momento
- **Atualização simultânea:** Ambos os canais na mesma iteração
- **Reset conjunto:** Ambos zerados simultaneamente

#### 2. Forma de Onda Gerada
```
Tensão (V)
3.3 |     /|     /|     /|
    |    / |    / |    / |
1.65|   /  |   /  |   /  |
    |  /   |  /   |  /   |
0.0 | /    | /    | /    |
    +------+------+------+---- Tempo
    0     20.48s  40.96s
```

## Clock Configuration com PLL

### PLL Configuration
```c
void SystemClock_Config(void)
{
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;      // Divisor de entrada
    RCC_OscInitStruct.PLL.PLLN = 80;     // Multiplicador
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // Divisor principal
    RCC_OscInitStruct.PLL.PLLQ = 4;      // Divisor USB/SDIO
}
```

### Cálculo dos Clocks
```
VCO = (HSI / PLLM) × PLLN = (16MHz / 8) × 80 = 160MHz
SYSCLK = VCO / PLLP = 160MHz / 2 = 80MHz
```

### Divisores de Bus
```c
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;   // AHB = 80MHz
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;    // APB1 = 40MHz
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;    // APB2 = 80MHz
```

**Clocks resultantes:**
- **SYSCLK:** 80MHz
- **AHB:** 80MHz
- **APB1:** 40MHz (Clock do DAC)
- **APB2:** 80MHz

## Aplicações Práticas

### 1. Geração de Sinais de Teste
```c
// Gerar dois sinais de teste para calibração
// Canal 1: Rampa crescente
// Canal 2: Rampa decrescente
while (1)
{
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 4095 - dac1);
    
    dac1++;
    if (dac1 > 4095) dac1 = 0;
    
    HAL_Delay(5);
}
```

### 2. Controle de Motores Stepper
```c
// Gerar sinais de controle para motor stepper
uint16_t sine_table[256];  // Tabela pré-calculada
uint8_t phase_a = 0;
uint8_t phase_b = 64;      // 90° defasado

while (1)
{
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, sine_table[phase_a]);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, sine_table[phase_b]);
    
    phase_a++;
    phase_b++;
    
    HAL_Delay(10);
}
```

### 3. Gerador de Função Duplo
```c
// Gerar duas formas de onda diferentes
typedef enum {
    WAVEFORM_SINE,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH
} waveform_t;

void generate_dual_waveform(waveform_t wave1, waveform_t wave2, uint16_t phase)
{
    static uint16_t index = 0;
    
    uint16_t value1 = get_waveform_value(wave1, index);
    uint16_t value2 = get_waveform_value(wave2, (index + phase) % 4096);
    
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value2);
    
    index++;
    if (index >= 4096) index = 0;
}
```

### 4. Sistema I/Q (Quadratura)
```c
// Gerar sinais I e Q para aplicações RF
void generate_iq_signals(uint16_t frequency_step)
{
    static uint16_t phase = 0;
    
    // I = cos(φ), Q = sin(φ)
    uint16_t i_value = (uint16_t)(2048 + 2047 * cosf(2 * M_PI * phase / 4096));
    uint16_t q_value = (uint16_t)(2048 + 2047 * sinf(2 * M_PI * phase / 4096));
    
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, i_value);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, q_value);
    
    phase += frequency_step;
    if (phase >= 4096) phase = 0;
}
```

## Análise de Performance

### Timing do Sistema
```c
Tempo por iteração: ~5ms + tempo de execução
Tempo de execução: ~10µs (estimativa)
Jitter: ±1ms (devido ao HAL_Delay)
```

### Limitações
- **Resolução temporal:** Limitada pelo HAL_Delay(5)
- **Jitter:** Variação no tempo de amostragem
- **Frequência máxima:** ~100Hz devido ao delay
- **Sincronização:** Não é hardware-sincronizada

## Melhorias e Otimizações

### 1. Uso de Timer para Precisão
```c
// Usar timer para timing preciso
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac1);
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac2);
        
        dac1++;
        dac2++;
        
        if ((dac1 > 4095) && (dac2 > 4095))
        {
            dac1 = 0;
            dac2 = 0;
        }
    }
}
```

### 2. DMA para Performance
```c
// Usar DMA para transferência automática
uint16_t waveform_buffer[2][4096];  // Buffer para ambos os canais

// Configurar DMA dual para DAC
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)waveform_buffer[0], 4096, DAC_ALIGN_12B_R);
HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)waveform_buffer[1], 4096, DAC_ALIGN_12B_R);
```

### 3. Sincronização Hardware
```c
// Usar trigger comum para ambos os canais
sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;  // Timer 6 trigger
```

### 4. Diferentes Formas de Onda
```c
typedef struct {
    uint16_t amplitude;
    uint16_t offset;
    uint16_t phase;
    waveform_t type;
} channel_config_t;

channel_config_t ch1_config = {2047, 2048, 0, WAVEFORM_SINE};
channel_config_t ch2_config = {1023, 2048, 1024, WAVEFORM_TRIANGLE};
```

## Medição e Análise

### Equipamentos Necessários
- **Osciloscópio dual-channel:** Para ver ambos os sinais
- **Gerador de funções:** Para comparação
- **Analisador de espectro:** Para análise harmônica
- **Multímetro true-RMS:** Para medições AC

### Parâmetros de Medição
```c
Frequência fundamental: ~49Hz
Amplitude: 0V a 3.3V (6.6Vpp)
Rise time: 5ms × 4096 = 20.48s
Linearidade: Verificar com osciloscópio
THD: Analisar harmônicos
```

### Verificações Importantes
1. **Sincronização:** Ambos os canais devem estar em fase
2. **Linearidade:** Rampa deve ser linear
3. **Amplitude:** 0V a 3.3V em cada canal
4. **Repetitividade:** Rampas devem ser idênticas
5. **Ruído:** Verificar clean da rampa

## Troubleshooting

### Problemas Comuns

#### 1. Canais Não Sincronizados
**Sintomas:** Defasagem entre os canais
**Solução:** Verificar ordem de atualização no código

#### 2. Rampa Não Linear
**Sintomas:** Degraus ou distorção na rampa
**Solução:** Verificar delay e sequência de incremento

#### 3. Range Incorreto
**Sintomas:** Amplitude diferente do esperado
**Solução:** Verificar VDDA e configuração do buffer

## Conceitos Aprendidos
1. **DAC dual-channel** - Operação simultânea de dois canais
2. **Sincronização de sinais** - Coordenação temporal
3. **Geração de rampa** - Sinal dente-de-serra
4. **Clock PLL** - Configuração para 80MHz
5. **Timing preciso** - Controle temporal de sinais
6. **Buffer output** - Capacidade de drive para dois canais
7. **Aplicações dual-DAC** - Casos de uso práticos

## Melhorias Sugeridas
1. **Timer-based timing** - Substituir HAL_Delay por timer
2. **DMA operation** - Usar DMA para performance
3. **Waveform tables** - Implementar diferentes formas de onda
4. **Hardware triggering** - Sincronização por hardware
5. **Amplitude/offset control** - Controle independente por canal
6. **Frequency control** - Frequência variável da rampa

## Hardware Recomendado
- **Osciloscópio dual:** Para visualizar ambos os canais
- **Buffer amplifiers:** Para cargas pesadas
- **Anti-aliasing filters:** Para aplicações de alta frequência
- **Precision voltage reference:** Para aplicações críticas
- **Isolation amplifiers:** Para proteção

## Observações Importantes
- **Load matching:** Cargas similares para melhor matching
- **Thermal effects:** Considerar deriva térmica
- **Power supply:** VDDA estável para ambos os canais
- **PCB layout:** Roteamento cuidadoso para minimizar crosstalk
- **Calibration:** Calibração independente pode ser necessária
- **Timing accuracy:** HAL_Delay não é preciso para aplicações críticas