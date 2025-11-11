# Projeto31 - Standby Mode

## Descrição do Projeto
Este projeto demonstra o uso do **Standby Mode** no STM32F407VET6. O sistema implementa o modo de baixo consumo onde CPU, periféricos e clocks são desligados, mas o domínio de backup permanece ativo, permitindo preservar dados críticos e funcionamento do RTC. O projeto mostra como entrar em standby e como acordar via wake-up pin.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino LED:** PA6 (D1)
- **Wake-up Pin:** PWR_WAKEUP_PIN1 (PA0)

## Standby Mode - Baixo Consumo com Backup Domain

### O que é o Standby Mode?
O **Standby Mode** é um modo de baixo consumo intermediário do STM32F4, onde:
- **CPU desligada:** Processador completamente off
- **Periféricos desligados:** Todos os periféricos off
- **Clocks desligados:** Sistema de clocks parado
- **RAM perdida:** Conteúdo da SRAM não preservado
- **Backup domain ativo:** RTC e backup registers preservados
- **Consumo baixo:** ~3µA (incluindo backup domain)
- **Wake-up flexível:** Wake-up pin, RTC alarm, ou reset

### Comparação dos Modos de Poder
| Modo | CPU | Periféricos | Clocks | RAM | Backup | Consumo | Wake-up |
|------|-----|-------------|--------|-----|--------|---------|---------|
| **Sleep** | OFF | ON | ON | ON | ON | ~50mA | Qualquer IRQ |
| **Stop** | OFF | OFF | OFF | ON | ON | ~300µA | EXTI, RTC |
| **Standby** | OFF | OFF | OFF | OFF | **ON** | **~3µA** | **WKUP, RTC** |
| **Shutdown** | OFF | OFF | OFF | OFF | OFF | ~2µA | WKUP only |

### Vantagens do Standby Mode
```
✓ Consumo muito baixo (~3µA)
✓ RTC continua funcionando
✓ Backup registers preservados
✓ Wake-up por RTC alarm
✓ Wake-up pin disponível
✓ Tempo de wake-up rápido
✓ Backup SRAM opcional
```

## Configuração do Sistema

### Clock Configuration (HSI 16MHz)
```c
void SystemClock_Config(void)
{
    // Configuração simples com HSI apenas
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // PLL desabilitado
    
    // Clock configuration
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // 16MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;      // 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;       // 16MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;       // 16MHz
    
    // Flash Latency 0 para baixa frequência
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}
```

### GPIO Configuration
```c
static void MX_GPIO_Init(void)
{
    // LED D1 (PA6) - Output push-pull
    GPIO_InitStruct.Pin = D1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(D1_GPIO_Port, &GPIO_InitStruct);
    
    // Inicializar LED apagado
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
}
```

## Operação do Standby Mode

### Sequência Principal
```c
while (1)
{
    // 1. Piscar LED 15 vezes (indicação pré-standby)
    for (uint8_t i = 0; i < 15; i++)
    {
        HAL_GPIO_TogglePin(D1_GPIO_Port, D1_Pin);
        HAL_Delay(100);  // 100ms on/off
    }
    
    // 2. Habilitar Wake-up Pin 1 (PA0)
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    // 3. Limpar flag de Standby
    __HAL_PWR_CLEAR_FLAG(PWR_CSR_SBF);  // Clear Standby Flag
    
    // 4. Entrar em Standby Mode
    HAL_PWR_EnterSTANDBYMode();
    
    /* Sistema em standby - aguardando wake-up */
    
    // 5. Código após wake-up (sistema reiniciou)
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
    HAL_Delay(5000);  // 5s após wake-up
}
```

### Análise do Fluxo de Execução
```
t=0:      Sistema inicia
t=0-1.5s: Piscar LED 15 vezes (15 × 100ms × 2)
t=1.5s:   Configurar wake-up pin
t=1.501s: Limpar flags
t=1.502s: Entrar em STANDBY MODE
          .
          . (Sistema em standby ~3µA)
          .
t=X:      Wake-up (botão pressionado)
t=X+1ms:  Sistema reinicia (reset completo)
t=X+2ms:  main() inicia novamente
t=X+3ms:  LED apaga por 5s
t=X+8s:   Novo ciclo inicia
```

