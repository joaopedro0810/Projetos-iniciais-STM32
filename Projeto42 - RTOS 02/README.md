# Projeto 42 - RTOS 02

## Descrição do Projeto
Este projeto avança no uso do FreeRTOS implementando múltiplas tarefas com diferentes prioridades e funcionalidades no microcontrolador STM32F407VET6. O sistema demonstra conceitos avançados de gerenciamento de tarefas, incluindo suspensão e resumo dinâmico de tasks, diferentes níveis de prioridade, comunicação serial concorrente e controle inter-task através de contadores compartilhados.

## Funcionalidades
- **Múltiplas Tarefas**: Três tarefas executando com prioridades diferentes
- **Gerenciamento Dinâmico**: Suspensão e resumo de tarefas em tempo de execução
- **Sistema de Prioridades**: AboveNormal, Normal e BelowNormal
- **Comunicação Serial**: Múltiplas mensagens via UART concorrentes
- **Controle Inter-task**: Uma tarefa controlando estado de outras
- **Counter System**: Sistema de contagem para temporização de eventos
- **Task Suspension**: Demonstração de osThreadSuspend() e osThreadResume()

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- Conversor USB-Serial (FTDI, CP2102, CH340, etc.)
- Cabos jumper para conexões UART
- Terminal serial (PuTTY, Tera Term, Arduino IDE Serial Monitor)
- Computador com porta USB

## Configuração dos Pinos
| Pino STM32 | Função UART | Conversor USB-Serial |
|------------|-------------|----------------------|
| PA9 | USART1_TX | RX |
| PA10 | USART1_RX | TX |
| GND | Ground | GND |

## Análise Técnica

### Configuração das Tarefas
O projeto implementa três tarefas com prioridades escalonadas:
```c
// Task 1 - Prioridade mais alta
osThreadDef(EnviaSerial1, EnviaSerial1_fun, osPriorityAboveNormal, 0, 128);

// Task 2 - Prioridade média
osThreadDef(EnviaSerial2, EnviaSerial2_fun, osPriorityNormal, 0, 128);

// Task 3 - Prioridade mais baixa
osThreadDef(EnviaSerial3, EnviaSerial3_fun, osPriorityBelowNormal, 0, 128);
```

### Hierarquia de Prioridades
- **osPriorityAboveNormal**: EnviaSerial1 (prioridade 4)
- **osPriorityNormal**: EnviaSerial2 (prioridade 3)
- **osPriorityBelowNormal**: EnviaSerial3 (prioridade 2)

## Código Principal

