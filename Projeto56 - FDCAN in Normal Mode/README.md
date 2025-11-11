# Projeto56 - FDCAN in Normal Mode

## Descrição do Projeto
Este projeto é o **culminar da série FDCAN**, demonstrando comunicação CAN-FD **real** em modo normal usando **ambas as instâncias FDCAN1 e FDCAN2** simultaneamente. O sistema implementa um ping-pong bidirecional onde FDCAN1 transmite mensagens que são recebidas por FDCAN2, que por sua vez responde automaticamente, criando um fluxo contínuo de comunicação entre as duas instâncias.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32H7A3ZIT6Q
- **Core:** ARM Cortex-M7 @ 280MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Instâncias:** FDCAN1 + FDCAN2 (Dual simultâneo)

## Arquitetura do Sistema Dual FDCAN

### Evolução da Série FDCAN
- **Projeto54:** FDCAN1 Loopback (teste interno)
- **Projeto55:** FDCAN2 Loopback (FIFO1 demo)
- **Projeto56:** **FDCAN1 + FDCAN2 Normal Mode (comunicação real)**

### Configuração Dual Simultânea
```c
FDCAN_HandleTypeDef hfdcan1;  // Instância 1
FDCAN_HandleTypeDef hfdcan2;  // Instância 2

// Hardware independente
FDCAN1: PD0 (RX), PD1 (TX)    // Pinos FDCAN1
FDCAN2: PB12 (RX), PB13 (TX)  // Pinos FDCAN2
```

## Fluxo de Comunicação Ping-Pong

### Comportamento do Sistema
```
FDCAN1 (Master)                    FDCAN2 (Slave/Responder)
     |                                        |
     | TX: ID=0x11 "FDCAN1TX 0" ─────────────→ | RX callback ativo
     |                                        | ↓
     | ←───────────── TX: ID=0x22 "FDCAN2TX 0" | Resposta automática
     |                                        |
     | TX: ID=0x11 "FDCAN1TX 1" ─────────────→ | RX callback ativo
     |                                        | ↓
     | ←───────────── TX: ID=0x22 "FDCAN2TX 1" | Resposta automática
```

### Lógica de Funcionamento
1. **FDCAN1:** Transmite mensagem ID=0x11 a cada segundo (loop principal)
2. **FDCAN2:** Recebe mensagem ID=0x11 via callback FIFO1
3. **FDCAN2:** Responde automaticamente com ID=0x22 (dentro do callback)
4. **FDCAN1:** Recebe resposta ID=0x22 via callback FIFO0
5. **Ciclo continua indefinidamente**

## Configuração FDCAN1 (Master/Transmissor)

### Modo e Parâmetros
```c
hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;        // Modo normal
hfdcan1.Init.NominalPrescaler = 1;            // Prescaler reduzido
hfdcan1.Init.NominalSyncJumpWidth = 13;       // SJW
hfdcan1.Init.NominalTimeSeg1 = 86;            // Time Seg1  
hfdcan1.Init.NominalTimeSeg2 = 13;            // Time Seg2
```

**Taxa de bits calculada:**
- Clock FDCAN ≈ 50MHz (PLL configurado)
- Bit Time = 1 + 86 + 13 = 100 TQ
- **Baud Rate = 50MHz / (1 × 100) = 500 Kbps**

### Message RAM FDCAN1
```c
hfdcan1.Init.MessageRAMOffset = 0;            // Offset 0 (primeiro)
hfdcan1.Init.RxFifo0ElmtsNbr = 1;             // FIFO0 habilitado
hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_12; // 12 bytes
hfdcan1.Init.TxEventsNbr = 1;                 // TX Events habilitados
hfdcan1.Init.TxBuffersNbr = 1;                // TX Buffers habilitados
```

### Filtro FDCAN1
```c
sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // Para FIFO0
sFilterConfig.FilterID1 = 0x22;               // Aceita apenas ID 0x22
sFilterConfig.FilterID2 = 0x22;               // Máscara específica
```

## Configuração FDCAN2 (Slave/Responder)

### Modo e Parâmetros (Idênticos)
```c
hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;        // Modo normal
hfdcan2.Init.NominalPrescaler = 1;            // Mesmo prescaler
// Timing idêntico ao FDCAN1 para compatibilidade
```

### Message RAM FDCAN2
```c
hfdcan2.Init.MessageRAMOffset = 11;           // Offset diferente (independente)
hfdcan2.Init.RxFifo1ElmtsNbr = 1;             // FIFO1 habilitado
hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_12; // 12 bytes
```

