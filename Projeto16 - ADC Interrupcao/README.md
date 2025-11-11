# Projeto16 - ADC Interrupção ⚡

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-ADC1%20Interrupt-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-30%20min-orange.svg)

## 📋 Descrição

Este projeto evolui do conceito de polling para **ADC baseado em interrupções**. Demonstra como usar `HAL_ADC_Start_IT()` e callback `HAL_ADC_ConvCpltCallback()` para uma abordagem não-bloqueante de aquisição ADC. Essencial para sistemas que precisam processar outras tarefas enquanto aguardam conversões ADC.

## 🎯 Objetivos de Aprendizado

- Implementar ADC com interrupções (interrupt-driven)
- Usar callback de conversão completa
- Entender vantagens sobre polling
- Configurar NVIC para ADC interrupts
- Trabalhar com continuous conversion mode
- Sistema não-bloqueante vs bloqueante

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Potenciômetro:** 10kΩ (para teste)
- **LED** (opcional, para indicação visual)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
Potenciômetro     |    STM32F407VET6
==================================
VCC (3.3V) --------> 3.3V
GND ----------------> GND
Wiper (saída) -----> PA3 (ADC1_IN3)

LED (Opcional):
Anodo -------------> PA5 (ou outro pino)
Catodo ------------> GND (via resistor 330Ω)
```

### Configuração de Pinos
- **PA3:** ADC1_IN3 (Channel 3)

## 💻 Principais Conceitos

### 1. Configuração ADC com Interrupção
```c
ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Configuração ADC
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_8B;         // 8 bits = 0-255
    hadc1.Init.ScanConvMode = ENABLE;                  // Scan enabled
    hadc1.Init.ContinuousConvMode = ENABLE;            // Continuous mode!
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
    
    // Configuração Canal 3
    sConfig.Channel = ADC_CHANNEL_3;                   // PA3
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;   // Alta precisão
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Inicialização com Interrupção
```c
// Variável global para resultado ADC
float adc;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    
    // Iniciar ADC com interrupção (não-bloqueante!)
    HAL_ADC_Start_IT(&hadc1);
    
    while (1)
    {
        // CPU LIVRE para outras tarefas!
        // ADC roda em background via interrupções
        
        // Exemplo de outras tarefas:
        HAL_Delay(100);           // ou processar outros dados
        // toggle_led();          // ou piscar LED
        // process_uart();        // ou comunicação serial
        // update_display();      // ou atualizar display
    }
}
```

### 3. Callback de Conversão Completa
```c
// Callback executado automaticamente a cada conversão
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1)
    {
        // Ler valor ADC
        adc = HAL_ADC_GetValue(&hadc1);
        
        // NOTA: Em continuous mode, callback é chamado
        // continuamente sem precisar restar o ADC
        
        // Opcional: processar dado imediatamente
        // float voltage = adc * (3.3f / 255.0f);  // 8 bits
        // if (voltage > 2.0f) { /* ação */ }
    }
}
```

## 🔍 Funcionalidades

### Características Principais:
- **Resolução:** 8 bits (0-255) - mais rápido que 12 bits
- **Modo:** Continuous conversion + Interrupt
- **CPU:** Non-blocking (CPU livre para outras tarefas)
- **Callback:** Automático a cada conversão

### Vantagens sobre Polling:
- **CPU livre:** Não trava aguardando conversão
- **Multitasking:** Pode processar outras tarefas simultaneamente
- **Eficiência:** Menor consumo de energia
- **Real-time:** Resposta imediata via interrupt

## ⚙️ Configuração STM32CubeMX

### ADC1 Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Resolution | 8 bits | Mais rápido que 12 bits |
| Scan Conversion | Enabled | Mesmo para single channel |
| Continuous Conversion | **Enabled** | Conversões automáticas |
| Sampling Time | 480 Cycles | Alta precisão |

### NVIC Settings:
| Interrupt | Priority | Description |
|-----------|----------|-------------|
| ADC global interrupt | Enabled | Necessário para callbacks |

### Channel Configuration:
| Channel | Pin | Rank | Sampling Time |
|---------|-----|------|---------------|
| IN3 | PA3 | 1 | 480 Cycles |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure ADC1 em continuous mode
   - Enable ADC global interrupt no NVIC
   - Set resolução para 8 bits (mais rápido)
   - Generate code

2. **Hardware Setup:**
   - Conecte potenciômetro ao pino PA3
   - Teste variando a tensão de entrada

3. **Debugging:**
   - Use breakpoint no callback para verificar funcionamento
   - Monitor variável `adc` em tempo real

## 📊 Análise de Performance

### Comparação com Polling:
| Aspecto | Polling | Interrupt |
|---------|---------|-----------|
| CPU Usage | 100% bloqueado | CPU livre |
| Responsividade | Limitada | Imediata |
| Multitasking | Impossível | Possível |
| Complexidade | Simples | Média |
| Power Consumption | Alto | Baixo |

