# Projeto 41 - RTOS 01

## Descrição do Projeto
Este projeto implementa a introdução ao FreeRTOS (Free Real-Time Operating System) no microcontrolador STM32F407VET6. O sistema demonstra os conceitos fundamentais de sistema operacional em tempo real, incluindo criação de tarefas (tasks), escalonamento preemptivo, gerenciamento de prioridades e multitarefa concorrente através de duas tarefas independentes que controlam LEDs com períodos diferentes.

## 🎯 Objetivos de Aprendizado

- Compreender conceitos de sistemas operacionais em tempo real
- Criar e gerenciar tarefas (tasks)
## Funcionalidades
- **FreeRTOS Básico**: Implementação fundamental de sistema operacional em tempo real
- **Multitarefa**: Duas tarefas executando concorrentemente
- **Escalonamento Preemptivo**: Scheduler automático baseado em prioridades
- **Task Management**: Criação, configuração e gerenciamento de tarefas
- **Delay Functions**: Uso de osDelay() para suspensão cooperativa de tarefas
- **LED Control**: Controle independente de dois LEDs com períodos distintos
- **Priority System**: Sistema de prioridades (Normal vs Idle)

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- 2 LEDs conectados aos pinos PA0 (LED_1) e PA1 (LED_2)
- Resistores limitadores para LEDs (220Ω recomendado)
- Fonte de alimentação
- Programador ST-Link

## Configuração dos Pinos
| Pino STM32 | Função | Descrição |
|------------|--------|-----------|
| PA0 | LED_1 | LED da Task 1 (5s) |
| PA1 | LED_2 | LED da Task 2 (2.5s) |
| GND | Ground | Terra comum |

## Análise Técnica

### Configuração FreeRTOS
O projeto utiliza FreeRTOS com as seguintes configurações:
- **Scheduler**: Preemptive scheduler
- **Tick Rate**: 1000 Hz (1ms tick)
- **Task Stack**: 128 words (512 bytes) por task
- **Heap**: Dynamic memory allocation
- **API**: CMSIS-RTOS v1

### Estrutura das Tarefas
```c
// Task 1 - Prioridade Normal
osThreadDef(Liga_Led_1, ligaLed1_Fun, osPriorityNormal, 0, 128);
Liga_Led_1Handle = osThreadCreate(osThread(Liga_Led_1), NULL);

// Task 2 - Prioridade Idle
```

### Prioridades de Task
- **osPriorityNormal**: Task 1 (Liga_Led_1) - Prioridade mais alta
- **osPriorityIdle**: Task 2 (Liga_Led_2) - Prioridade mais baixa

## Código Principal

### Criação das Tarefas
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    // Inicializar o kernel do RTOS
    osKernelInitialize();
    
    // Criar tarefas
    Liga_Led_1Handle = osThreadCreate(osThread(Liga_Led_1), NULL);
    Liga_Led_2Handle = osThreadCreate(osThread(Liga_Led_2), NULL);
    
    // Iniciar o scheduler
    osKernelStart();
    
    // Nunca deveria chegar aqui
    while (1) {}
}
```

### 3. Implementação das Tarefas
```c
// Tarefa 1: LED1 pisca a cada 500ms
void ligaLed1_Fun(void const * argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
        osDelay(500);  // Delay do RTOS (não bloqueia outras tarefas)
    }
}

