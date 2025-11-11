# Projeto 40 - SPI ADXL345

## Descrição do Projeto
Este projeto implementa comunicação SPI (Serial Peripheral Interface) com o acelerômetro ADXL345 usando o microcontrolador STM32F407VET6. O sistema demonstra os fundamentos do protocolo SPI de alta velocidade, incluindo controle manual de Chip Select (CS), operações de leitura e escrita separadas, processamento de dados multi-byte, e conversão de dados de aceleração com suporte completo para operações simultâneas nos 3 eixos.

## Funcionalidades
- **Comunicação SPI**: Interface de alta velocidade com ADXL345 usando SPI1
- **Controle Manual CS**: Gerenciamento direto do Chip Select para sincronização precisa
- **Operações Separadas**: Transmissão e recepção independentes para máxima flexibilidade
- **Multi-byte Operations**: Leitura sequencial de múltiplos registradores
- **Read/Write Flags**: Implementação completa de flags de controle SPI
- **Data Conversion**: Conversão de dados raw para valores de aceleração em 'g'
- **High-Speed Transfer**: Comunicação de alta velocidade até vários MHz

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- Sensor acelerômetro ADXL345 com interface SPI
- Resistor pull-up para CS se necessário (10kΩ)
- Cabos jumper para conexões
- Protoboard ou PCB para montagem
- Fonte de alimentação 3.3V
- Capacitores de desacoplamento (100nF)

## Configuração dos Pinos
| Pino STM32 | Função SPI | ADXL345 | Descrição |
|------------|------------|---------|-----------|
| PA5 | SPI1_SCK | SCL/SCLK | Clock SPI |
| PA6 | SPI1_MISO | SDO | Master In Slave Out |
| PA7 | SPI1_MOSI | SDA/SDI | Master Out Slave In |
| PA4 | CS_ADXL | CS | Chip Select (GPIO) |
| 3.3V | VCC | VCC | Alimentação |
| GND | GND | GND | Terra |

## Análise Técnica

### Protocolo SPI
O ADXL345 suporta SPI com as seguintes características:
- **Clock Speed**: Até 5MHz
- **Mode**: Mode 3 (CPOL=1, CPHA=1) ou Mode 0 (CPOL=0, CPHA=0)
- **Bit Order**: MSB first
- **CS Active**: Low (ativo em nível baixo)
- **Data Length**: 8 bits por transferência

### Flags de Controle SPI
```c
#define ADXL345_MULTIBYTE        0x40  // Bit 6: Multiple byte read
#define ADXL345_READ_OPERATION   0x80  // Bit 7: Read operation flag
```

### Formação de Comandos
```c
// Para escrita (single byte):
// comando = registrador | ADXL345_MULTIBYTE
// Exemplo: reg 0x2D → comando = 0x6D

// Para leitura (multiple bytes):
// comando = registrador | ADXL345_READ_OPERATION | ADXL345_MULTIBYTE
// Exemplo: reg 0x32 → comando = 0xF2
```

## Estrutura de Dados

### Estrutura Principal (Idêntica ao I2C)
```c
static struct
{
    uint8_t device_id;              // ID do dispositivo (0xE5)
    uint8_t data_register[6];       // Buffer raw dos 6 bytes (X,Y,Z)

    struct {
        int16_t x, y, z;            // Dados processados (16-bit signed)
    } data_readed_register;

    struct {
        float x, y, z;              // Dados convertidos para 'g'
    } data_converted_g;

} adxl_data = {0};
```

## Código Principal

### Inicialização do ADXL345
```c
void adxl_init(void)
{
    // Configura formato: ±4g range, full resolution
    adxl_write(DATA_FORMAT_REGISTER, 0x01);
    
    // Reset power control
    adxl_write(DATA_POWER_CTRL_REGISTER, 0x00);
    
    // Habilita measurement mode
    adxl_write(DATA_POWER_CTRL_REGISTER, 0x08);

    // Lê device ID para verificação
    adxl_read(DATA_DEVID_REGISTER, &adxl_data.device_id, sizeof(adxl_data.device_id));
}
```