### Filtro FDCAN2
```c
sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1; // Para FIFO1
sFilterConfig.FilterID1 = 0x11;               // Aceita apenas ID 0x11
sFilterConfig.FilterID2 = 0x11;               // Máscara específica
```

## Cabeçalhos de Transmissão

### FDCAN1 TX Header
```c
TxHeader1.Identifier = 0x11;                  // ID do FDCAN1
TxHeader1.DataLength = FDCAN_DLC_BYTES_12;    // 12 bytes
TxHeader1.BitRateSwitch = FDCAN_BRS_OFF;      // Sem BRS
TxHeader1.FDFormat = FDCAN_FD_CAN;            // Formato CAN-FD
```

### FDCAN2 TX Header
```c
TxHeader2.Identifier = 0x22;                  // ID do FDCAN2 (diferente)
TxHeader2.DataLength = FDCAN_DLC_BYTES_12;    // 12 bytes
TxHeader2.BitRateSwitch = FDCAN_BRS_OFF;      // Sem BRS
TxHeader2.FDFormat = FDCAN_FD_CAN;            // Formato CAN-FD
```

## Callbacks Especializados

### FDCAN1 Callback (FIFO0)
```c
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader1, RxData1);
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        // Apenas recebe (não responde)
    }
}
```

### FDCAN2 Callback (FIFO1) - Com Resposta Automática
```c
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET) {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader2, RxData2);
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
        
        // RESPOSTA AUTOMÁTICA
        sprintf((char *)TxData2, "FDCAN2TX %d", indx++);
        HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader2, TxData2);
    }
}
```

**Diferença crucial:** FDCAN2 callback **transmite automaticamente** uma resposta.

## Loop Principal - Apenas FDCAN1
```c
while (1) {
    sprintf((char *)TxData1, "FDCAN1TX %d", indx++);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader1, TxData1);
    HAL_Delay(1000);  // 1 segundo entre transmissões
}
```

**Observe:** Apenas FDCAN1 transmite no loop principal. FDCAN2 transmite apenas como resposta.

## Hardware Externo Necessário

### Conexão Física Entre FDCAN1 e FDCAN2
```
FDCAN1                    FDCAN2
PD1 (TX) ────┐       ┌──── PB12 (RX)
             │       │
      [Transceiver CAN-FD]
             │       │  
PD0 (RX) ────┘       └──── PB13 (TX)
```

### Transceiver CAN-FD
- **TJA1043/TJA1044:** NXP CAN-FD transceivers
- **MCP2518FD:** Microchip CAN-FD controller
- **SN65HVD230:** TI CAN transceiver (compatível)

### Barramento CAN
- **CAN_H e CAN_L:** Differential pair
- **Terminações 120Ω:** Nas extremidades
- **Comprimento máximo:** Dependente da velocidade (80m @ 500 Kbps)

## Recursos Avançados Habilitados

### TX Events
```c
hfdcan1.Init.TxEventsNbr = 1;    // FDCAN1 com TX Events
hfdcan2.Init.TxEventsNbr = 1;    // FDCAN2 com TX Events
```
- **Função:** Confirma transmissão bem-sucedida
- **Callback:** `HAL_FDCAN_TxEventFifoCallback`

### TX Buffers
```c
hfdcan1.Init.TxBuffersNbr = 1;   // Buffer dedicado
hfdcan2.Init.TxBuffersNbr = 1;   // Buffer dedicado
```
- **Função:** Transmissão com timing preciso
- **Uso:** Mensagens críticas

### Transmit Pause
```c
hfdcan1.Init.TransmitPause = ENABLE;  // Pausa de transmissão
hfdcan2.Init.TransmitPause = ENABLE;  // Controle de fluxo
```

## Aplicações Práticas do Sistema Dual

### 1. Gateway CAN-FD ↔ CAN-FD
```c
// FDCAN1: Rede de alta velocidade (500 Kbps)
// FDCAN2: Rede de baixa velocidade (250 Kbps)
// Roteamento inteligente com conversão de velocidade
```

### 2. Sistema Master-Slave
```c
// FDCAN1: Controlador mestre (envia comandos)
// FDCAN2: Dispositivo escravo (responde automaticamente)
// Protocolo de comunicação proprietário
```

### 3. Redundância de Comunicação
```c
// FDCAN1: Canal principal
// FDCAN2: Canal de backup/verificação
// Comparação de mensagens para validação
```