## Wake-up do Standby Mode

### Wake-up Sources
O Standby Mode pode ser acordado por:

#### 1. Wake-up Pin (PWR_WAKEUP_PIN1)
```c
HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
```
- **Pino:** PA0 (WKUP1)
- **Trigger:** Rising edge (0V → 3.3V)
- **Pull-down:** Interno ativo
- **Tempo mínimo:** >20µs

#### 2. RTC Alarm (se configurado)
```c
// Exemplo de configuração RTC alarm
HAL_RTC_SetAlarm_IT(&hrtc, &alarm_config, RTC_FORMAT_BIN);
```

#### 3. Reset Pins
- NRST pin
- Power-on reset
- Brown-out reset

### Detecção da Causa do Wake-up
```c
void check_wakeup_source(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
    {
        // Acordou do Standby Mode
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
        
        if (__HAL_PWR_GET_FLAG(PWR_FLAG_WU))
        {
            // Wake-up por WKUP pin
            __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
            handle_wakeup_pin_event();
        }
        else
        {
            // Wake-up por RTC ou outro
            handle_rtc_wakeup_event();
        }
    }
    else
    {
        // Power-on reset normal
        handle_normal_startup();
    }
}
```

### Flag Management
```c
// Flag PWR_CSR_SBF (Standby Flag)
__HAL_PWR_CLEAR_FLAG(PWR_CSR_SBF);  // Limpar antes de entrar

// Flag PWR_FLAG_WU (Wake-up Flag)  
__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);  // Limpar após wake-up
```

## Backup Domain no Standby Mode

### O que é Preservado
No Standby Mode, apenas o **backup domain** permanece ativo:
- **RTC:** Real-Time Clock continua funcionando
- **Backup registers:** 20 registradores de 32 bits preservados
- **Backup SRAM:** 4KB de SRAM preservada (se habilitada)
- **LSE oscillator:** Cristal externo de 32.768kHz (se usado)

### Configuração do Backup Domain
```c
void setup_backup_domain(void)
{
    // Habilitar acesso ao backup domain
    HAL_PWR_EnableBkUpAccess();
    
    // Configurar LSE para RTC (opcional)
    __HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET);
    
    // RTC clock source
    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
    __HAL_RCC_RTC_ENABLE();
    
    // Backup SRAM (opcional)
    __HAL_RCC_BKPSRAM_CLK_ENABLE();
    HAL_PWREx_EnableBkUpReg();  // Regulator para backup SRAM
}
```

### Uso de Backup Registers
```c
// Salvar dados críticos antes do standby
void save_critical_data_to_backup(void)
{
    HAL_PWR_EnableBkUpAccess();
    
    // Salvar diferentes tipos de dados
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, system_state);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, error_count);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, wake_up_count);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, last_sensor_reading);
}

// Restaurar dados após wake-up
void restore_critical_data_from_backup(void)
{
    system_state = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
    error_count = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    wake_up_count = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    last_sensor_reading = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
}
```

## Aplicações Práticas

### 1. Data Logger com Wake-up Programado
```c
typedef struct {
    uint32_t log_count;
    uint32_t next_log_time;
    uint16_t sensor_readings[50];
    uint8_t reading_index;
} data_logger_backup_t;

void rtc_data_logger_main(void)
{
    RTC_HandleTypeDef hrtc;
    data_logger_backup_t *backup_data;
    
    // Configurar backup domain
    setup_rtc_and_backup_domain();
    
    // Usar backup SRAM para dados estruturados
    backup_data = (data_logger_backup_t*)BACKUP_SRAM_BASE;
    
    if (was_standby_wakeup())
    {
        // Acordou do standby
        backup_data->log_count++;
        
        // Verificar se é hora de coletar dados
        uint32_t current_time = get_rtc_timestamp();
        if (current_time >= backup_data->next_log_time)
        {
            // Coletar dados rapidamente
            uint16_t reading = read_sensor_fast();
            backup_data->sensor_readings[backup_data->reading_index++] = reading;
            
            // Buffer cheio? Transmitir
            if (backup_data->reading_index >= 50)
            {
                transmit_data_batch(backup_data->sensor_readings);
                backup_data->reading_index = 0;
            }
            
            // Programar próxima coleta
            backup_data->next_log_time = current_time + LOG_INTERVAL;
            setup_rtc_alarm(LOG_INTERVAL);
        }
        
        // Retornar ao standby
        enter_standby_mode();
    }
    else
    {
        // Inicialização normal
        initialize_data_logger(backup_data);
        setup_first_rtc_alarm();
        enter_standby_mode();
    }
}
```

