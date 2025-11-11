# Projeto32 - RTC 1

## Descrição do Projeto
Este projeto demonstra o uso básico do **RTC (Real-Time Clock)** no STM32F407VET6. O sistema implementa um relógio em tempo real usando o LSI (Low Speed Internal) como fonte de clock, mostrando como configurar, inicializar e ler data/hora do RTC. O projeto estabelece a base para aplicações que necessitam controle de tempo preciso.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **RTC Clock Source:** LSI (Low Speed Internal ~32kHz)
- **RTC Instance:** RTC peripheral integrado

## RTC - Real-Time Clock

### O que é o RTC?
O **RTC (Real-Time Clock)** é um periférico dedicado para manter data e hora precisas, mesmo quando o sistema principal está em low-power mode. Características principais:
- **Clock independente:** Funciona com clock separado (LSI/LSE)
- **Backup domain:** Preservado em standby/shutdown modes
- **Calendar function:** Data completa (dia, mês, ano, dia da semana)
- **Time function:** Hora completa (horas, minutos, segundos, subsegundos)
- **Alarm capability:** Capacidade de alarme e wake-up
- **Tamper detection:** Detecção de violação (em alguns pinos)

### Arquitetura do RTC STM32F4
```
LSI (32kHz) ────┐
LSE (32.768kHz) ┼──→ [Prescaler] ──→ [RTC Core] ──→ Calendar/Time
HSE/32 ─────────┘      ↓                ↓
                   AsynchPrediv     Alarms/Wake-up
                   SynchPrediv      Timestamps
```

### Clock Sources Disponíveis
| Source | Frequency | Accuracy | Power | Uso |
|--------|-----------|----------|-------|-----|
| **LSI** | ~32kHz | ±5% | Baixo | Desenvolvimento/Teste |
| **LSE** | 32.768kHz | ±20ppm | Muito baixo | Produção |
| **HSE/32** | Variable | Alta | Alto | Aplicações especiais |

## Configuração do Sistema

### Clock Configuration com LSI
```c
void SystemClock_Config(void)
{
    // Configuração de oscillators
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;  // LSI para RTC
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    
    // Clock configuration
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // 16MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;      // 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;       // 16MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;       // 16MHz
}
```

### RTC Initialization
```c
static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // Configuração básica do RTC
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;        // Formato 24 horas
    hrtc.Init.AsynchPrediv = 127;                    // Prescaler assíncrono
    hrtc.Init.SynchPrediv = 255;                     // Prescaler síncrono
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;           // Saída desabilitada
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração inicial de tempo (00:00:00)
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
    
    // Configuração inicial de data (01/01/2000, Segunda-feira)
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;      // Dia 1
    sDate.Year = 0x0;      // Ano 2000 (offset)
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}
```

## Cálculo dos Prescalers

### Fórmula dos Prescalers
```
RTC_Clock = LSI_Frequency / ((AsynchPrediv + 1) × (SynchPrediv + 1))
```

### Para LSI (~32kHz)
```c
LSI_Frequency ≈ 32000 Hz
AsynchPrediv = 127
SynchPrediv = 255

RTC_Clock = 32000 / ((127 + 1) × (255 + 1))
RTC_Clock = 32000 / (128 × 256)
RTC_Clock = 32000 / 32768 ≈ 0.977 Hz

Período = 1 / 0.977 ≈ 1.024 segundos
```

### Ajuste para 1Hz Exato
Para obter exatamente 1Hz com LSI:
```c
// Medição precisa do LSI (exemplo: 32768 Hz)
uint32_t measured_lsi = 32768;

// Cálculo dos prescalers para 1Hz
hrtc.Init.AsynchPrediv = 127;  // 128 divisões
hrtc.Init.SynchPrediv = (measured_lsi / 128) - 1;  // 255 para LSI típico
```

## Operação Principal