// Tarefa 2: LED2 pisca a cada 1000ms
void ligaLed2_Fun(void const * argument)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
        osDelay(1000);  // Delay diferente para demonstrar concorrência
    }
}
```

## 🔍 Funcionalidades RTOS

### Características do FreeRTOS:
- **Preemptive Scheduling:** Tarefas de maior prioridade interrompem as de menor
- **Round-Robin:** Tarefas de mesma prioridade alternam execução
- **Stack Individual:** Cada tarefa tem seu próprio stack
- **Context Switching:** Troca automática entre tarefas

### Funções Principais:
- `osKernelInitialize()` - Inicializa o kernel
- `osThreadCreate()` - Cria nova tarefa
- `osKernelStart()` - Inicia o scheduler
- `osDelay()` - Delay não-bloqueante

## ⚙️ Configuração STM32CubeMX

### FreeRTOS Configuration:
| Parâmetro | Valor |
|-----------|-------|
| Interface | CMSIS_V1 |
| Tick Source | SysTick |
| Tick Rate | 1000 Hz |
| Max Priorities | 56 |
| Minimal Stack Size | 128 Words |
| Heap Size | 15360 Bytes |

### Task Configuration:
| Task | Priority | Stack Size | Entry Function |
|------|----------|------------|----------------|
| Liga_Led_1 | Normal | 128 | ligaLed1_Fun |
| Liga_Led_2 | Normal | 128 | ligaLed2_Fun |

### Memory Management:
- **Heap:** 15KB para alocação dinâmica
- **Stack per Task:** 512 bytes (128 words × 4)
- **Scheduler Stack:** Compartilhado com main stack

## 🚀 Como Executar

1. **Configure FreeRTOS no CubeMX:**
   - Middleware → FreeRTOS → Enable
   - Configure tasks e parâmetros

2. **Compile e Execute:**
   - Build project
   - Flash para o microcontrolador
   - Observe comportamento dos LEDs

3. **Comportamento Esperado:**
   - LED1 pisca a cada 500ms
   - LED2 pisca a cada 1000ms
   - Execução simultânea e independente

## 📊 Análise de Performance

### Overhead do RTOS:
- **Context Switch:** ~10-20 μs
- **Memory Overhead:** ~8KB (kernel + heap)
- **CPU Usage:** ~2-5% (idle task + scheduler)

### Task Scheduling:
```
Tempo: 0    500   1000  1500  2000  2500  3000
LED1:  ▲     ▼     ▲     ▼     ▲     ▼     ▲
LED2:  ▲           ▼           ▲           ▼
```

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Mais LEDs:** Adicione Task3 com LED3
2. **Prioridades:** Altere prioridades das tarefas
3. **Períodos diferentes:** Teste outros valores de delay

### Para Intermediários:
1. **Task com UART:** Adicione tarefa de comunicação serial
2. **Monitoramento:** Implemente task para monitorar sistema
3. **Botões:** Controle tarefas via botões externos

### Para Avançados:
1. **Semáforos:** Sincronize tarefas com semáforos
2. **Queues:** Comunique entre tarefas com filas
3. **Timers:** Use software timers do FreeRTOS

## 🐛 Possíveis Problemas

### Sistema não inicia:
- Stack size insuficiente
- Heap size insuficiente
- Configuração de clock incorreta

### Hard Fault:
- Stack overflow em alguma tarefa
- Ponteiro nulo em função de tarefa
- Configuração de prioridade inválida

### LEDs não piscam corretamente:
- osDelay() não está sendo usado
- Tarefa bloqueada ou em deadlock
- Configuração GPIO incorreta

## 🔍 Debug e Monitoramento

### Ferramentas de Debug:
- **STM32CubeIDE Debugger:** Breakpoints em tarefas
- **Task List View:** Visualizar estado das tarefas
- **Stack Usage:** Monitorar uso de stack

### Debugging Tips:
```c
// Verificar stack usage
UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

// Estado da tarefa
eTaskState taskState = eTaskGetState(Liga_Led_1Handle);
```

## 📈 Conceitos Avançados RTOS

### Estados de Tarefa:
- **Running:** Executando atualmente
- **Ready:** Pronta para executar
- **Blocked:** Aguardando evento
- **Suspended:** Suspensa manualmente

### Scheduler Behavior:
1. **Tick Interrupt:** A cada 1ms
2. **Priority Check:** Verifica tarefa de maior prioridade
3. **Context Switch:** Troca se necessário
4. **Round-Robin:** Entre tarefas de mesma prioridade

## 📚 Próximos Passos

Sequência recomendada de projetos RTOS:
- **[Projeto42 - RTOS 02](../Projeto42%20-%20RTOS%2002/):** Múltiplas tarefas e prioridades
- **[Projeto43 - RTOS 03](../Projeto43%20-%20RTOS%2003/):** Semáforos binários
- **[Projeto44 - RTOS 04](../Projeto44%20-%20RTOS%2004/):** Queues (filas)
- **[Projeto45 - RTOS 05](../Projeto45%20-%20RTOS%2005/):** Mutex

## 📖 Referências

- [FreeRTOS Official Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [STM32 FreeRTOS Guide](https://www.st.com/resource/en/user_manual/um1722-developing-applications-on-stm32cube-with-rtos-stmicroelectronics.pdf)
- [Real-Time Systems Design](https://en.wikipedia.org/wiki/Real-time_operating_system)
- [CMSIS-RTOS API](https://arm-software.github.io/CMSIS_5/RTOS/html/index.html)

---

⚡ **Nota:** Este é um projeto fundamental para sistemas em tempo real! FreeRTOS abre possibilidades para aplicações complexas e profissionais.