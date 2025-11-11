# Projeto30 - Shutdown Mode

## Descrição do Projeto
Este projeto demonstra o uso do **Shutdown Mode** no STM32F407VET6. O sistema implementa o modo de consumo ultra-baixo onde praticamente todo o circuito é desligado, mantendo apenas o domínio de backup ativo. O projeto mostra como configurar o wake-up pin e gerenciar o modo de menor consumo disponível no STM32.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino LED:** PA6 (D1) - Modo analógico para baixo consumo
- **Wake-up Pin:** PWR_WAKEUP_PIN1 (PA0)

## Shutdown Mode - Ultra Low Power

### O que é o Shutdown Mode?
O **Shutdown Mode** é o modo de consumo mais baixo disponível no STM32F4, onde:
- **CPU desligada:** Processador completamente off
- **Periféricos desligados:** Todos os periféricos off
- **Clocks desligados:** Todos os clocks parados
- **RAM perdida:** Conteúdo da memória não preservado
- **Consumo mínimo:** ~2-3µA (apenas backup domain)
- **Wake-up limitado:** Apenas por wake-up pin ou reset

### Comparação dos Modos de Poder
| Modo | CPU | Periféricos | Clocks | RAM | Consumo | Wake-up |
|------|-----|-------------|--------|-----|---------|---------|
| **Normal** | ON | ON | ON | ON | ~100mA | N/A |
| **Sleep** | OFF | ON | ON | ON | ~50mA | Qualquer IRQ |
| **Stop** | OFF | OFF | OFF | ON | ~300µA | EXTI, RTC |
| **Standby** | OFF | OFF | OFF | OFF | ~3µA | WKUP, RTC |
| **Shutdown** | OFF | OFF | OFF | OFF | **~2µA** | **WKUP only** |

### Características do Shutdown Mode
```
Consumo:         1.7µA @ 3.3V (típico)
Wake-up time:    ~1ms (tempo de boot)
Context:         Completamente perdido
Backup domain:   Mantido (RTC, backup registers)
Reset sources:   WKUP pin, NRST, POR
```

## Configuração do Sistema

### Clock Configuration (HSI 16MHz)
```c
void SystemClock_Config(void)
{
    // Usar apenas HSI para simplicidade
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // PLL desabilitado
    
    // Clock configuration simples
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // 16MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;      // 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;       // 8MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;       // 16MHz
    
    // Flash Latency 0 para baixa frequência
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}
```

### GPIO Configuration para Baixo Consumo
```c
static void MX_GPIO_Init(void)
{
    // LED D1 (PA6) configurado como ANALOG para minimizar consumo
    GPIO_InitStruct.Pin = D1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;  // Modo analógico
    GPIO_InitStruct.Pull = GPIO_NOPULL;       // Sem pull-up/down
    HAL_GPIO_Init(D1_GPIO_Port, &GPIO_InitStruct);
}
```

**Razão do modo analógico:**
- Pinos em modo analógico consomem menos corrente
- Elimina correntes de fuga dos buffers digitais
- Essencial para consumo ultra-baixo

## Operação do Shutdown Mode

### Sequência Principal
```c
while (1)
{
    // 1. Piscar LED 15 vezes (indicação visual)
    for (uint8_t i = 0; i < 15; i++)
    {
        HAL_GPIO_TogglePin(D1_GPIO_Port, D1_Pin);
        HAL_Delay(100);
    }
    
    // 2. Habilitar Wake-up Pin 1 (PA0)
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    // 3. Entrar em Shutdown Mode (comentado por segurança)
    // HAL_PWR_EnterSHUTDOWNMode();
    
    /* Sistema completamente desligado até wake-up */
}
```

### Por que está Comentado?
```c
// HAL_PWR_EnterSHUTDOWNMode();  // Comentado
```
**Razões de segurança:**
- **Debug impossível:** Debugger perde conexão
- **Programação difícil:** Difícil reprogramar o chip
- **Recovery complexo:** Necessita hardware externo para wake-up
- **Desenvolvimento seguro:** Evita "brick" durante desenvolvimento

## Wake-up Pin Configuration

### PWR_WAKEUP_PIN1 (PA0)
```c
HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
```

