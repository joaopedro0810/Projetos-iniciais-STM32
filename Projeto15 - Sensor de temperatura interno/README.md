# Projeto15 - Sensor de Temperatura Interno 🌡️

![Dificuldade](https://img.shields.io/badge/Dificuldade-Iniciante-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-ADC1%20Temperature%20Sensor-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-25%20min-orange.svg)

## 📋 Descrição

Este projeto demonstra como usar o **sensor de temperatura interno** do STM32F407VET6. Utiliza o canal especial `ADC_CHANNEL_TEMPSENSOR` para medir a temperatura do die do microcontrolador, aplicando a fórmula de conversão oficial da ST Microelectronics. Excelente introdução aos sensores internos do STM32.

## 🎯 Objetivos de Aprendizado

- Usar canal especial ADC (TEMPSENSOR)
- Entender sensor de temperatura interno
- Aplicar fórmula de conversão de temperatura
- Trabalhar com calibração de sensores
- Conceitos de tensão de referência (V25, avgSlope)
- Limitações e precisão de sensores internos

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Nenhum hardware externo necessário!** 🎉
- **Multímetro** (opcional, para medição externa)
- **Termômetro** (para comparação)

## 📐 Esquema de Ligação

```
Nenhuma conexão externa necessária!

O sensor de temperatura é INTERNO ao microcontrolador:
- Localizado no die do chip
- Conectado internamente ao ADC1
- Acessível via ADC_CHANNEL_TEMPSENSOR
- Não requer pinos GPIO
```

### Configuração de Pinos
- **Nenhum pino externo** - sensor totalmente interno

## 💻 Principais Conceitos

### 1. Configuração ADC para Sensor Interno
```c
ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Configuração ADC padrão
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;        // 12 bits para precisão
    hadc1.Init.ScanConvMode = DISABLE;                 // Single channel
    hadc1.Init.ContinuousConvMode = DISABLE;           // Single conversion
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    
    // Configuração do canal de temperatura
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;          // Canal especial!
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;   // Tempo adequado
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Cálculo de Temperatura
```c
// Constantes de calibração (do datasheet STM32F407)
#define CONST_TENSAO    0.0032258064516129032258064516129  // Conversão ADC (INCORRETA!)
float const v25 = 0.76;        // Tensão do sensor a 25°C (V)
float const avgSlope = 0.0025; // Slope médio (V/°C)

// Variáveis globais
float adc;         // Valor ADC bruto
float vSense;      // Tensão do sensor (V)
float interTemp;   // Temperatura calculada (°C)

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_ADC1_Init();
    
    while (1)
    {
        // Iniciar conversão
        HAL_ADC_Start(&hadc1);
        
        // Aguardar conversão
        HAL_ADC_PollForConversion(&hadc1, 100);
        
        // Ler valor ADC
        adc = HAL_ADC_GetValue(&hadc1);
        
        // Converter para tensão
        vSense = adc * CONST_TENSAO;
        
        // Calcular temperatura usando fórmula oficial
        interTemp = (vSense - v25) / avgSlope + 25;
        
        // Parar ADC
        HAL_ADC_Stop(&hadc1);
        
        HAL_Delay(500);
    }
}
```

### 3. Fórmula de Conversão (Datasheet STM32F407)
```c
// Fórmula oficial da ST:
// Temperature (°C) = {(VSENSE - V25) / Avg_Slope} + 25
//
// Onde:
// VSENSE = Tensão lida do sensor
// V25 = Tensão do sensor a 25°C (tipicamente 0.76V)
// Avg_Slope = Slope médio (tipicamente 2.5mV/°C)
//
// Exemplo:
// Se VSENSE = 0.78V
// Temp = (0.78 - 0.76) / 0.0025 + 25 = 8°C + 25 = 33°C
```

## 🔍 Funcionalidades

### Características do Sensor:
- **Localização:** Interno ao die do STM32
- **Range:** -40°C a +125°C (típico)
- **Precisão:** ±2°C (típica)
- **Resolução:** Dependente do ADC (12 bits)
- **Tempo de resposta:** Relativamente lento (segundos)

### Limitações:
- **Auto-aquecimento:** CPU aquece o sensor
- **Offset individual:** Cada chip tem offset único
- **Drift térmico:** Precisão varia com temperatura
- **Não calibrado de fábrica**

## ⚙️ Configuração STM32CubeMX

### ADC1 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Instance | ADC1 | ADC número 1 |
| Resolution | 12 bits | Máxima precisão |
| Clock Prescaler | PCLK2/2 | Clock adequado |
| Scan Conversion | Disabled | Single channel |
| Continuous Conversion | Disabled | Single shot |

### Channel Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Channel | Temperature Sensor | Canal interno especial |
| Rank | 1 | Primeira conversão |
| Sampling Time | 144 Cycles | Tempo adequado |

### Configurações Especiais:
- **Temperature sensor:** Enabled
- **Vref:** Internal (não configurável)

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure ADC1 em modo single conversion
   - Select "Temperature Sensor" channel
   - Set resolução para 12 bits
   - Generate code

2. **Não precisa hardware externo!**
   - Sensor é interno ao microcontrolador

3. **Teste:**
   - Compile e execute
   - Use debugger para monitorar `interTemp`
   - Compare com temperatura ambiente

## 📊 Análise de Precisão

### Problemas no Código Atual:
```c
// PROBLEMA: Constante incorreta!
#define CONST_TENSAO    0.0032258064516129032258064516129  // Para 10 bits
// Código usa 12 bits, mas constante é de 10 bits!

