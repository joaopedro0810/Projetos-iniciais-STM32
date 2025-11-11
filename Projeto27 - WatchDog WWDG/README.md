# Projeto27 - WatchDog WWDG

## Descrição do Projeto
Este projeto demonstra o uso do **WWDG (Window Watchdog)** no STM32F407VET6. O sistema implementa um watchdog com janela temporal que deve ser alimentado apenas dentro de um intervalo específico, proporcionando detecção tanto de código muito lento quanto muito rápido, garantindo timing preciso em aplicações críticas.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Clock WWDG:** PCLK1 (8MHz com APB1 DIV2)
- **Pino LED:** PA5 (LED_D1)

## WWDG - Window Watchdog

### O que é o Window Watchdog?
O **WWDG (Window Watchdog)** é um watchdog avançado que monitora não apenas se o software está executando, mas também se está executando na velocidade correta. Diferente do IWDG, o WWDG possui uma "janela" - período onde o refresh é permitido.

### Características do WWDG STM32F407
- **Window control:** Refresh permitido apenas em janela específica
- **Early warning:** Interrupção antes do reset (opcional)
- **Clock dependente:** Usa PCLK1 (afetado por falhas do clock)
- **Timeout curto:** 683µs a 87.38ms máximo
- **Configurável:** Prescaler e valores de window/counter

### WWDG vs IWDG
| Característica | WWDG | IWDG |
|----------------|------|------|
| **Clock** | PCLK1 (sistema) | LSI (independente) |
| **Window** | Sim (timing preciso) | Não |
| **Early Warning** | Sim (EWI) | Não |
| **Timeout Range** | 683µs - 87ms | 125µs - 32.7s |
| **Aplicação** | Timing crítico | Segurança geral |

## Configuração do WWDG

### Parâmetros de Configuração
```c
static void MX_WWDG_Init(void)
{
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_2;    // Prescaler = 2
    hwwdg.Init.Window = 89;                     // Window = 89
    hwwdg.Init.Counter = 113;                   // Counter inicial = 113
    hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;      // Early Warning desabilitado
    
    if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### Clock Configuration para WWDG
```c
void SystemClock_Config(void)
{
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     // HCLK = 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;      // APB1 = 8MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;      // APB2 = 16MHz
}
```

**Clock WWDG:**
```
PCLK1 = 16MHz / 2 = 8MHz
WWDG_Clock = PCLK1 / 4096 = 8MHz / 4096 ≈ 1953Hz
```

## Cálculo do Timing

### Fórmula do Timeout
```
Timeout = (Counter_Value / WWDG_Clock) × Prescaler
```

### Para o Projeto
```c
WWDG_Clock = PCLK1 / 4096 = 8MHz / 4096 = 1953Hz
Prescaler = 2
Counter = 113

Timeout = (113 / 1953) × 2 ≈ 115.7ms
```

### Window Calculation
```c
Window_Start = ((Counter - Window) / WWDG_Clock) × Prescaler
Window_Start = ((113 - 89) / 1953) × 2 = (24 / 1953) × 2 ≈ 24.6ms

