# Projeto 39 - I2C ADXL345

## Descrição do Projeto
Este projeto implementa comunicação I2C com o acelerômetro ADXL345 usando o microcontrolador STM32F407VET6. O sistema demonstra os fundamentos do protocolo I2C, incluindo inicialização do sensor, leitura de registradores, conversão de dados de aceleração e processamento de dados em formato little-endian, com capacidade de medição de aceleração em 3 eixos (X, Y, Z).

## Funcionalidades
- **Comunicação I2C**: Interface com ADXL345 usando protocolo I2C
- **Detecção de Dispositivo**: Verificação automática de presença do sensor
- **Inicialização Completa**: Configuração de power management e formato de dados
- **Leitura Multi-eixo**: Aquisição simultânea de dados X, Y, Z
- **Conversão de Dados**: Transformação de dados raw para valores em 'g'
- **Endianness Handling**: Processamento correto de dados little-endian
- **Error Handling**: Tratamento de erros com indicação visual via LED

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- Sensor acelerômetro ADXL345
- Resistores pull-up para I2C (4.7kΩ ou 10kΩ)
- LED para indicação de erro
- Cabos jumper para conexões
- Protoboard ou PCB para montagem
- Fonte de alimentação 3.3V

## Configuração dos Pinos
| Pino STM32 | Função I2C | ADXL345 | Descrição |
|------------|------------|---------|-----------|
| PB6 | I2C1_SCL | SCL | Clock I2C |
| PB7 | I2C1_SDA | SDA | Dados I2C |
| PA5 | LED | - | LED de erro |
| 3.3V | VCC | VCC | Alimentação |
| GND | GND | GND | Terra |

## Análise Técnica

### Protocolo I2C
O ADXL345 usa comunicação I2C com as seguintes características:
- **Clock Speed**: 100kHz (Standard Mode) a 400kHz (Fast Mode)
- **Addressing**: 7-bit address (0x53 ou 0x1D dependendo do pino ALT ADDRESS)
- **Endereço Write**: 0xA6 (0x53 << 1)
- **Endereço Read**: 0xA7 ((0x53 << 1) | 1)

### Registradores ADXL345
```c
#define DATA_DEVID_REGISTER          0x00  // Device ID (sempre 0xE5)
#define DATA_POWER_CTRL_REGISTER     0x2D  // Power Control
#define DATA_FORMAT_REGISTER         0x31  // Data Format
#define DATA_X0_REGISTER             0x32  // X-axis LSB
#define DATA_X1_REGISTER             0x33  // X-axis MSB
#define DATA_Y0_REGISTER             0x34  // Y-axis LSB
#define DATA_Y1_REGISTER             0x35  // Y-axis MSB
#define DATA_Z0_REGISTER             0x36  // Z-axis LSB
#define DATA_Z1_REGISTER             0x37  // Z-axis MSB
```

### Constante de Conversão
```c
#define ADXL_CONST_CONVERT    0.0078    // 7.8mg/LSB para ±2g range
#define ADXL_CONVERT_REGISTER_DATA(data_to_convert) (data_to_convert * ADXL_CONST_CONVERT)
```

## Estrutura de Dados

