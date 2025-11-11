# Projeto 43 - RTOS 03

## Descrição do Projeto
Este projeto implementa semáforos binários no FreeRTOS usando STM32F407VET6. O sistema demonstra sincronização entre tarefas através de semáforos, incluindo operações de release e acquire com timeout, comunicação serial coordenada e controle de acesso a recursos compartilhados.

## Funcionalidades
- **Binary Semaphore**: Semáforo binário para sincronização entre tasks
- **Task Synchronization**: Coordenação entre duas tarefas usando semáforo
- **Timeout Management**: osSemaphoreAcquire com timeout configurável
- **Serial Communication**: Mensagens seriais coordenadas por semáforo
- **Resource Control**: Controle de acesso a recurso compartilhado (UART)

## Hardware Necessário
- Placa STM32F407VET6, Conversor USB-Serial, Terminal serial

## Código Principal

### Criação do Semáforo
```c
osSemaphoreId_t myBinarySem01Handle;
// Semáforo criado com count máximo 1, valor inicial 0
myBinarySem01Handle = osSemaphoreNew(1, 0, &myBinarySem01_attributes);
```

### Task 1 - Libera Semáforo
```c
void StartTask01(void *argument)
{
    for(;;)
    {
        // Envia mensagem e libera semáforo
        HAL_UART_Transmit(&huart1, (uint8_t*)"Task 1: Liberando semáforo\r\n", 29, 100);
        osSemaphoreRelease(myBinarySem01Handle);
        osDelay(2000);
    }
}
```

### Task 2 - Aguarda Semáforo
```c
void StartTask02(void *argument)
{
    for(;;)
    {
        // Aguarda semáforo com timeout de 5 segundos
        if(osSemaphoreAcquire(myBinarySem01Handle, 5000) == osOK)
        {
            HAL_UART_Transmit(&huart1, (uint8_t*)"Task 2: Semáforo adquirido\r\n", 29, 100);
        }
        else
        {
            HAL_UART_Transmit(&huart1, (uint8_t*)"Task 2: Timeout\r\n", 18, 100);
        }
        osDelay(1000);
    }
}
```

## Conceitos de Semáforos

### Binary Semaphore
- **Count = 1**: Apenas um "token" disponível
- **Initial Value = 0**: Inicialmente indisponível
- **Release**: Disponibiliza o semáforo (count = 1)
- **Acquire**: Consome o semáforo (count = 0)

### Aplicações Práticas
1. **Sincronização Producer-Consumer**
2. **Controle de Acesso a Recursos**
3. **Sinalização entre Tasks**
4. **Coordenação de Eventos**

## Recursos de Aprendizado
- **Conceitos**: Binary Semaphore, Task Synchronization, Resource Protection
- **FreeRTOS**: osSemaphoreNew, osSemaphoreRelease, osSemaphoreAcquire
- **Patterns**: Producer-Consumer, Event signaling