### Loop Principal
```c
// Variáveis globais para RTC
RTC_DateTypeDef data;
RTC_TimeTypeDef tempo;

while (1)
{
    // Ler data atual
    HAL_RTC_GetDate(&hrtc, &data, RTC_FORMAT_BIN);
    
    // Ler tempo atual
    HAL_RTC_GetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
    
    // Dados disponíveis nas estruturas:
    // tempo.Hours, tempo.Minutes, tempo.Seconds
    // data.Date, data.Month, data.Year, data.WeekDay
}
```

### Estruturas de Data e Tempo
```c
// RTC_TimeTypeDef
typedef struct
{
    uint8_t Hours;         // 0-23 (formato 24h) ou 1-12 (formato 12h)
    uint8_t Minutes;       // 0-59
    uint8_t Seconds;       // 0-59
    uint8_t TimeFormat;    // RTC_HOURFORMAT12_AM/PM (só formato 12h)
    uint32_t SubSeconds;   // Subsegundos (0 - SynchPrediv)
    uint32_t SecondFraction; // Fração de segundo
    uint32_t DayLightSaving; // Horário de verão
    uint32_t StoreOperation; // Operação de armazenamento
} RTC_TimeTypeDef;

// RTC_DateTypeDef  
typedef struct
{
    uint8_t WeekDay;       // RTC_WEEKDAY_MONDAY até RTC_WEEKDAY_SUNDAY
    uint8_t Month;         // RTC_MONTH_JANUARY até RTC_MONTH_DECEMBER
    uint8_t Date;          // 1-31
    uint8_t Year;          // 0-99 (offset from 2000)
} RTC_DateTypeDef;
```

## Formatos de Data/Hora

### Formato BCD vs BIN
```c
// BCD Format (Binary Coded Decimal)
HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
// Valores em BCD: 0x23 = 23, 0x59 = 59

// BIN Format (Binary)
HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
// Valores binários normais: 23 = 23, 59 = 59
```

### Conversão BCD ↔ BIN
```c
// BCD para BIN
uint8_t bcd_to_bin(uint8_t bcd_val)
{
    return ((bcd_val >> 4) * 10) + (bcd_val & 0x0F);
}

// BIN para BCD
uint8_t bin_to_bcd(uint8_t bin_val)
{
    return ((bin_val / 10) << 4) | (bin_val % 10);
}
```

## Aplicações Práticas

### 1. Relógio Digital Simples
```c
void display_digital_clock(void)
{
    RTC_DateTypeDef current_date;
    RTC_TimeTypeDef current_time;
    char time_string[32];
    char date_string[32];
    
    // Ler RTC
    HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN);
    
    // Formatar tempo
    sprintf(time_string, "%02d:%02d:%02d", 
            current_time.Hours, 
            current_time.Minutes, 
            current_time.Seconds);
    
    // Formatar data
    sprintf(date_string, "%02d/%02d/20%02d", 
            current_date.Date, 
            current_date.Month, 
            current_date.Year);
    
    // Exibir no display/serial
    printf("Time: %s\n", time_string);
    printf("Date: %s\n", date_string);
}
```

### 2. Data Logger com Timestamp
```c
typedef struct {
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    float sensor_value;
    uint32_t log_id;
} timestamped_log_t;

timestamped_log_t sensor_logs[100];
uint8_t log_index = 0;

void log_sensor_data(float sensor_reading)
{
    timestamped_log_t *current_log = &sensor_logs[log_index];
    
    // Capturar timestamp
    HAL_RTC_GetDate(&hrtc, &current_log->date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &current_log->time, RTC_FORMAT_BIN);
    
    // Salvar dados
    current_log->sensor_value = sensor_reading;
    current_log->log_id = log_index;
    
    log_index = (log_index + 1) % 100;
    
    // Exemplo de saída
    printf("LOG #%03d [%02d/%02d/20%02d %02d:%02d:%02d]: %.2f\n",
           current_log->log_id,
           current_log->date.Date, current_log->date.Month, current_log->date.Year,
           current_log->time.Hours, current_log->time.Minutes, current_log->time.Seconds,
           current_log->sensor_value);
}
```