### 2. Sistema de Alarme com Múltiplos Wake-up
```c
typedef struct {
    uint32_t alarm_events;
    uint32_t false_alarms;
    uint32_t system_uptime;
    uint8_t alarm_zones[8];
} alarm_system_backup_t;

void security_alarm_system(void)
{
    alarm_system_backup_t *backup_data;
    
    // Usar backup registers para dados críticos
    restore_alarm_data_from_backup();
    
    if (was_standby_wakeup())
    {
        wakeup_source_t source = get_wakeup_source();
        
        switch (source)
        {
            case WAKEUP_BUTTON:
                // Wake-up manual (botão de teste)
                backup_data->alarm_events++;
                perform_system_test();
                break;
                
            case WAKEUP_RTC_ALARM:
                // Wake-up programado (verificação periódica)
                perform_periodic_health_check();
                update_system_status();
                break;
                
            case WAKEUP_EXTERNAL:
                // Sensor de segurança acionado
                backup_data->alarm_events++;
                handle_security_breach();
                send_immediate_alert();
                break;
        }
        
        // Salvar estado e voltar ao standby
        save_alarm_data_to_backup();
        setup_next_wakeup();
        enter_standby_mode();
    }
    else
    {
        // Inicialização do sistema
        initialize_alarm_system();
        setup_security_monitoring();
        enter_standby_mode();
    }
}
```

### 3. Medidor de Energia com RTC
```c
typedef struct {
    float total_energy_kwh;
    uint32_t measurement_count;
    uint32_t last_measurement_time;
    float daily_energy[31];  // 31 dias
} energy_meter_backup_t;

void energy_meter_main(void)
{
    energy_meter_backup_t *backup_data = get_backup_data();
    RTC_TimeTypeDef current_time;
    RTC_DateTypeDef current_date;
    
    if (was_standby_wakeup())
    {
        // Acordou para medição
        HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN);
        
        // Medir consumo instantâneo
        float current_power = measure_power_consumption();
        float energy_increment = calculate_energy_increment(current_power);
        
        // Atualizar totalizadores
        backup_data->total_energy_kwh += energy_increment;
        backup_data->measurement_count++;
        
        // Atualizar energia diária
        uint8_t day_index = current_date.Date - 1;
        backup_data->daily_energy[day_index] += energy_increment;
        
        // Verificar se precisa transmitir dados
        if (backup_data->measurement_count % 144 == 0)  // A cada 144 medições (24h)
        {
            transmit_daily_energy_report(backup_data);
        }
        
        // Programar próxima medição (10 minutos)
        setup_rtc_alarm_relative(10 * 60);
        
        // Voltar ao standby
        save_energy_data_to_backup();
        enter_standby_mode();
    }
    else
    {
        // Inicialização
        initialize_energy_meter();
        setup_rtc_for_measurements();
        enter_standby_mode();
    }
}
```