Window_Duration = Window_Start = 24.6ms
Valid_Refresh_Window = 24.6ms a 115.7ms após inicialização
```

### Timing Diagram
```
t=0ms:    WWDG Starts (Counter = 113)
t=24.6ms: Window Opens (Counter = 89)  ← Refresh permitido
t=35ms:   HAL_WWDG_Refresh() no projeto ← Dentro da window
t=115.7ms: Timeout (Counter = 64)      ← Reset se não refresh
```

## Prescaler Options

### Valores Disponíveis
```c
WWDG_PRESCALER_1    // Prescaler = 1    (menor timeout)
WWDG_PRESCALER_2    // Prescaler = 2    (usado no projeto)
WWDG_PRESCALER_4    // Prescaler = 4
WWDG_PRESCALER_8    // Prescaler = 8    (maior timeout)
```

### Range de Timeout por Prescaler
```c
PRESCALER_1: 683µs a 43.69ms
PRESCALER_2: 1.37ms a 87.38ms  (usado no projeto)
PRESCALER_4: 2.73ms a 174.76ms
PRESCALER_8: 5.46ms a 349.53ms
```

## Operação da Window

### Conceito de Window
O WWDG possui um **contador decrescente** que:
1. **Inicia** no valor Counter (113)
2. **Decrementa** continuamente
3. **Window abre** quando atinge valor Window (89)
4. **Reset ocorre** se atingir 63 (0x3F)

### Estados de Operação
```
Counter > Window (113-90): REFRESH PROIBIDO (muito cedo)
Counter = Window (89):     WINDOW ABRE
89 ≥ Counter > 63:        REFRESH PERMITIDO
Counter = 63:             RESET AUTOMÁTICO
```

### Refresh Timing
```c
while (1)
{
    HAL_Delay(35);                    // Aguardar 35ms
    HAL_WWDG_Refresh(&hwwdg);         // Refresh na janela permitida
}
```

**Análise do timing:**
- **Delay:** 35ms
- **Window opening:** ~24.6ms
- **Window closing:** ~115.7ms
- **35ms está dentro da janela válida** ✓

## Cenários de Operação

### 1. Operação Normal (Refresh na Window)
```
t=0ms:   Counter=113, WWDG inicia
t=24.6ms: Counter=89, Window abre
t=35ms:  HAL_WWDG_Refresh() → Counter volta para 113 ✓
t=59.6ms: Counter=89, Window abre novamente
t=70ms:  HAL_WWDG_Refresh() → Counter volta para 113 ✓
...      Operação continua normalmente
```

### 2. Refresh Muito Cedo (Fora da Window)
```c
// Código problemático
while (1)
{
    HAL_Delay(10);                // 10ms < 24.6ms window opening
    HAL_WWDG_Refresh(&hwwdg);     // RESET! (refresh muito cedo)
}
```

### 3. Refresh Muito Tarde (Timeout)
```c
// Código problemático  
while (1)
{
    HAL_Delay(120);               // 120ms > 115.7ms timeout
    HAL_WWDG_Refresh(&hwwdg);     // RESET! (timeout)
}
```

### 4. Sistema Travado
```c
while (1)
{
    // Sistema trava aqui
    infinite_loop();              // Sem refresh
    // RESET automático em ~115.7ms
}
```

## Early Warning Interrupt (EWI)

### Conceito do EWI
```c
hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;  // Habilitar EWI
```

O **EWI (Early Warning Interrupt)** é gerado quando counter = 64, permitindo ação antes do reset.

### Callback de EWI
```c
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    // Executado quando counter = 64 (antes do reset)
    
    // Ações de emergência
    save_critical_data();
    log_watchdog_warning();
    
    // Refresh de emergência (se possível)
    HAL_WWDG_Refresh(hwwdg);
    
    // LED de aviso
    HAL_GPIO_TogglePin(LED_WARNING_GPIO_Port, LED_WARNING_Pin);
}
```

### Timing com EWI
```
Counter = 113: WWDG Start
Counter = 89:  Window Opens
Counter = 64:  EWI Interrupt → HAL_WWDG_EarlyWakeupCallback()
Counter = 63:  RESET (se não houve refresh após EWI)
```

## LED de Status

### Indicação Visual
```c
// LED inicialmente apagado
HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, 0);

HAL_Delay(250);  // Aguardar inicialização

// LED aceso durante operação
HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, 1);
```

### Comportamento do LED
- **Inicialização:** LED apagado por 250ms
- **Operação normal:** LED aceso continuamente  
- **Reset WWDG:** LED apaga e reinicia sequência

## Aplicações Práticas

### 1. Controle de Loop de Tempo Real
```c
void real_time_control_loop(void)
{
    static uint32_t last_execution = 0;
    uint32_t current_time = HAL_GetTick();
    
    // Verificar se loop está executando na frequência correta
    if ((current_time - last_execution) >= 30 && 
        (current_time - last_execution) <= 40)
    {
        // Timing correto, executar tarefas críticas
        execute_control_algorithm();
        process_sensor_data();
        update_actuators();
        
        // Refresh WWDG (timing correto)
        HAL_WWDG_Refresh(&hwwdg);
        last_execution = current_time;
    }
    // Se timing incorreto, WWDG resetará o sistema
}
```

### 2. Sistema de Aquisição de Dados
```c
typedef struct {
    uint32_t sample_period_ms;
    uint32_t last_sample_time;
    uint8_t timing_valid;
} data_acquisition_t;

void data_acquisition_system(data_acquisition_t *daq)
{
    uint32_t current_time = HAL_GetTick();
    uint32_t elapsed = current_time - daq->last_sample_time;
    
    // Verificar se está na janela temporal correta
    if (elapsed >= (daq->sample_period_ms - 5) && 
        elapsed <= (daq->sample_period_ms + 5))
    {
        // Timing correto
        acquire_sensor_data();
        process_samples();
        
        HAL_WWDG_Refresh(&hwwdg);  // Sistema sincronizado
        daq->last_sample_time = current_time;
        daq->timing_valid = 1;
    }
    else
    {
        daq->timing_valid = 0;
        // WWDG resetará por timing incorreto
    }
}
```

### 3. Motor Control com Timing Crítico
```c
void motor_control_system(void)
{
    static uint32_t control_cycle_start = 0;
    uint32_t cycle_time;
    
    control_cycle_start = HAL_GetTick();
    
    // Executar controle do motor
    read_encoder_position();
    calculate_pid_control();
    update_pwm_duty_cycle();
    
    cycle_time = HAL_GetTick() - control_cycle_start;
    
    // Verificar se ciclo de controle está dentro do timing
    if (cycle_time >= 25 && cycle_time <= 45)  // 30ms ±15ms
    {
        HAL_WWDG_Refresh(&hwwdg);  // Timing adequado
    }
    // Se fora do timing, deixar WWDG resetar
}
```

### 4. Comunicação Temporal Crítica
```c
typedef struct {
    uint32_t message_interval;
    uint32_t last_message_time;
    uint8_t communication_health;
} comm_timing_t;

