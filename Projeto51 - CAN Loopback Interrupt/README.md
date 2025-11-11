# Projeto51 - CAN Loopback Interrupt

## Descrição do Projeto
Este projeto demonstra comunicação CAN (Controller Area Network) em modo loopback usando interrupções para recepção assíncrona de mensagens. Ao invés de polling contínuo, o sistema utiliza callbacks para processar mensagens CAN de forma eficiente e não-bloqueante.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 168MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library

## Evolução: Polling → Interrupções

### Projeto49 (Polling)
- **Recepção:** Verificação manual do FIFO
- **Eficiência:** Baixa (CPU ocupada verificando)
- **Uso:** Testes simples e validação

### Projeto51 (Interrupções)
- **Recepção:** Automática via callback
- **Eficiência:** Alta (CPU livre para outras tarefas)
- **Uso:** Aplicações reais e sistemas complexos

## Configuração das Interrupções CAN

### Ativação da Notificação
```c
HAL_CAN_Start(&hcan1);  // Inicia o CAN
HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
```

### Tipos de Interrupções CAN Disponíveis
```c
CAN_IT_TX_MAILBOX_EMPTY         // Mailbox de TX vazio
CAN_IT_RX_FIFO0_MSG_PENDING     // Mensagem pendente no FIFO0
CAN_IT_RX_FIFO0_FULL            // FIFO0 cheio
CAN_IT_RX_FIFO0_OVERRUN         // Overflow do FIFO0
CAN_IT_RX_FIFO1_MSG_PENDING     // Mensagem pendente no FIFO1
CAN_IT_RX_FIFO1_FULL            // FIFO1 cheio
CAN_IT_RX_FIFO1_OVERRUN         // Overflow do FIFO1
CAN_IT_WAKEUP                   // Despertar do sleep mode
CAN_IT_SLEEP_ACK                // Confirmação do sleep mode
CAN_IT_ERROR_WARNING            // Aviso de erro
CAN_IT_ERROR_PASSIVE            // Modo passivo de erro
CAN_IT_BUSOFF                   // Bus-off condition
CAN_IT_LAST_ERROR_CODE          // Último código de erro
CAN_IT_ERROR                    // Erro geral
```

## Callback de Recepção
```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
    intg++;  // Contador de mensagens recebidas
}
```

### Características do Callback:
- **Automático:** Chamado quando mensagem chega
- **Não-bloqueante:** Execução rápida obrigatória
- **Context:** Executado em contexto de interrupção
- **Prioridade:** Alta prioridade do NVIC

## Fluxo de Mensagens do Projeto

### Loop Principal - Transmissão Contínua
```c
while (1) {
    // Mensagem 1: ID 0x11, 2 bytes
    TxHeader.StdId = 0x11;
    TxHeader.DLC = 2;
    TxData[0] = 0x03;
    TxData[1] = 0x04;
    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
    HAL_Delay(2500);
    
    // Mensagem 2: ID 0x12, 3 bytes
    TxHeader.StdId = 0x12;
    TxHeader.DLC = 3;
    TxData[0] = 0x08;
    TxData[1] = 0x09;
    TxData[2] = 0x0A;
    HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
    HAL_Delay(2500);
}
```

### Sequência Temporal
```
Tempo (ms)    Ação                      Resultado
0             TX: ID=0x11, Data=0x03,0x04   → Callback executado, intg++
2500          TX: ID=0x12, Data=0x08,0x09,0x0A → Callback executado, intg++
5000          TX: ID=0x11, Data=0x03,0x04   → Callback executado, intg++
7500          TX: ID=0x12, Data=0x08,0x09,0x0A → Callback executado, intg++
...           Ciclo continua
```

## Variável de Controle `intg`
```c
uint8_t intg = 0;  // Contador global de mensagens
```

### Funcionalidades:
- **Contador:** Número total de mensagens recebidas
- **Debug:** Verificação do funcionamento do sistema
- **Monitoramento:** Taxa de recepção de mensagens
- **Validação:** Confirmação de que callbacks estão funcionando

