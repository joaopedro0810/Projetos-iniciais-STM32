# Projeto53 - CAN Filter

## Descrição do Projeto
Este projeto demonstra o uso de filtros CAN em **modo lista** (`CAN_FILTERMODE_IDLIST`), permitindo aceitar apenas IDs específicos predefinidos. O sistema transmite três mensagens diferentes (IDs 0x11, 0x12, 0x13) mas o filtro aceita apenas duas delas (0x11 e 0x12), demonstrando filtragem seletiva avançada.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 168MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library

## Evolução dos Filtros (Série CAN)

### Progressão da Série:
- **Projeto49:** Filtro "aceita tudo" (básico)
- **Projeto51:** Filtro com interrupções (eficiência)
- **Projeto52:** Filtro ID/Máscara (seletividade)
- **Projeto53:** **Filtro Lista (especificidade)**

## Modo Lista vs. Modo ID/Máscara

### Modo ID/Máscara (`CAN_FILTERMODE_IDMASK`)
- **Operação:** Bitwise AND com máscara
- **Flexibilidade:** Ranges de IDs
- **Uso:** Aceitar faixas ou padrões de IDs

### Modo Lista (`CAN_FILTERMODE_IDLIST`)
- **Operação:** Comparação direta
- **Flexibilidade:** IDs específicos únicos
- **Uso:** Lista exata de IDs permitidos

## Configuração do Filtro Lista

### Estrutura do Filtro
```c
CAN_FilterTypeDef sFilterConfig;
sFilterConfig.FilterBank = 0;
sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;    // Modo Lista
sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;   // Scale 32-bit
sFilterConfig.FilterIdHigh = 0x0220;                 // ID 0x11
sFilterConfig.FilterIdLow = 0x0000;                  // Não usado (Standard ID)
sFilterConfig.FilterMaskIdHigh = 0x0240;             // ID 0x12 (reutiliza campo máscara)
sFilterConfig.FilterMaskIdLow = 0x0000;              // Não usado (Standard ID)
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
```

### Interpretação no Modo Lista
No modo lista, os campos são reinterpretados:
- **FilterIdHigh:** Primeiro ID aceito (0x11)
- **FilterMaskIdHigh:** Segundo ID aceito (0x12)
- **FilterIdLow/FilterMaskIdLow:** Para IDs estendidos (não usado aqui)

## Comportamento do Sistema

### Mensagens Transmitidas (Loop Sequencial)
```c
// Ciclo de 7.5 segundos:
1. ID = 0x11, Data = [0x03, 0x04]        → ACEITA (callback executado)
2. ID = 0x12, Data = [0x08, 0x09, 0x0A]  → ACEITA (callback executado)  
3. ID = 0x13, Data = [0x05, 0x07, 0x08]  → REJEITADA (filtro bloqueia)
```

### Resultado Esperado
- **Mensagem 0x11:** ✅ Aceita pelo filtro
- **Mensagem 0x12:** ✅ Aceita pelo filtro
- **Mensagem 0x13:** ❌ Rejeitada pelo filtro
- **Taxa de aceitação:** 2/3 das mensagens (66.7%)

### Sequência Temporal Detalhada
```
Tempo | TX         | Filtro     | RX         | intg | Observação
------|------------|------------|------------|------|------------
0ms   | ID=0x11    | ACEITA     | Callback   | 1    | Lista: 0x11 ✓
2.5s  | ID=0x12    | ACEITA     | Callback   | 2    | Lista: 0x12 ✓
5.0s  | ID=0x13    | REJEITA    | ---        | 2    | Lista: 0x13 ✗
7.5s  | ID=0x11    | ACEITA     | Callback   | 3    | Lista: 0x11 ✓
10s   | ID=0x12    | ACEITA     | Callback   | 4    | Lista: 0x12 ✓
12.5s | ID=0x13    | REJEITA    | ---        | 4    | Lista: 0x13 ✗
```

## Configurações de Scale no Modo Lista

### Scale 32-bit (Atual)
```c
CAN_FILTERSCALE_32BIT
// Dois IDs Standard (11 bits cada)
FilterIdHigh = ID1 << 5      // Primeiro ID
FilterMaskIdHigh = ID2 << 5  // Segundo ID
```

### Scale 16-bit (Alternativa)
```c
CAN_FILTERSCALE_16BIT
// Quatro IDs Standard possíveis
FilterIdHigh = ID1 << 5      // ID 1
FilterIdLow = ID2 << 5       // ID 2
FilterMaskIdHigh = ID3 << 5  // ID 3
FilterMaskIdLow = ID4 << 5   // ID 4
```

## Vantagens do Modo Lista

### 1. Precisão Absoluta
- **Zero ambiguidade:** Apenas IDs específicos
- **Sem wildcards:** Não aceita IDs não listados
- **Controle total:** Exatamente quais mensagens processar

### 2. Performance Otimizada
- **Comparação direta:** Hardware compara diretamente
- **Sem cálculos:** Não há operações de máscara
- **Determinístico:** Tempo de filtragem constante

### 3. Simplicidade Conceitual
- **Lista explícita:** Fácil de entender e configurar
- **Debug simples:** IDs aceitos são óbvios
- **Manutenção clara:** Adicionar/remover IDs é direto

