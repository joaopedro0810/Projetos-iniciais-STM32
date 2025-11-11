# Projeto49 - CAN Loopback

## Descrição do Projeto
Este projeto implementa comunicação CAN (Controller Area Network) em modo loopback usando STM32F407VET6. O modo loopback permite que o microcontrolador transmita e receba mensagens CAN internamente, ideal para testes e validação do protocolo sem necessidade de outro nó na rede.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 168MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library

## Protocolo CAN
O CAN (Controller Area Network) é um protocolo de comunicação serial robusto, amplamente utilizado em aplicações automotivas e industriais. Características principais:
- **Comunicação diferencial** em dois fios (CAN_H e CAN_L)
- **Multi-master** com arbitragem por prioridade
- **Detecção e correção de erros**
- **Velocidades** de 10 Kbps a 1 Mbps
- **Máximo 127 nós** por rede

## Configuração do Hardware

### Pinos CAN1
- **CAN1_RX:** PA11
- **CAN1_TX:** PA12
- **Clock:** APB1 (42 MHz)

### Configuração dos Parâmetros de Timing
```c
hcan1.Init.Prescaler = 10;           // Divisor do clock
hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;  // 1 Time Quantum
hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;      // 13 Time Quanta
hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;       // 2 Time Quanta
```

**Cálculo da Taxa de Bits:**
- Clock CAN = APB1_Clock / Prescaler = 42MHz / 10 = 4.2MHz
- Bit Time = 1 + 13 + 2 = 16 TQ
- Baud Rate = 4.2MHz / 16 = 262.5 Kbps

## Modo Loopback
O modo loopback (`CAN_MODE_LOOPBACK`) conecta internamente o transmissor ao receptor, permitindo:
- **Autoteste** do controlador CAN
- **Validação** da configuração
- **Desenvolvimento** sem hardware externo
- **Testes** de software

## Estrutura das Mensagens CAN

### Cabeçalho de Transmissão
```c
TxHeader.StdId = 0x11;                    // ID padrão (11 bits)
TxHeader.RTR = CAN_RTR_DATA;              // Frame de dados
TxHeader.IDE = CAN_ID_STD;                // ID padrão (não estendido)
TxHeader.DLC = 2;                         // Data Length Code (2 bytes)
TxHeader.TransmitGlobalTime = DISABLE;    // Timestamp desabilitado
```

### Dados da Mensagem
```c
TxData[0] = 0xCA;    // Primeiro byte
TxData[1] = 0xFE;    // Segundo byte
// Forma a palavra 0xCAFE
```

## Configuração do Filtro CAN
```c
CAN_FilterTypeDef sFilterConfig;
sFilterConfig.FilterBank = 0;                        // Banco de filtros 0
sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;    // Modo máscara
sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;   // Escala 32 bits
sFilterConfig.FilterIdHigh = 0x0000;                 // ID alto
sFilterConfig.FilterIdLow = 0x0000;                  // ID baixo
sFilterConfig.FilterMaskIdHigh = 0x0000;             // Máscara alta
sFilterConfig.FilterMaskIdLow = 0x0000;              // Máscara baixa
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;   // FIFO 0
sFilterConfig.FilterActivation = ENABLE;             // Ativo
```

O filtro configurado aceita **todas as mensagens** (máscara 0x0000).

## Fluxo de Operação

### 1. Inicialização
```c
HAL_CAN_Init(&hcan1);           // Inicializa o CAN
HAL_CAN_ConfigFilter(&hcan1);   // Configura filtros
HAL_CAN_Start(&hcan1);          // Inicia o periférico
```

### 2. Transmissão
```c
HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) {}  // Aguarda transmissão
```

### 3. Recepção
```c
if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 1) {
    Error_Handler();  // Erro se FIFO não contém 1 mensagem
}
HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData);
```

### 4. Validação
```c
if((RxHeader.StdId != 0x11) ||
   (RxHeader.RTR != CAN_RTR_DATA) ||
   (RxHeader.IDE != CAN_ID_STD) ||
   (RxHeader.DLC != 2) ||
   ((RxData[0]<<8 | RxData[1]) != 0xCAFE)) {
    return;  // Erro na validação
}
```

## Recursos do Projeto

### FIFOs de Recepção
- **FIFO0:** Recebe mensagens filtradas
- **FIFO1:** Disponível para outros filtros
- **Profundidade:** 3 mensagens cada

### Mailboxes de Transmissão
- **3 Mailboxes** de transmissão
- **Prioridade:** Por ID da mensagem
- **Status:** Monitoramento do nível livre

### Tratamento de Erros
- **Error_Handler():** Para erros críticos
- **Timeouts:** Prevenção de travamentos
- **Validação:** Verificação de integridade

## Conceitos Aprendidos
1. **Protocolo CAN:** Fundamentos e aplicações
2. **Modo Loopback:** Testes internos
3. **Configuração de Timing:** Cálculo de baud rate
4. **Filtros CAN:** Seleção de mensagens
5. **FIFOs e Mailboxes:** Gerenciamento de mensagens
6. **Frames CAN:** Estrutura e validação

## Aplicações Práticas
- **Automotiva:** ECUs, sensores, atuadores
- **Industrial:** Automação, controle de processos
- **Médica:** Equipamentos, monitoramento
- **Aeroespacial:** Sistemas embarcados críticos

## Próximos Passos
- **Projeto50:** CAN em modo normal com dois nós
- **Projeto51:** CAN com interrupções
- **Projeto52:** Filtros por ID e máscara
- **Projeto53:** Filtros avançados

## Observações
- O modo loopback **não requer** transceiver CAN externo
- Ideal para **validação** de configurações
- Base para **projetos CAN** mais complexos
- **Taxa calculada:** 262.5 Kbps com configuração atual