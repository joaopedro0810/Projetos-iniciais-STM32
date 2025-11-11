# Projeto33 - RTC 2

## Descrição do Projeto
Este projeto demonstra o uso **avançado do RTC** no STM32F407VET6 com **LSE (Low Speed External)** como fonte de clock e saída **MCO (Master Clock Output)**. O sistema implementa um RTC de alta precisão usando cristal externo de 32.768kHz e disponibiliza o clock LSE no pino PA8 para medição/debug. O projeto inclui indicação visual com LED piscando a cada segundo.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **RTC Clock Source:** LSE (32.768kHz crystal)
- **Pino LED:** PA6 (LED1) - Pisca a cada segundo
- **MCO Output:** PA8 - Saída do LSE clock

## RTC com LSE - High Precision Clock

### Diferenças LSI vs LSE
| Característica | LSI | LSE |
|----------------|-----|-----|
| **Tipo** | RC interno | Cristal externo |
| **Frequência** | ~32000Hz | 32768Hz exato |
| **Precisão** | ±5% | ±20ppm |
| **Estabilidade** | Varia com temp. | Estável |
| **Consumo** | ~1µA | ~0.5µA |
| **Custo** | Incluído | Cristal adicional |
| **Aplicação** | Desenvolvimento | Produção |

### Vantagens do LSE
```
✓ Precisão excepcional (±20ppm = ±1.7s/dia)
✓ Estabilidade térmica superior
✓ Frequência padrão para RTC (32768 = 2^15)
✓ Menor consumo de energia
✓ Calibração desnecessária
✓ Padrão industrial para relógios
```

## Configuração do Sistema

### Clock Configuration com LSE
```c
void SystemClock_Config(void)
{
    // Configuração com LSE habilitado
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;               // HSI para sistema
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;               // LSE para RTC
    
    // Configurar MCO1 para saída do LSE
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
}
```

### LSE Crystal Connection
```
STM32F407VET6 Crystal Connection:
    PC14-OSC32_IN  ────┐
                       │  32.768kHz
    PC15-OSC32_OUT ────┤  Crystal
                       │  ±20ppm
                    ┌──┴──┐
                   │ C1  │ C2 │  Load capacitors
                   │10pF │10pF│  (typical)
                    └─────┘
                       │
                      GND
```

### RTC Initialization com LSE
```c
static void MX_RTC_Init(void)
{
    // Configuração otimizada para LSE
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;        // Formato 24 horas
    hrtc.Init.AsynchPrediv = 127;                    // 128 divisões assíncronas
    hrtc.Init.SynchPrediv = 255;                     // 256 divisões síncronas
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;           // Saída RTC desabilitada
    
    // Prescaler calculation para LSE
    // 32768 / (128 × 256) = 32768 / 32768 = 1Hz exato
}
```

## MCO - Master Clock Output

### Configuração do MCO
```c
// Configurar MCO1 no PA8 para saída do LSE
HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
```

### GPIO Configuration para MCO
```c
static void MX_GPIO_Init(void)
{
    // PA8 configurado para MCO (Alternate Function)
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;          // Alternate Function Push-Pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_MCO;        // MCO Alternate Function
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
```

### MCO Sources Available
```c
// Fontes disponíveis para MCO1
RCC_MCO1SOURCE_HSI         // HSI clock (16MHz)
RCC_MCO1SOURCE_LSE         // LSE clock (32.768kHz) - usado no projeto
RCC_MCO1SOURCE_HSE         // HSE clock 
RCC_MCO1SOURCE_PLLCLK      // PLL clock

// Divisores disponíveis
RCC_MCODIV_1               // Sem divisão (usado no projeto)
RCC_MCODIV_2               // Dividir por 2
RCC_MCODIV_3               // Dividir por 3
RCC_MCODIV_4               // Dividir por 4
RCC_MCODIV_5               // Dividir por 5
```

## Operação do Sistema

### Inicialização com Tempo Personalizado
```c
// Variável de tempo inicializada com valores específicos
RTC_TimeTypeDef tempo = 
{
    .Seconds = 45,           // 45 segundos
    .Minutes = 12,           // 12 minutos
    // .Hours não especificado = 0
};

// Configurar tempo personalizado após inicialização
HAL_RTC_SetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
```

