# Projeto23 - DAC 04

## Descrição do Projeto
Este projeto implementa um **gerador de onda senoidal** usando DAC, DMA e Timer no STM32F407VET6. O sistema gera uma **onda senoidal pura** de alta qualidade com 64 pontos por ciclo, demonstrando aplicações avançadas de síntese de sinais analógicos.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 80MHz (PLL)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino DAC:** PA4 (DAC_OUT1)
- **Recursos:** Timer 6 + DMA1 Stream 5 + Math Library

## Geração de Onda Senoidal

### Características do Sinal
- **Resolução:** 64 pontos por ciclo
- **Amplitude:** 0V a 3.3V (1.65V offset)
- **Frequência:** ~1.25kHz
- **Taxa de amostragem:** 80kHz
- **THD:** <1% (alta pureza)

### Parâmetros Matemáticos
```c
#define DOIS_PI    6.238           // 2π (aproximado)
uint32_t seno[64];                // Buffer circular de 64 amostras
```

## Cálculo Matemático da Senoidal

### Fórmula Implementada
```c
for (uint8_t i = 0; i < sizeof(seno); i++)
{
    seno[i] = (sin(i * (DOIS_PI / sizeof(seno))) + 1) * ((0xFFF + 1) / 2);
}
```

### Análise da Fórmula

#### Componentes da Equação
```c
ângulo = i * (DOIS_PI / 64)          // Ângulo por amostra
sin_value = sin(ângulo)              // Valor senoidal [-1, +1]
offset_sin = sin_value + 1           // Offset para [0, +2]
dac_value = offset_sin * 2048        // Escala para [0, 4095]
```

#### Processo Detalhado
1. **Ângulo por amostra:** 2π/64 = 0.098 rad = 5.625°
2. **Range senoidal:** sin(x) ∈ [-1, +1]
3. **Offset:** +1 → range [0, +2]
4. **Escala:** ×2048 → range [0, 4095]
5. **Resultado:** Senoidal centrada em 1.65V

### Valores Calculados (Exemplos)
```c
i=0:  sin(0°) = 0     → DAC = 2048  (1.65V - centro)
i=16: sin(90°) = 1    → DAC = 4095  (3.30V - pico)
i=32: sin(180°) = 0   → DAC = 2048  (1.65V - centro)
i=48: sin(270°) = -1  → DAC = 0     (0.00V - vale)
```

## Configuração do Timer 6

### Parâmetros Alta Velocidade
```c
static void MX_TIM6_Init(void)
{
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 8-1;         // Prescaler = 8
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 100-1;          // Período = 99
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    // Configurar TRGO
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
}
```

### Cálculo da Frequência
**Com PLL @ 80MHz:**
```
Clock Timer = APB1 × 2 = 40MHz × 2 = 80MHz (devido ao prescaler)
Frequência Timer = 80MHz / (8 × 100) = 100kHz
Período por amostra = 10µs
```

**Frequência da Senoidal:**
```
Amostras por ciclo = 64
Frequência senoidal = 100kHz / 64 = 1.5625kHz
Período da senoidal = 640µs
```

## Clock Configuration com PLL

### Configuração 80MHz
```c
void SystemClock_Config(void)
{
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;      // Divisor HSI
    RCC_OscInitStruct.PLL.PLLN = 80;     // Multiplicador
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // Divisor SYSCLK
    RCC_OscInitStruct.PLL.PLLQ = 4;      // Divisor USB
}
```

### Cálculo dos Clocks
```
VCO = (HSI / PLLM) × PLLN = (16MHz / 8) × 80 = 160MHz
SYSCLK = VCO / PLLP = 160MHz / 2 = 80MHz
HCLK = SYSCLK / 1 = 80MHz
APB1 = HCLK / 2 = 40MHz
APB2 = HCLK / 1 = 80MHz
Timer Clock = APB1 × 2 = 80MHz (quando APB1 prescaler ≠ 1)
```

## Arquitetura do Sistema

### Fluxo de Dados
```
Math Library → Sine Lookup Table → DMA Buffer → Timer Trigger → DAC Output
     ↑                ↓                 ↓            ↓           ↓
  sin(x)        seno[64]         Circular DMA     100kHz     1.5625kHz
```

