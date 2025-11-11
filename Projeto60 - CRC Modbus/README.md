# Projeto60 - CRC Modbus

## Descrição do Projeto
Este projeto implementa o **cálculo CRC (Cyclic Redundancy Check)** utilizando o algoritmo padrão **Modbus RTU**. Demonstra a implementação de verificação de integridade de dados essencial para comunicações industriais confiáveis, especialmente no protocolo Modbus RTU amplamente usado em automação industrial.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 72MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Protocolo:** Modbus RTU CRC-16

## CRC (Cyclic Redundancy Check)

### O que é CRC?
CRC é um **algoritmo de detecção de erros** baseado em matemática polinomial que gera um código de verificação (checksum) para um bloco de dados. É amplamente usado em:
- **Protocolos de comunicação** (Modbus, Ethernet, USB)
- **Armazenamento de dados** (HD, SSD, memória)
- **Transmissão de arquivos** (ZIP, RAR)
- **Comunicação serial** (RS-485, RS-232)

### Vantagens do CRC
1. **Alta eficiência:** Detecção rápida de erros
2. **Boa cobertura:** Detecta erros burst e aleatórios
3. **Implementação simples:** Algoritmo direto
4. **Baixo overhead:** Apenas 2 bytes para CRC-16
5. **Padronização:** Algoritmos bem definidos

## Algoritmo CRC-16 Modbus

### Especificação Técnica
- **Polinômio:** 0x8005 (x^16 + x^15 + x^2 + 1)
- **Polinômio reverso:** 0xA001
- **Valor inicial:** 0xFFFF
- **XOR final:** Não aplicado
- **Ordem dos bytes:** Little-endian (LSB primeiro)

### Características do CRC Modbus
```c
Polynomial:    0x8005 (normal)    | 0xA001 (reversed)
Init value:    0xFFFF             | Preset registradores
Reflect input: Yes                | Processa LSB primeiro  
Reflect output: Yes               | Resultado refletido
XOR output:    0x0000             | Sem XOR final
```

### Por que Polinômio Reverso?
O algoritmo Modbus processa os bits começando pelo **LSB (Least Significant Bit)**:
- **Polinômio normal:** 0x8005 = 1000 0000 0000 0101
- **Polinômio reverso:** 0xA001 = 1010 0000 0000 0001

## Implementação do Algoritmo

### Função Principal
```c
static uint16_t calc_crc(uint8_t *buf, uint16_t buf_size)
{
    uint16_t crc = 0xFFFF;          // Inicialização padrão Modbus

    for (uint16_t i = 0; i < buf_size; i++)  // Para cada byte
    {
        crc ^= buf[i];              // XOR com byte atual
        
        for (uint8_t j = 8; j != 0; j--)     // Para cada bit (8 bits)
        {
            if ((crc & 0x0001) != 0)         // Se LSB = 1
            {
                crc >>= 1;          // Shift direita
                crc ^= 0xA001;      // XOR com polinômio reverso
            }
            else
            {
                crc >>= 1;          // Apenas shift direita
            }
        }
    }
    
    // Trocar ordem dos bytes (little-endian para Modbus)
    return (((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8));
}
```

### Análise Passo a Passo

#### 1. Inicialização
```c
uint16_t crc = 0xFFFF;
```
- **Valor inicial:** 0xFFFF (padrão Modbus)
- **Finalidade:** Detectar zeros iniciais
- **Alternativas:** 0x0000 (CRC-CCITT)

#### 2. Loop Principal
```c
for (uint16_t i = 0; i < buf_size; i++)
{
    crc ^= buf[i];              // Misturar byte com CRC
}
```
- **XOR:** Combina dado com estado atual do CRC
- **Efeito:** Cada byte influencia o resultado final

#### 3. Processamento Bit a Bit
```c
for (uint8_t j = 8; j != 0; j--)
{
    if ((crc & 0x0001) != 0)    // Testa LSB
    {
        crc >>= 1;              // Shift direita
        crc ^= 0xA001;          // Aplica polinômio
    }
    else
    {
        crc >>= 1;              // Apenas shift
    }
}
```

**Lógica do algoritmo:**
- **Testa LSB:** Se bit menos significativo = 1
- **Divisão polinomial:** Shift + XOR simula divisão
- **8 iterações:** Processa todos os bits do byte

#### 4. Inversão de Bytes
```c
return (((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8));
```
- **Byte swap:** Converte para formato Modbus
- **Little-endian:** LSB transmitido primeiro
- **Resultado:** CRC pronto para transmissão

## Exemplo de Cálculo Manual

### Dados de Teste
```c
uint8_t buffer[3] = {0x20, 0x35, 0xDC};
```