### Loop Principal com LED Second Tick
```c
while (1)
{
    // Ler data e tempo atual
    HAL_RTC_GetDate(&hrtc, &data, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
    
    // Variável estática para detectar mudança de segundo
    static uint8_t lastSecond = 0;
    
    // LED pisca a cada mudança de segundo
    if (lastSecond != tempo.Seconds)
    {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
    
    lastSecond = tempo.Seconds;
}
```

### Análise do Comportamento
```
t=0.000s: Sistema inicia, RTC = 00:12:45
t=1.000s: Segundo muda para 46, LED toggle
t=2.000s: Segundo muda para 47, LED toggle
t=3.000s: Segundo muda para 48, LED toggle
...
t=15.000s: RTC = 00:13:00, LED toggle
...

MCO Output (PA8): 32.768kHz square wave continuous
LED Toggle: Exactly every 1.000 second (LSE precision)
```

## Precisão e Timing Analysis

### LSE Precision Calculation
```c
// LSE typical accuracy: ±20ppm
// Drift calculation:
// 1 day = 86400 seconds
// Error = 86400 × 20ppm = 86400 × 20/1000000 = 1.728 seconds/day

// Monthly drift: 1.728s × 30 = 51.84 seconds/month
// Yearly drift: 1.728s × 365 = 630.72 seconds/year ≈ 10.5 minutes/year
```

### Prescaler Accuracy com LSE
```c
// LSE = 32768Hz (exato)
// AsynchPrediv = 127 → 128 divisões
// SynchPrediv = 255 → 256 divisões
// Total division = 128 × 256 = 32768

// RTC frequency = 32768 / 32768 = 1.000000Hz (perfeito)
// Period = 1.000000 second (sem erro de prescaler)
```

## Aplicações Práticas

### 1. Relógio de Precisão com Calibração
```c
typedef struct {
    int16_t calibration_ppm;     // Calibração em ppm
    uint32_t last_calibration;   // Timestamp da última calibração
    uint32_t drift_seconds;      // Deriva acumulada
} rtc_calibration_t;

rtc_calibration_t rtc_cal = {0};

void calibrate_rtc(int16_t measured_error_ppm)
{
    // Aplicar calibração baseada em medição
    rtc_cal.calibration_ppm = measured_error_ppm;
    
    // STM32 suporta calibração automática
    // Valor de calibração: -511 a +512
    int16_t cal_value = (measured_error_ppm * 512) / 1000000;
    
    if (cal_value > 512) cal_value = 512;
    if (cal_value < -511) cal_value = -511;
    
    // Aplicar calibração (implementação depende do hardware)
    HAL_RTCEx_SetSmoothCalib(&hrtc, RTC_SMOOTHCALIB_PERIOD_32SEC,
                             RTC_SMOOTHCALIB_PLUSPULSES_RESET,
                             abs(cal_value));
    
    printf("RTC calibrated: %d ppm, cal_value: %d\n", 
           measured_error_ppm, cal_value);
}

void measure_rtc_accuracy(void)
{
    // Comparar com referência externa (GPS, NTP, etc.)
    uint32_t rtc_timestamp = get_rtc_unix_timestamp();
    uint32_t reference_timestamp = get_reference_timestamp();
    
    int32_t error_seconds = rtc_timestamp - reference_timestamp;
    uint32_t elapsed_time = rtc_timestamp - rtc_cal.last_calibration;
    
    if (elapsed_time > 86400)  // Pelo menos 1 dia de dados
    {
        // Calcular erro em ppm
        int32_t error_ppm = (error_seconds * 1000000) / elapsed_time;
        
        printf("RTC drift: %d seconds in %d seconds = %d ppm\n",
               error_seconds, elapsed_time, error_ppm);
        
        // Auto-calibrar se erro > 5ppm
        if (abs(error_ppm) > 5)
        {
            calibrate_rtc(-error_ppm);  // Correção oposta
        }
        
        rtc_cal.last_calibration = rtc_timestamp;
    }
}
```