### Timing Analysis:
```c
// Continuous mode com 8 bits:
// Conversion time: 480 + 8 = 488 cycles
// ADC Clock: 42MHz (PCLK2/2)
// Conversion rate: 42MHz / 488 ≈ 86kHz
// Período: ~11.6μs por conversão
// 
// Callback executado ~86.000 vezes por segundo!
```

### Resolução vs Velocidade:
```c
// 8 bits:  Conversion = 488 cycles, Rate = 86kHz
// 10 bits: Conversion = 490 cycles, Rate = 85.7kHz  
// 12 bits: Conversion = 492 cycles, Rate = 85.4kHz
// 
// Para este projeto: 8 bits oferece boa velocidade
// LSB = 3.3V / 256 = 12.9mV (resolução adequada)
```

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **LED indicator:** Piscar LED a cada N conversões
2. **Threshold detection:** Acionar ação quando ADC > valor
3. **Convert to voltage:** Calcular tensão no callback

### Para Intermediários:
1. **Averaging buffer:** Buffer circular para média móvel
2. **Multiple tasks:** Adicionar outras tarefas no main loop
3. **State machine:** Implementar máquina de estados baseada em ADC

### Para Avançados:
1. **Priority management:** Diferentes prioridades de interrupts
2. **Trigger modes:** Usar timer trigger ao invés de continuous
3. **Error handling:** Tratamento de erros de conversão

## 🐛 Possíveis Problemas

### Callback não executa:
- **NVIC disabled:** Verificar ADC interrupt habilitado
- **Wrong callback:** Verificar nome da função callback
- **HAL_ADC_Start_IT() não chamado:** Verificar inicialização

### Performance issues:
- **Callback muito pesado:** Evitar operações longas no callback
- **Stack overflow:** Callback executado em contexto de interrupt
- **Race conditions:** Cuidado com variáveis compartilhadas

### Debugging Tips:
```c
// Adicionar contador no callback
volatile uint32_t conversion_count = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc == &hadc1)
    {
        conversion_count++;  // Monitor interrupt frequency
        adc = HAL_ADC_GetValue(&hadc1);
        
        // Toggle LED a cada 1000 conversões (para visualizar)
        if (conversion_count % 1000 == 0)
        {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        }
    }
}
```

## 🔍 Boas Práticas

### Callback Design:
```c
// ✅ BOM: Callback rápido e simples
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc = HAL_ADC_GetValue(&hadc1);  // Rápido
    new_data_flag = 1;               // Sinalizar main loop
}

// ❌ RUIM: Callback pesado
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    HAL_Delay(100);         // NUNCA usar delay em interrupt!
    printf("ADC: %d\n", x); // Operações longas bloqueiam sistema
}
```

### Data Processing:
```c
// Processar dados no main loop, não no callback
volatile uint8_t new_adc_data = 0;
volatile uint16_t adc_value = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_value = HAL_ADC_GetValue(&hadc1);
    new_adc_data = 1;  // Sinalizar nova leitura
}

int main(void)
{
    // ... init code ...
    HAL_ADC_Start_IT(&hadc1);
    
    while (1)
    {
        if (new_adc_data)
        {
            new_adc_data = 0;  // Clear flag
            
            // Processar dados aqui (contexto normal)
            float voltage = adc_value * (3.3f / 255.0f);
            process_voltage(voltage);
        }
        
        // Outras tarefas...
        handle_uart();
        update_display();
    }
}
```

## 📈 Aplicações Práticas

### Sistema de Monitoramento:
```c
#define THRESHOLD_HIGH    200    // ~2.6V
#define THRESHOLD_LOW     50     // ~0.6V

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    uint8_t adc_val = HAL_ADC_GetValue(&hadc1);
    
    if (adc_val > THRESHOLD_HIGH)
        alarm_high_flag = 1;
    else if (adc_val < THRESHOLD_LOW)
        alarm_low_flag = 1;
}
```

### Data Acquisition:
```c
#define BUFFER_SIZE 100
uint8_t adc_buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_buffer[buffer_index] = HAL_ADC_GetValue(&hadc1);
    buffer_index = (buffer_index + 1) % BUFFER_SIZE;
}
```

## 📚 Próximos Passos

Continue explorando técnicas avançadas de ADC:
- **[Projeto17 - ADC com DMA](../Projeto17%20-%20ADC%20com%20DMA/):** High-speed continuous sampling
- **[Projeto18 - Frequência Amostragem](../Projeto18%20-%20Frequencia%20de%20amostrage%201/):** Timer-triggered conversions
- **[Projeto13 - ADC Simples](../Projeto13%20-%20ADC%20Simples/)** (revisitar): Comparar polling vs interrupt

## 📖 Referências

- [STM32F4 ADC Interrupts](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [HAL ADC Driver](https://www.st.com/resource/en/user_manual/um1725-description-of-stm32f4-hal-and-lowlayer-drivers-stmicroelectronics.pdf)
- [Interrupt Best Practices](https://www.st.com/resource/en/application_note/an4044-floating-point-unit-demonstration-on-stm32-microcontrollers-stmicroelectronics.pdf)

---

⚡ **Dica:** Interrupts liberam a CPU para multitasking. Mantenha callbacks rápidos e processe dados no main loop!