### 4. Estação Meteorológica Autônoma
```c
typedef struct {
    float temperature_history[24];
    float humidity_history[24];
    float pressure_history[24];
    uint8_t history_index;
    uint32_t last_transmission_time;
} weather_station_backup_t;

void autonomous_weather_station(void)
{
    weather_station_backup_t *backup_data = get_backup_data();
    
    if (was_standby_wakeup())
    {
        wakeup_source_t source = determine_wakeup_source();
        
        if (source == WAKEUP_RTC_HOURLY)
        {
            // Coleta horária de dados
            backup_data->temperature_history[backup_data->history_index] = read_temperature();
            backup_data->humidity_history[backup_data->history_index] = read_humidity();
            backup_data->pressure_history[backup_data->history_index] = read_pressure();
            
            backup_data->history_index = (backup_data->history_index + 1) % 24;
            
            // Transmitir dados a cada 6 horas
            uint32_t current_time = get_rtc_timestamp();
            if ((current_time - backup_data->last_transmission_time) >= (6 * 3600))
            {
                transmit_weather_data(backup_data);
                backup_data->last_transmission_time = current_time;
            }
            
            // Programar próxima coleta
            setup_rtc_alarm_hourly();
        }
        else if (source == WAKEUP_BUTTON)
        {
            // Wake-up manual - transmitir dados imediatos
            transmit_current_weather_data();
        }
        
        // Salvar dados e voltar ao standby
        save_weather_data_to_backup();
        enter_standby_mode();
    }
    else
    {
        // Inicialização
        initialize_weather_sensors();
        setup_rtc_for_weather_monitoring();
        enter_standby_mode();
    }
}
```

## RTC Integration com Standby Mode

### Configuração de RTC para Wake-up
```c
void setup_rtc_wakeup_alarm(uint32_t seconds_from_now)
{
    RTC_AlarmTypeDef alarm_config = {0};
    RTC_TimeTypeDef current_time;
    RTC_DateTypeDef current_date;
    
    // Obter tempo atual
    HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN);
    
    // Calcular tempo do alarme
    uint32_t alarm_seconds = current_time.Seconds + seconds_from_now;
    uint32_t alarm_minutes = current_time.Minutes + (alarm_seconds / 60);
    uint32_t alarm_hours = current_time.Hours + (alarm_minutes / 60);
    
    alarm_config.AlarmTime.Seconds = alarm_seconds % 60;
    alarm_config.AlarmTime.Minutes = alarm_minutes % 60;
    alarm_config.AlarmTime.Hours = alarm_hours % 24;
    alarm_config.AlarmTime.SubSeconds = 0;
    alarm_config.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    
    alarm_config.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
    alarm_config.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    alarm_config.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarm_config.Alarm = RTC_ALARM_A;
    
    // Configurar alarme
    HAL_RTC_SetAlarm_IT(&hrtc, &alarm_config, RTC_FORMAT_BIN);
}
```

### Wake-up Timer (Alternativa)
```c
void setup_rtc_wakeup_timer(uint32_t seconds)
{
    // Configurar wake-up timer (mais simples que alarm)
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, seconds, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}
```

## Otimização de Consumo

### 1. Configuração de Pinos para Standby
```c
void configure_pins_for_standby(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Habilitar clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    // Configurar pinos não usados como analógicos
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    
    // GPIOA (exceto PA0 - WKUP)
    GPIO_InitStruct.Pin = GPIO_PIN_All & ~GPIO_PIN_0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Outros GPIOs
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
```

### 2. Desabilitação de Periféricos
```c
void disable_peripherals_for_standby(void)
{
    // Desabilitar periféricos não essenciais
    __HAL_RCC_TIM2_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE();
    __HAL_RCC_TIM4_CLK_DISABLE();
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_USART2_CLK_DISABLE();
    __HAL_RCC_I2C1_CLK_DISABLE();
    __HAL_RCC_I2C2_CLK_DISABLE();
    __HAL_RCC_SPI1_CLK_DISABLE();
    __HAL_RCC_SPI2_CLK_DISABLE();
    __HAL_RCC_ADC1_CLK_DISABLE();
    __HAL_RCC_DMA1_CLK_DISABLE();
    __HAL_RCC_DMA2_CLK_DISABLE();
}
```

### 3. Função Completa de Entrada em Standby
```c
void enter_optimized_standby_mode(void)
{
    // 1. Configurar pinos para baixo consumo
    configure_pins_for_standby();
    
    // 2. Desabilitar periféricos desnecessários
    disable_peripherals_for_standby();
    
    // 3. Configurar wake-up sources
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    // 4. Configurar RTC alarm se necessário
    // setup_rtc_wakeup_alarm(3600);  // 1 hora
    
    // 5. Limpar flags
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    
    // 6. Entrar em standby
    HAL_PWR_EnterSTANDBYMode();
}
```

