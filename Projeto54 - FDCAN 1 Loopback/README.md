# Projeto54 - FDCAN 1 Loopback

## Descrição do Projeto
Este projeto marca a transição do CAN clássico para **CAN-FD (CAN with Flexible Data-Rate)** usando STM32H7A3ZI. O projeto demonstra comunicação FDCAN em modo loopback externo, enviando strings formatadas com até 12 bytes de dados, mostrando as capacidades estendidas do protocolo CAN-FD.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32H7A3ZIT6Q
- **Core:** ARM Cortex-M7 @ 280MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Família:** STM32H7 (High Performance)

## Evolução CAN → CAN-FD

### CAN Clássico (Projetos 49-53)
- **Payload máximo:** 8 bytes
- **Taxa fixa:** Uma velocidade para todo o frame
- **Microcontrolador:** STM32F4/F7
- **Protocolo:** CAN 2.0A/2.0B

### CAN-FD (Projetos 54-56)
- **Payload estendido:** Até 64 bytes
- **Taxa flexível:** Velocidade diferente para dados
- **Microcontrolador:** STM32H7 (FDCAN peripheral)
- **Protocolo:** ISO 11898-1:2015

## Protocolo CAN-FD

### Vantagens do CAN-FD
1. **Maior throughput de dados:** Até 64 bytes por frame
2. **Bit Rate Switch (BRS):** Velocidade maior na seção de dados
3. **Melhoria na eficiência:** Mais dados por frame
4. **Compatibilidade:** Coexiste com CAN clássico
5. **Detecção de erros aprimorada:** CRC polinomial melhorado

### Estrutura de Frame CAN-FD
```
SOF | ID | Control | Data (até 64 bytes) | CRC | ACK | EOF
    |    | FDF=1   |                    |     |     |
    |    | BRS     |                    |     |     |
```

- **FDF (FD Format):** Bit que identifica frame CAN-FD
- **BRS (Bit Rate Switch):** Alterna velocidade na seção de dados
- **ESI (Error State Indicator):** Estado de erro do nó

## Configuração FDCAN STM32H7A3ZI

### Hardware Configuration
- **FDCAN1_RX:** PD0
- **FDCAN1_TX:** PD1
- **Clock Source:** PLL1Q (configurable)
- **Mode:** External Loopback

### Parâmetros de Timing FDCAN

#### Fase Nominal (Arbitração)
```c
hfdcan1.Init.NominalPrescaler = 2;        // Divisor do clock
hfdcan1.Init.NominalSyncJumpWidth = 13;   // 13 TQ para sincronização
hfdcan1.Init.NominalTimeSeg1 = 86;        // 86 TQ para fase 1
hfdcan1.Init.NominalTimeSeg2 = 13;        // 13 TQ para fase 2
```

**Cálculo da Taxa Nominal:**
- Clock FDCAN = 25MHz (configurado via PLL)
- Bit Time = 1 + 86 + 13 = 100 TQ
- **Baud Rate Nominal = 25MHz / (2 × 100) = 250 Kbps**

#### Fase de Dados (BRS Off neste projeto)
```c
hfdcan1.Init.DataPrescaler = 25;          // Divisor para dados
hfdcan1.Init.DataSyncJumpWidth = 1;       // 1 TQ
hfdcan1.Init.DataTimeSeg1 = 2;            // 2 TQ
hfdcan1.Init.DataTimeSeg2 = 1;            // 1 TQ
```

### Configuração de Frame
```c
hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;  // CAN-FD sem BRS
hfdcan1.Init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;  // Loopback externo
```

- **FDCAN_FRAME_FD_NO_BRS:** CAN-FD com taxa única
- **FDCAN_MODE_EXTERNAL_LOOPBACK:** Loopback através dos pinos

## Configuração de Memória FDCAN

### Message RAM Organization
```c
hfdcan1.Init.MessageRAMOffset = 0;          // Offset na Message RAM
hfdcan1.Init.StdFiltersNbr = 1;             // 1 filtro padrão
hfdcan1.Init.ExtFiltersNbr = 0;             // 0 filtros estendidos
hfdcan1.Init.RxFifo0ElmtsNbr = 1;           // 1 elemento no RX FIFO0
hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_12; // 12 bytes por elemento
hfdcan1.Init.TxFifoQueueElmtsNbr = 1;       // 1 elemento no TX FIFO
hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_12; // 12 bytes por elemento
```

### Tamanhos de Dados CAN-FD Disponíveis
```c
FDCAN_DATA_BYTES_8   // 8 bytes (compatível CAN clássico)
FDCAN_DATA_BYTES_12  // 12 bytes
FDCAN_DATA_BYTES_16  // 16 bytes
FDCAN_DATA_BYTES_20  // 20 bytes
FDCAN_DATA_BYTES_24  // 24 bytes
FDCAN_DATA_BYTES_32  // 32 bytes
FDCAN_DATA_BYTES_48  // 48 bytes
FDCAN_DATA_BYTES_64  // 64 bytes (máximo)
```

## Filtro FDCAN
```c
FDCAN_FilterTypeDef sFilterConfig;
sFilterConfig.IdType = FDCAN_STANDARD_ID;      // ID padrão
sFilterConfig.FilterIndex = 0;                 // Índice do filtro
sFilterConfig.FilterType = FDCAN_FILTER_MASK;  // Tipo máscara
sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // Destino FIFO0
sFilterConfig.FilterID1 = 0x11;                // ID para aceitar
sFilterConfig.FilterID2 = 0x11;                // Máscara (aceita só 0x11)
```