### 2. Data Logger de Alta Precisão
```c
typedef struct {
    uint32_t timestamp;          // Unix timestamp
    uint16_t subseconds;         // Subsegundos para precisão extra
    float sensor_value;
    uint16_t sequence_number;
} precision_log_t;

precision_log_t precision_logs[1000];
uint16_t log_count = 0;

void log_with_high_precision(float sensor_value)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    
    // Ler com máxima precisão
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    
    precision_log_t *log = &precision_logs[log_count % 1000];
    
    // Timestamp com precisão de subsegundo
    log->timestamp = rtc_to_unix_timestamp(&date, &time);
    log->subseconds = time.SubSeconds;       // Precisão adicional
    log->sensor_value = sensor_value;
    log->sequence_number = log_count;
    
    // Calcular tempo exato com subsegundos
    // Resolução: 1/256 segundo ≈ 3.9ms
    float exact_time = log->timestamp + (float)(255 - time.SubSeconds) / 256.0;
    
    printf("LOG #%04d: %.3f @ %.3f = %.2f\n",
           log->sequence_number, exact_time, 
           (float)(255 - time.SubSeconds) / 256.0,
           log->sensor_value);
    
    log_count++;
}
```

### 3. Sincronização de Rede com RTC
```c
typedef struct {
    uint32_t ntp_timestamp;
    uint32_t rtc_timestamp;
    int32_t offset_ms;
    uint8_t sync_valid;
} time_sync_t;

time_sync_t time_sync = {0};

void synchronize_with_ntp(uint32_t ntp_time)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    
    // Capturar RTC no momento da recepção NTP
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    
    uint32_t rtc_time = rtc_to_unix_timestamp(&date, &time);
    
    // Calcular offset
    int32_t offset = ntp_time - rtc_time;
    
    time_sync.ntp_timestamp = ntp_time;
    time_sync.rtc_timestamp = rtc_time;
    time_sync.offset_ms = offset * 1000;
    time_sync.sync_valid = 1;
    
    printf("NTP Sync: RTC=%u, NTP=%u, Offset=%d seconds\n",
           rtc_time, ntp_time, offset);
    
    // Se offset > 2 segundos, ajustar RTC
    if (abs(offset) > 2)
    {
        set_rtc_from_unix_timestamp(ntp_time);
        printf("RTC adjusted to NTP time\n");
    }
    else if (abs(offset) < 1)
    {
        // Pequenos offsets: usar para calibração
        int32_t drift_ppm = (offset * 1000000) / 86400;  // Assumir 1 dia
        calibrate_rtc(-drift_ppm);
    }
}

void set_rtc_from_unix_timestamp(uint32_t unix_time)
{
    // Converter Unix timestamp para RTC structures
    time_t timestamp = unix_time;
    struct tm *time_info = gmtime(&timestamp);
    
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    sTime.Hours = time_info->tm_hour;
    sTime.Minutes = time_info->tm_min;
    sTime.Seconds = time_info->tm_sec;
    
    sDate.Date = time_info->tm_mday;
    sDate.Month = time_info->tm_mon + 1;
    sDate.Year = time_info->tm_year - 100;  // Years since 2000
    sDate.WeekDay = time_info->tm_wday == 0 ? 7 : time_info->tm_wday;
    
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}
```

### 4. Sistema de Backup com MCO Monitoring
```c
typedef struct {
    uint8_t lse_active;
    uint32_t mco_frequency;
    uint32_t last_mco_check;
    uint8_t backup_mode;
} clock_monitoring_t;

clock_monitoring_t clock_mon = {0};

void monitor_lse_health(void)
{
    // Verificar se LSE está funcionando
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY))
    {
        clock_mon.lse_active = 1;
        
        // MCO output permite medição externa da frequência
        // Frequência esperada no PA8: 32.768kHz
        
        // Simular medição de frequência (implementação hardware-específica)
        clock_mon.mco_frequency = measure_mco_frequency();
        
        // Verificar se frequência está dentro da tolerância
        if (clock_mon.mco_frequency < 32760 || clock_mon.mco_frequency > 32776)
        {
            printf("WARNING: LSE frequency out of spec: %dHz\n", 
                   clock_mon.mco_frequency);
        }
        else
        {
            printf("LSE healthy: %dHz\n", clock_mon.mco_frequency);
        }
    }
    else
    {
        clock_mon.lse_active = 0;
        printf("ERROR: LSE not ready!\n");
        
        // Fallback para LSI se LSE falhar
        switch_to_lsi_backup();
    }
    
    clock_mon.last_mco_check = HAL_GetTick();
}

void switch_to_lsi_backup(void)
{
    printf("Switching RTC to LSI backup...\n");
    
    // Reconfigurar RTC para LSI
    __HAL_RCC_RTC_DISABLE();
    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);
    __HAL_RCC_RTC_ENABLE();
    
    // Ajustar prescalers para LSI (~32000Hz)
    hrtc.Init.AsynchPrediv = 124;  // 125 divisões
    hrtc.Init.SynchPrediv = 255;   // 256 divisões
    // 32000 / (125 × 256) ≈ 1Hz
    
    HAL_RTC_Init(&hrtc);
    
    clock_mon.backup_mode = 1;
}

uint32_t measure_mco_frequency(void)
{
    // Esta função seria implementada com timer capture
    // ou contador externo para medir a frequência real
    // no pino PA8 (MCO output)
    
    // Implementação simulada retornando valor típico
    return 32768;  // Hz
}
```

