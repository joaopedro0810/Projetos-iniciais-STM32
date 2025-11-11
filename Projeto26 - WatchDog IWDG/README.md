# Projeto26 - WatchDog IWDG

## Descrição do Projeto
Este projeto demonstra o uso do **IWDG (Independent Watchdog)** no STM32F407VET6. O sistema implementa um watchdog independente que monitora a execução do software principal, forçando um reset caso o programa trave ou entre em loop infinito, garantindo a confiabilidade e robustez do sistema embarcado.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Clock IWDG:** LSI (32kHz interno)
- **Pino LED:** PA5 (LED_D1)

## IWDG - Independent Watchdog

### O que é um Watchdog?
O **Watchdog Timer** é um mecanismo de segurança que monitora a execução do software. Se o programa não "alimentar" o watchdog dentro de um tempo pré-determinado, o sistema é automaticamente resetado, recuperando de travamentos ou falhas.

### Características do IWDG STM32F407
- **Independente:** Clock próprio (LSI) independente do sistema
- **Simples:** Apenas contador decrescente
- **Confiável:** Não afetado por falhas do clock principal
- **Hardware:** Implementação totalmente em hardware
- **Timeout:** 125µs a 32.768s (configurável)

### Vantagens do IWDG
1. **Segurança crítica:** Recuperação automática de falhas
2. **Independência:** Clock LSI separado do sistema
3. **Simplicidade:** Configuração direta
4. **Baixo consumo:** LSI consome poucos µA
5. **Resistência a falhas:** Não afetado por EMI no clock principal

## Configuração do IWDG

### Parâmetros de Configuração
```c
static void MX_IWDG_Init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_16;    // Prescaler = 16
    hiwdg.Init.Reload = 4095;                    // Reload = 4095 (máximo)
    
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### Cálculo do Timeout
**Fórmula:**
```
Timeout = (Prescaler × Reload) / LSI_Frequency
```

**Para o projeto:**
```
LSI = 32kHz (típico)
Prescaler = 16
Reload = 4095

Timeout = (16 × 4095) / 32000 = 65520 / 32000 ≈ 2.05 segundos
```

### Opções de Prescaler
```c
IWDG_PRESCALER_4     // Prescaler = 4    (min timeout)
IWDG_PRESCALER_8     // Prescaler = 8
IWDG_PRESCALER_16    // Prescaler = 16   (usado no projeto)
IWDG_PRESCALER_32    // Prescaler = 32
IWDG_PRESCALER_64    // Prescaler = 64
IWDG_PRESCALER_128   // Prescaler = 128
IWDG_PRESCALER_256   // Prescaler = 256  (max timeout)
```

### Range de Timeout
```c
Mínimo: (4 × 1) / 32000 = 125µs        (PRESCALER_4, Reload=1)
Máximo: (256 × 4095) / 32000 = 32.768s (PRESCALER_256, Reload=4095)
```

## Clock Configuration

### LSI (Low Speed Internal)
```c
void SystemClock_Config(void)
{
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;           // Para sistema principal
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;           // Para IWDG
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;     // Sem PLL
}
```

### Características do LSI
- **Frequência nominal:** 32kHz
- **Tolerância:** ±30% (22kHz a 45kHz)
- **Temperatura:** Varia com temperatura
- **Consumo:** ~1µA
- **Startup time:** ~200µs

### Clock Independence
```
Sistema Principal: HSI (16MHz) → CPU, Periféricos
IWDG Clock:       LSI (32kHz) → Watchdog Timer

Vantagem: Se HSI falhar, IWDG continua operando!
```

## Operação do Sistema

### Sequência de Inicialização
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    // LED indica inicialização
    HAL_Delay(250);
    HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, 0);  // LED OFF
    
    // Inicializar IWDG (inicia contagem!)
    MX_IWDG_Init();
    
    // Loop principal
    while (1)
    {
        HAL_Delay(1100);                    // Simular processamento
        HAL_IWDG_Refresh(&hiwdg);           // "Alimentar" o watchdog
    }
}
```

### Análise do Timing
- **Timeout IWDG:** ~2.05 segundos
- **Refresh period:** 1100ms (1.1 segundos)
- **Margem de segurança:** 2050ms - 1100ms = 950ms

### Função HAL_IWDG_Refresh
```c
HAL_StatusTypeDef HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg);
```
- **Função:** Recarrega o contador do watchdog
- **Efeito:** Reinicia contagem de timeout
- **Timing crítico:** Deve ser chamada antes do timeout

## Cenários de Operação

