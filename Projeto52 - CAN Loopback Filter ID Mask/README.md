# Projeto52 - CAN Loopback Filter ID Mask

## Descrição do Projeto
Este projeto demonstra o uso avançado de filtros CAN com ID específico e máscara, permitindo recepção seletiva de mensagens baseada no identificador. O filtro é configurado para aceitar apenas mensagens com ID 0x12, demonstrando como implementar filtragem precisa em redes CAN.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 168MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library

## Evolução dos Filtros CAN

### Projetos Anteriores:
- **Projeto49:** Filtro "aceita tudo" (máscara 0x0000)
- **Projeto51:** Filtro "aceita tudo" com interrupções

### Projeto52 (Atual):
- **Filtro específico:** Aceita apenas ID 0x12
- **Máscara precisa:** Configuração seletiva
- **Demonstração prática:** TX de duas mensagens, RX de apenas uma

## Fundamentos dos Filtros CAN

### Operação ID e Máscara
Os filtros CAN funcionam com operação **bitwise AND**:
```
Mensagem_ID & FilterMask == FilterID & FilterMask
```

Se a condição for **verdadeira**, a mensagem é aceita.

### Exemplo do Projeto:
```c
FilterIdHigh = 0x0240;    // ID 0x12 (shifted)
FilterMaskIdHigh = 0x0240; // Máscara para ID 0x12
```

**Análise Bitwise:**
```
ID 0x11: 0x0220 & 0x0240 = 0x0200 ≠ 0x0240 & 0x0240 = 0x0240 → REJEITADA
ID 0x12: 0x0240 & 0x0240 = 0x0240 = 0x0240 & 0x0240 = 0x0240 → ACEITA
```

## Configuração Detalhada do Filtro

### Estrutura do Filtro
```c
CAN_FilterTypeDef sFilterConfig;
sFilterConfig.FilterBank = 0;                    // Banco 0
sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; // Modo ID/Máscara
sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; // 32 bits
sFilterConfig.FilterIdHigh = 0x0240;             // ID específico (0x12)
sFilterConfig.FilterIdLow = 0x0000;              // Não usado (STD ID)
sFilterConfig.FilterMaskIdHigh = 0x0240;         // Máscara (0x12)
sFilterConfig.FilterMaskIdLow = 0x0000;          // Não usado (STD ID)
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // FIFO destino
sFilterConfig.FilterActivation = CAN_FILTER_ENABLE; // Ativo
```

### Formato do ID no Registrador CAN
Para **Standard ID (11 bits)**, o formato é:
```
FilterIdHigh[15:5] = StdId[10:0]
FilterIdHigh[4:0] = 0 (não usado)
```

**Conversão ID → Registrador:**
- ID 0x11 → 0x11 << 5 = 0x220 → FilterIdHigh = 0x0220
- ID 0x12 → 0x12 << 5 = 0x240 → FilterIdHigh = 0x0240

## Comportamento do Sistema

### Mensagens Transmitidas
```c
// Loop principal transmite alternadamente:
1. ID = 0x11, Data = [0x03, 0x04]     → FILTRADA (não aparece no callback)
2. ID = 0x12, Data = [0x08, 0x09, 0x0A] → ACEITA (chama callback)
```

### Resultado Esperado
- **Mensagem 0x11:** Transmitida mas **não recebida** (filtro bloqueia)
- **Mensagem 0x12:** Transmitida e **recebida** (filtro aceita)
- **Contador `intg`:** Incrementa **apenas** para mensagens 0x12

### Sequência Temporal
```
Tempo  | TX         | Filtro     | RX         | intg
-------|------------|------------|------------|-----
0ms    | ID=0x11    | REJEITA    | ---        | 0
2500ms | ID=0x12    | ACEITA     | Callback   | 1
5000ms | ID=0x11    | REJEITA    | ---        | 1
7500ms | ID=0x12    | ACEITA     | Callback   | 2
...    | ...        | ...        | ...        | ...
```

## Tipos de Filtros CAN

### 1. Modo ID/Máscara (`CAN_FILTERMODE_IDMASK`)
- **Flexibilidade:** Alta
- **Configuração:** ID + Máscara
- **Uso:** Aceitar range de IDs ou IDs específicos

### 2. Modo Lista (`CAN_FILTERMODE_IDLIST`)
- **Flexibilidade:** Limitada
- **Configuração:** Lista de IDs específicos
- **Uso:** Poucos IDs bem definidos

## Cálculo de Máscaras CAN

### Máscara Específica (um ID)
```c
FilterId = target_id << 5;     // ID desejado
FilterMask = 0x07FF << 5;      // Todos os bits do Standard ID
```

### Máscara de Range (múltiplos IDs)
Para aceitar IDs 0x10-0x1F:
```c
FilterId = 0x10 << 5;          // Base do range
FilterMask = 0x07F0 << 5;      // Máscara para 4 bits superiores
```