### Características do Wake-up Pin
```
Pino:           PA0 (PWR_WAKEUP_PIN1)
Trigger:        Rising edge (0V → 3.3V)
Pull-down:      Interno ativo
Tempo mínimo:   >20µs pulso
Corrente:       <1µA quando baixo
```

### Circuito de Wake-up Típico
```
                    3.3V
                     |
                   [R1] 10kΩ (pull-up externo opcional)
                     |
  Button ----+------- PA0 (WKUP1)
             |
           [R2] 10kΩ (pull-down interno ativo)
             |
            GND

Operação:
- Repouso: PA0 = 0V (pull-down interno)
- Pressed: PA0 = 3.3V (rising edge → wake-up)
```

## Análise de Consumo

### Breakdown do Consumo
```
Shutdown Mode:
├── Core power domain:     OFF
├── Peripheral domain:     OFF
├── Clock system:          OFF
├── SRAM:                  OFF (perdido)
├── Backup domain:         ~1.5µA
│   ├── RTC:              ~1µA
│   ├── Backup registers: ~0.3µA
│   └── LSE oscillator:   ~0.2µA
└── Wake-up circuitry:     ~0.5µA
    ├── WKUP pin detect:  ~0.3µA
    └── Power management: ~0.2µA

Total: ~2µA @ 3.3V, 25°C
```

### Fatores que Afetam o Consumo
```c
void optimize_for_shutdown(void)
{
    // 1. Configurar todos os pinos como analógicos
    configure_all_pins_analog();
    
    // 2. Desabilitar periféricos não essenciais
    disable_all_peripherals();
    
    // 3. Configurar domínio de backup se necessário
    configure_backup_domain();
    
    // 4. Limpar flags de wake-up
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    
    // 5. Entrar em shutdown
    HAL_PWR_EnterSHUTDOWNMode();
}
```

## Processo de Wake-up

### Wake-up Sequence
```
t=0:     Shutdown mode ativo (~2µA)
         .
         . (Sistema completamente off)
         .
t=X:     Rising edge no PA0 (botão pressionado)
t=X+1ms: Power-on reset gerado
t=X+2ms: Boot sequence inicia
t=X+5ms: SystemInit() executado
t=X+6ms: main() inicia
t=X+7ms: HAL_Init() executado
t=X+8ms: Sistema totalmente funcional
```

### Reset vs Wake-up
No Shutdown Mode, **wake-up = reset completo**:
- Não há diferença entre wake-up e power-on reset
- Todo o código reinicia do início
- RAM e registradores perdidos
- Sistema reinicializa completamente

### Detecção da Causa do Wake-up
```c
void check_wakeup_source(void)
{
    // Verificar se foi wake-up do shutdown
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
    {
        // Sistema acordou do Standby/Shutdown
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
        
        // Indicar wake-up (ex: piscar LED diferente)
        indicate_wakeup_from_shutdown();
    }
    else
    {
        // Power-on normal ou outro reset
        indicate_normal_startup();
    }
}
```

## Aplicações Práticas

### 1. Sensor Node com Bateria
```c
typedef struct {
    uint32_t measurement_count;
    uint32_t battery_level;
    uint8_t sensor_data[16];
} sensor_node_t;

void battery_powered_sensor_main(void)
{
    static sensor_node_t node_data __attribute__((section(".backup")));
    
    // Verificar fonte do wake-up
    if (was_wakeup_from_shutdown())
    {
        // Acordou para medição
        node_data.measurement_count++;
        
        // Ler sensores rapidamente
        read_sensors_fast(node_data.sensor_data);
        
        // Transmitir dados se necessário
        if (node_data.measurement_count % 10 == 0)
        {
            transmit_sensor_data(&node_data);
        }
        
        // Voltar para shutdown imediatamente
        setup_next_wakeup_timer();
        enter_shutdown_mode();
    }
    else
    {
        // Inicialização normal
        initialize_sensor_node(&node_data);
        setup_periodic_wakeup();
        enter_shutdown_mode();
    }
}

void setup_periodic_wakeup(void)
{
    // Configurar RTC para wake-up periódico
    // (RTC funciona mesmo em shutdown)
    RTC_WakeUpTypeDef wakeup_config;
    wakeup_config.WakeUpCounter = 3600;  // 1 hora
    wakeup_config.WakeUpClock = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;
    
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, &wakeup_config);
}
```