### Função de Escrita SPI
```c
void adxl_write(uint8_t reg, uint8_t data)
{
    uint8_t data_write[2] = {(reg | ADXL345_MULTIBYTE), data};

    // Ativa CS (nível baixo)
    HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_RESET);

    // Transmite comando e dados
    if (HAL_SPI_Transmit(&hspi1, data_write, sizeof(data_write), 100) != HAL_OK) {
        Error_Handler();
    }

    // Desativa CS (nível alto)
    HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_SET);
}
```

### Função de Leitura SPI
```c
void adxl_read(uint8_t reg, uint8_t *data_readed, const size_t data_size)
{
    // Prepara comando de leitura
    reg = ((reg | ADXL345_READ_OPERATION) | ADXL345_MULTIBYTE);

    // Ativa CS (nível baixo)
    HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_RESET);

    // Transmite comando de leitura
    if (HAL_SPI_Transmit(&hspi1, &reg, sizeof(reg), 100) != HAL_OK) {
        Error_Handler();
    }

    // Recebe dados
    if (HAL_SPI_Receive(&hspi1, data_readed, data_size, 100) != HAL_OK) {
        Error_Handler();
    }

    // Desativa CS (nível alto)
    HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_SET);
}
```

### Loop Principal
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();

    adxl_init();

    while (1)
    {
        // Lê 6 bytes sequenciais (X0,X1,Y0,Y1,Z0,Z1)
        adxl_read(DATA_X0_REGISTER, adxl_data.data_register, sizeof(adxl_data.data_register));

        // Converte little-endian para int16_t
        adxl_data.data_readed_register.x = little_endian_2_bytes(&adxl_data.data_register[0]);
        adxl_data.data_readed_register.y = little_endian_2_bytes(&adxl_data.data_register[2]);
        adxl_data.data_readed_register.z = little_endian_2_bytes(&adxl_data.data_register[4]);

        // Converte para valores em 'g'
        adxl_data.data_converted_g.x = ADXL_CONVERT_REGISTER_DATA(adxl_data.data_readed_register.x);
        adxl_data.data_converted_g.y = ADXL_CONVERT_REGISTER_DATA(adxl_data.data_readed_register.y);
        adxl_data.data_converted_g.z = ADXL_CONVERT_REGISTER_DATA(adxl_data.data_readed_register.z);

        loopCont++;
    }
}
```

## Configuração SPI

### Parâmetros Típicos
```c
// Configuração STM32CubeMX para SPI1
hspi1.Instance = SPI1;
hspi1.Init.Mode = SPI_MODE_MASTER;
hspi1.Init.Direction = SPI_DIRECTION_2LINES;
hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;    // CPOL = 1
hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;         // CPHA = 1
hspi1.Init.NSS = SPI_NSS_SOFT;                 // CS manual
hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // ~2.6MHz
hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
```

### Cálculo de Velocidade
```c
// Com SYSCLK = 168MHz, APB2 = 84MHz
// Prescaler 32: SPI_CLK = 84MHz / 32 = 2.625MHz
// Prescaler 16: SPI_CLK = 84MHz / 16 = 5.25MHz (máximo recomendado)
// Prescaler 64: SPI_CLK = 84MHz / 64 = 1.3125MHz (mais conservativo)
```

## Vantagens do SPI vs I2C

| Aspecto | SPI | I2C |
|---------|-----|-----|
| **Velocidade** | Até 5MHz+ | Até 400kHz |
| **Linhas** | 4 (MOSI, MISO, SCK, CS) | 2 (SDA, SCL) |
| **Complexidade** | Simples | Mais complexa |
| **Multi-slave** | CS separado por slave | Endereçamento |
| **Full-duplex** | Sim | Não |
| **Overhead** | Baixo | Médio (addressing) |

## Timing SPI

### Sequência de Comunicação
```
1. CS vai para LOW (ativa slave)
2. Master gera clock (SCK)
3. Dados transmitidos em MOSI (Master → Slave)
4. Dados recebidos em MISO (Slave → Master)
5. CS vai para HIGH (desativa slave)
```

### Timing Crítico
- **CS Setup Time**: >10ns antes do primeiro clock
- **CS Hold Time**: >10ns após último clock
- **Clock Period**: Mínimo 200ns (5MHz)
- **Data Setup/Hold**: Conforme datasheet ADXL345

## Aplicações Práticas
1. **Sistemas de Alta Velocidade**: Aquisição rápida de dados de movimento
2. **Múltiplos Sensores**: Vários ADXL345 com CS individuais
3. **Real-time Control**: Controle de estabilização em tempo real
4. **Gaming de Precisão**: Controles com resposta ultra-rápida
5. **Instrumentação**: Medição precisa de vibração
6. **Robótica Avançada**: Controle de equilíbrio de alta precisão
7. **Aeronáutica**: Sistemas de navegação inercial

## Performance e Throughput

### Cálculos de Performance
```c
// A 2.6MHz, leitura de 6 bytes:
// Tempo = (8 bits/byte × 7 bytes) / 2.6MHz ≈ 21.5μs
// Taxa máxima ≈ 46kHz de leituras completas

