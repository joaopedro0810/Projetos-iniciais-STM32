# Projeto28 - Sleep Mode 01

## Descrição do Projeto
Este projeto demonstra o uso do **Sleep Mode** no STM32F407VET6. O sistema implementa o modo de baixo consumo onde a CPU é suspensa mas os periféricos continuam funcionando, permitindo wake-up por interrupções externas. O sistema entra em sleep e desperta via botão K0, demonstrando uma aplicação básica de gerenciamento de energia.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 80MHz (PLL)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino LED:** PA6 (D1)
- **Pino Botão:** PE4 (K0) - External Interrupt

## Sleep Mode - Modo de Baixo Consumo

### O que é o Sleep Mode?
O **Sleep Mode** é o modo de baixo consumo mais básico do STM32, onde:
- **CPU parada:** Processador suspenso (não executa instruções)
- **Periféricos ativos:** Todos os periféricos continuam funcionando
- **Clocks ativos:** HSI, PLL e clocks de periféricos mantidos
- **RAM preservada:** Conteúdo da memória mantido
- **Wake-up rápido:** Retorno imediato à execução

### Modos de Sleep no STM32F4
| Modo | CPU | Periféricos | Clocks | RAM | Wake-up |
|------|-----|-------------|--------|-----|---------|
| **Sleep** | OFF | ON | ON | ON | Qualquer IRQ |
| **Stop** | OFF | OFF | OFF | ON | EXTI, RTC |
| **Standby** | OFF | OFF | OFF | OFF | WKUP, RTC |

### Sleep vs outros modos
```
Normal:    ~100mA @ 80MHz
Sleep:     ~50mA  (CPU off, periféricos on)
Stop:      ~300µA (CPU + periféricos off)
Standby:   ~3µA   (quase tudo off)
```

## Configuração do Sistema

### Clock Configuration (80MHz)
```c
void SystemClock_Config(void)
{
    // HSI como fonte do PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;        // HSI/8 = 2MHz
    RCC_OscInitStruct.PLL.PLLN = 80;       // 2MHz × 80 = 160MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // 160MHz/2 = 80MHz
    
    // Clock configuration
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  // 80MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;         // 80MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;          // 40MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;          // 80MHz
}
```

### GPIO Configuration
```c
static void MX_GPIO_Init(void)
{
    // LED D1 (PA6) - Output
    GPIO_InitStruct.Pin = D1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(D1_GPIO_Port, &GPIO_InitStruct);
    
    // Botão K0 (PE4) - External Interrupt
    GPIO_InitStruct.Pin = K0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Falling edge interrupt
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(K0_GPIO_Port, &GPIO_InitStruct);
    
    // EXTI4 Interrupt Enable
    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}
```

## Operação do Sleep Mode

### Sequência Principal
```c
while (1)
{
    // 1. LED apagado por 4 segundos (ativo)
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
    HAL_Delay(4000);
    
    // 2. LED aceso (pré-sleep)
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
    
    // 3. Configurar Sleep-on-Exit
    HAL_PWR_EnableSleepOnExit();
    
    // 4. Suspender SysTick (economizar energia)
    HAL_SuspendTick();
    
    // 5. Entrar em Sleep Mode
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    
    /* Sistema dormindo - CPU parada */
    
    // 6. Wake-up por interrupção (retorno aqui)
    HAL_ResumeTick();
    
    // 7. Piscar LED 20 vezes (confirmação wake-up)
    for (uint8_t i = 0; i < 20; i++)
    {
        HAL_GPIO_TogglePin(D1_GPIO_Port, D1_Pin);
        HAL_Delay(100);
    }
}
```

### Sleep-on-Exit Feature
```c
HAL_PWR_EnableSleepOnExit();
```
**Função:** Após a interrupção ser processada, o sistema automaticamente retorna ao sleep mode sem executar o código após `HAL_PWR_EnterSLEEPMode()`.

### SysTick Management
```c
HAL_SuspendTick();    // Antes do sleep
HAL_ResumeTick();     // Após wake-up
```
**Razão:** SysTick gera interrupções regulares (1ms), impedindo sleep profundo e consumindo energia desnecessariamente.