### 3. Sistema de Eventos Programados
```c
typedef struct {
    RTC_DateTypeDef target_date;
    RTC_TimeTypeDef target_time;
    char description[32];
    uint8_t active;
    void (*callback)(void);
} scheduled_event_t;

scheduled_event_t events[10];
uint8_t event_count = 0;

void schedule_event(uint8_t day, uint8_t month, uint8_t year,
                   uint8_t hour, uint8_t minute, uint8_t second,
                   const char* desc, void (*callback)(void))
{
    if (event_count < 10)
    {
        scheduled_event_t *event = &events[event_count];
        
        event->target_date.Date = day;
        event->target_date.Month = month;
        event->target_date.Year = year;
        event->target_time.Hours = hour;
        event->target_time.Minutes = minute;
        event->target_time.Seconds = second;
        
        strncpy(event->description, desc, 31);
        event->callback = callback;
        event->active = 1;
        
        event_count++;
    }
}

void check_scheduled_events(void)
{
    RTC_DateTypeDef current_date;
    RTC_TimeTypeDef current_time;
    
    HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN);
    
    for (int i = 0; i < event_count; i++)
    {
        scheduled_event_t *event = &events[i];
        
        if (event->active &&
            current_date.Date == event->target_date.Date &&
            current_date.Month == event->target_date.Month &&
            current_date.Year == event->target_date.Year &&
            current_time.Hours == event->target_time.Hours &&
            current_time.Minutes == event->target_time.Minutes &&
            current_time.Seconds == event->target_time.Seconds)
        {
            // Executar evento
            printf("Event triggered: %s\n", event->description);
            if (event->callback) event->callback();
            
            event->active = 0;  // Desativar após execução
        }
    }
}
```

### 4. Controle de Acesso com Tempo
```c
typedef struct {
    uint32_t user_id;
    RTC_DateTypeDef access_date;
    RTC_TimeTypeDef access_time;
    uint8_t access_granted;
} access_log_t;

access_log_t access_history[50];
uint8_t access_log_index = 0;

uint8_t check_time_based_access(uint32_t user_id)
{
    RTC_DateTypeDef current_date;
    RTC_TimeTypeDef current_time;
    
    HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN);
    
    uint8_t access_granted = 0;
    
    // Verificar horário de funcionamento (8:00 às 18:00)
    if (current_time.Hours >= 8 && current_time.Hours < 18)
    {
        // Verificar dias úteis (Segunda a Sexta)
        if (current_date.WeekDay >= RTC_WEEKDAY_MONDAY && 
            current_date.WeekDay <= RTC_WEEKDAY_FRIDAY)
        {
            access_granted = 1;
        }
    }
    
    // Log do acesso
    access_log_t *log_entry = &access_history[access_log_index];
    log_entry->user_id = user_id;
    log_entry->access_date = current_date;
    log_entry->access_time = current_time;
    log_entry->access_granted = access_granted;
    
    access_log_index = (access_log_index + 1) % 50;
    
    printf("Access %s for user %d at %02d:%02d:%02d on %02d/%02d/20%02d\n",
           access_granted ? "GRANTED" : "DENIED", user_id,
           current_time.Hours, current_time.Minutes, current_time.Seconds,
           current_date.Date, current_date.Month, current_date.Year);
    
    return access_granted;
}
```

## Configuração de Tempo Inicial

### Configuração Manual
```c
void set_initial_time_date(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // Configurar tempo atual (exemplo: 14:30:00)
    sTime.Hours = 14;
    sTime.Minutes = 30;
    sTime.Seconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    
    // Configurar data atual (exemplo: 15/03/2024, Sexta-feira)
    sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
    sDate.Month = RTC_MONTH_MARCH;
    sDate.Date = 15;
    sDate.Year = 24;  // 2024
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}
```

