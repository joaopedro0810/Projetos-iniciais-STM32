# Projeto50 - CAN Normal Mode

## Descrição do Projeto
Este projeto demonstra comunicação CAN (Controller Area Network) em modo normal usando STM32F756ZG. Diferente do modo loopback, o modo normal permite comunicação real entre nós CAN através de um barramento físico, simulando condições reais de uma rede CAN.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F756ZGT6
- **Core:** ARM Cortex-M7 @ 216MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library

## Modo CAN Normal vs. Loopback

### Modo Normal (`CAN_MODE_NORMAL`)
- **Comunicação externa:** Através de pinos físicos CAN_TX/CAN_RX
- **Múltiplos nós:** Suporta rede CAN real
- **Hardware externo:** Requer transceiver CAN (ex: TJA1050, MCP2551)
- **Aplicação real:** Produção e desenvolvimento final

### Diferenças do Loopback
- **Loopback:** Comunicação interna (teste)
- **Normal:** Comunicação externa (aplicação real)
- **Hardware:** Normal requer transceiver externo

## Configuração do Hardware

### STM32F756ZG - Dual CAN Controller
O STM32F756ZG possui **dois controladores CAN independentes**:

#### CAN1 (Master)
- **CAN1_RX:** PD0
- **CAN1_TX:** PD1
- **Papel:** Controlador principal
- **Filtros:** 0-13 (14 filtros)

#### CAN2 (Slave) 
- **CAN2_RX:** PB12
- **CAN2_TX:** PB13
- **Papel:** Controlador secundário
- **Filtros:** 14-27 (compartilhados com CAN1)

### Configuração dos Parâmetros de Timing
```c
// Ambos CAN1 e CAN2 usam os mesmos parâmetros
hcan.Init.Prescaler = 10;           // Divisor do clock
hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;  // 1 Time Quantum
hcan.Init.TimeSeg1 = CAN_BS1_13TQ;      // 13 Time Quanta
hcan.Init.TimeSeg2 = CAN_BS2_2TQ;       // 2 Time Quanta
```

**Cálculo da Taxa de Bits (STM32F756ZG):**
- Clock do sistema configurado para HSI (16MHz)
- APB1 Clock = 16MHz (sem divisão)
- Clock CAN = APB1_Clock / Prescaler = 16MHz / 10 = 1.6MHz
- Bit Time = 1 + 13 + 2 = 16 TQ
- **Baud Rate = 1.6MHz / 16 = 100 Kbps**

## Hardware Externo Necessário

### Transceiver CAN
Para comunicação real, é necessário um **transceiver CAN**:

#### Opções Populares:
- **TJA1050:** Philips/NXP, até 1 Mbps
- **MCP2551:** Microchip, até 1 Mbps  
- **SN65HVD230:** Texas Instruments, 3.3V
- **ISO1050:** Texas Instruments, isolado

#### Conexões Típicas:
```
STM32F756ZG      Transceiver CAN    Barramento CAN
PD1 (CAN1_TX) -> TXD               
PD0 (CAN1_RX) <- RXD               
3.3V          -> VCC               
GND           -> VSS               
              -> CANH ------------ CAN_H
              -> CANL ------------ CAN_L
```

### Terminação da Rede
- **Resistores de terminação:** 120Ω em cada extremidade do barramento
- **Impedância característica:** 120Ω do cabo CAN
- **Máximo comprimento:** Depende da velocidade (40m @ 1Mbps, 1000m @ 10Kbps)

## Configuração Dual CAN

### Inicialização Independente
```c
MX_CAN1_Init();  // Inicializa CAN1 (Master)
MX_CAN2_Init();  // Inicializa CAN2 (Slave)
```

### Características do Sistema Dual:
1. **CAN1 (Master):**
   - Controla filtros 0-13
   - Pode operar independentemente
   - Filtros próprios e compartilhados

2. **CAN2 (Slave):**
   - Usa filtros 14-27
   - Depende de CAN1 para alguns recursos
   - Filtros compartilhados com CAN1