### Componentes Integrados
- **Math.h:** Funções trigonométricas em ponto flutuante
- **Lookup Table:** Buffer pré-calculado de 64 amostras
- **Timer 6:** Trigger preciso a 100kHz
- **DMA Circular:** Transferência automática contínua
- **DAC:** Conversão para sinal analógico

## Qualidade do Sinal

### Resolução Angular
```c
Resolução = 360° / 64 = 5.625° por amostra
Precisão angular = 0.098 radianos
Nyquist frequency = 50kHz (para 100kHz sample rate)
```

### Análise Harmônica
```c
Fundamental: 1.5625kHz
2ª Harmônica: 3.125kHz (-40dB típico)
3ª Harmônica: 4.6875kHz (-45dB típico)
Aliasing start: >47kHz (bem acima da fundamental)
```

### Distorção Harmônica Total (THD)
- **Teórica:** <0.5% (com 64 pontos)
- **Prática:** ~1-2% (limitações do DAC)
- **Dominant harmonics:** 2ª e 3ª harmônicas

## Aplicações Práticas

### 1. Oscilador de Áudio
```c
// Gerar tom de 1kHz para testes de áudio
void generate_audio_tone(float frequency)
{
    uint32_t timer_freq = 80000000;  // 80MHz
    uint32_t samples = 64;
    uint32_t sample_rate = frequency * samples;
    uint32_t period = timer_freq / sample_rate;
    
    __HAL_TIM_SET_AUTORELOAD(&htim6, period - 1);
}
```

### 2. Gerador de Função
```c
typedef enum {
    WAVE_SINE,
    WAVE_COSINE,
    WAVE_TRIANGLE,
    WAVE_SAWTOOTH
} waveform_type_t;

void generate_waveform(waveform_type_t type)
{
    for (uint8_t i = 0; i < 64; i++)
    {
        float angle = i * (2 * M_PI / 64);
        switch (type)
        {
            case WAVE_SINE:
                seno[i] = (sin(angle) + 1) * 2048;
                break;
            case WAVE_COSINE:
                seno[i] = (cos(angle) + 1) * 2048;
                break;
            case WAVE_TRIANGLE:
                seno[i] = triangle_function(angle) * 2048;
                break;
        }
    }
}
```

### 3. Modulação AM
```c
// Amplitude Modulation
void generate_am_signal(float carrier_freq, float mod_freq, float mod_depth)
{
    static uint32_t sample_index = 0;
    
    for (uint8_t i = 0; i < 64; i++)
    {
        float t = (sample_index + i) / 100000.0f;  // Time
        float carrier = sin(2 * M_PI * carrier_freq * t);
        float modulation = 1 + mod_depth * sin(2 * M_PI * mod_freq * t);
        float am_signal = carrier * modulation;
        
        seno[i] = (am_signal + 1) * 2048;
    }
    sample_index += 64;
}
```

### 4. Sweep de Frequência
```c
// Frequency Sweep Generator
void frequency_sweep(float start_freq, float end_freq, float sweep_time)
{
    static float current_freq = start_freq;
    float freq_step = (end_freq - start_freq) / (sweep_time * 1562.5);  // Steps per second
    
    // Update timer for new frequency
    uint32_t sample_rate = current_freq * 64;
    uint32_t period = 80000000 / sample_rate;
    __HAL_TIM_SET_AUTORELOAD(&htim6, period - 1);
    
    current_freq += freq_step;
    if (current_freq > end_freq) current_freq = start_freq;  // Reset sweep
}
```

## Otimizações e Melhorias

### 1. Lookup Table Pré-calculada
```c
// Calcular tabela em compile-time ou inicialização
const uint32_t sine_lut_64[64] = {
    2048, 2248, 2447, 2642, 2831, 3013, 3185, 3346,
    3495, 3630, 3750, 3853, 3939, 4007, 4056, 4085,
    4095, 4085, 4056, 4007, 3939, 3853, 3750, 3630,
    3495, 3346, 3185, 3013, 2831, 2642, 2447, 2248,
    2048, 1847, 1648, 1453, 1264, 1082, 910,  749,
    600,  465,  345,  242,  156,  88,   39,   10,
    0,    10,   39,   88,   156,  242,  345,  465,
    600,  749,  910,  1082, 1264, 1453, 1648, 1847
};
```

### 2. Múltiplas Tabelas
```c
// Diferentes resoluções para diferentes aplicações
uint32_t sine_32[32];   // Menor resolução, maior frequência
uint32_t sine_128[128]; // Maior resolução, menor distorção
uint32_t sine_256[256]; // Máxima resolução
```