### Processo Detalhado

#### Byte 1: 0x20
```
Inicial: CRC = 0xFFFF
XOR:     CRC = 0xFFFF ^ 0x20 = 0xFFDF

Bit 7 (LSB de 0xFFDF = 1):
  CRC = 0xFFDF >> 1 = 0x7FEF
  CRC = 0x7FEF ^ 0xA001 = 0xDFEE
  
Bit 6 (LSB de 0xDFEE = 0):
  CRC = 0xDFEE >> 1 = 0x6FF7
  
... (continua para todos os 8 bits)
```

#### Resultado Esperado
Para os dados `{0x20, 0x35, 0xDC}`, o CRC deve ser calculado e invertido conforme o padrão Modbus.

## Clock Configuration

### Configuração PLL
```c
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;    // HSI 16MHz
RCC_OscInitStruct.PLL.PLLM = 8;                         // Divisor
RCC_OscInitStruct.PLL.PLLN = 72;                        // Multiplicador  
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;            // Divisor P
```

**Cálculo do SYSCLK:**
- **VCO = (HSI / PLLM) × PLLN = (16MHz / 8) × 72 = 144MHz**
- **SYSCLK = VCO / PLLP = 144MHz / 2 = 72MHz**

### Voltage Scaling
```c
__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
```
- **Scale 3:** Modo de baixo consumo para ≤72MHz
- **Vantagem:** Reduz consumo de energia
- **Limitação:** Frequência máxima reduzida

### Divisores de Clock
```c
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;      // AHB = 72MHz
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;       // APB1 = 36MHz
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;       // APB2 = 72MHz
```

## Aplicações Práticas do CRC

### 1. Protocolo Modbus RTU
```c
// Frame Modbus típico
uint8_t modbus_frame[] = {
    0x01,           // Slave Address
    0x03,           // Function Code (Read Holding Registers)
    0x00, 0x00,     // Starting Address (0x0000)
    0x00, 0x02,     // Quantity of Registers (2)
    // CRC será calculado e adicionado aqui
};

uint16_t crc = calc_crc(modbus_frame, 6);
modbus_frame[6] = crc & 0xFF;         // CRC Low
modbus_frame[7] = (crc >> 8) & 0xFF;  // CRC High
```

### 2. Verificação de Integridade
```c
bool verify_modbus_frame(uint8_t *frame, uint16_t length)
{
    if (length < 3) return false;
    
    // Calcular CRC dos dados (sem os 2 bytes de CRC)
    uint16_t calculated_crc = calc_crc(frame, length - 2);
    
    // Extrair CRC recebido
    uint16_t received_crc = frame[length-2] | (frame[length-1] << 8);
    
    return (calculated_crc == received_crc);
}
```

### 3. Detecção de Erros de Transmissão
```c
void process_received_data(uint8_t *buffer, uint16_t length)
{
    if (verify_modbus_frame(buffer, length))
    {
        // Dados íntegros - processar comando
        handle_modbus_command(buffer);
    }
    else
    {
        // Erro detectado - descartar frame
        send_error_response();
    }
}
```

## Otimizações Possíveis

### 1. Tabela de Lookup (LUT)
```c
// Tabela pré-calculada para acelerar CRC
static const uint16_t crc_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    // ... 248 valores restantes
};

uint16_t calc_crc_fast(uint8_t *buf, uint16_t buf_size)
{
    uint16_t crc = 0xFFFF;
    
    for (uint16_t i = 0; i < buf_size; i++)
    {
        uint8_t tbl_idx = ((crc ^ buf[i]) & 0xFF);
        crc = ((crc >> 8) ^ crc_table[tbl_idx]);
    }
    
    return (((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8));
}
```

### 2. CRC por Hardware (STM32)
```c
// Usar CRC peripheral do STM32 (configuração necessária)
uint32_t hardware_crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)buffer, length);
```

### 3. DMA para Grandes Volumes
```c
// Processar CRC com DMA para dados grandes
HAL_CRC_Calculate_DMA(&hcrc, (uint32_t*)large_buffer, large_length);
```

## Comparação de Algoritmos CRC

| Algoritmo | Polinômio | Init | Aplicação |
|-----------|-----------|------|-----------|
| CRC-16 Modbus | 0x8005 | 0xFFFF | Modbus RTU |
| CRC-16 CCITT | 0x1021 | 0xFFFF | X.25, HDLC |
| CRC-16 IBM | 0x8005 | 0x0000 | Bisync |
| CRC-32 | 0x04C11DB7 | 0xFFFFFFFF | Ethernet, ZIP |