## Wake-up por Interrupção Externa

### EXTI Callback
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // Toggle LED imediatamente
    HAL_GPIO_TogglePin(D1_GPIO_Port, D1_Pin);
    
    // Desabilitar Sleep-on-Exit
    HAL_PWR_DisableSleepOnExit();
}
```

### Processo de Wake-up
1. **Botão pressionado** → Falling edge no PE4
2. **EXTI4 interrupt** → CPU desperta
3. **Callback executed** → `HAL_GPIO_EXTI_Callback()`
4. **Sleep-on-Exit disabled** → Permitir retorno ao loop principal
5. **Código após sleep** → Piscar LED 20 vezes

## Análise de Timing

### Timeline de Operação
```
t=0s:     LED OFF, HAL_Delay(4000)
t=4s:     LED ON, preparar sleep
t=4.001s: Entrar em SLEEP MODE
         .
         . (CPU parada, aguardando interrupção)
         .
t=X:      Botão pressionado → WAKE-UP
t=X+1ms:  LED toggle na callback
t=X+2ms:  Retomar execução principal
t=X+3ms:  Iniciar piscar LED (20x)
t=X+5s:   Fim do piscar, voltar ao início
```

### Consumo de Energia
```
Modo Normal (80MHz):  ~100mA
Sleep Mode:           ~50mA  (50% economia)
Wake-up time:         <1µs   (imediato)
```

## Modos de Entrada no Sleep

### 1. WFI (Wait For Interrupt) - Usado no projeto
```c
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
```
- **Wake-up:** Qualquer interrupção (habilitada ou não)
- **Resposta:** Imediata
- **Uso:** Aplicações que precisam responder a qualquer evento

### 2. WFE (Wait For Event)
```c
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
```
- **Wake-up:** Eventos específicos
- **Resposta:** Baseada em eventos
- **Uso:** Aplicações event-driven

## Regulator Modes

### 1. Main Regulator ON (usado no projeto)
```c
PWR_MAINREGULATOR_ON
```
- **Consumo:** Maior (~50mA)
- **Wake-up:** Mais rápido
- **Uso:** Sleep curto com wake-up frequente

### 2. Low Power Regulator
```c
PWR_LOWPOWERREGULATOR_ON
```
- **Consumo:** Menor (~20mA)
- **Wake-up:** Mais lento
- **Uso:** Sleep longo com wake-up raro

## Aplicações Práticas

### 1. Sistema de Monitoramento com Botão
```c
typedef struct {
    uint32_t sleep_count;
    uint32_t wake_events;
    uint32_t total_runtime;
} power_stats_t;

power_stats_t stats = {0};

void monitoring_system_main(void)
{
    while (1)
    {
        // Sistema ativo por tempo limitado
        active_monitoring_routine();
        HAL_Delay(5000);  // 5s ativo
        
        // Entrar em sleep para economizar energia
        stats.sleep_count++;
        enter_power_save_mode();
        
        // Wake-up por sensor ou botão
        stats.wake_events++;
        handle_wake_up_event();
    }
}

void enter_power_save_mode(void)
{
    // Desabilitar periféricos desnecessários
    disable_non_essential_peripherals();
    
    // Configurar wake-up sources
    HAL_PWR_EnableSleepOnExit();
    HAL_SuspendTick();
    
    // Sleep com regulator otimizado
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    
    HAL_ResumeTick();
}
```

### 2. Data Logger com Economia de Energia
```c
typedef struct {
    uint16_t sensor_data[100];
    uint8_t data_index;
    uint32_t last_reading_time;
} data_logger_t;

data_logger_t logger = {0};

void data_logger_main(void)
{
    while (1)
    {
        // Coletar dados dos sensores
        collect_sensor_data();
        
        // Processar e armazenar
        process_and_store_data();
        
        // Sleep entre leituras (economizar bateria)
        if (get_next_reading_time() > HAL_GetTick())
        {
            setup_rtc_wakeup(get_sleep_duration());
            enter_sleep_mode();
        }
    }
}