## MCO Applications e Debug

### MCO para Debug e Medição
```c
void setup_mco_for_debug(void)
{
    // Diferentes clocks podem ser roteados para MCO
    
    // LSE para verificação de cristal
    HAL_RCC_MCOConfig(RCC_MCO1, RTC_MCO1SOURCE_LSE, RCC_MCODIV_1);
    printf("MCO1 (PA8): LSE output at 32.768kHz\n");
    
    // HSI para debug do sistema
    // HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_16);
    // printf("MCO1 (PA8): HSI/16 output at 1MHz\n");
    
    // SYSCLK para verificação de clock do sistema
    // HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_16);
    // printf("MCO1 (PA8): SYSCLK/16 output\n");
}

void measure_clock_accuracy_via_mco(void)
{
    // Usar osciloscópio ou frequency counter no PA8
    printf("Connect frequency counter to PA8 (MCO1)\n");
    printf("Expected: 32768.000 Hz ±20ppm\n");
    printf("Tolerance: 32767.344 Hz to 32768.656 Hz\n");
    
    // Loop de medição
    for (int i = 0; i < 10; i++)
    {
        HAL_Delay(1000);
        printf("Measure frequency on PA8... (%d/10)\n", i+1);
    }
}
```

### MCO como Clock Source Externo
```c
void use_mco_as_external_clock(void)
{
    // MCO pode ser usado como clock para outros dispositivos
    
    // Configurar MCO com clock estável
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_4);
    // PA8 output: 16MHz / 4 = 4MHz
    
    printf("PA8 providing 4MHz clock for external devices\n");
    
    // Aplicações:
    // - Clock para microcontroladores externos
    // - Referência para ADCs externos
    // - Clock para communication interfaces
    // - Sincronização de sistemas distribuídos
}
```

## Troubleshooting

### Problemas Comuns

#### 1. LSE não inicia
**Sintomas:** `RCC_FLAG_LSERDY` nunca fica ready
**Causas possíveis:**
- Cristal 32.768kHz não conectado
- Capacitores de load incorretos
- Trilhas muito longas/ruidosas
- Cristal danificado

**Solução:**
```c
// Verificar LSE com timeout
uint32_t timeout = HAL_GetTick() + 5000;  // 5 segundos
__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);

while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
{
    if (HAL_GetTick() > timeout)
    {
        printf("ERROR: LSE failed to start - using LSI backup\n");
        switch_to_lsi_backup();
        break;
    }
    HAL_Delay(100);
}
```

#### 2. MCO não funciona
**Sintomas:** Sem sinal no PA8
**Causas possíveis:**
- PA8 não configurado para MCO
- Clock source não disponível
- Pin conflito

**Solução:**
```c
// Verificar configuração completa do MCO
__HAL_RCC_GPIOA_CLK_ENABLE();

// Configurar PA8 para MCO
GPIO_InitStruct.Pin = GPIO_PIN_8;
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// Configurar MCO após GPIO
HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
```

#### 3. RTC impreciso mesmo com LSE
**Sintomas:** Deriva além de ±20ppm
**Causas possíveis:**
- Capacitores de load incorretos
- Temperatura extrema
- Interferência eletromagnética
- Cristal de baixa qualidade

**Solução:**
```c
// Verificar e ajustar capacitores de load
// Valores típicos: 6pF, 10pF, 12.5pF
// Verificar datasheet do cristal

// Implementar compensação térmica se necessário
void temperature_compensate_rtc(float temperature_c)
{
    // Cristais típicos: -0.04ppm/°C²
    float temp_error = -0.04 * pow(temperature_c - 25, 2);
    calibrate_rtc((int16_t)temp_error);
}
```

