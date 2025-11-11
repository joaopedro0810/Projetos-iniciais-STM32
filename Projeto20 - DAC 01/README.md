# Projeto20 - DAC 01

## Descrição do Projeto
Este projeto demonstra o uso básico do **DAC (Digital-to-Analog Converter)** do STM32F407VET6. O sistema gera uma tensão analógica constante de aproximadamente **1.65V** na saída PA4, mostrando os conceitos fundamentais de conversão digital-analógica.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino DAC:** PA4 (DAC_OUT1)

## DAC - Digital-to-Analog Converter

### O que é um DAC?
O **DAC (Digital-to-Analog Converter)** é um periférico que converte valores digitais em tensões analógicas proporcionais. É o complemento do ADC e permite que microcontroladores controlem sistemas analógicos.

### Características do DAC STM32F407
- **Resolução:** 12 bits (4096 níveis)
- **Canais:** 2 canais independentes (DAC_OUT1 e DAC_OUT2)
- **Range de saída:** 0V a VDDA (tipicamente 3.3V)
- **Impedância de saída:** ~15kΩ
- **Settling time:** ~3µs (typ)
- **DNL:** ±1 LSB (typ)

### Pinos do DAC
```c
DAC_OUT1 → PA4  (Canal 1)
DAC_OUT2 → PA5  (Canal 2)
```

## Cálculo da Tensão de Saída

### Fórmula Básica
```
Vout = (Valor_Digital / 4095) × VDDA
```

### Para o Projeto (Valor = 2048)
```
Vout = (2048 / 4095) × 3.3V ≈ 1.65V
```

**Observação:** 2048 representa exatamente a metade da escala (50% de 3.3V).

### Tabela de Valores Comuns
| Valor Digital | Tensão de Saída | Percentual |
|--------------|----------------|------------|
| 0            | 0.00V          | 0%         |
| 1024         | 0.83V          | 25%        |
| 2048         | 1.65V          | 50%        |
| 3072         | 2.48V          | 75%        |
| 4095         | 3.30V          | 100%       |

## Configuração do DAC

### Inicialização Principal
```c
static void MX_DAC_Init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração do Canal 1
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;        // Trigger manual
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;  // Buffer habilitado
    
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### Parâmetros de Configuração

#### Trigger do DAC
```c
DAC_TRIGGER_NONE        // Trigger por software (manual)
DAC_TRIGGER_T2_TRGO     // Timer 2 TRGO
DAC_TRIGGER_T4_TRGO     // Timer 4 TRGO
DAC_TRIGGER_T5_TRGO     // Timer 5 TRGO
DAC_TRIGGER_T6_TRGO     // Timer 6 TRGO (comum para geração de ondas)
DAC_TRIGGER_T7_TRGO     // Timer 7 TRGO
DAC_TRIGGER_T8_TRGO     // Timer 8 TRGO
DAC_TRIGGER_EXT_IT9     // External trigger
DAC_TRIGGER_SOFTWARE    // Software trigger
```

#### Output Buffer
```c
DAC_OUTPUTBUFFER_ENABLE     // Buffer habilitado (padrão)
DAC_OUTPUTBUFFER_DISABLE    // Buffer desabilitado (maior corrente)
```

**Vantagens do Buffer:**
- **Menor impedância de saída** (~1Ω vs ~15kΩ)
- **Maior capacidade de corrente** (±20mA vs ±1mA)
- **Melhor estabilidade** com cargas variáveis

## Loop Principal

### Código de Execução
```c
int main(void)
{
    // Inicializações...
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);    // Iniciar DAC
    
    while (1)
    {
        // Definir valor analógico (1.65V)
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
        
        HAL_Delay(500);  // Aguardar 500ms
    }
}
```

### Funções Principais

#### 1. Iniciar DAC
```c
HAL_StatusTypeDef HAL_DAC_Start(DAC_HandleTypeDef *hdac, uint32_t Channel);
```
- **Função:** Habilita o canal DAC especificado
- **Parâmetros:** Handle do DAC e canal
- **Retorno:** HAL_OK ou HAL_ERROR

#### 2. Definir Valor
```c
HAL_StatusTypeDef HAL_DAC_SetValue(DAC_HandleTypeDef *hdac, uint32_t Channel, 
                                   uint32_t Alignment, uint32_t Data);