## Performance e Timing

### Complexidade Computacional
- **Algoritmo bit-a-bit:** O(n × 8) = O(8n)
- **Algoritmo com tabela:** O(n)
- **Hardware CRC:** O(1) em paralelo

### Tempo de Execução (estimativa)
```c
// Para buffer de 100 bytes em STM32F407 @ 72MHz:
// Bit-a-bit: ~800 ciclos × 100 bytes = 80k ciclos ≈ 1.1ms
// Com tabela: ~20 ciclos × 100 bytes = 2k ciclos ≈ 0.03ms
// Hardware: ~1 ciclo × 100 bytes = 100 ciclos ≈ 1.4µs
```

## Detecção de Erros

### Capacidade de Detecção
CRC-16 pode detectar:
- **Todos os erros de 1 bit**
- **Todos os erros de 2 bits**
- **Todos os erros ímpares** (devido ao polinômio)
- **Todos os burst errors ≤ 16 bits**
- **99.998% dos burst errors > 16 bits**

### Tipos de Erro Detectados
1. **Bit flip:** Mudança de 0→1 ou 1→0
2. **Burst error:** Múltiplos bits consecutivos
3. **Insertion/deletion:** Bytes perdidos ou duplicados
4. **Ruído:** Interferência eletromagnética

## Casos de Teste

### Teste 1: Dados Simples
```c
uint8_t test1[] = {0x20, 0x35, 0xDC};
uint16_t crc1 = calc_crc(test1, 3);
// Resultado esperado: depende da implementação específica
```

### Teste 2: Frame Modbus Real
```c
uint8_t test2[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02};
uint16_t crc2 = calc_crc(test2, 6);
// Resultado esperado: 0xC40B (para este frame específico)
```

### Teste 3: Dados Todos Zero
```c
uint8_t test3[] = {0x00, 0x00, 0x00, 0x00};
uint16_t crc3 = calc_crc(test3, 4);
// Teste para comportamento com zeros
```

## Validação e Debug

### Ferramentas de Verificação
1. **Calculadoras online:** CRC-16 Modbus
2. **Software Modbus:** ModbusPoll, QModMaster
3. **Analisadores de protocolo:** Wireshark
4. **Simuladores:** Modbus simulators

### Debug do Algoritmo
```c
void debug_crc_calculation(uint8_t *buf, uint16_t size)
{
    uint16_t crc = 0xFFFF;
    printf("Initial CRC: 0x%04X\n", crc);
    
    for (uint16_t i = 0; i < size; i++)
    {
        printf("Byte %d: 0x%02X\n", i, buf[i]);
        crc ^= buf[i];
        printf("After XOR: 0x%04X\n", crc);
        
        for (uint8_t j = 8; j != 0; j--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
                printf("  Bit %d: shift+XOR -> 0x%04X\n", j-1, crc);
            }
            else
            {
                crc >>= 1;
                printf("  Bit %d: shift -> 0x%04X\n", j-1, crc);
            }
        }
    }
    
    uint16_t final_crc = (((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8));
    printf("Final CRC: 0x%04X\n", final_crc);
}
```

## Conceitos Aprendidos
1. **Algoritmo CRC-16** - Cálculo de checksum
2. **Matemática polinomial** - Base teórica do CRC
3. **Bit manipulation** - Operações em nível de bit
4. **Detecção de erros** - Verificação de integridade
5. **Protocolo Modbus** - Padrão industrial
6. **Otimização** - Tabelas de lookup e hardware
7. **Endianness** - Ordem de bytes em comunicação

## Melhorias Sugeridas
1. **Implementar tabela lookup** - Performance otimizada
2. **Usar CRC hardware** - Peripheral dedicado STM32
3. **Adicionar testes unitários** - Validação automática
4. **Implementar outros CRCs** - CRC-32, CRC-CCITT
5. **Benchmark performance** - Comparar implementações
6. **Validação cruzada** - Comparar com ferramentas externas

## Hardware Recomendado
- **Analisador lógico:** Para debug de sinais
- **Oscilloscópio:** Verificação de timing
- **Conversor RS-485:** Para testes Modbus real
- **Simulador Modbus:** Teste de comunicação
- **Multímetro:** Verificação elétrica

## Observações Importantes
- **Byte order:** Modbus usa little-endian para CRC
- **Polinômio reverso:** Otimização para processamento LSB
- **Valor inicial:** 0xFFFF detecta zeros iniciais
- **Performance:** Algoritmo simples mas pode ser otimizado
- **Aplicabilidade:** Base para implementação Modbus completa
- **Portabilidade:** Código funciona em qualquer microcontrolador