### Estrutura Principal
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
    // Verifica se dispositivo está presente
    if (HAL_I2C_IsDeviceReady(&hi2c1, ADXL345_ADDRESS_WRITE, 2, 10) != HAL_OK)
        Error_Handler();

    // Reset power control
    adxl_write_byte(DATA_POWER_CTRL_REGISTER, 0x00);
    
    // Habilita measurement mode
    adxl_write_byte(DATA_POWER_CTRL_REGISTER, 0x08);
    
    // Configura formato: ±4g range, full resolution
    adxl_write_byte(DATA_FORMAT_REGISTER, 0x01);

    // Lê device ID para verificação
    adxl_read_byte(DATA_DEVID_REGISTER, &adxl_data.device_id);
}
```

### Escrita de Dados
```c
void adxl_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t data_write[2] = {reg, data};

    if (HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDRESS_WRITE, 
                               data_write, sizeof(data_write), 100) != HAL_OK) {
        Error_Handler();
    }
}
```

### Leitura de Dados
```c
void adxl_read_byte(uint8_t reg, uint8_t *data_readed)
{
    // Envia endereço do registrador
    if (HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDRESS_WRITE, 
                               &reg, sizeof(reg), 100) != HAL_OK) {
        Error_Handler();
    }

    // Lê dados do registrador
    if (HAL_I2C_Master_Receive(&hi2c1, ADXL345_ADDRESS_READ, 
                              data_readed, 1, 100) != HAL_OK) {
        Error_Handler();
    }
}
```

### Leitura Múltipla (Otimizada)
```c
void adxl_read_values(uint8_t reg, uint8_t *data_rec)
{
    // Lê 6 bytes sequenciais (X0,X1,Y0,Y1,Z0,Z1) em uma operação
    if (HAL_I2C_Mem_Read(&hi2c1, ADXL345_ADDRESS_WRITE, reg, 1, 
                        data_rec, 6, 100) != HAL_OK) {
        Error_Handler();
    }
}
```

### Conversão Little-Endian
```c
uint16_t little_endian_2_bytes(uint8_t *data)
{
    // Combina LSB e MSB corretamente
    return ((data[1] << 8) | data[0]);
}
```

### Loop Principal
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    adxl_init();

    while (1)
    {
        // Lê 6 bytes dos registradores de dados
        adxl_read_values(DATA_X0_REGISTER, adxl_data.data_register);

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

## Configurações do ADXL345

### Power Control Register (0x2D)
- **0x00**: Standby mode (reset)
- **0x08**: Measurement mode ativo

### Data Format Register (0x31)
- **Bit 3**: FULL_RES (full resolution)
- **Bits 1-0**: Range selection
  - 00: ±2g (7.8 mg/LSB)
  - 01: ±4g (15.6 mg/LSB)
  - 10: ±8g (31.2 mg/LSB)
  - 11: ±16g (62.5 mg/LSB)

## Aplicações Práticas
1. **Detecção de Movimento**: Alarmes e sistemas de segurança
2. **Controle de Orientação**: Dispositivos com tela rotativa
3. **Análise de Vibração**: Monitoramento de máquinas industriais
4. **Gaming**: Controles sensíveis ao movimento
5. **Fitness Tracker**: Contadores de passos e atividade
6. **Robótica**: Controle de equilíbrio e orientação
7. **Instrumentação**: Medição de aceleração em veículos

## Interpretação dos Dados

### Orientação Típica
- **X = 0, Y = 0, Z = 1g**: Sensor horizontal (face para cima)
- **X = 0, Y = 0, Z = -1g**: Sensor horizontal (face para baixo)
- **X = 1g, Y = 0, Z = 0**: Sensor vertical (lado direito para baixo)
- **X = 0, Y = 1g, Z = 0**: Sensor vertical (frente para baixo)

### Cálculo de Magnitude
```c
float magnitude = sqrt(x*x + y*y + z*z);
// Em repouso: magnitude ≈ 1.0g
```

## Vantagens do I2C
- **Dois Fios**: Apenas SDA e SCL necessários
- **Multi-slave**: Múltiplos dispositivos no mesmo barramento
- **Endereçamento**: Sistema de endereços para identificação
- **Velocidade Configurável**: 100kHz a 400kHz
- **Bi-direcional**: Comunicação full-duplex
- **Built-in Error Detection**: ACK/NACK automático

## Troubleshooting

### Sensor não detectado
- Verificar conexões SDA/SCL
- Confirmar resistores pull-up (4.7kΩ)
- Verificar alimentação 3.3V
- Testar continuidade dos cabos
- Verificar endereço I2C correto

### Dados corrompidos
- Verificar velocidade I2C (reduzir se necessário)
- Confirmar resistores pull-up adequados
- Verificar interferência eletromagnética
- Testar com cabos mais curtos
- Verificar timing de leitura

### LED de erro aceso
- Verificar todas as conexões I2C
- Confirmar alimentação do sensor
- Testar com outro sensor ADXL345
- Verificar configuração do I2C no CubeMX
- Debug passo a passo das funções I2C

### Valores inconsistentes
- Verificar conversão little-endian
- Confirmar constante de conversão
- Verificar range configurado (±2g, ±4g, etc.)
- Testar calibração do sensor
- Filtrar ruído com média móvel

## Melhorias Sugeridas
1. **Filtro Digital**: Implementar filtro passa-baixa
2. **Calibração**: Procedimento de calibração automática
3. **Interrupt**: Usar interrupções do ADXL345 para eventos
4. **FIFO**: Utilizar buffer FIFO interno do sensor
5. **Sleep Mode**: Gerenciamento de energia
6. **Data Rate**: Configurar taxa de amostragem otimizada
7. **Threshold Detection**: Detecção de movimento/inatividade
8. **Serial Output**: Enviar dados via UART para visualização

## Exemplo de Dados
```c
// Sensor em repouso (horizontal)
// adxl_data.data_converted_g.x ≈ 0.0g
// adxl_data.data_converted_g.y ≈ 0.0g  
// adxl_data.data_converted_g.z ≈ 1.0g

// Sensor sendo balançado
// adxl_data.data_converted_g.x ≈ -0.5g a +0.5g
// adxl_data.data_converted_g.y ≈ -0.3g a +0.8g
// adxl_data.data_converted_g.z ≈ 0.8g a 1.2g
```

## Performance e Timing
- **Taxa de Leitura**: Até 3200 Hz (configurável)
- **Resolução**: 10-bit a 13-bit (dependendo do range)
- **Latência I2C**: ~1ms para leitura completa a 100kHz
- **Consumo**: ~40μA em measurement mode

## Recursos de Aprendizado
- **Conceitos**: I2C Protocol, Accelerometer, Little-Endian, Sensor Fusion
- **Periféricos**: I2C1, GPIO, NVIC
- **HAL Functions**: HAL_I2C_Master_Transmit, HAL_I2C_Master_Receive, HAL_I2C_Mem_Read
- **Sensor**: ADXL345 datasheet, register map, coordinate system
- **Mathematics**: Vector calculations, magnitude, filtering

Este projeto fornece uma base sólida para trabalhar com sensores I2C e acelerômetros, estabelecendo fundamentos essenciais para aplicações de movimento e orientação.