```
- **hdac:** Handle do DAC
- **Channel:** DAC_CHANNEL_1 ou DAC_CHANNEL_2
- **Alignment:** Alinhamento dos dados
- **Data:** Valor digital (0-4095)

#### 3. Tipos de Alinhamento
```c
DAC_ALIGN_12B_R     // 12-bit right aligned  [11:0]
DAC_ALIGN_12B_L     // 12-bit left aligned   [15:4]
DAC_ALIGN_8B_R      // 8-bit right aligned   [7:0]
```

## Clock Configuration

### HSI sem PLL (16MHz)
```c
void SystemClock_Config(void)
{
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // PLL desabilitado
    
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // HSI direto
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     // 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;      // 16MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;      // 16MHz
}
```

**Clocks resultantes:**
- **SYSCLK:** 16MHz
- **AHB:** 16MHz
- **APB1:** 16MHz (Clock do DAC)
- **APB2:** 16MHz

### Voltage Scaling
```c
__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
```
- **Scale 1:** Performance máximo, até 168MHz
- **Consumo:** Maior, mas adequado para aplicações que requerem precisão

## Características Técnicas do DAC

### Especificações Elétricas
- **Tensão de alimentação:** 2.4V a 3.6V
- **Consumo:** ~380µA por canal (típico)
- **Temperatura de operação:** -40°C a +85°C
- **Linearidade:** ±2 LSB (máx)
- **Offset error:** ±10mV (típico)
- **Gain error:** ±0.5% (típico)

### Limitações
- **Impedância alta:** 15kΩ sem buffer
- **Corrente limitada:** ±1mA sem buffer
- **Settling time:** Dependente da carga
- **Ruído:** ~20µV RMS (típico)

## Aplicações Práticas

### 1. Controle de Tensão de Referência
```c
// Definir tensão de referência para comparadores
uint16_t voltage_ref = (uint16_t)(2.5 * 4095 / 3.3);  // 2.5V
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, voltage_ref);
```

### 2. Bias para Amplificadores
```c
// Bias de 1.65V para amplificadores single-supply
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
```

### 3. Controle de LEDs
```c
// Controle de intensidade de LED via tensão
for (uint16_t brightness = 0; brightness <= 4095; brightness += 100)
{
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, brightness);
    HAL_Delay(50);
}
```

### 4. Simulação de Sensores
```c
// Simular saída de sensor de temperatura (0-3.3V = -40°C a +125°C)
float temperature = 25.0;  // 25°C
uint16_t dac_value = (uint16_t)((temperature + 40) * 4095 / 165);
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
```

## Medição e Verificação

### Equipamentos Necessários
- **Multímetro:** Para medir tensão DC
- **Osciloscópio:** Para verificar estabilidade
- **Carga resistiva:** Para teste de corrente
- **Fonte variável:** Para comparação

### Procedimento de Teste
1. **Conectar multímetro** no pino PA4
2. **Programar e executar** o código
3. **Verificar tensão:** Deve estar próxima de 1.65V
4. **Testar diferentes valores:** 0, 1024, 2048, 3072, 4095
5. **Medir settling time** com osciloscópio

### Valores Esperados
```c
Valor 0:    0.00V ± 0.01V
Valor 1024: 0.83V ± 0.02V
Valor 2048: 1.65V ± 0.02V
Valor 3072: 2.48V ± 0.02V
Valor 4095: 3.30V ± 0.02V
```

## Troubleshooting

### Problemas Comuns

#### 1. Tensão Incorreta
**Sintomas:** Tensão diferente do esperado
**Causas possíveis:**
- VDDA diferente de 3.3V
- Carga excessiva no pino
- Configuração incorreta do alinhamento

#### 2. Instabilidade na Saída
**Sintomas:** Tensão oscilante
**Causas possíveis:**
- Fonte de alimentação ruidosa
- Buffer desabilitado com carga alta
- Interferência eletromagnética

#### 3. DAC Não Funciona
**Sintomas:** Tensão sempre 0V
**Causas possíveis:**
- DAC não inicializado
- Clock do DAC desabilitado
- Configuração incorreta do GPIO

### Soluções
```c
// Verificar se DAC foi inicializado
if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
{
    Error_Handler();  // Debug aqui
}

// Verificar valor válido
if (dac_value > 4095) dac_value = 4095;

// Usar buffer para cargas baixas
sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
```

## Melhorias e Extensões

### 1. DAC Dual Channel
```c
// Usar ambos os canais simultaneamente
HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 1024);  // PA4
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 3072);  // PA5
```

### 2. Controle via Potenciômetro
```c
// Ler ADC e aplicar no DAC
uint16_t adc_value = HAL_ADC_GetValue(&hadc1);
uint16_t dac_value = adc_value;  // Mapeamento 1:1
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
```

### 3. Interface Serial
```c
// Controlar DAC via UART
uint16_t dac_value;
if (HAL_UART_Receive(&huart2, (uint8_t*)&dac_value, 2, 100) == HAL_OK)
{
    if (dac_value <= 4095)
    {
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    }
}
```

### 4. Calibração
```c
// Sistema de calibração simples
typedef struct {
    uint16_t offset;
    float gain;
} dac_calibration_t;

dac_calibration_t cal = {.offset = 0, .gain = 1.0f};

uint16_t calibrated_value = (uint16_t)(raw_value * cal.gain + cal.offset);
HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, calibrated_value);
```

## Conceitos Aprendidos
1. **Conversão D/A** - Transformação digital para analógico
2. **Resolução DAC** - 12 bits = 4096 níveis
3. **Configuração básica** - Trigger, buffer e alinhamento
4. **Cálculo de tensão** - Relação valor digital vs tensão
5. **HAL DAC** - Funções de inicialização e controle
6. **Output buffer** - Importância para drive de carga
7. **Clock configuration** - HSI simples para aplicações básicas

## Melhorias Sugeridas
1. **Implementar rampa** - Variação gradual da tensão
2. **Controle via interface** - UART, SPI ou I2C
3. **Calibração automática** - Compensação de offset/gain
4. **Proteção de sobrecarga** - Limitação de corrente
5. **Filtragem analógica** - RC para reduzir ruído
6. **Dual channel** - Usar ambos os canais DAC

## Hardware Recomendado
- **Multímetro digital:** Precisão ±0.1%
- **Osciloscópio:** Para análise temporal
- **Buffer amplifier:** Para cargas pesadas
- **Filtro RC:** Para redução de ruído
- **Conectores:** Para facilitar medições

## Observações Importantes
- **VDDA = VDD:** Usar mesma alimentação para precisão
- **Decoupling:** Capacitores próximos ao DAC
- **Ground:** Referência estável é fundamental
- **Load:** Considerar impedância da carga
- **Settling time:** Aguardar estabilização após mudança
- **Temperatura:** Considerações para aplicações críticas