### Configuração via Serial/Interface
```c
void set_time_from_serial(char* time_string)
{
    // Formato esperado: "HH:MM:SS"
    RTC_TimeTypeDef sTime = {0};
    
    if (sscanf(time_string, "%02d:%02d:%02d", 
               &sTime.Hours, &sTime.Minutes, &sTime.Seconds) == 3)
    {
        if (sTime.Hours <= 23 && sTime.Minutes <= 59 && sTime.Seconds <= 59)
        {
            HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            printf("Time set to %02d:%02d:%02d\n", 
                   sTime.Hours, sTime.Minutes, sTime.Seconds);
        }
        else
        {
            printf("Invalid time format\n");
        }
    }
}

void set_date_from_serial(char* date_string)
{
    // Formato esperado: "DD/MM/YY"
    RTC_DateTypeDef sDate = {0};
    uint8_t day, month, year;
    
    if (sscanf(date_string, "%02d/%02d/%02d", &day, &month, &year) == 3)
    {
        if (day >= 1 && day <= 31 && month >= 1 && month <= 12)
        {
            sDate.Date = day;
            sDate.Month = month;
            sDate.Year = year;
            sDate.WeekDay = calculate_weekday(day, month, year + 2000);
            
            HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
            printf("Date set to %02d/%02d/20%02d\n", day, month, year);
        }
        else
        {
            printf("Invalid date format\n");
        }
    }
}
```

## Backup Domain e Persistência

### Verificação de Inicialização
```c
void check_rtc_initialization(void)
{
    // Verificar se RTC já foi inicializado
    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2)
    {
        // Primeira inicialização - configurar RTC
        set_initial_time_date();
        
        // Marcar como inicializado
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
        
        printf("RTC initialized for first time\n");
    }
    else
    {
        printf("RTC already initialized, keeping current time\n");
    }
}
```

### Backup de Configurações
```c
typedef struct {
    uint8_t timezone_offset;
    uint8_t daylight_saving;
    uint8_t date_format;  // DD/MM/YY ou MM/DD/YY
    uint8_t time_format;  // 12h ou 24h
} rtc_config_t;

void save_rtc_config(rtc_config_t *config)
{
    HAL_PWR_EnableBkUpAccess();
    
    // Usar backup registers para configurações
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 
                        (config->timezone_offset << 24) |
                        (config->daylight_saving << 16) |
                        (config->date_format << 8) |
                        config->time_format);
}

void load_rtc_config(rtc_config_t *config)
{
    uint32_t backup_data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    
    config->timezone_offset = (backup_data >> 24) & 0xFF;
    config->daylight_saving = (backup_data >> 16) & 0xFF;
    config->date_format = (backup_data >> 8) & 0xFF;
    config->time_format = backup_data & 0xFF;
}
```

## Troubleshooting

### Problemas Comuns

#### 1. RTC não funciona
**Sintomas:** Tempo não avança
**Causas possíveis:**
- LSI não inicializado
- Prescalers incorretos
- RTC não habilitado

**Solução:**
```c
// Verificar LSI
if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
{
    printf("LSI not ready!\n");
}

// Verificar RTC habilitado
if (__HAL_RCC_RTC_IS_ENABLED() == 0)
{
    __HAL_RCC_RTC_ENABLE();
}
```

#### 2. Tempo impreciso
**Sintomas:** RTC adianta/atrasa
**Causas:** LSI não é preciso (~±5%)

**Solução:**
```c
// Calibrar LSI ou usar LSE
uint32_t measured_lsi = calibrate_lsi_frequency();
adjust_rtc_prescalers(measured_lsi);
```

#### 3. Data/Hora perdida após reset
**Sintomas:** RTC reseta a cada boot
**Causas:** Backup domain não configurado

**Solução:**
```c
// Habilitar backup domain
HAL_PWR_EnableBkUpAccess();
__HAL_RCC_BACKUP_FORCE_RESET();
__HAL_RCC_BACKUP_RELEASE_RESET();
```

#### 4. Formato BCD/BIN incorreto
**Sintomas:** Valores estranhos
**Cause:** Mistura de formatos

**Solução:**
```c
// Sempre usar mesmo formato
HAL_RTC_GetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
HAL_RTC_SetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
```

## Utilitários RTC