## Troubleshooting

### Problemas Comuns

#### 1. Sistema não entra em Standby
**Sintomas:** Código continua executando após `HAL_PWR_EnterSTANDBYMode()`
**Causas possíveis:**
- Interrupção pendente
- Peripheral ativo impedindo standby
- Debug session ativa

**Solução:**
```c
// Limpar todas as interrupções pendentes
__disable_irq();
SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
__enable_irq();

// Desabilitar debug em standby
DBGMCU->CR = 0;
```

#### 2. Wake-up não funciona
**Sintomas:** Sistema não acorda com botão
**Causas possíveis:**
- WKUP pin não configurado
- Flag não limpa antes de entrar
- Circuito de botão inadequado

**Solução:**
```c
// Verificar configuração completa
HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
```

#### 3. Dados perdidos após wake-up
**Sintomas:** Variáveis resetadas
**Causas:** RAM não preservada em standby

**Solução:**
```c
// Usar backup domain
save_data_to_backup_registers();
// ou
save_data_to_backup_sram();
```

#### 4. RTC não funciona em standby
**Sintomas:** RTC para durante standby
**Causas possíveis:**
- LSE não configurado
- Backup domain não inicializado
- Power supply insuficiente

**Solução:**
```c
// Configurar LSE corretamente
__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET);
__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
```

## Medição de Consumo

### Consumo Típico em Standby
```
STM32F407VET6 @ 25°C, 3.3V:

Standby Mode (LSI RTC):      2.8µA
Standby Mode (LSE RTC):      3.2µA
Standby + Backup SRAM:       4.1µA
Temperature coefficient:     +30nA/°C

Componentes:
- Core standby logic:        ~0.8µA
- RTC (LSI):                ~1.5µA
- RTC (LSE):                ~1.9µA
- Backup registers:         ~0.3µA
- Backup SRAM (4KB):        ~0.9µA
- Wake-up detection:        ~0.2µA
```

### Setup para Medição
```c
void prepare_for_standby_current_measurement(void)
{
    // Configuração mínima para medição
    configure_pins_for_standby();
    disable_peripherals_for_standby();
    
    // RTC mínimo
    setup_minimal_rtc();
    
    // Entrar em standby
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU | PWR_FLAG_SB);
    HAL_PWR_EnterSTANDBYMode();
}
```

## Conceitos Aprendidos
1. **Standby Mode** - Baixo consumo com backup domain
2. **Wake-up sources** - Múltiplas fontes de wake-up (WKUP, RTC)
3. **Backup domain** - Preservação de dados críticos
4. **RTC integration** - Integração com Real-Time Clock
5. **Flag management** - Gerenciamento de flags de power
6. **Current optimization** - Otimização de consumo
7. **Context preservation** - Preservação seletiva de dados

## Melhorias Sugeridas
1. **Multiple wake-up sources** - Implementar múltiplos wake-up
2. **Advanced RTC usage** - Uso avançado do RTC com alarms
3. **Backup SRAM utilization** - Uso eficiente da backup SRAM
4. **Power profiling** - Profile detalhado de consumo
5. **Wake-up source detection** - Detecção precisa da fonte de wake-up
6. **Battery monitoring** - Monitoramento integrado de bateria

## Hardware Recomendado
- **Microamperímetro:** Para medir consumo de 1-10µA
- **32.768kHz crystal:** Para RTC preciso
- **Wake-up button:** Botão com pull-down adequado
- **Battery holder:** Para testes com bateria
- **Power analyzer:** Para análise detalhada

## Observações Importantes
- **Context loss:** RAM principal perdida (usar backup domain)
- **Reset behavior:** Wake-up = reset completo do sistema
- **RTC preservation:** RTC continua funcionando
- **Debug limitations:** Debugger pode não funcionar em standby
- **Power supply:** Requer alimentação estável para backup domain
- **Wake-up timing:** Wake-up mais rápido que shutdown mode
- **Battery applications:** Excelente para aplicações com bateria