### 3. Controle de Amplitude e Offset
```c
typedef struct {
    float amplitude;    // 0.0 to 1.0
    float offset;       // 0.0 to 1.0 (of full scale)
    float phase;        // 0.0 to 2π
} sine_params_t;

void generate_parametric_sine(sine_params_t *params)
{
    for (uint8_t i = 0; i < 64; i++)
    {
        float angle = i * (2 * M_PI / 64) + params->phase;
        float sine_val = sin(angle) * params->amplitude;
        seno[i] = (sine_val + params->offset) * 4095;
        
        // Clamp to valid range
        if (seno[i] > 4095) seno[i] = 4095;
        if (seno[i] < 0) seno[i] = 0;
    }
}
```

### 4. Controle de Frequência Variável
```c
void set_sine_frequency(float frequency_hz)
{
    uint32_t timer_freq = 80000000;  // Timer clock
    uint32_t samples_per_cycle = 64;
    uint32_t sample_rate = frequency_hz * samples_per_cycle;
    
    if (sample_rate > 0 && sample_rate < timer_freq)
    {
        uint32_t period = timer_freq / sample_rate;
        __HAL_TIM_SET_AUTORELOAD(&htim6, period - 1);
    }
}
```

## Medição e Análise

### Equipamentos Recomendados
- **Osciloscópio:** Para visualizar forma de onda
- **Analisador de espectro:** Para análise harmônica
- **Frequency counter:** Para verificar frequência
- **Distortion analyzer:** Para medir THD
- **RMS multimeter:** Para medições precisas

### Parâmetros de Teste
```c
Frequência fundamental: 1.5625kHz ±1%
Amplitude: 3.3V pp ±2%
Offset: 1.65V DC ±1%
THD: <2%
Rise/Fall time: <10µs
Jitter: <1µs
```

### Verificações Importantes
1. **Forma senoidal:** Verificar com osciloscópio
2. **Espectro limpo:** Analisar harmônicos
3. **Frequência precisa:** Conferir com contador
4. **Amplitude correta:** Medir RMS e peak
5. **Offset adequado:** Verificar componente DC

## Troubleshooting

### Problemas Comuns

#### 1. Onda Distorcida
**Sintomas:** Formato não-senoidal
**Causas:** Buffer mal calculado, trigger muito rápido
**Solução:** Verificar cálculos matemáticos

#### 2. Frequência Incorreta
**Sintomas:** Frequência diferente do esperado
**Solução:** Recalcular timer e verificar clocks

#### 3. Amplitude Baixa
**Sintomas:** Sinal fraco na saída
**Causas:** Output buffer desabilitado, carga alta
**Solução:** Verificar configuração DAC e carga

## Conceitos Aprendidos
1. **Síntese digital** - Geração de sinais por lookup table
2. **Matemática embarcada** - Uso de funções trigonométricas
3. **High-speed DAC** - DAC em alta velocidade com DMA
4. **Signal processing** - Processamento digital de sinais
5. **Waveform synthesis** - Síntese de formas de onda
6. **Harmonic analysis** - Análise de conteúdo harmônico
7. **Real-time generation** - Geração em tempo real

## Melhorias Sugeridas
1. **Variable frequency** - Controle dinâmico de frequência
2. **Multiple waveforms** - Seno, quadrada, triangular, dente-de-serra
3. **Amplitude/phase control** - Controle independente de parâmetros
4. **Dual-channel** - Geração simultânea em dois canais
5. **Modulation capabilities** - AM, FM, PM
6. **Higher resolution** - 128 ou 256 pontos por ciclo

## Hardware Recomendado
- **Low-pass filter:** Para remover harmônicos de alta frequência
- **Precision op-amp:** Para amplificação limpa
- **Impedance matching:** Para cargas específicas
- **Isolation transformer:** Para aplicações sensíveis
- **Precision voltage reference:** Para estabilidade

## Observações Importantes
- **Math library overhead:** Funções sin() são computacionalmente caras
- **Pre-calculated tables:** Preferível para performance crítica
- **Timer precision:** Precisão da frequência depende do clock
- **DAC settling time:** Considerar para altas frequências
- **Anti-aliasing:** Filtro necessário para aplicações críticas
- **Temperature drift:** Considerar para aplicações de precisão