### 1. Operação Normal
```
t=0s:     IWDG_Init() - Contador inicia em 4095
t=1.1s:   HAL_IWDG_Refresh() - Contador volta para 4095
t=2.2s:   HAL_IWDG_Refresh() - Contador volta para 4095
t=3.3s:   HAL_IWDG_Refresh() - Contador volta para 4095
...       Operação continua normalmente
```

### 2. Sistema Travado (Sem Refresh)
```
t=0s:     IWDG_Init() - Contador inicia em 4095
t=1.1s:   Sistema trava - SEM HAL_IWDG_Refresh()
t=2.05s:  Contador chega a 0 - RESET automático!
t=2.05s:  Sistema reinicia automaticamente
```

### 3. Refresh Muito Lento
```c
// Exemplo de código problemático
while (1)
{
    HAL_Delay(2500);                // 2.5s > 2.05s timeout
    HAL_IWDG_Refresh(&hiwdg);       // Muito tarde!
    // Sistema resetará antes de chegar aqui
}
```

## LED de Status

### Indicação Visual
```c
// LED inicialmente aceso (sistema iniciando)
HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, 1);  // LED ON

HAL_Delay(250);  // Aguardar 250ms

// LED apagado (sistema operacional)
HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, 0);  // LED OFF
```

### Comportamento Esperado
- **Power-on:** LED aceso por 250ms
- **Operação normal:** LED permanece apagado
- **Reset por IWDG:** LED aceso novamente por 250ms

## Aplicações Práticas

### 1. Sistema de Controle Industrial
```c
void industrial_control_task(void)
{
    // Ler sensores
    read_sensors();
    
    // Processar dados
    process_control_algorithm();
    
    // Atualizar atuadores
    update_actuators();
    
    // Comunicação
    handle_communications();
    
    // Watchdog refresh (sistema funcionando)
    HAL_IWDG_Refresh(&hiwdg);
}
```

### 2. Sistema de Monitoramento
```c
void monitoring_system(void)
{
    static uint32_t last_refresh = 0;
    uint32_t current_time = HAL_GetTick();
    
    // Executar tarefas de monitoramento
    monitor_critical_parameters();
    
    // Refresh periódico (a cada 1 segundo)
    if (current_time - last_refresh >= 1000)
    {
        if (system_health_check())
        {
            HAL_IWDG_Refresh(&hiwdg);  // Sistema saudável
            last_refresh = current_time;
        }
        // Se sistema não está saudável, deixa IWDG resetar
    }
}
```

### 3. Sistema de Comunicação
```c
typedef struct {
    uint8_t communication_active;
    uint32_t last_message_time;
    uint32_t timeout_threshold;
} comm_watchdog_t;

void communication_watchdog(comm_watchdog_t *comm)
{
    uint32_t current_time = HAL_GetTick();
    
    // Verificar se comunicação está ativa
    if ((current_time - comm->last_message_time) < comm->timeout_threshold)
    {
        comm->communication_active = 1;
        HAL_IWDG_Refresh(&hiwdg);  // Comunicação OK
    }
    else
    {
        comm->communication_active = 0;
        // Deixar IWDG resetar para tentar reestabelecer comunicação
    }
}
```

### 4. Sistema de Detecção de Travamento
```c
void deadlock_detection_system(void)
{
    static uint32_t task1_counter = 0;
    static uint32_t task2_counter = 0;
    static uint32_t last_task1_count = 0;
    static uint32_t last_task2_count = 0;
    
    // Executar tarefas críticas
    task1();  task1_counter++;
    task2();  task2_counter++;
    
    // Verificar se tarefas estão progredindo
    if ((task1_counter > last_task1_count) && 
        (task2_counter > last_task2_count))
    {
        HAL_IWDG_Refresh(&hiwdg);  // Tarefas progredindo normalmente
        last_task1_count = task1_counter;
        last_task2_count = task2_counter;
    }
    // Se tarefas travaram, IWDG irá resetar
}
```

## Configurações Alternativas

### 1. Timeout Mais Curto (Alta Responsividade)
```c
hiwdg.Init.Prescaler = IWDG_PRESCALER_4;     // Prescaler = 4
hiwdg.Init.Reload = 800;                     // Reload = 800

// Timeout = (4 × 800) / 32000 = 100ms
// Requer refresh < 100ms
```

### 2. Timeout Mais Longo (Sistema Lento)
```c
hiwdg.Init.Prescaler = IWDG_PRESCALER_64;    // Prescaler = 64
hiwdg.Init.Reload = 4095;                    // Reload máximo

// Timeout = (64 × 4095) / 32000 ≈ 8.2s
// Permite processamento mais lento
```