## Cabeçalho de Transmissão FDCAN
```c
FDCAN_TxHeaderTypeDef TxHeader;
TxHeader.Identifier = 0x11;                    // ID padrão 0x11
TxHeader.IdType = FDCAN_STANDARD_ID;           // Tipo ID padrão
TxHeader.TxFrameType = FDCAN_DATA_FRAME;       // Frame de dados
TxHeader.DataLength = FDCAN_DLC_BYTES_12;      // 12 bytes de dados
TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // ESI ativo
TxHeader.BitRateSwitch = FDCAN_BRS_OFF;        // BRS desabilitado
TxHeader.FDFormat = FDCAN_FD_CAN;              // Formato CAN-FD
TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // Sem eventos TX
TxHeader.MessageMarker = 0;                    // Marcador da mensagem
```

### Significado dos Campos:
- **DataLength:** Especifica quantos bytes válidos (não buffer size)
- **ESI:** Indica se o nó está em estado de erro
- **BRS:** Habilitaria velocidade maior na seção de dados
- **FDFormat:** Diferencia CAN-FD de CAN clássico

## Fluxo de Operação

### 1. Inicialização
```c
HAL_FDCAN_Init(&hfdcan1);           // Inicializar FDCAN
HAL_FDCAN_ConfigFilter(&hfdcan1);   // Configurar filtro
HAL_FDCAN_Start(&hfdcan1);          // Iniciar operação
HAL_FDCAN_ActivateNotification();   // Ativar interrupções
```

### 2. Loop Principal - Transmissão
```c
sprintf((char *)TxData, "FDCAN_TX %d", indx++);  // Formatar string
HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
HAL_Delay(1000);  // 1 segundo entre mensagens
```

### 3. Callback de Recepção
```c
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData);
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }
}
```

## Dados Transmitidos

### Formato da String
```c
sprintf((char *)TxData, "FDCAN_TX %d", indx++);
```

### Exemplos de Dados:
```
Iteração 0: "FDCAN_TX 0" (10 chars + \0 = 11 bytes)
Iteração 1: "FDCAN_TX 1" (10 chars + \0 = 11 bytes)
Iteração 2: "FDCAN_TX 2" (10 chars + \0 = 11 bytes)
...
Iteração 99: "FDCAN_TX 99" (11 chars + \0 = 12 bytes)
```

### Vantagem sobre CAN Clássico:
- **CAN clássico:** Máximo 8 bytes → strings truncadas
- **CAN-FD:** 12 bytes → strings completas com contador

## Clock Configuration STM32H7A3ZI

### PLL Configuration
```c
RCC_OscInitStruct.PLL.PLLM = 4;    // Divisor de entrada
RCC_OscInitStruct.PLL.PLLN = 9;    // Multiplicador
RCC_OscInitStruct.PLL.PLLP = 2;    // Divisor P (sistema)
RCC_OscInitStruct.PLL.PLLQ = 3;    // Divisor Q (periféricos)
RCC_OscInitStruct.PLL.PLLFRACN = 3072; // Parte fracionária
```

**Cálculo do Clock:**
- HSI = 64MHz (HSI_DIV1)  
- VCO = 64MHz / 4 × (9 + 3072/8192) ≈ 168MHz
- SYSCLK = VCO / 2 = 84MHz
- FDCAN Clock ≈ 25MHz (via PLL1Q)

### Voltage Scaling
```c
__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
```
- **Scale 0:** Máxima performance (até 280MHz)

## Comparação CAN vs CAN-FD

| Aspecto | CAN Clássico | CAN-FD |
|---------|--------------|--------|
| **Payload máximo** | 8 bytes | 64 bytes |
| **Taxa de bits** | Fixa | Flexível (BRS) |
| **Eficiência** | Baixa | Alta |
| **Compatibilidade** | Universal | Requer suporte CAN-FD |
| **Complexidade** | Simples | Moderada |
| **Performance** | Limitada | Superior |

## Vantagens do STM32H7A3ZI para FDCAN

### 1. Performance Superior
- **280MHz:** Processamento rápido de dados
- **L1 Cache:** Acesso rápido à memória
- **Dual core option:** H747/H757 têm Cortex-M4 adicional

### 2. Periférico FDCAN Avançado
- **Message RAM:** Flexível e configurável
- **Filtros avançados:** Múltiplos tipos e configurações
- **Timestamps:** Precisão temporal alta
- **Error handling:** Detecção e correção robusta

### 3. Conectividade Rica
- **Ethernet:** Para gateways CAN-FD/IP
- **USB:** Interfaces de debug e programação
- **Multiple FDCAN:** Até 2 instâncias FDCAN

## Conceitos Aprendidos
1. **Protocolo CAN-FD** - Extensão do CAN com payload maior
2. **STM32H7 FDCAN peripheral** - Implementação avançada
3. **External Loopback mode** - Teste através de pinos
4. **Payload estendido** - Até 12 bytes neste exemplo
5. **Message RAM organization** - Gestão de memória flexível
6. **String transmission** - Dados textuais em CAN-FD

## Próximos Passos
- **Projeto55:** FDCAN 2 Loopback (dual instance)
- **Projeto56:** FDCAN Normal Mode (comunicação real)
- **Implementar BRS:** Bit Rate Switch para alta velocidade
- **Expandir payload:** Testar com 32 ou 64 bytes

## Aplicações CAN-FD
- **Automotiva moderna:** Veículos conectados, ADAS
- **Automação industrial:** Redes de alta velocidade
- **Aerospace:** Sistemas críticos com alta bandwidth
- **IoT Industrial:** Sensores com dados complexos

## Observações Importantes
- **External Loopback:** Requer conexão física TX→RX
- **BRS desabilitado:** Velocidade única neste projeto  
- **Payload 12 bytes:** Demonstra capacidade estendida
- **Filtro específico:** Aceita apenas ID 0x11
- **String null-terminated:** Importante para processamento de texto
- **STM32H7 required:** CAN-FD não disponível em F4/F7