### 2. Emergency Backup System
```c
typedef struct {
    uint32_t emergency_events;
    uint32_t last_emergency_time;
    uint8_t system_status;
} emergency_system_t;

void emergency_backup_system(void)
{
    emergency_system_t *backup_data = get_backup_data();
    
    if (was_emergency_wakeup())
    {
        // Acordou por emergência
        backup_data->emergency_events++;
        backup_data->last_emergency_time = get_rtc_time();
        
        // Ações de emergência
        handle_emergency_situation();
        
        // Transmitir alerta
        send_emergency_alert(backup_data);
        
        // Log no backup domain
        log_emergency_event(backup_data);
        
        // Retornar ao shutdown
        prepare_for_shutdown();
        HAL_PWR_EnterSHUTDOWNMode();
    }
    else
    {
        // Inicialização normal
        initialize_emergency_system();
        setup_emergency_detection();
        HAL_PWR_EnterSHUTDOWNMode();
    }
}

void setup_emergency_detection(void)
{
    // Configurar múltiplos wake-up sources
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);  // Emergency button
    
    // RTC alarm para verificações periódicas
    setup_rtc_alarm_for_health_check();
    
    // Configurar backup domain para preservar estado
    enable_backup_domain();
}
```

### 3. Long-term Data Logger
```c
typedef struct {
    uint32_t log_entries;
    uint32_t next_log_time;
    float sensor_readings[100];
    uint8_t log_index;
} data_logger_t;

void long_term_data_logger(void)
{
    // Usar backup SRAM para preservar dados críticos
    data_logger_t *logger = (data_logger_t*)BACKUP_SRAM_BASE;
    
    if (was_scheduled_wakeup())
    {
        // Wake-up programado
        uint32_t current_time = get_rtc_timestamp();
        
        if (current_time >= logger->next_log_time)
        {
            // Hora de coletar dados
            float reading = read_sensor_quickly();
            logger->sensor_readings[logger->log_index++] = reading;
            logger->log_entries++;
            
            // Buffer cheio? Transmitir dados
            if (logger->log_index >= 100)
            {
                transmit_data_batch(logger);
                logger->log_index = 0;
            }
            
            // Programar próxima coleta
            logger->next_log_time = current_time + LOG_INTERVAL;
            setup_rtc_wakeup(LOG_INTERVAL);
        }
        
        // Retornar ao shutdown
        HAL_PWR_EnterSHUTDOWNMode();
    }
    else
    {
        // Inicialização
        initialize_logger(logger);
        setup_first_wakeup();
        HAL_PWR_EnterSHUTDOWNMode();
    }
}
```

### 4. Remote Monitoring Device
```c
typedef enum {
    WAKEUP_BUTTON,
    WAKEUP_RTC_ALARM,
    WAKEUP_EXTERNAL_TRIGGER,
    WAKEUP_POWER_ON
} wakeup_source_t;

void remote_monitoring_device(void)
{
    wakeup_source_t wakeup_source = determine_wakeup_source();
    
    switch (wakeup_source)
    {
        case WAKEUP_BUTTON:
            // Wake-up manual - entrar em modo configuração
            enter_configuration_mode();
            break;
            
        case WAKEUP_RTC_ALARM:
            // Wake-up programado - coletar e transmitir dados
            collect_monitoring_data();
            transmit_to_remote_server();
            schedule_next_wakeup();
            break;
            
        case WAKEUP_EXTERNAL_TRIGGER:
            // Evento externo - processar alarme
            process_external_alarm();
            send_immediate_alert();
            break;
            
        case WAKEUP_POWER_ON:
            // Inicialização normal
            initialize_monitoring_system();
            setup_monitoring_schedule();
            break;
    }
    
    // Sempre retornar ao shutdown para máxima economia
    prepare_for_shutdown();
    HAL_PWR_EnterSHUTDOWNMode();
}

wakeup_source_t determine_wakeup_source(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_WU))
    {
        return WAKEUP_BUTTON;
    }
    else if (rtc_alarm_triggered())
    {
        return WAKEUP_RTC_ALARM;
    }
    else if (external_trigger_detected())
    {
        return WAKEUP_EXTERNAL_TRIGGER;
    }
    else
    {
        return WAKEUP_POWER_ON;
    }
}
```