void time_critical_communication(comm_timing_t *comm)
{
    uint32_t current_time = HAL_GetTick();
    
    // Enviar mensagem
    send_periodic_message();
    
    // Verificar timing de comunicação
    uint32_t interval = current_time - comm->last_message_time;
    
    if (interval >= (comm->message_interval - 2) && 
        interval <= (comm->message_interval + 2))
    {
        comm->communication_health = 1;
        HAL_WWDG_Refresh(&hwwdg);  // Comunicação no tempo
        comm->last_message_time = current_time;
    }
    else
    {
        comm->communication_health = 0;
        // WWDG resetará por timing de comunicação incorreto
    }
}
```

## Configurações Alternativas

### 1. Window Mais Restritiva
```c
hwwdg.Init.Window = 100;      // Window maior
hwwdg.Init.Counter = 113;     // Counter mesmo

// Window duration menor: (113-100) = 13 ticks
// Timing mais restritivo
```

### 2. Timeout Mais Longo
```c
hwwdg.Init.Prescaler = WWDG_PRESCALER_8;  // Prescaler maior
hwwdg.Init.Window = 80;
hwwdg.Init.Counter = 127;                 // Counter máximo

// Timeout ≈ 350ms (para sistemas mais lentos)
```

### 3. Window Mais Flexível
```c
hwwdg.Init.Window = 70;       // Window menor
hwwdg.Init.Counter = 120;     // Counter maior

// Window duration maior: (120-70) = 50 ticks
// Mais flexibilidade no timing
```

## Troubleshooting

### Problemas Comuns

#### 1. Reset Imediato (Refresh Muito Cedo)
**Sintomas:** Sistema reseta logo após inicialização
**Causa:** Refresh antes da window abrir

**Solução:**
```c
// Aumentar delay para entrar na window
HAL_Delay(30);  // Aguardar window abrir
HAL_WWDG_Refresh(&hwwdg);
```

#### 2. Reset por Timeout
**Sintomas:** Reset após tempo específico
**Causa:** Refresh muito tarde ou não executado

**Solução:**
```c
// Reduzir delay ou verificar se refresh está sendo chamado
HAL_Delay(25);  // Menor que timeout
HAL_WWDG_Refresh(&hwwdg);
```

#### 3. Timing Inconsistente
**Sintomas:** Reset aleatório
**Causa:** Timing do loop variável

**Solução:**
```c
// Usar timing mais determinístico
if ((HAL_GetTick() % 35) == 0)
{
    HAL_WWDG_Refresh(&hwwdg);
}
```

## Detecção de Reset WWDG

### Verificar Causa do Reset
```c
void check_reset_source(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        // Reset foi causado pelo WWDG
        printf("Sistema resetado pelo WWDG - Timing error!\n");
        
        // Limpar flag
        __HAL_RCC_CLEAR_RESET_FLAGS();
        
        // Ações específicas
        handle_timing_error();
    }
}
```

## Conceitos Aprendidos
1. **Window watchdog** - Watchdog com janela temporal
2. **Timing validation** - Validação de timing de execução
3. **Early warning** - Aviso antes do reset
4. **Clock dependency** - Dependência do clock do sistema
5. **Precise timing** - Controle temporal preciso
6. **Real-time constraints** - Restrições de tempo real
7. **System synchronization** - Sincronização de sistema

## Melhorias Sugeridas
1. **EWI implementation** - Implementar Early Warning Interrupt
2. **Adaptive timing** - Timing adaptativo baseado na carga
3. **Timing statistics** - Estatísticas de timing do sistema
4. **Dual watchdog** - Combinação WWDG + IWDG
5. **Timing diagnostics** - Diagnóstico de problemas de timing
6. **Load monitoring** - Monitoramento de carga da CPU

## Hardware Recomendado
- **Osciloscópio:** Para medir timing preciso
- **Logic analyzer:** Para debug de sequências temporais
- **LED indicators:** Para status visual de timing
- **Real-time monitor:** Para análise de performance
- **Precision clock:** Para referência temporal

## Observações Importantes
- **Clock dependency:** WWDG para se PCLK1 falhar
- **Window timing:** Refresh apenas na janela permitida
- **Shorter timeouts:** Timeouts menores que IWDG
- **Real-time applications:** Ideal para sistemas de tempo real
- **Timing precision:** Requer timing de software preciso
- **Debug challenges:** Mais difícil de debug que IWDG
- **Application specific:** Não adequado para todas as aplicações