### Máscara "Aceita Tudo"
```c
FilterId = 0x0000;             // Qualquer valor
FilterMask = 0x0000;           // Não verifica nenhum bit
```

## Configurações Avançadas

### Scale 32-bit vs 16-bit
```c
// 32-bit: Um filtro completo
CAN_FILTERSCALE_32BIT
FilterIdHigh + FilterIdLow = 32 bits completos

// 16-bit: Dois filtros independentes  
CAN_FILTERSCALE_16BIT
FilterIdHigh = Filtro 1 (16 bits)
FilterIdLow = Filtro 2 (16 bits)
```

### Extended ID (29 bits)
Para IDs estendidos:
```c
TxHeader.IDE = CAN_ID_EXT;     // ID estendido
TxHeader.ExtId = 0x12345678;   // 29 bits

// Filtro para Extended ID
FilterIdHigh = (ExtId >> 13) & 0xFFFF;
FilterIdLow = ((ExtId << 3) | CAN_ID_EXT) & 0xFFFF;
```

## Vantagens da Filtragem Específica

### 1. Eficiência de CPU
- **Menos interrupções:** Apenas mensagens relevantes
- **Callback otimizado:** Processamento direcionado
- **Menor latência:** Resposta mais rápida

### 2. Determinismo
- **Comportamento previsível:** Filtragem garantida em hardware
- **Real-time capability:** Processamento em tempo real
- **Redução de jitter:** Menos variabilidade temporal

### 3. Escalabilidade
- **Múltiplos filtros:** Até 28 filtros (STM32F4)
- **Roteamento avançado:** FIFOs diferentes por categoria
- **Hierarquia de prioridades:** Filtros ordenados

## Aplicações Práticas

### 1. Sistema Automotivo
```c
// ECU de motor aceita apenas comandos de controle
FilterId = 0x100 << 5;    // Comandos de controle
FilterMask = 0x700 << 5;  // Range 0x100-0x1FF
```

### 2. Automação Industrial
```c
// PLC aceita apenas sensores da zona A
FilterId = 0x200 << 5;    // Sensores zona A  
FilterMask = 0x7F0 << 5;  // Range 0x200-0x20F
```

### 3. Sistema de Monitoramento
```c
// Gateway aceita apenas alarmes críticos
FilterId = 0x7F0 << 5;    // IDs de alarme
FilterMask = 0x7F0 << 5;  // Apenas 0x7F0-0x7FF
```

## Debugging e Análise

### Variáveis para Monitoramento
```c
uint8_t intg;                    // Contador de mensagens aceitas
CAN_RxHeaderTypeDef RxHeader;    // Último cabeçalho recebido
uint8_t RxData[8];               // Últimos dados recebidos
```

### Testes de Validação
1. **Transmitir ID 0x11:** Verificar que `intg` **não** incrementa
2. **Transmitir ID 0x12:** Verificar que `intg` **incrementa**
3. **Modificar filtro:** Testar diferentes configurações
4. **Monitor CAN:** Usar analisador para confirmar transmissão

## Comparação de Performance

| Aspecto | Filtro "Aceita Tudo" | Filtro Específico |
|---------|----------------------|-------------------|
| Interrupções | Todas as mensagens | Apenas relevantes |
| CPU Usage | Alta (processamento desnecessário) | Baixa (otimizada) |
| Latência | Variável | Consistente |
| Determinismo | Baixo | Alto |
| Eficiência | Baixa | Alta |

## Conceitos Aprendidos
1. **Filtros ID/Máscara** - Operação bitwise para seleção
2. **Formato de registradores** - Shift de 5 bits para Standard ID
3. **Filtragem seletiva** - Aceitar apenas mensagens relevantes
4. **Otimização de sistema** - Redução de overhead de CPU
5. **Configuração precisa** - Cálculo de máscaras específicas
6. **Debugging de filtros** - Validação através de contadores

## Próximos Passos
- **Projeto53:** Múltiplos filtros e configurações avançadas
- **Implementar:** Filtros para Extended ID (29 bits)
- **Testar:** Filtros de range com máscaras complexas
- **Adicionar:** FIFOs diferentes para categorias de mensagens

## Exercícios Sugeridos
1. **Modificar filtro** para aceitar apenas ID 0x11
2. **Criar range filter** para IDs 0x10-0x1F
3. **Implementar filtro duplo** (scale 16-bit)
4. **Adicionar Extended ID** e filtros correspondentes

## Observações Importantes
- **Hardware filtering:** Muito mais eficiente que software
- **Ordem dos filtros:** Primeiro filtro que aceita, processa
- **FIFO assignment:** Pode rotear para FIFOs diferentes
- **Filtro 0:** Sempre disponível, outros dependem do hardware
- **Máscara 0x0000:** Aceita todas as mensagens (bypass)