## Otimização para Ultra-Baixo Consumo

### 1. Configuração de Pinos
```c
void configure_all_pins_for_shutdown(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Habilitar todos os clocks de GPIO
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    // Configurar todos os pinos como analógicos
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    
    // GPIOA
    GPIO_InitStruct.Pin = GPIO_PIN_All & ~GPIO_PIN_0;  // Exceto WKUP1
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // GPIOB, C, D, E
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    // Desabilitar clocks após configuração
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
}
```

### 2. Desabilitação de Periféricos
```c
void disable_all_peripherals_for_shutdown(void)
{
    // Desabilitar todos os periféricos
    __HAL_RCC_TIM2_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE();
    __HAL_RCC_TIM4_CLK_DISABLE();
    __HAL_RCC_TIM5_CLK_DISABLE();
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_USART2_CLK_DISABLE();
    __HAL_RCC_USART3_CLK_DISABLE();
    __HAL_RCC_I2C1_CLK_DISABLE();
    __HAL_RCC_I2C2_CLK_DISABLE();
    __HAL_RCC_I2C3_CLK_DISABLE();
    __HAL_RCC_SPI1_CLK_DISABLE();
    __HAL_RCC_SPI2_CLK_DISABLE();
    __HAL_RCC_SPI3_CLK_DISABLE();
    __HAL_RCC_ADC1_CLK_DISABLE();
    __HAL_RCC_ADC2_CLK_DISABLE();
    __HAL_RCC_ADC3_CLK_DISABLE();
    __HAL_RCC_CAN1_CLK_DISABLE();
    __HAL_RCC_CAN2_CLK_DISABLE();
    
    // Desabilitar DMA
    __HAL_RCC_DMA1_CLK_DISABLE();
    __HAL_RCC_DMA2_CLK_DISABLE();
}
```

### 3. Backup Domain Setup
```c
void setup_backup_domain_for_shutdown(void)
{
    // Habilitar acesso ao backup domain
    HAL_PWR_EnableBkUpAccess();
    
    // Configurar LSE se necessário para RTC
    __HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET);
    
    // Selecionar LSE como fonte do RTC
    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
    __HAL_RCC_RTC_ENABLE();
    
    // Backup SRAM para dados persistentes
    __HAL_RCC_BKPSRAM_CLK_ENABLE();
    
    // Regulator de backup em low power
    HAL_PWREx_EnableBkUpReg();
}
```

## Troubleshooting

### Problemas Comuns

#### 1. Sistema não acorda
**Sintomas:** Sistema permanece em shutdown
**Causas possíveis:**
- Wake-up pin não configurado
- Pino WKUP1 não conectado corretamente
- Pulso muito curto no wake-up pin

**Solução:**
```c
// Verificar configuração do wake-up pin
HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

// Verificar conexão de hardware
// PA0 deve ter rising edge para wake-up
```

#### 2. Consumo maior que esperado
**Sintomas:** Consumo >10µA em shutdown
**Causas possíveis:**
- Pinos não configurados como analógicos
- Periféricos ainda habilitados
- Clocks não desabilitados

**Solução:**
```c
// Configurar todos os pinos como analógicos
configure_all_pins_for_shutdown();

// Desabilitar todos os periféricos
disable_all_peripherals_for_shutdown();
```

#### 3. Dificuldade para debug/programar
**Sintomas:** ST-Link não conecta
**Causas possíveis:**
- Sistema entra em shutdown muito rápido
- Debugger perde conexão

**Solução:**
```c
// Adicionar delay antes do shutdown para debug
HAL_Delay(5000);  // 5s para conectar debugger
HAL_PWR_EnterSHUTDOWNMode();

// Ou comentar temporariamente
// HAL_PWR_EnterSHUTDOWNMode();
```

#### 4. Perda de dados críticos
**Sintomas:** Dados perdidos após wake-up
**Causas:** RAM não preservada em shutdown