// CORRETO para 12 bits:
#define CONST_TENSAO_12BIT    (3.3f / 4096.0f)  // = 0.0008056640625
```

### Versão Corrigida:
```c
// Constantes corretas
#define CONST_TENSAO_12BIT    (3.3f / 4096.0f)    // Conversão ADC correta
const float V25 = 0.76f;                           // V @ 25°C
const float AVG_SLOPE = 0.0025f;                   // V/°C

// Cálculo corrigido
uint16_t adc_raw = HAL_ADC_GetValue(&hadc1);
float vSense = (float)adc_raw * CONST_TENSAO_12BIT;
float temperature = ((vSense - V25) / AVG_SLOPE) + 25.0f;
```

### Calibração Individual:
```c
// Para melhor precisão, calibrar cada chip individualmente
float calibration_offset = 0.0f;  // Determinado experimentalmente
float calibrated_temp = temperature + calibration_offset;
```

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Corrigir constante:** Usar constante correta para 12 bits
2. **Display temperature:** Mostrar temperatura via UART/LCD
3. **Threshold alarm:** Alertar se temperatura > limite

### Para Intermediários:
1. **Averaging:** Média móvel para estabilizar leituras
2. **Calibration:** Implementar calibração automática
3. **Celsius/Fahrenheit:** Converter entre escalas

### Para Avançados:
1. **Compensation:** Compensar auto-aquecimento da CPU
2. **Curve fitting:** Usar polinômio ao invés de linear
3. **Factory calibration:** Usar valores de calibração da memória

## 🐛 Possíveis Problemas

### Temperatura incorreta:
- **Constante errada:** Código usa constante de 10 bits para 12 bits
- **Valores típicos:** V25 e avgSlope são valores típicos, não individuais
- **Auto-aquecimento:** CPU carregada aquece o sensor

### Instabilidade:
- **Sampling time baixo:** Aumentar tempo de amostragem
- **Conversões rápidas:** Sensor tem constante de tempo alta
- **Ruído:** Filtro digital pode ajudar

### Debugging:
```c
// Verificar valores intermediários
printf("ADC: %d\n", (int)adc);
printf("vSense: %.3fV\n", vSense);
printf("Temp: %.1f°C\n", interTemp);

// Valores esperados a ~25°C:
// ADC: ~945 (para vSense ~0.76V com 12 bits)
// vSense: ~0.76V
// Temp: ~25°C
```

## 🔍 Entendendo o Sensor

### Princípio de Funcionamento:
```c
// Sensor é um diodo interno com características conhecidas
// Tensão do diodo varia linearmente com temperatura
// Coeficiente térmico: ~-2.5mV/°C (negativo!)
// Fórmula: V(T) = V25 + slope * (T - 25)
// Invertendo: T = (V - V25) / slope + 25
```

### Limitações Físicas:
- **Localização:** No die, próximo ao CPU
- **Constante de tempo:** Lenta resposta térmica
- **Variabilidade:** Cada chip é ligeiramente diferente
- **Range:** Limitado pela tecnologia do processo

### Aplicações Típicas:
- **Thermal protection:** Detectar superaquecimento
- **Performance scaling:** Reduzir clock se muito quente
- **Rough ambient:** Estimativa da temperatura ambiente
- **Self-diagnostics:** Monitoramento interno

## 📈 Aplicações Práticas

### Sistema de Proteção Térmica:
```c
#define TEMP_WARNING_THRESHOLD    70.0f    // °C
#define TEMP_CRITICAL_THRESHOLD   85.0f    // °C

void thermal_management(float temperature)
{
    if (temperature > TEMP_CRITICAL_THRESHOLD)
    {
        // Reduzir clock ou entrar em sleep
        enter_thermal_protection_mode();
    }
    else if (temperature > TEMP_WARNING_THRESHOLD)
    {
        // Avisar usuário
        thermal_warning_led_on();
    }
}
```

### Data Logger com Temperatura:
```c
typedef struct {
    uint32_t timestamp;
    float temperature;
    // outros sensores...
} sensor_reading_t;

sensor_reading_t log_buffer[100];
uint8_t log_index = 0;

void log_temperature(void)
{
    log_buffer[log_index].timestamp = HAL_GetTick();
    log_buffer[log_index].temperature = interTemp;
    log_index = (log_index + 1) % 100;
}
```

### Compensação de Outros Sensores:
```c
// Muitos sensores são afetados pela temperatura
float compensated_reading = raw_sensor_value * (1 + temp_coefficient * (interTemp - 25.0f));
```

## 📚 Próximos Passos

Continue explorando sensores e ADC avançado:
- **[Projeto16 - ADC Interrupção](../Projeto16%20-%20ADC%20Interrupcao/):** ADC com interrupts
- **[Projeto17 - ADC com DMA](../Projeto17%20-%20ADC%20com%20DMA/):** High-speed sampling
- **[Projeto13 - ADC Simples](../Projeto13%20-%20ADC%20Simples/)** (revisitar): Comparar sensores externos vs internos

## 📖 Referências

- [STM32F407 Temperature Sensor](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN3964: Temperature Sensor Calibration](https://www.st.com/resource/en/application_note/an3964-temperature-sensor-calibration-on-stm32f4xx-microcontrollers-stmicroelectronics.pdf)
- [STM32F407 Datasheet - Electrical Characteristics](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf)

---

🌡️ **Dica:** Sensor interno é útil para proteção térmica, mas não substitui sensor externo para medições precisas de ambiente!