## Interrupções CAN
O arquivo .ioc mostra interrupções habilitadas:
```c
NVIC.CAN1_RX0_IRQn=true    // Interrupção CAN1 FIFO0
NVIC.CAN2_RX0_IRQn=true    // Interrupção CAN2 FIFO0
```

Isso permite:
- **Recepção assíncrona** de mensagens
- **Processamento em callback** 
- **Menor uso de CPU** (sem polling)

## Clock Configuration Análise
```c
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
RCC_OscInitStruct.HSIState = RCC_HSI_ON;
RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
```

- **Clock source:** HSI (16MHz interno)
- **Vantagem:** Não requer cristal externo
- **Desvantagem:** Menos preciso que HSE
- **APB1 = APB2 = HCLK = SYSCLK = 16MHz**

## Fluxo de Operação (Modo Normal)

### 1. Inicialização do Hardware
```c
HAL_Init();                 // Sistema HAL
SystemClock_Config();       // Configurar clocks
MX_GPIO_Init();            // GPIOs dos transceivers
MX_CAN1_Init();            // CAN1 configuração
MX_CAN2_Init();            // CAN2 configuração
```

### 2. Configuração dos Filtros (necessária)
```c
CAN_FilterTypeDef sFilterConfig;
// Configurar filtros para CAN1 e CAN2
HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig);
```

### 3. Iniciar Comunicação
```c
HAL_CAN_Start(&hcan1);      // Ativar CAN1
HAL_CAN_Start(&hcan2);      // Ativar CAN2
```

### 4. Transmissão/Recepção
```c
// Transmitir mensagem
HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);

// Receber mensagem (com interrupção ou polling)
HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData);
```

## Cenários de Aplicação

### 1. Rede CAN Simples (2 nós)
- **Nó A:** STM32F756ZG (CAN1)
- **Nó B:** Outro microcontrolador
- **Barramento:** CAN_H, CAN_L, terminações 120Ω

### 2. Rede CAN Complexa (múltiplos nós)
- **Gateway:** STM32F756ZG (CAN1 + CAN2)
- **Sensores:** Vários nós no barramento
- **Atuadores:** Controlados via mensagens CAN

### 3. Bridge CAN-CAN
- **CAN1:** Rede de sensores (baixa velocidade)
- **CAN2:** Rede de controle (alta velocidade)
- **Bridge:** Filtragem e roteamento de mensagens

## Vantagens do STM32F756ZG
1. **Dual CAN:** Dois controladores independentes
2. **Alta Performance:** Cortex-M7 @ 216MHz
3. **Flexibilidade:** Múltiplas configurações de rede
4. **Filtros Avançados:** 28 filtros total
5. **DMA Support:** Transferência eficiente de dados

## Conceitos Aprendidos
1. **Modo Normal CAN:** Comunicação real vs. teste
2. **Hardware Externo:** Transceivers e terminações
3. **Dual CAN Controller:** Operação simultânea
4. **Clock Management:** HSI vs HSE para CAN
5. **Filtros Compartilhados:** Master/Slave relationship
6. **Interrupções CAN:** Processamento assíncrono

## Próximos Passos
- **Projeto51:** Adicionar interrupções para recepção
- **Projeto52:** Implementar filtros específicos
- **Projeto53:** Filtros avançados e otimização
- **Projetos 54-56:** FDCAN com STM32H7

## Hardware Recomendado para Testes
- **2x STM32F756ZG Discovery/Nucleo**
- **2x Transceiver CAN (TJA1050)**
- **Resistores 120Ω** (terminação)
- **Cabo twisted pair** (CAN_H/CAN_L)
- **Fonte 5V** para transceivers

## Observações Importantes
- **Não funciona** sem transceiver CAN externo
- **Terminações obrigatórias** nas extremidades
- **Clock stability** importante para baud rate
- **Filtros devem ser configurados** antes do start
- **Modo Normal** é para aplicações reais, não teste interno