**Solução:**
```c
// Usar backup SRAM ou backup registers
void save_critical_data(uint32_t data)
{
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, data);
}

uint32_t restore_critical_data(void)
{
    return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
}
```

## Implementação Segura para Desenvolvimento

### 1. Shutdown com Timeout
```c
void safe_shutdown_with_timeout(uint32_t timeout_ms)
{
    static uint32_t shutdown_count = 0;
    
    // Permitir alguns ciclos para debug
    if (shutdown_count < 3)
    {
        HAL_Delay(timeout_ms);  // Delay para debug
        shutdown_count++;
    }
    else
    {
        // Após 3 ciclos, entrar em shutdown real
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
        HAL_PWR_EnterSHUTDOWNMode();
    }
}
```

### 2. Conditional Shutdown
```c
void conditional_shutdown(void)
{
    #ifdef DEBUG_MODE
        // Em modo debug, não entrar em shutdown
        HAL_Delay(1000);
        return;
    #else
        // Em release, entrar em shutdown
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
        HAL_PWR_EnterSHUTDOWNMode();
    #endif
}
```

### 3. Recovery Mechanism
```c
void setup_recovery_mechanism(void)
{
    // Configurar múltiplos wake-up sources
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    
    // RTC alarm como backup
    setup_rtc_wakeup_alarm(300000);  // 5 minutos
    
    // Watchdog como última linha de defesa
    setup_independent_watchdog();
}
```

## Medição de Consumo

### Setup para Medição Precisa
```c
void prepare_for_current_measurement(void)
{
    // 1. Configurar todos os pinos
    configure_all_pins_for_shutdown();
    
    // 2. Desabilitar todos os periféricos
    disable_all_peripherals_for_shutdown();
    
    // 3. Configurar backup domain mínimo
    setup_minimal_backup_domain();
    
    // 4. Limpar flags
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    
    // 5. Entrar em shutdown
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWR_EnterSHUTDOWNMode();
}
```

### Resultados Típicos de Medição
```
STM32F407VET6 @ 25°C, 3.3V:

Shutdown Mode (typical):    1.7µA
Shutdown Mode (maximum):    3.0µA
Temperature coefficient:    +50nA/°C
Voltage coefficient:        Linear com VDD

Componentes do consumo:
- Backup domain:           ~1.2µA
- Wake-up detection:       ~0.3µA  
- Leakage currents:        ~0.2µA
```

## Conceitos Aprendidos
1. **Shutdown Mode** - Modo de consumo ultra-baixo
2. **Wake-up pin** - Único método de wake-up do shutdown
3. **Context loss** - Perda completa de estado do sistema
4. **Backup domain** - Único domínio preservado
5. **Current optimization** - Técnicas de otimização de consumo
6. **Pin configuration** - Configuração analógica para baixo consumo
7. **Development safety** - Práticas seguras durante desenvolvimento

## Melhorias Sugeridas
1. **RTC wake-up** - Implementar wake-up por RTC alarm
2. **Backup SRAM usage** - Usar SRAM de backup para dados críticos
3. **Multiple wake-up sources** - Implementar múltiplas fontes de wake-up
4. **Current profiling** - Profile detalhado de consumo
5. **Battery monitoring** - Monitoramento de bateria integrado
6. **Data persistence** - Estratégias de persistência de dados

## Hardware Recomendado
- **Microamperímetro de precisão:** Para medir consumo ultra-baixo
- **Power analyzer:** Para análise detalhada de consumo
- **Wake-up button circuit:** Circuito de botão adequado
- **Battery simulation:** Para teste com alimentação por bateria
- **Isolation switches:** Para isolar circuitos durante medição

## Observações Importantes
- **Debug difficulty:** Debugger não funciona em shutdown
- **Programming challenges:** Difícil reprogramar se shutdown muito rápido
- **Complete reset:** Wake-up = reset completo do sistema
- **Data loss:** RAM e registradores completamente perdidos
- **Battery applications:** Ideal para aplicações ultra-baixo consumo
- **Development care:** Requer cuidado especial durante desenvolvimento
- **Recovery planning:** Sempre planejar mecanismos de recovery