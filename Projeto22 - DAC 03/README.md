# Projeto22 - DAC 03

## Descrição do Projeto
Este projeto demonstra **DAC com DMA e Timer** no STM32F407VET6, gerando uma **forma de onda escalonada** usando trigger por hardware. O sistema utiliza Timer 6 para timing preciso e DMA para transferência automática de dados, criando um sinal analógico com 6 níveis diferentes.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino DAC:** PA4 (DAC_OUT1)
- **Recursos:** Timer 6 + DMA1 Stream 5

## Arquitetura do Sistema

### Componentes Integrados
- **DAC:** Conversão digital-analógica
- **Timer 6:** Geração de trigger preciso (1ms)
- **DMA:** Transferência automática de dados
- **Buffer circular:** 6 amostras em loop contínuo

### Fluxo de Dados
```
Timer 6 (1ms) → DAC Trigger → DMA Transfer → DAC Output (PA4)
     ↑                                           ↓
   1kHz TRGO    ←─── Buffer Circular ←─── Forma de Onda
```

## Configuração do Timer 6

### Parâmetros do Timer
```c
static void MX_TIM6_Init(void)
{
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 16-1;        // Divisor = 16
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 1000;           // Período = 1000
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    // Configurar TRGO (Trigger Output)
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
}
```

### Cálculo da Frequência
```
Clock Timer = PCLK1 = 16MHz
Frequência Timer = PCLK1 / (Prescaler × Period)
Frequência Timer = 16MHz / (16 × 1000) = 1kHz
Período = 1ms
```

### Master Output Trigger (TRGO)
- **TIM_TRGO_UPDATE:** Gera trigger a cada update event
- **Frequência TRGO:** 1kHz (1ms por trigger)
- **Função:** Sincronizar DAC automaticamente

## Buffer de Forma de Onda

### Definição do Buffer
```c
uint32_t sinal[6];  // Array de 6 amostras

// Inicialização no main()
for (uint8_t i = 0; i < sizeof(sinal); i++)
{
    sinal[i] = 500 * (i + 1);  // Valores escalonados
}
```

### Valores Calculados
```c
sinal[0] = 500    // ~0.40V
sinal[1] = 1000   // ~0.81V  
sinal[2] = 1500   // ~1.21V
sinal[3] = 2000   // ~1.61V
sinal[4] = 2500   // ~2.02V
sinal[5] = 3000   // ~2.42V
```

### Forma de Onda Resultante
```
Tensão (V)
2.42 |     ___
     |    |   |
2.02 |    |   |___
     |    |       |
1.61 |    |       |___
     |    |           |
1.21 |    |           |___
     |    |               |
0.81 |    |               |___
     |    |                   |
0.40 |____|                   |____
     +----+----+----+----+----+----+-- Tempo
     0   1ms  2ms  3ms  4ms  5ms  6ms
```

## Configuração do DAC com DMA

### Inicialização do DAC
```c
static void MX_DAC_Init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração do Canal 1
    sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;     // Timer 6 trigger
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### Trigger por Hardware
- **DAC_TRIGGER_T6_TRGO:** Timer 6 TRGO como trigger
- **Vantagem:** Timing preciso e independente da CPU
- **Sincronização:** Automática via hardware

## Configuração do DMA

### DMA1 Stream 5
```c
static void MX_DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    // Configurar interrupção DMA
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}
```

### Parâmetros DMA (configurados via CubeMX)
- **Direction:** Memory to Peripheral
- **Mode:** Circular (loop infinito)
- **Data Width:** Word (32-bit)
- **Increment:** Memory increment enabled

## Inicialização do Sistema

### Sequência de Start
```c
int main(void)
{
    // Inicializações...
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DAC_Init();
    MX_TIM6_Init();
    
    // Inicializar buffer ANTES do DMA
    for (uint8_t i = 0; i < sizeof(sinal); i++)
    {
        sinal[i] = 500 * (i + 1);
    }
    
    // Iniciar sistema
    HAL_TIM_Base_Start(&htim6);                    // Timer primeiro
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1,        // DMA depois
                      sinal, sizeof(sinal), 
                      DAC_ALIGN_12B_R);
    
    while (1)
    {
        // Sistema roda automaticamente
    }
}
```

### Ordem de Inicialização Importante
1. **Buffer inicializado:** Dados válidos antes do DMA
2. **Timer iniciado:** Trigger disponível
3. **DMA+DAC iniciados:** Sistema operacional

## Análise Temporal

### Características do Sinal
- **Período total:** 6ms (6 amostras × 1ms)
- **Frequência fundamental:** ~167Hz (1/6ms)
- **Taxa de amostragem:** 1kHz
- **Resolução temporal:** 1ms
- **Duty cycle por nível:** 16.67% cada

### Espectro de Frequência
```c
Fundamental: 167Hz
Harmônicos: 333Hz, 500Hz, 667Hz, 833Hz, 1000Hz...
Nyquist: 500Hz (limitado pela taxa de amostragem)
```

## Vantagens da Arquitetura

### 1. Timing Preciso
- **Hardware timer:** Eliminação de jitter de software
- **Crystal accuracy:** Precisão limitada apenas pelo cristal
- **No CPU intervention:** Sistema independente após start

### 2. Performance
- **Zero CPU load:** DMA opera independentemente
- **Continuous operation:** Loop automático
- **Deterministic:** Timing sempre constante

### 3. Flexibilidade
- **Buffer modificável:** Alterar forma de onda em runtime
- **Frequência ajustável:** Mudar período do timer
- **Amplitude variável:** Modificar valores do buffer

## Aplicações Práticas

### 1. Gerador de Função Escalonado
```c
// Diferentes formas de onda
uint32_t square_wave[2] = {0, 4095};           // Onda quadrada
uint32_t triangle_wave[4] = {0, 1365, 2730, 4095}; // Triangular
uint32_t sine_wave[16];                        // Senoidal (16 pontos)