#### 4. LED não pisca corretamente
**Sintomas:** LED pisca irregularmente
**Causas possíveis:**
- Loop muito lento
- Polling ineficiente
- Problema de timing

**Solução:**
```c
// Otimizar loop de detecção
while (1)
{
    HAL_RTC_GetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
    
    static uint8_t lastSecond = 255;  // Valor inicial inválido
    
    if (lastSecond != tempo.Seconds)
    {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        lastSecond = tempo.Seconds;
        
        // Debug timing
        printf("Second tick: %02d\n", tempo.Seconds);
    }
    
    // Pequeno delay para não sobrecarregar
    HAL_Delay(10);
}
```

## Hardware Requirements

### Cristal LSE Specifications
```
Frequency:        32.768kHz ±20ppm
Load Capacitance: 6pF, 10pF, ou 12.5pF (verificar datasheet)
ESR:             <50kΩ (preferred <35kΩ)
Drive Level:     <1µW
Package:         3.2×1.5mm, 2.0×1.2mm típico
Temperature:     -40°C to +85°C

Recomendados:
- Abracon ABS07-32.768KHZ-T
- Epson FC-135 32.7680KA-A3
- Micro Crystal CM7V-T1A-32.768kHz
```

### PCB Layout Guidelines
```
Crystal Placement:
✓ Próximo aos pinos PC14/PC15 (<5mm)
✓ Ground plane embaixo do cristal
✓ Via de alívio sob o cristal
✓ Trilhas curtas e diretas
✓ Afastar de clocks de alta frequência

Capacitors:
✓ Capacitores de load próximos ao cristal
✓ Ground via próximo a cada capacitor
✓ Valores típicos: 10pF para CL=12.5pF

Shield:
✓ Ground guard rings ao redor
✓ Via stitching para shield
```

## Advanced Features

### RTC Subsecond Precision
```c
void get_high_precision_time(void)
{
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    
    // SubSeconds conta de 255 para 0 (down counter)
    // Resolução: 1/256 segundo ≈ 3.9ms
    float subsec_fraction = (float)(255 - time.SubSeconds) / 256.0;
    
    printf("Precise time: %02d:%02d:%02d.%03d\n",
           time.Hours, time.Minutes, time.Seconds,
           (int)(subsec_fraction * 1000));
}
```

### Multiple MCO Outputs
```c
void setup_dual_mco_output(void)
{
    // MCO1 (PA8): LSE output
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
    
    // MCO2 (PC9): System clock output (se disponível)
    // HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_4);
    
    printf("MCO1 (PA8): 32.768kHz LSE\n");
    printf("MCO2 (PC9): 4MHz SYSCLK\n");
}
```

## Conceitos Aprendidos
1. **LSE configuration** - Configuração de cristal externo de 32.768kHz
2. **MCO output** - Master Clock Output para debug e medição
3. **High precision timing** - Timing de alta precisão com LSE
4. **Clock monitoring** - Monitoramento da saúde do clock
5. **Subsecond resolution** - Resolução de subsegundos
6. **Crystal characterization** - Caracterização de cristais
7. **Backup clock strategies** - Estratégias de backup de clock

## Melhorias Sugeridas
1. **Temperature compensation** - Compensação térmica para precisão
2. **NTP synchronization** - Sincronização via NTP
3. **Clock failure detection** - Detecção automática de falha de clock
4. **Smooth calibration** - Calibração suave do RTC
5. **Multiple time zones** - Suporte a múltiplos fusos horários
6. **Alarm implementation** - Implementação de alarmes RTC

## Hardware Recomendado
- **32.768kHz crystal:** Cristal de precisão ±20ppm
- **Load capacitors:** 10pF cerâmicos NP0/C0G
- **Frequency counter:** Para medição de MCO
- **Oscilloscope:** Para análise de clock quality
- **Temperature chamber:** Para testes de precisão

## Observações Importantes
- **Crystal selection:** Escolha de cristal crítica para precisão
- **PCB layout:** Layout de PCB afeta performance do LSE
- **Load capacitance:** Deve ser calculada corretamente
- **MCO loading:** MCO não deve ser sobrecarregado
- **Backup power:** LSE funciona com backup battery
- **Startup time:** LSE demora mais para inicializar que LSI
- **Power consumption:** LSE consome menos energia que LSI