## Limitações do Modo Lista

### 1. Quantidade Limitada
- **Scale 32-bit:** Máximo 2 IDs Standard por filtro
- **Scale 16-bit:** Máximo 4 IDs Standard por filtro
- **Múltiplos filtros:** Necessário para mais IDs

### 2. Sem Flexibilidade
- **IDs exatos:** Não permite ranges ou padrões
- **Mudanças:** Requer reconfiguração para novos IDs
- **Escalabilidade:** Limitada por número de filtros

## Comparação dos Modos de Filtro

| Aspecto | ID/Máscara | Lista |
|---------|------------|-------|
| **Flexibilidade** | Alta (ranges, padrões) | Baixa (IDs específicos) |
| **Precisão** | Configurável | Absoluta |
| **IDs por filtro** | Muitos (com máscara) | Poucos (2-4) |
| **Configuração** | Complexa (cálculo máscara) | Simples (lista direta) |
| **Performance** | Operação AND | Comparação direta |
| **Uso típico** | Categorias, ranges | Lista específica |

## Aplicações Práticas do Modo Lista

### 1. Sistema de Segurança
```c
// Aceitar apenas comandos críticos específicos
FilterIdHigh = 0x7FE << 5;   // Comando emergência
FilterMaskIdHigh = 0x7FF << 5; // Comando shutdown
```

### 2. Gateway Simples
```c
// Rotear apenas mensagens específicas entre redes
FilterIdHigh = 0x100 << 5;   // Sensor temperatura
FilterMaskIdHigh = 0x200 << 5; // Sensor pressão
```

### 3. ECU Especializada
```c
// Aceitar apenas sinais de controle do motor
FilterIdHigh = 0x7E0 << 5;   // Diagnostic request
FilterMaskIdHigh = 0x7E8 << 5; // Diagnostic response
```

## Múltiplos Filtros Lista

### Configuração de Dois Filtros
```c
// Filtro 0: IDs 0x11, 0x12
CAN_FilterTypeDef Filter0;
Filter0.FilterBank = 0;
Filter0.FilterMode = CAN_FILTERMODE_IDLIST;
Filter0.FilterIdHigh = 0x0220;    // ID 0x11
Filter0.FilterMaskIdHigh = 0x0240; // ID 0x12

// Filtro 1: IDs 0x21, 0x22
CAN_FilterTypeDef Filter1;
Filter1.FilterBank = 1;
Filter1.FilterMode = CAN_FILTERMODE_IDLIST;
Filter1.FilterIdHigh = 0x0420;    // ID 0x21
Filter1.FilterMaskIdHigh = 0x0440; // ID 0x22
```

## Filtragem com Múltiplos FIFOs

### Roteamento por Categoria
```c
// Filtro 0 → FIFO0: Sensores
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
FilterIdHigh = 0x100 << 5;   // Sensores
FilterMaskIdHigh = 0x101 << 5;

// Filtro 1 → FIFO1: Atuadores  
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
FilterIdHigh = 0x200 << 5;   // Atuadores
FilterMaskIdHigh = 0x201 << 5;
```

### Callbacks Diferentes
```c
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    // Processar mensagens de sensores
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    // Processar mensagens de atuadores
}
```

## Debugging e Análise

### Monitoramento de Taxa de Aceitação
```c
uint32_t tx_count = 0;        // Total transmitido
uint32_t rx_count = 0;        // Total recebido (intg)
float acceptance_rate = (float)rx_count / tx_count * 100;
```

### Análise Temporal
```c
uint32_t last_rx_time[2];     // Tempo das últimas recepções
uint32_t msg_intervals[2];    // Intervalos entre mensagens aceitas
```

## Conceitos Aprendidos
1. **Modo Lista CAN** - Filtragem por IDs específicos
2. **Reutilização de campos** - FilterMask usado para segundo ID
3. **Scale 32-bit vs 16-bit** - Capacidade de IDs por filtro
4. **Filtragem seletiva** - Aceitar subset de mensagens transmitidas
5. **Configuração multimodal** - Diferentes estratégias de filtro
6. **Performance de filtragem** - Hardware vs software filtering

## Próximos Passos
- **Projeto54:** Transição para FDCAN (CAN-FD protocol)
- **Implementar:** Filtros híbridos (Lista + ID/Máscara)
- **Testar:** Múltiplos filtros com diferentes FIFOs
- **Otimizar:** Configuração dinâmica de filtros

## Exercícios Sugeridos
1. **Modificar lista** para aceitar IDs 0x12 e 0x13
2. **Implementar Scale 16-bit** com 4 IDs
3. **Adicionar segundo filtro** com FIFO1
4. **Criar filtro híbrido** (um Lista, um ID/Máscara)

## Observações Importantes
- **Modo Lista:** Ideal para poucos IDs específicos
- **Ordem não importa:** FilterIdHigh e FilterMaskIdHigh são equivalentes
- **Filtros independentes:** Cada filtro pode usar modo diferente
- **Hardware limitation:** Máximo 28 filtros (STM32F4)
- **FIFO routing:** Permite processamento categorizado de mensagens