### Criação das Tarefas
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    // Criação das três tarefas com prioridades diferentes
    osThreadDef(EnviaSerial1, EnviaSerial1_fun, osPriorityAboveNormal, 0, 128);
    EnviaSerial1Handle = osThreadCreate(osThread(EnviaSerial1), NULL);

    osThreadDef(EnviaSerial2, EnviaSerial2_fun, osPriorityNormal, 0, 128);
    EnviaSerial2Handle = osThreadCreate(osThread(EnviaSerial2), NULL);

    osThreadDef(EnviaSerial3, EnviaSerial3_fun, osPriorityBelowNormal, 0, 128);
    EnviaSerial3Handle = osThreadCreate(osThread(EnviaSerial3), NULL);

    // Inicia scheduler
    osKernelStart();

    while (1) {
        // Nunca executado
    }
}
```

### Task 1 - Controle Principal (Alta Prioridade)
```c
void EnviaSerial1_fun(void const * argument)
{
    for(;;)
    {
        HAL_UART_Transmit(&huart1, (const uint8_t *) "Mensagem 1\r\n", 
                         strlen("Mensagem 1\r\n"), 100);

        teste++;
        
        // Suspende Task 3 após 10 execuções
        if (teste == 10)
        {
            osThreadSuspend(EnviaSerial3Handle);
        }

        // Resume Task 3 após 20 execuções e reinicia contador
        if (teste == 20)
        {
            osThreadResume(EnviaSerial3Handle);
            teste = 0;
        }

        osDelay(1000);
    }
}
```

### Task 2 - Execução Contínua (Prioridade Normal)
```c
void EnviaSerial2_fun(void const * argument)
{
    for(;;)
    {
        HAL_UART_Transmit(&huart1, (const uint8_t *) "Mensagem 2\r\n", 
                         strlen("Mensagem 2\r\n"), 100);
        osDelay(1000);
    }
}
```

### Task 3 - Controle Dinâmico (Baixa Prioridade)
```c
void EnviaSerial3_fun(void const * argument)
{
    for(;;)
    {
        HAL_UART_Transmit(&huart1, (const uint8_t *) "Mensagem 3\r\n", 
                         strlen("Mensagem 3\r\n"), 100);
        osDelay(1000);
    }
}
```

## Funcionamento do Sistema

### Sequência de Execução
```
Tempo 0-9s:    Task1, Task2, Task3 (todas executando)
Tempo 10-19s:  Task1, Task2 (Task3 suspensa)
Tempo 20-29s:  Task1, Task2, Task3 (Task3 resumida)
Tempo 30-39s:  Task1, Task2 (Task3 suspensa novamente)
... Ciclo continua
```

### Comportamento das Tasks
1. **Task 1**: Executa continuamente e controla Task 3
2. **Task 2**: Executa continuamente (não é afetada)
3. **Task 3**: Alternadamente suspensa e resumida

### Timeline de Mensagens Serial
```
0s:  "Mensagem 1" (Task 1)
0s:  "Mensagem 2" (Task 2)  
0s:  "Mensagem 3" (Task 3)
1s:  "Mensagem 1" (Task 1)
1s:  "Mensagem 2" (Task 2)
1s:  "Mensagem 3" (Task 3)
...
10s: "Mensagem 1" (Task 1) -> Task 3 suspensa
10s: "Mensagem 2" (Task 2)
11s: "Mensagem 1" (Task 1)
11s: "Mensagem 2" (Task 2) -> Task 3 permanece suspensa
...
20s: "Mensagem 1" (Task 1) -> Task 3 resumida
20s: "Mensagem 2" (Task 2)
20s: "Mensagem 3" (Task 3) -> Volta a executar
```

## Conceitos Avançados do FreeRTOS

### 1. **Task Priorities**
```c
// Ordem de execução (maior prioridade primeiro):
// 1. osPriorityAboveNormal (4)
// 2. osPriorityNormal (3)  
// 3. osPriorityBelowNormal (2)
```

### 2. **Task Suspension/Resume**
```c
osThreadSuspend(EnviaSerial3Handle);  // Suspende task indefinidamente
osThreadResume(EnviaSerial3Handle);   // Resume task suspensa
```

### 3. **Preemptive Scheduling**
- Task com maior prioridade sempre executa primeiro
- Context switch automático quando task de maior prioridade fica pronta
- Round-robin entre tasks de mesma prioridade

### 4. **Task States**
- **Running**: Executando atualmente
- **Ready**: Pronta para executar
- **Blocked**: Aguardando evento (osDelay)
- **Suspended**: Suspensa por outra task

## Aplicações Práticas
1. **Sistemas de Monitoramento**: Múltiplos sensores com prioridades diferentes
2. **Controle Industrial**: Tarefas críticas vs não-críticas
3. **Interface de Usuário**: Display + botões + comunicação
4. **Data Acquisition**: Coleta, processamento e transmissão de dados
5. **Robótica**: Navegação + sensores + atuadores
6. **Sistemas de Alarme**: Detecção + notificação + logging
7. **IoT Devices**: Sensoriamento + comunicação + controle local

## Vantagens do Multi-Task System

### **Organização Modular**
- Cada função em task separada
- Código mais limpo e organizados
- Facilita manutenção e debug

### **Priorização Automática**
- Sistema crítico sempre tem prioridade
- Balanceamento automático de recursos
- Responsividade garantida

### **Controle Dinâmico**
- Tasks podem ser suspensas/resumidas
- Adaptação em tempo real
- Gerenciamento de recursos

## Performance e Recursos

### Context Switch Overhead
- Tempo: ~1-2μs por context switch
- Frequência: Depende das prioridades e delays
- Impact: Mínimo com delays de 1000ms

### Memory Usage
```c
// Por task:
// Stack: 128 words × 4 bytes = 512 bytes
// Total: 3 tasks × 512 bytes = 1536 bytes
// Kernel overhead: ~2KB
// Total RAM: ~3.5KB
```

## Troubleshooting

### Mensagens não aparecem no terminal
- Verificar configuração UART (9600 baud, 8N1)
- Confirmar conexões TX/RX
- Testar comunicação UART isoladamente
- Verificar terminal serial

### Task 3 não suspende/resume
- Verificar handle da task (EnviaSerial3Handle)
- Confirmar valor do contador 'teste'
- Debug com breakpoints
- Verificar se task foi criada corretamente

### Ordem das mensagens incorreta
- Verificar prioridades das tasks
- Confirmar scheduler preemptivo
- Verificar delays das tasks
- Analisar timing no terminal

### Sistema trava ou comportamento errático
- Stack overflow das tasks
- Heap insuficiente
- Conflito no acesso à UART
- Verificar configuração FreeRTOS

## Melhorias Sugeridas
1. **Mutex para UART**: Proteger acesso concorrente à UART
2. **Message Queues**: Comunicação estruturada entre tasks
3. **Semaphores**: Sincronização mais sofisticada
4. **Task Notifications**: Comunicação mais eficiente
5. **Error Handling**: Tratamento de falhas de criação/suspensão
6. **Task Statistics**: Monitoramento de performance das tasks
7. **Dynamic Priority**: Alteração de prioridades em runtime
8. **Watchdog per Task**: Monitoramento individual das tasks

## Configuração do Terminal
Para visualizar as mensagens:
- **Baudrate**: 9600
- **Data bits**: 8
- **Parity**: None
- **Stop bits**: 1
- **Flow control**: None

## Exemplo de Saída Esperada
```
Mensagem 1
Mensagem 2
Mensagem 3
Mensagem 1
Mensagem 2
Mensagem 3
...
(após 10 segundos)
Mensagem 1
Mensagem 2
Mensagem 1
Mensagem 2
...
(após mais 10 segundos)
Mensagem 1
Mensagem 2
Mensagem 3
...
```

## Recursos de Aprendizado
- **Conceitos**: Multi-tasking, Task priorities, Task management, Inter-task control
- **FreeRTOS**: osThreadSuspend, osThreadResume, Priority levels
- **CMSIS-RTOS**: Advanced task operations, Task handles
- **Patterns**: Priority-based design, Dynamic task control
- **Architecture**: Preemptive scheduling, Task states, Resource sharing

Este projeto demonstra conceitos avançados de gerenciamento de tarefas no FreeRTOS, estabelecendo a base para implementações mais complexas com sincronização e comunicação inter-task.