### Cálculo de Dia da Semana
```c
uint8_t calculate_weekday(uint8_t day, uint8_t month, uint16_t year)
{
    // Algoritmo de Zeller
    if (month < 3) {
        month += 12;
        year--;
    }
    
    uint8_t k = year % 100;
    uint8_t j = year / 100;
    
    uint8_t h = (day + ((13 * (month + 1)) / 5) + k + (k / 4) + (j / 4) - 2 * j) % 7;
    
    // Converter para formato STM32 (1=Monday, 7=Sunday)
    return ((h + 5) % 7) + 1;
}
```

### Validação de Data/Hora
```c
uint8_t validate_time(RTC_TimeTypeDef *time)
{
    if (time->Hours > 23) return 0;
    if (time->Minutes > 59) return 0;
    if (time->Seconds > 59) return 0;
    return 1;
}

uint8_t validate_date(RTC_DateTypeDef *date)
{
    if (date->Month < 1 || date->Month > 12) return 0;
    if (date->Date < 1 || date->Date > 31) return 0;
    if (date->WeekDay < 1 || date->WeekDay > 7) return 0;
    
    // Verificar dias do mês
    uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Ano bissexto
    uint16_t full_year = 2000 + date->Year;
    if ((full_year % 4 == 0 && full_year % 100 != 0) || (full_year % 400 == 0))
    {
        days_in_month[1] = 29;  // Fevereiro
    }
    
    if (date->Date > days_in_month[date->Month - 1]) return 0;
    
    return 1;
}
```

### Conversão de Timestamp
```c
uint32_t rtc_to_unix_timestamp(RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
{
    // Converter RTC para Unix timestamp (segundos desde 1970)
    uint16_t full_year = 2000 + date->Year;
    
    // Cálculo simplificado (não considera anos bissextos completamente)
    uint32_t days = (full_year - 1970) * 365;
    days += (full_year - 1969) / 4;  // Anos bissextos aproximados
    
    // Adicionar dias do ano atual
    uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int i = 0; i < date->Month - 1; i++)
    {
        days += days_in_month[i];
    }
    days += date->Date - 1;
    
    // Converter para segundos
    uint32_t timestamp = days * 86400;  // dias * segundos/dia
    timestamp += time->Hours * 3600;
    timestamp += time->Minutes * 60;
    timestamp += time->Seconds;
    
    return timestamp;
}
```

## Conceitos Aprendidos
1. **RTC configuration** - Configuração básica do Real-Time Clock
2. **Clock sources** - Fontes de clock para RTC (LSI/LSE)
3. **Prescaler calculation** - Cálculo de prescalers para timing preciso
4. **Data/Time structures** - Estruturas de data e hora do HAL
5. **BCD vs BIN formats** - Formatos de representação numérica
6. **Backup domain** - Domínio de backup para persistência
7. **Calendar functions** - Funções de calendário completo

## Melhorias Sugeridas
1. **LSE implementation** - Implementar com cristal LSE para precisão
2. **Alarm functionality** - Adicionar alarmes e wake-up
3. **Calibration system** - Sistema de calibração para LSI
4. **Time zone support** - Suporte a fusos horários
5. **NTP synchronization** - Sincronização via NTP
6. **Display integration** - Integração com displays LCD/OLED

## Hardware Recomendado
- **32.768kHz crystal:** Para RTC preciso com LSE
- **Backup battery:** Para manter RTC em power-off
- **Display LCD/OLED:** Para mostrar data/hora
- **Serial interface:** Para configuração de tempo
- **RTC module externo:** Para comparação e validação

## Observações Importantes
- **LSI accuracy:** LSI tem precisão de ±5% (não ideal para produção)
- **Backup domain:** RTC preservado em low-power modes
- **Format consistency:** Sempre usar mesmo formato (BCD/BIN)
- **Initial setup:** Configurar data/hora inicial corretamente
- **Prescaler calculation:** Prescalers devem ser calculados para clock source
- **Leap year:** Considerar anos bissextos em cálculos
- **Week day calculation:** Algoritmos específicos para dia da semana