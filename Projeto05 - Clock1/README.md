# Projeto05 - Clock1 ⏰

![Dificuldade](https://img.shields.io/badge/Dificuldade-Básico-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-GPIO%20%7C%20Clock-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-25%20min-orange.svg)

## 📋 Descrição

Este projeto foca na configuração e compreensão do sistema de clock do STM32, combinando sinalização visual com controle de tempo. Demonstra como usar diferentes configurações de clock e implementar um contador simples com indicação LED.

## 🎯 Objetivos de Aprendizado

- Compreender sistema de clock do STM32
- Configurar diferentes fontes de clock (HSI, HSE, PLL)
- Implementar contador em software
- Trabalhar com variáveis globais
- Otimizar configurações de clock para aplicação

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 2 LEDs externos
- **Resistores:** 2x 330Ω
- **Osciloscópio ou analisador lógico** (opcional, para medição)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
led1_Pin  -----> |+| LED1 (Status) --|> GND
                      |
                    330Ω

led2_Pin  -----> |+| LED2 (Clock) --|> GND
                      |
                    330Ω
```

### Configuração de Pinos
- **led1_Pin:** GPIO_Output (LED indicador principal)
- **led2_Pin:** GPIO_Output (LED de status de clock)

## 💻 Principais Conceitos

### 1. Configuração de Clock
```c
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    // Configurar fonte de clock principal
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    // Configurar oscilador
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Implementação com Contador
```c
uint8_t data;  // Contador global

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    // LED2 indica inicialização completa
    HAL_GPIO_TogglePin(led2_GPIO_Port, led2_Pin);
    
    while (1)
    {
        data++;  // Incrementar contador
        HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
        HAL_Delay(1000);  // Delay baseado no clock configurado
    }
}
```

## 🔍 Funcionalidades

### Sistema de Clock:
- **Fonte:** HSI (Internal High Speed oscillator) - 16 MHz
- **SYSCLK:** 16 MHz (sem PLL)
- **AHB Clock:** 16 MHz
- **APB1/APB2:** 16 MHz cada

### Comportamento do Sistema:
- **LED2:** Liga uma vez na inicialização (status)
- **LED1:** Pisca a cada segundo (heartbeat)
- **Contador:** Incrementa a cada ciclo (data++)
- **Monitoramento:** Possível debug via variável 'data'

## ⚙️ Configuração STM32CubeMX

### Clock Configuration:
| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Input frequency | 16 MHz | HSI internal oscillator |
| HCLK | 16 MHz | AHB bus clock |
| PCLK1 | 16 MHz | APB1 peripheral clock |
| PCLK2 | 16 MHz | APB2 peripheral clock |
| SysTick | 1 ms | Timer tick para HAL_Delay |

### GPIO Configuration:
| Pino | Label | Mode | Pull | Speed |
|------|--------|------|------|-------|
| PAx | led1 | Output Push Pull | No Pull | Low |
| PAy | led2 | Output Push Pull | No Pull | Low |

## 🚀 Como Executar

1. **Configuração inicial:**
   - Abra arquivo .ioc no STM32CubeMX
   - Verifique configuração de clock na aba "Clock Configuration"
   - Gere código se necessário

2. **Programação:**
   - Compile projeto
   - Flash para microcontrolador
   - Execute em modo debug para monitorar variável 'data'

3. **Verificação:**
   - LED2 deve ligar na inicialização
   - LED1 deve piscar a cada 1 segundo
   - Contador 'data' deve incrementar continuamente

## 📊 Análise de Clock

### Clock Tree STM32F407:
```
HSI (16MHz) → SYSCLK (16MHz) → AHB Prescaler (/1) → HCLK (16MHz)
                                                  ↓
                             APB1 Prescaler (/1) → PCLK1 (16MHz)
                             APB2 Prescaler (/1) → PCLK2 (16MHz)
```

### Medições Recomendadas:
- **Frequência LED1:** 0.5 Hz (2 segundos período completo)
- **Precisão SysTick:** ±1% (dependente do HSI)
- **Consumo:** ~8mA (modo normal, HSI)

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Clock externo:** Configurar HSE com cristal externo
2. **Frequência diferente:** Alterar delay para diferentes períodos
3. **Contador visível:** Usar LEDs para mostrar contagem binária

### Para Intermediários:
1. **PLL habilitado:** Configurar PLL para 84MHz ou 168MHz
2. **Prescalers:** Alterar divisores APB1/APB2
3. **Clock output:** Configurar MCO1/MCO2 para medição externa

### Para Avançados:
1. **LSE/LSI:** Configurar clocks de baixa velocidade
2. **Clock security:** Implementar CSS (Clock Security System)
3. **Dynamic clock:** Alterar frequência em runtime

## 🐛 Possíveis Problemas

### LED não pisca com período correto:
- Verificar configuração SysTick
- Confirmar clock source no CubeMX
- Medir com osciloscópio se disponível

### Sistema não inicializa:
- Verificar Error_Handler() em SystemClock_Config()
- Confirmar configuração de voltage scaling
- Testar com configuração mínima (só HSI)

### Drift temporal:
- HSI tem tolerância de ±1%
- Para precisão, usar HSE com cristal externo
- Considerar calibração periódica

## 🔍 Debug e Monitoramento

### Variáveis para Monitorar:
```c
// No debugger, observar:
uint8_t data;           // Contador principal
uint32_t SystemCoreClock; // Frequência atual do core
HAL_StatusTypeDef status; // Status das configurações
```

### Medições com Osciloscópio:
- **LED1 frequency:** Deve ser ~0.5Hz
- **SysTick:** 1kHz (se configurado MCO)
- **Jitter:** Deve ser mínimo com HSI

## 📈 Configurações Alternativas

### Clock de Alta Performance:
```c
// Para 168MHz com PLL:
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
RCC_OscInitStruct.HSEState = RCC_HSE_ON;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
RCC_OscInitStruct.PLL.PLLM = 8;
RCC_OscInitStruct.PLL.PLLN = 336;
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
RCC_OscInitStruct.PLL.PLLQ = 7;
```

### Clock de Baixo Consumo:
```c
// Para economia de energia:
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;  // Reduzir HCLK
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;    // Reduzir APB1
```

## 📚 Próximos Passos

Continue sua jornada com:
- **[Projeto06 - GPIO1](../Projeto06%20-%20GPIO1/):** GPIO com entrada e saída
- **[Projeto08 - Timers](../Projeto08%20-%20Timers/):** Uso de timers hardware
- **[Projeto09 - Timer](../Projeto09%20-%20Timer/):** Timer avançado

## 📖 Referências

- [STM32F4 Clock Tree](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4661: Clock configuration tool for STM32F4](https://www.st.com/resource/en/application_note/an4661-clock-configuration-tool-for-stm32f4xx-microcontrollers-stmicroelectronics.pdf)
- [Clock Configuration Best Practices](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_RCC)

---

⏰ **Dica:** O domínio do sistema de clock é fundamental! Ele afeta performance, consumo e precisão de todos os periféricos do STM32.