# Projeto55 - FDCAN 2 Loopback

## Descrição do Projeto
Este projeto demonstra o uso da **segunda instância FDCAN** (FDCAN2) do STM32H7A3ZI em modo loopback externo, explorando recursos avançados como diferentes FIFOs de recepção e configurações de Message RAM. O projeto mostra como utilizar FDCAN2 com **FIFO1** ao invés do FIFO0, demonstrando flexibilidade na arquitetura de recepção.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32H7A3ZIT6Q
- **Core:** ARM Cortex-M7 @ 280MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Instância:** FDCAN2 (segunda instância)

## Capacidades Dual FDCAN do STM32H7A3ZI

### Instâncias FDCAN Disponíveis
- **FDCAN1:** Primeira instância (PD0/PD1)
- **FDCAN2:** Segunda instância (PB12/PB13)
- **Independentes:** Cada uma com sua própria Message RAM
- **Paralelas:** Podem operar simultaneamente

### Vantagens da Dual FDCAN
1. **Redundância:** Backup e fault tolerance
2. **Gateway:** Bridge entre redes CAN-FD
3. **Segregação:** Diferentes tipos de tráfego
4. **Performance:** Duplica a largura de banda total

## Diferenças do Projeto54 (FDCAN1)

### Projeto54 - FDCAN1 Loopback
- **Instância:** FDCAN1
- **Pinos:** PD0 (RX), PD1 (TX)
- **FIFO usado:** FIFO0
- **Callback:** `HAL_FDCAN_RxFifo0Callback`

### Projeto55 - FDCAN2 Loopback (Atual)
- **Instância:** FDCAN2
- **Pinos:** PB12 (RX), PB13 (TX)
- **FIFO usado:** FIFO1
- **Callback:** `HAL_FDCAN_RxFifo1Callback`

## Configuração FDCAN2

### Hardware Configuration
```c
FDCAN_HandleTypeDef hfdcan2;  // Handle para FDCAN2
```

**Pinos utilizados:**
- **FDCAN2_RX:** PB12
- **FDCAN2_TX:** PB13
- **GPIO Clock:** GPIOB

### Parâmetros de Timing (Idênticos ao FDCAN1)
```c
hfdcan2.Init.NominalPrescaler = 2;        // Divisor nominal
hfdcan2.Init.NominalSyncJumpWidth = 13;   // SJW nominal
hfdcan2.Init.NominalTimeSeg1 = 86;        // Time Seg1 nominal
hfdcan2.Init.NominalTimeSeg2 = 13;        // Time Seg2 nominal
```

**Taxa nominal calculada:** 250 Kbps (mesma do Projeto54)

### Configurações de Dados
```c
hfdcan2.Init.DataPrescaler = 26;          // Divisor para dados (vs 25 no FDCAN1)
hfdcan2.Init.DataSyncJumpWidth = 1;       // SJW dados
hfdcan2.Init.DataTimeSeg1 = 2;            // Time Seg1 dados
hfdcan2.Init.DataTimeSeg2 = 1;            // Time Seg2 dados
```

## Configuração de Message RAM

### Diferenças Chave na Organização
```c
// FDCAN2 - Configuração FIFO
hfdcan2.Init.RxFifo0ElmtsNbr = 0;           // FIFO0 desabilitado (0 elementos)
hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;  // Tamanho não usado

hfdcan2.Init.RxFifo1ElmtsNbr = 1;           // FIFO1 habilitado (1 elemento)
hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_12; // 12 bytes por elemento
```

### Comparação FIFO0 vs FIFO1

| Aspecto | FIFO0 (Projeto54) | FIFO1 (Projeto55) |
|---------|-------------------|-------------------|
| **Habilitado** | Sim (1 elemento) | Não (0 elementos) |
| **Desabilitado** | Não (0 elementos) | Sim (1 elemento) |
| **Callback** | `RxFifo0Callback` | `RxFifo1Callback` |
| **Interrupção** | `FDCAN_IT_RX_FIFO0_NEW_MESSAGE` | `FDCAN_IT_RX_FIFO1_NEW_MESSAGE` |
| **Get Message** | `FDCAN_RX_FIFO0` | `FDCAN_RX_FIFO1` |

## Filtro FDCAN2
```c
FDCAN_FilterTypeDef sFilterConfig;
sFilterConfig.IdType = FDCAN_STANDARD_ID;
sFilterConfig.FilterIndex = 0;
sFilterConfig.FilterType = FDCAN_FILTER_MASK;
sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;  // Direciona para FIFO1
sFilterConfig.FilterID1 = 0x11;
sFilterConfig.FilterID2 = 0x11;
```

**Diferença crucial:** `FDCAN_FILTER_TO_RXFIFO1` direciona mensagens para FIFO1.

## Callback FIFO1
```c
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET) {
        // Recuperar mensagem do FIFO1
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader, RxData);
        
        // Reativar notificação FIFO1
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
    }
}
```

### Diferenças do Callback FIFO0:
- **Parâmetro:** `RxFifo1ITs` vs `RxFifo0ITs`
- **Flag:** `FDCAN_IT_RX_FIFO1_NEW_MESSAGE` vs `FDCAN_IT_RX_FIFO0_NEW_MESSAGE`
- **FIFO:** `FDCAN_RX_FIFO1` vs `FDCAN_RX_FIFO0`