### 4. Bridge Protocolar
```c
// FDCAN1: CAN-FD nativo (com BRS)
// FDCAN2: CAN-FD compatível (sem BRS)
// Tradução entre modos
```

## Clock Configuration Otimizada

### PLL para FDCAN
```c
RCC_OscInitStruct.PLL.PLLM = 4;      // Divisor entrada
RCC_OscInitStruct.PLL.PLLN = 9;      // Multiplicador
RCC_OscInitStruct.PLL.PLLQ = 3;      // Divisor Q (FDCAN)
```

**Clock FDCAN calculado:**
- HSI = 64MHz
- VCO = 64MHz / 4 × 9 = 144MHz
- FDCAN Clock = VCO / 3 = 48MHz

## Sequência Temporal Esperada

### Timeline de Comunicação
```
Tempo | FDCAN1 TX        | FDCAN2 RX         | FDCAN2 TX        | FDCAN1 RX
------|------------------|-------------------|------------------|------------------
0ms   | "FDCAN1TX 0"     | Callback trigered | "FDCAN2TX 0"     | Callback trigered
1000ms| "FDCAN1TX 1"     | Callback trigered | "FDCAN2TX 1"     | Callback trigered
2000ms| "FDCAN1TX 2"     | Callback trigered | "FDCAN2TX 2"     | Callback trigered
...   | ...              | ...               | ...              | ...
```

### Contadores Independentes
- **FDCAN1:** Incrementa `indx` no loop principal
- **FDCAN2:** Incrementa `indx` no callback (resposta)
- **Resultado:** Números podem divergir se houver perda de mensagens

## Debugging e Monitoramento

### Variáveis de Debug
```c
// Estruturas independentes
FDCAN_RxHeaderTypeDef RxHeader1, RxHeader2;  // Headers recebidos
uint8_t RxData1[12], RxData2[12];            // Dados recebidos
uint8_t TxData1[12], TxData2[12];            // Dados transmitidos

// Contadores
uint32_t fdcan1_tx_count = 0;  // Transmissões FDCAN1
uint32_t fdcan1_rx_count = 0;  // Recepções FDCAN1  
uint32_t fdcan2_tx_count = 0;  // Transmissões FDCAN2
uint32_t fdcan2_rx_count = 0;  // Recepções FDCAN2
```

### Análise de Performance
```c
// Latência de resposta
uint32_t request_time = HAL_GetTick();    // Momento da transmissão
uint32_t response_time;                   // Momento da recepção
uint32_t round_trip_time;                 // RTT calculado
```

## Conceitos Avançados Aprendidos
1. **Dual FDCAN simultâneo** - Operação paralela de duas instâncias
2. **Ping-pong communication** - Protocolo de requisição-resposta
3. **Message RAM isolation** - Offsets independentes
4. **Callback differentiation** - Processamento específico por instância
5. **Automatic response** - Resposta dentro do callback de recepção
6. **FIFO routing** - FIFO0 vs FIFO1 para diferentes instâncias

## Vantagens do Modo Normal
1. **Comunicação real:** Teste de condições reais de rede
2. **Latência realística:** Timing de hardware real
3. **Interferência externa:** Teste de robustez
4. **Debugging avançado:** Análise com analisadores CAN
5. **Validação completa:** Sistema pronto para produção

## Aplicações em Produção
- **Automotive ECUs:** Módulos de controle do motor
- **Industrial automation:** PLCs distribuídos
- **Medical devices:** Monitoramento crítico
- **Aerospace systems:** Comunicação redundante

## Próximos Passos
- **Implementar BRS:** Bit Rate Switch para alta velocidade
- **Payload 64 bytes:** Máxima capacidade CAN-FD
- **Filtros avançados:** Multiple IDs por instância
- **Error handling:** Detecção e recuperação de erros

## Observações Críticas
- **Hardware externo obrigatório:** Transceivers CAN-FD necessários
- **Conexão física:** Barramento CAN real entre instâncias
- **Timing sincronizado:** Mesmo clock para ambas as instâncias
- **Filtros complementares:** IDs cruzados para ping-pong
- **Callback efficiency:** Resposta automática pode causar saturação
- **Message RAM offset:** Fundamental para evitar conflitos

Este projeto representa o **estado da arte** em comunicação CAN-FD com STM32H7, demonstrando todas as capacidades avançadas do protocolo e da arquitetura dual FDCAN.