void collect_sensor_data(void)
{
    // Wake-up rápido permite leitura imediata
    uint16_t adc_value = read_adc_sensor();
    uint16_t temp_value = read_temperature();
    
    logger.sensor_data[logger.data_index++] = adc_value;
    logger.last_reading_time = HAL_GetTick();
    
    if (logger.data_index >= 100)
    {
        // Buffer cheio, transmitir dados
        transmit_data_batch();
        logger.data_index = 0;
    }
}
```

### 3. Sistema de Alarme com Múltiplos Sensores
```c
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    uint8_t active;
} sensor_input_t;

sensor_input_t sensors[] = {
    {GPIOE, GPIO_PIN_4, 0},  // Door sensor
    {GPIOE, GPIO_PIN_5, 0},  // Window sensor  
    {GPIOE, GPIO_PIN_6, 0},  // Motion sensor
};

void alarm_system_main(void)
{
    while (1)
    {
        // Sistema armado - entrar em sleep
        arm_alarm_system();
        
        // Sleep até sensor ser acionado
        HAL_PWR_EnableSleepOnExit();
        HAL_SuspendTick();
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        HAL_ResumeTick();
        
        // Wake-up por sensor - processar alarme
        handle_alarm_trigger();
        
        // Período ativo para verificações
        active_monitoring_period();
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // Identificar qual sensor foi acionado
    for (int i = 0; i < 3; i++)
    {
        if (GPIO_Pin == sensors[i].pin)
        {
            sensors[i].active = 1;
            trigger_alarm_sequence(i);
        }
    }
    
    HAL_PWR_DisableSleepOnExit();
}
```

### 4. Controle de Motor com Sleep Entre Comandos
```c
typedef enum {
    MOTOR_IDLE,
    MOTOR_RUNNING,
    MOTOR_SLEEP
} motor_state_t;

motor_state_t motor_state = MOTOR_IDLE;

void motor_control_main(void)
{
    while (1)
    {
        switch (motor_state)
        {
            case MOTOR_IDLE:
                // Aguardar comando
                if (command_received())
                {
                    motor_state = MOTOR_RUNNING;
                }
                else
                {
                    motor_state = MOTOR_SLEEP;
                }
                break;
                
            case MOTOR_RUNNING:
                execute_motor_command();
                motor_state = MOTOR_IDLE;
                break;
                
            case MOTOR_SLEEP:
                // Sleep para economizar energia
                enter_sleep_until_command();
                motor_state = MOTOR_IDLE;
                break;
        }
    }
}

void enter_sleep_until_command(void)
{
    // Sleep até receber comando (UART, SPI, etc.)
    HAL_PWR_EnableSleepOnExit();
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    HAL_ResumeTick();
}
```

## Wake-up Sources para Sleep Mode

### 1. External Interrupts (EXTI)
```c
// Qualquer EXTI pode acordar do sleep
GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;   // Falling edge
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;    // Rising edge  
GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; // Both edges
```

### 2. Timer Interrupts
```c
// Timer pode acordar do sleep
HAL_TIM_Base_Start_IT(&htim2);
```

### 3. UART Interrupts
```c
// Recepção UART pode acordar
HAL_UART_Receive_IT(&huart1, rx_buffer, 1);
```

### 4. ADC Interrupts
```c
// Conversão ADC pode acordar
HAL_ADC_Start_IT(&hadc1);
```

### 5. SysTick (se habilitado)
```c
// SysTick sempre acorda (por isso suspendemos)
HAL_SuspendTick();  // Evitar wake-up desnecessário
```

## Configurações Alternativas

### 1. Sleep com Low Power Regulator
```c
// Menor consumo, wake-up mais lento
HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
```

### 2. Sleep com WFE
```c
// Wake-up por eventos específicos
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
```

### 3. Sleep sem Sleep-on-Exit
```c
// Executar código após cada wake-up
// HAL_PWR_EnableSleepOnExit();  // Comentado
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
// Código aqui é executado após cada interrupção
```

### 4. Multiple Wake-up Sources
```c
void setup_multiple_wakeup_sources(void)
{
    // EXTI para botões
    setup_button_interrupts();
    
    // Timer para wake-up periódico
    setup_timer_wakeup();
    
    // UART para comandos remotos
    setup_uart_wakeup();
    
    // ADC para monitoramento de sensores
    setup_adc_threshold_wakeup();
}
```

## Troubleshooting

### Problemas Comuns

#### 1. Sistema não entra em Sleep
**Sintomas:** Código continua executando após `HAL_PWR_EnterSLEEPMode()`
**Causas possíveis:**
- SysTick não suspenso
- Interrupção pendente
- Peripheral interrupt ativo

**Solução:**
```c
// Verificar e limpar interrupções pendentes
HAL_SuspendTick();
__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
```

#### 2. Wake-up não funciona
**Sintomas:** Sistema não desperta com botão
**Causas possíveis:**
- EXTI não configurado
- Interrupt não habilitado
- Sleep-on-Exit impede retorno

**Solução:**
```c
// Verificar configuração EXTI
HAL_NVIC_EnableIRQ(EXTI4_IRQn);
HAL_PWR_DisableSleepOnExit();  // Na callback
```

#### 3. Wake-up muito frequente
**Sintomas:** Sistema acorda constantemente
**Causas possíveis:**
- SysTick ativo
- Peripheral interrupt
- Ruído no pino

**Solução:**
```c
// Suspender SysTick e verificar periféricos
HAL_SuspendTick();
disable_unnecessary_peripherals();
```

#### 4. Timing incorreto após wake-up
**Sintomas:** `HAL_Delay()` não funciona corretamente
**Causas:**
- SysTick não resumido
- Clock configuration perdida

**Solução:**
```c
// Sempre resumir SysTick após wake-up
HAL_ResumeTick();
```

## Medição de Consumo

### Setup para Medição
```c
void power_measurement_setup(void)
{
    // Minimizar consumo para medição precisa
    
    // 1. Desabilitar periféricos não usados
    __HAL_RCC_TIM2_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE();
    __HAL_RCC_USART1_CLK_DISABLE();
    
    // 2. Configurar pinos não usados como input pullup
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    
    // 3. Usar Low Power Regulator
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
```

### Resultados Típicos (STM32F407)
```
Normal mode @ 80MHz:     ~100mA
Sleep (Main Reg):        ~50mA
Sleep (LP Reg):          ~20mA
Stop mode:               ~300µA
Standby mode:            ~3µA
```

## Conceitos Aprendidos
1. **Sleep Mode** - Modo básico de baixo consumo
2. **WFI/WFE** - Diferentes métodos de entrada em sleep
3. **Sleep-on-Exit** - Retorno automático ao sleep após IRQ
4. **SysTick management** - Controle do timer de sistema
5. **External interrupts** - Wake-up por EXTI
6. **Power regulators** - Main vs Low Power regulator
7. **Energy optimization** - Otimização de consumo de energia

## Melhorias Sugeridas
1. **Multiple wake-up sources** - Várias fontes de wake-up
2. **Power measurement** - Medição de consumo real
3. **Sleep statistics** - Estatísticas de uso de energia
4. **Adaptive sleep** - Sleep adaptativo baseado na aplicação
5. **Wake-up reasons** - Identificação da causa do wake-up
6. **Power profiling** - Profile de consumo da aplicação

## Hardware Recomendado
- **Multímetro de precisão:** Para medir consumo de corrente
- **Power analyzer:** Para análise detalhada de consumo
- **Osciloscópio:** Para verificar timing de wake-up
- **Current probe:** Para medição não invasiva
- **Logic analyzer:** Para debug de sequências de wake-up

## Observações Importantes
- **Wake-up time:** ~1µs (muito rápido)
- **Context preservation:** RAM e registradores preservados
- **Peripheral operation:** Periféricos continuam funcionando
- **Clock dependency:** Depende dos clocks do sistema
- **Debug difficulty:** Debugger pode impedir sleep
- **Real-time capability:** Mantém capacidade de tempo real
- **Battery applications:** Ideal para aplicações com bateria