// A 5MHz, leitura de 6 bytes:
// Tempo = (8 bits/byte × 7 bytes) / 5MHz ≈ 11.2μs
// Taxa máxima ≈ 89kHz de leituras completas
```

### Recursos Utilizados
- **SPI1**: Full-duplex master mode
- **GPIO**: CS control (PA4)
- **RAM**: ~80 bytes (estruturas de dados)
- **CPU**: <1% a 2.6MHz SPI

## Troubleshooting

### Sem comunicação
- Verificar conexões SPI (SCK, MOSI, MISO, CS)
- Confirmar alimentação 3.3V no ADXL345
- Verificar configuração CS (ativo baixo)
- Testar continuidade de todas as linhas

### Dados incorretos
- Verificar polaridade do clock (CPOL/CPHA)
- Confirmar velocidade SPI (reduzir se necessário)
- Verificar flags de comando (READ_OPERATION, MULTIBYTE)
- Testar com leitura de device ID primeiro

### CS não funciona
- Verificar configuração do pino CS como GPIO output
- Confirmar lógica ativa baixa
- Verificar timing de CS (setup/hold times)
- Testar com osciloscópio se disponível

### Performance baixa
- Aumentar velocidade SPI (reduzir prescaler)
- Otimizar código de conversão
- Usar DMA para transferências SPI
- Minimizar overhead entre leituras

## Melhorias Sugeridas
1. **SPI com DMA**: Transferências automáticas sem CPU
2. **Interrupt Mode**: SPI não-bloqueante com callbacks
3. **Multiple Sensors**: Suporte a múltiplos ADXL345
4. **Error Recovery**: Recuperação automática de erros
5. **Burst Mode**: Leitura de múltiplos samples
6. **FIFO Usage**: Utilizar FIFO interno do ADXL345
7. **Real-time Processing**: Processamento em tempo real
8. **Data Streaming**: Stream contínuo via UART/USB

## Comparação I2C vs SPI (Projeto 39 vs 40)

### Código Similaridades
- Estrutura de dados idêntica
- Mesma lógica de conversão
- Mesmos registradores ADXL345
- Processamento de dados equivalente

### Principais Diferenças
```c
// I2C (Projeto 39)
HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDRESS_WRITE, data, size, timeout);
HAL_I2C_Mem_Read(&hi2c1, ADXL345_ADDRESS_WRITE, reg, 1, buffer, 6, timeout);

// SPI (Projeto 40)
HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_RESET);
HAL_SPI_Transmit(&hspi1, &reg, 1, timeout);
HAL_SPI_Receive(&hspi1, buffer, 6, timeout);
HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_SET);
```

## Exemplo de Uso
```c
// Leitura rápida e contínua
while(1) {
    adxl_read(DATA_X0_REGISTER, adxl_data.data_register, 6);
    
    // Processamento imediato
    process_acceleration_data();
    
    // Sem delay - máxima taxa de aquisição
}
```

## Recursos de Aprendizado
- **Conceitos**: SPI Protocol, Master-Slave, Full-duplex, Chip Select
- **Periféricos**: SPI1, GPIO, Clock Management
- **HAL Functions**: HAL_SPI_Transmit, HAL_SPI_Receive, HAL_GPIO_WritePin
- **Hardware**: Timing diagrams, Protocol analysis, Signal integrity
- **Performance**: Throughput optimization, Real-time constraints

Este projeto demonstra a implementação de comunicação SPI de alta performance, ideal para aplicações que requerem aquisição rápida de dados de sensores e comunicação de baixa latência.