// Calcular onda senoidal
for (int i = 0; i < 16; i++)
{
    sine_wave[i] = (uint32_t)(2048 + 2047 * sin(2 * M_PI * i / 16));
}
```

### 2. Simulador de Sensor
```c
// Simular saída de sensor de pressão variável
uint32_t pressure_simulation[10] = {
    500,   // 0.5 bar
    750,   // 0.75 bar
    1000,  // 1.0 bar
    1200,  // 1.2 bar
    1500,  // 1.5 bar
    1800,  // 1.8 bar
    2000,  // 2.0 bar
    1700,  // 1.7 bar
    1200,  // 1.2 bar
    800    // 0.8 bar
};
```

### 3. Controle PWM Analógico
```c
// Simular PWM com diferentes duty cycles
uint32_t pwm_levels[8] = {
    0,     // 0% duty
    585,   // 12.5% duty
    1170,  // 25% duty
    1755,  // 37.5% duty
    2340,  // 50% duty
    2925,  // 62.5% duty
    3510,  // 75% duty
    4095   // 100% duty
};
```

### 4. Calibração de ADC
```c
// Gerar níveis de referência para calibração
uint32_t calibration_levels[11];

void generate_calibration_levels(void)
{
    for (int i = 0; i <= 10; i++)
    {
        calibration_levels[i] = (4095 * i) / 10;  // 0% a 100% em 10% steps
    }
}
```

## Modificações Dinâmicas

### 1. Alterar Forma de Onda
```c
void change_waveform(uint32_t *new_waveform, uint16_t size)
{
    // Parar DMA
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    
    // Atualizar buffer
    memcpy(sinal, new_waveform, size * sizeof(uint32_t));
    
    // Reiniciar com novo buffer
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, sinal, size, DAC_ALIGN_12B_R);
}
```

### 2. Alterar Frequência
```c
void change_frequency(uint32_t new_period)
{
    // Parar timer
    HAL_TIM_Base_Stop(&htim6);
    
    // Alterar período
    __HAL_TIM_SET_AUTORELOAD(&htim6, new_period);
    
    // Reiniciar timer
    HAL_TIM_Base_Start(&htim6);
}
```

### 3. Controle em Tempo Real
```c
void update_buffer_value(uint8_t index, uint32_t value)
{
    if (index < sizeof(sinal))
    {
        sinal[index] = value;
        // Buffer é atualizado automaticamente no próximo ciclo DMA
    }
}
```

## Troubleshooting

### Problemas Comuns

#### 1. Saída Sempre Zero
**Causas possíveis:**
- Timer não iniciado
- DMA não configurado corretamente
- Buffer não inicializado

#### 2. Frequência Incorreta
**Causas possíveis:**
- Prescaler ou período incorretos
- Clock do timer não habilitado
- TRGO não configurado

#### 3. Forma de Onda Distorcida
**Causas possíveis:**
- Buffer corrompido
- DMA em modo não-circular
- Valores fora do range 0-4095

### Verificações de Debug
```c
// Verificar se timer está rodando
uint32_t timer_count = __HAL_TIM_GET_COUNTER(&htim6);

// Verificar estado do DMA
HAL_DMA_StateTypeDef dma_state = HAL_DMA_GetState(hdac.DMA_Handle1);

// Verificar valores do buffer
for (int i = 0; i < 6; i++)
{
    printf("sinal[%d] = %lu\n", i, sinal[i]);
}
```

## Análise de Performance

### Consumo de CPU
- **Durante operação:** ~0% (sistema autônomo)
- **Durante inicialização:** Mínimo
- **Overhead DMA:** Desprezível

### Consumo de Memória
- **RAM:** 24 bytes (6 × 4 bytes) para buffer
- **Flash:** Código HAL + aplicação
- **DMA descriptors:** Gerenciados pelo HAL

### Precisão Temporal
- **Jitter:** Limitado pela precisão do HSI (~1%)
- **Drift:** Dependente da temperatura
- **Absolute accuracy:** ±1% típico

## Conceitos Aprendidos
1. **DAC + DMA + Timer** - Integração de periféricos
2. **Hardware triggering** - Sincronização por hardware
3. **Circular DMA** - Operação contínua automática
4. **Waveform generation** - Geração de formas de onda
5. **Buffer management** - Gestão de dados em memória
6. **Autonomous operation** - Sistema independente da CPU
7. **Timing precision** - Controle temporal preciso

## Melhorias Sugeridas
1. **Bigger buffers** - Mais pontos para melhor resolução
2. **Dual channel** - Usar ambos os canais DAC
3. **Variable frequency** - Controle dinâmico da frequência
4. **Amplitude control** - Escala e offset ajustáveis
5. **Interrupt callbacks** - Notificações de fim de ciclo
6. **External triggering** - Trigger por sinal externo

## Hardware Recomendado
- **Osciloscópio:** Para visualizar forma de onda
- **Gerador de funções:** Para comparação
- **Frequency counter:** Para verificar timing
- **Buffer amplifier:** Para cargas pesadas
- **Anti-aliasing filter:** Para aplicações críticas

## Observações Importantes
- **Buffer initialization:** Deve ser feito ANTES do DMA start
- **Timer sequence:** Timer deve estar rodando antes do DAC+DMA
- **Memory alignment:** Buffer deve estar alinhado corretamente
- **Circular mode:** Essencial para operação contínua
- **Interrupt priority:** DMA deve ter prioridade adequada
- **Clock stability:** HSI pode ter drift, considerar HSE para precisão