### 3. Configuração Balanceada
```c
hiwdg.Init.Prescaler = IWDG_PRESCALER_32;    // Prescaler = 32
hiwdg.Init.Reload = 2000;                    // Reload = 2000

// Timeout = (32 × 2000) / 32000 = 2.0s
// Boa para maioria das aplicações
```

## Detecção de Reset por IWDG

### Verificar Causa do Reset
```c
void check_reset_source(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        // Reset foi causado pelo IWDG
        printf("Sistema foi resetado pelo IWDG!\n");
        
        // Limpar flag
        __HAL_RCC_CLEAR_RESET_FLAGS();
        
        // Ações específicas pós-reset IWDG
        handle_iwdg_reset();
    }
}
```

### Tratamento Pós-Reset
```c
void handle_iwdg_reset(void)
{
    // Log do evento
    log_watchdog_reset();
    
    // Verificar integridade do sistema
    system_integrity_check();
    
    // Reinicializar subsistemas críticos
    reinitialize_critical_systems();
    
    // Sinalizar reset para sistema de monitoramento
    signal_watchdog_reset_event();
}
```

## Troubleshooting

### Problemas Comuns

#### 1. Sistema Resetando Constantemente
**Sintomas:** Reset contínuo, sistema não opera
**Causas:**
- Timeout muito pequeno
- Refresh não sendo chamado
- Loop infinito no código

**Soluções:**
```c
// Aumentar timeout
hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
hiwdg.Init.Reload = 4095;

// Verificar se refresh está sendo chamado
HAL_IWDG_Refresh(&hiwdg);  // Adicionar em locais apropriados
```

#### 2. IWDG Não Resetando Sistema Travado
**Sintomas:** Sistema trava mas não reseta
**Causas:**
- IWDG não inicializado
- LSI não habilitado
- Configuração incorreta

**Soluções:**
```c
// Verificar inicialização LSI
RCC_OscInitStruct.LSIState = RCC_LSI_ON;

// Verificar se IWDG foi inicializado
MX_IWDG_Init();
```

#### 3. Timing Inconsistente
**Sintomas:** Reset em tempos aleatórios
**Causas:**
- Variação do LSI com temperatura
- Tolerância do LSI (±30%)

**Soluções:**
```c
// Usar margem de segurança maior
refresh_period = timeout * 0.5;  // 50% do timeout

// Calibrar LSI se necessário (avançado)
```

## Medição e Verificação

### Teste de Funcionamento
```c
void test_iwdg_functionality(void)
{
    printf("Testando IWDG...\n");
    
    // Não chamar HAL_IWDG_Refresh() por 3 segundos
    HAL_Delay(3000);  // Sistema deve resetar antes de 2.05s
    
    // Esta linha nunca será executada
    printf("ERRO: IWDG não funcionou!\n");
}
```

### Medição do Timeout Real
```c
void measure_iwdg_timeout(void)
{
    uint32_t start_time = HAL_GetTick();
    
    // Não fazer refresh e aguardar reset
    while (1)
    {
        // Sistema resetará automaticamente
        // Medir tempo até reset com osciloscópio no pino de reset
    }
}
```

## Conceitos Aprendidos
1. **Sistema de segurança** - Watchdog como proteção contra travamentos
2. **Clock independente** - LSI separado do sistema principal
3. **Timeout calculation** - Cálculo de tempo limite do watchdog
4. **Refresh strategy** - Estratégia de alimentação do watchdog
5. **Reset recovery** - Recuperação automática de falhas
6. **System reliability** - Confiabilidade de sistemas embarcados
7. **Fail-safe design** - Projeto à prova de falhas

## Melhorias Sugeridas
1. **Reset source detection** - Detectar e logar causa do reset
2. **Conditional refresh** - Refresh baseado em condições do sistema
3. **Timeout calibration** - Calibração do LSI para precisão
4. **Multiple watchdogs** - Combinação IWDG + WWDG
5. **Logging system** - Sistema de logs para análise de resets
6. **Health monitoring** - Monitoramento de saúde do sistema

## Hardware Recomendado
- **LED indicador:** Para visualizar resets
- **Reset button:** Para testes manuais
- **Osciloscópio:** Para medir tempos de reset
- **Logic analyzer:** Para debug de sequências
- **External watchdog:** Para redundância crítica

## Observações Importantes
- **LSI variation:** LSI varia ±30%, considerar nas aplicações críticas
- **Temperature effects:** Frequência LSI muda com temperatura
- **Initialization timing:** IWDG inicia assim que configurado
- **No disable:** IWDG não pode ser desabilitado após inicialização
- **Reset priority:** Reset IWDG tem alta prioridade
- **Power consumption:** LSI adiciona ~1µA ao consumo total
- **Debug mode:** IWDG pausa durante debug por padrão