## NVIC Configuration
```c
NVIC.FDCAN2_IT0_IRQn=true:0:0:false:false:true:true:true:true
```

**Interrupção específica:** `FDCAN2_IT0_IRQn` para FDCAN2 (vs `FDCAN1_IT0_IRQn`)

## Aplicações da Dual FDCAN

### 1. Gateway CAN-FD
```c
// FDCAN1: Rede de sensores (baixa velocidade)
// FDCAN2: Rede de controle (alta velocidade)
// Roteamento inteligente entre as redes
```

### 2. Sistema Redundante
```c
// FDCAN1: Canal principal
// FDCAN2: Canal de backup
// Detecção automática de falhas
```

### 3. Multiplexação de Dados
```c
// FDCAN1: Dados críticos (FIFO0)
// FDCAN2: Dados de diagnóstico (FIFO1)
// Processamento paralelo
```

### 4. Bridge Multi-Protocolo
```c
// FDCAN1: CAN-FD com BRS (alta velocidade)
// FDCAN2: CAN-FD sem BRS (compatibilidade)
// Conversão entre protocolos
```

## Vantagens do FIFO1

### Flexibilidade de Roteamento
- **FIFO0:** Mensagens de alta prioridade
- **FIFO1:** Mensagens de baixa prioridade
- **Processamento diferenciado** por categoria

### Balanceamento de Carga
- **Distribuição:** Mensagens em FIFOs diferentes
- **Paralelismo:** Callbacks independentes
- **Performance:** Reduz gargalos

### Debugging Avançado
- **Isolamento:** Diferentes tipos de tráfego
- **Análise:** Estatísticas por FIFO
- **Monitoramento:** Comportamento específico

## Clock Domain Isolation

### Message RAM Independent
Cada instância FDCAN tem sua própria seção da Message RAM:
```c
// FDCAN1: Message RAM Offset = 0
// FDCAN2: Message RAM Offset = calculado automaticamente
```

### Benefícios:
- **Sem interferência:** Uma instância não afeta a outra
- **Configuração independente:** Tamanhos diferentes de FIFO
- **Fault isolation:** Falha em uma não afeta a outra

## Exemplo de Sistema Dual FDCAN

### main.c Hipotético Dual
```c
// Configuração simultânea FDCAN1 + FDCAN2
MX_FDCAN1_Init();  // FIFO0, PD0/PD1
MX_FDCAN2_Init();  // FIFO1, PB12/PB13

// Start ambas as instâncias
HAL_FDCAN_Start(&hfdcan1);
HAL_FDCAN_Start(&hfdcan2);

// Callbacks independentes
void HAL_FDCAN_RxFifo0Callback() { /* FDCAN1 */ }
void HAL_FDCAN_RxFifo1Callback() { /* FDCAN2 */ }
```

## Performance Considerations

### Bandwidth Total
- **FDCAN1:** 250 Kbps (nominal)
- **FDCAN2:** 250 Kbps (nominal)  
- **Total teórico:** 500 Kbps agregado

### CPU Load Distribution
- **Interrupt load:** Distribuído entre FIFO0 e FIFO1
- **Processing:** Callbacks paralelos
- **Memory:** Message RAMs independentes

## Debugging FDCAN2

### Variáveis de Monitoramento
```c
// Específicas para FDCAN2
FDCAN_HandleTypeDef hfdcan2;      // Handle da instância 2
FDCAN_RxHeaderTypeDef RxHeader;   // Último header recebido
uint8_t RxData[12];               // Últimos dados recebidos

// Debug counters
uint32_t fdcan2_rx_count = 0;     // Mensagens recebidas FDCAN2
uint32_t fdcan2_tx_count = 0;     // Mensagens transmitidas FDCAN2
```

### Validation Tests
1. **FDCAN2 isolation:** FDCAN1 parado, FDCAN2 funcionando
2. **FIFO1 specific:** Verificar callback correto
3. **Pin mapping:** Confirmar PB12/PB13 functionality
4. **Message RAM:** Verificar offset independente

## Conceitos Aprendidos
1. **Dual FDCAN architecture** - Múltiplas instâncias independentes
2. **FIFO1 vs FIFO0** - Diferentes FIFOs de recepção
3. **Message RAM organization** - Gestão independente de memória
4. **Pin multiplexing** - FDCAN2 em pinos alternativos
5. **Callback differentiation** - Handlers específicos por FIFO
6. **Filter routing** - Direcionamento para FIFOs específicos

## Próximos Passos
- **Projeto56:** FDCAN Normal Mode (comunicação real)
- **Implementar:** Sistema dual FDCAN (1+2 simultâneos)
- **Testar:** Gateway entre diferentes redes CAN-FD
- **Otimizar:** Load balancing entre instâncias

## Aplicações Avançadas
- **Automotive gateways:** Múltiplas redes CAN-FD
- **Industrial automation:** Segregação de tráfego
- **Aerospace systems:** Redundância crítica
- **IoT hubs:** Múltiplos sensores/atuadores

## Observações Importantes
- **FDCAN2 independent:** Totalmente separado do FDCAN1
- **FIFO1 usage:** Demonstra flexibilidade de roteamento
- **Pin assignment:** PB12/PB13 específicos para FDCAN2
- **Same timing:** Parâmetros idênticos ao FDCAN1 para compatibilidade
- **Callback isolation:** Processamento independente por instância
- **Message RAM offset:** Automático pelo HAL para evitar conflitos