## Configuração do Sistema de Timing

### Clock Configuration
```c
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
PWR_REGULATOR_VOLTAGE_SCALE1;  // Performance máximo
```

### CAN Timing (mesmo do Projeto49)
- **Prescaler:** 10
- **Bit Time:** 16 TQ (1+13+2)
- **Baud Rate:** ~262.5 Kbps (HSI @ 16MHz)

## Vantagens das Interrupções CAN

### 1. Eficiência de CPU
- **Sem polling:** CPU livre para outras tarefas
- **Resposta imediata:** Processamento automático
- **Baixa latência:** Resposta rápida a mensagens

### 2. Escalabilidade
- **Múltiplas fontes:** Diferentes tipos de interrupção
- **Priorização:** NVIC gerencia prioridades
- **Modularidade:** Callbacks específicos por evento

### 3. Robustez
- **Não perde mensagens:** Processamento garantido
- **FIFO protection:** Menos overflow
- **Error handling:** Callbacks de erro específicos

## Contexto de Interrupção - Boas Práticas

### DO (Fazer):
```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);  // ✓ Rápido
    flag = 1;                                                     // ✓ Simples
    counter++;                                                    // ✓ Atômico
}
```

### DON'T (Não fazer):
```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    HAL_Delay(100);           // ✗ Nunca usar delay
    printf("Message received"); // ✗ I/O demorado
    complex_processing();     // ✗ Processamento pesado
}
```

## NVIC Configuration
O projeto habilita a interrupção CAN1_RX0 no NVIC:
```c
NVIC.CAN1_RX0_IRQn=true:0:0:false:false:true:true:true:true
```

### Parâmetros:
- **Enabled:** true
- **Preemption Priority:** 0 (máxima)
- **Sub Priority:** 0
- **Call in System View:** true

## Comparação de Performance

| Aspecto | Polling (Projeto49) | Interrupt (Projeto51) |
|---------|--------------------|-----------------------|
| CPU Usage | Alta (loop contínuo) | Baixa (event-driven) |
| Latência | Variável | Consistente e baixa |
| Escalabilidade | Limitada | Excelente |
| Complexidade | Simples | Moderada |
| Aplicação Real | Testes | Produção |

## Casos de Uso das Interrupções CAN

### 1. Sistemas Automotivos
- **ECUs:** Processamento de sensores
- **Gateway:** Roteamento entre redes
- **Diagnósticos:** Resposta a códigos OBD

### 2. Automação Industrial  
- **PLCs:** Controle distribuído
- **Sensores:** Aquisição contínua
- **Atuadores:** Resposta em tempo real

### 3. Sistemas Embarcados
- **IoT:** Comunicação entre dispositivos
- **Robótica:** Coordenação de movimentos
- **Monitoramento:** Coleta de dados críticos

## Conceitos Aprendidos
1. **Programação dirigida por eventos** (Event-driven)
2. **Callbacks CAN** e contexto de interrupção
3. **NVIC configuration** para CAN
4. **Eficiência de CPU** em sistemas embarcados
5. **Boas práticas** em ISRs (Interrupt Service Routines)
6. **Debugging com contadores** globais

## Próximos Passos
- **Projeto52:** Filtros CAN específicos com interrupções
- **Projeto53:** Múltiplos filtros e callbacks especializados
- **Implementar:** Buffers circulares para alta taxa de mensagens
- **Adicionar:** Callbacks de erro e tratamento robusto

## Debugging e Monitoramento
```c
// No debugger, observe:
uint8_t intg;           // Incrementa a cada mensagem recebida
CAN_RxHeaderTypeDef RxHeader;  // Último cabeçalho recebido
uint8_t RxData[8];      // Últimos dados recebidos
```

## Observações Importantes
- **Callbacks devem ser rápidos** (< 10µs recomendado)
- **Não usar HAL_Delay()** em contexto de interrupção
- **Variáveis compartilhadas** podem precisar de proteção
- **FIFO overflow** pode ocorrer se callback for lento
- **Priority inversion** deve ser considerada em sistemas complexos