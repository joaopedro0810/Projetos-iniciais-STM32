# Projeto07 - GPIO Interrupt ⚡

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-GPIO%20EXTI-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-35%20min-orange.svg)

## 📋 Descrição

Este projeto introduz o conceito de **interrupções GPIO** através do sistema EXTI (External Interrupt). Substitui o polling de botões por um sistema baseado em eventos, demonstrando programação orientada a interrupções - um conceito fundamental em sistemas embarcados.

## 🎯 Objetivos de Aprendizado

- Compreender sistema de interrupções EXTI
- Configurar GPIO para gerar interrupções
- Implementar callbacks de interrupção
- Diferença entre polling e interrupt-driven
- Trabalhar com prioridades de interrupção
- Programação assíncrona em microcontroladores

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 2 LEDs externos (led0, led1)
- **Botões:** 2 botões (k_up, k1)
- **Resistores:** 2x 330Ω (LEDs) + 2x 10kΩ (pull-up botões)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
led0_Pin  -----> |+| LED0 --|> GND
                      |
                    330Ω

led1_Pin  -----> |+| LED1 --|> GND
                      |
                    330Ω

k_up_Pin  <----- |  BTN1  |  (EXTI0)
(Pin 0)           |        |
                 10kΩ     GND
                  |
                 VCC

k1_Pin    <----- |  BTN2  |  (EXTI3)
(Pin 3)           |        |
                 10kΩ     GND
                  |
                 VCC
```

### Configuração de Pinos
- **led0_Pin:** GPIO_Output (LED controlado por k_up)
- **led1_Pin:** GPIO_Output (LED controlado por k1)
- **k_up_Pin:** GPIO_EXTI0 (Interrupção externa linha 0)
- **k1_Pin:** GPIO_EXTI3 (Interrupção externa linha 3)

## 💻 Principais Conceitos

### 1. Configuração EXTI
```c
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configurar LEDs (saída)
    HAL_GPIO_WritePin(led0_GPIO_Port, led0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_RESET);
    
    GPIO_InitStruct.Pin = led0_Pin | led1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configurar botões com interrupção
    GPIO_InitStruct.Pin = k_up_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Falling edge trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(k_up_GPIO_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = k1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Falling edge trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(k1_GPIO_Port, &GPIO_InitStruct);
    
    // Habilitar interrupções NVIC
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    
    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}
```

### 2. Handlers de Interrupção
```c
// Em stm32f4xx_it.c
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(k_up_Pin);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(k1_Pin);
}
```

### 3. Callback de Interrupção
```c
// Em main.c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == k_up_Pin)
    {
        HAL_GPIO_TogglePin(led0_GPIO_Port, led0_Pin);
    }
    else if (GPIO_Pin == k1_Pin)
    {
        HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
    }
}
```

### 4. Loop Principal Minimalista
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    // Loop principal vazio - tudo é tratado por interrupções!
    while (1)
    {
        // Microcontrolador em idle, aguardando interrupções
        // Pode entrar em modo de baixo consumo aqui
    }
}
```

## 🔍 Funcionalidades

### Sistema Baseado em Eventos:
- **k_up pressionado:** LED0 alterna estado instantaneamente
- **k1 pressionado:** LED1 alterna estado instantaneamente  
- **Resposta imediata:** Latência <10μs
- **Baixo consumo:** CPU idle quando não há eventos

### Características EXTI:
- **Trigger:** Falling edge (HIGH → LOW)
- **Debounce:** Hardware automático
- **Prioridade:** 0 (máxima)
- **Nested:** Interrupções podem ser aninhadas

## ⚙️ Configuração STM32CubeMX

### EXTI Configuration:
| Line | GPIO Pin | Mode | Trigger | Pull | IRQ |
|------|----------|------|---------|------|-----|
| EXTI0 | k_up_Pin | External Interrupt | Falling | Pull-up | EXTI0_IRQn |
| EXTI3 | k1_Pin | External Interrupt | Falling | Pull-up | EXTI3_IRQn |

### NVIC Configuration:
| IRQ | Priority | Sub Priority | Enable |
|-----|----------|--------------|--------|
| EXTI0_IRQn | 0 | 0 | ✓ |
| EXTI3_IRQn | 0 | 0 | ✓ |

### GPIO Settings:
| Pin | Mode | Trigger | Pull | User Label |
|-----|------|---------|------|------------|
| PA0 | External Interrupt | Falling | Pull-up | k_up |
| PA3 | External Interrupt | Falling | Pull-up | k1 |

## 🚀 Como Executar

1. **Configuração no CubeMX:**
   - Configure pinos como External Interrupt Mode
   - Set trigger to Falling Edge
   - Enable NVIC interrupts
   - Generate code

2. **Programação:**
   - Implemente HAL_GPIO_EXTI_Callback()
   - Compile e flash
   - Teste resposta dos botões

3. **Validação:**
   - Pressione k_up → LED0 deve alternar
   - Pressione k1 → LED1 deve alternar
   - Resposta deve ser instantânea

## 📊 Análise de Performance

### Comparação Polling vs Interrupt:

| Aspecto | Polling | Interrupt |
|---------|---------|-----------|
| **Latência** | 1-1000ms | <10μs |
| **CPU Usage** | 100% | <1% |
| **Consumo** | Alto | Baixo |
| **Complexidade** | Simples | Média |
| **Debounce** | Software | Hardware |

### Timing de Interrupção:
```
Botão pressionado → Hardware detecta edge → IRQ gerada
    ↓
NVIC processa → CPU salva contexto → Chama handler
    ↓  
Handler executa → HAL_GPIO_EXTI_IRQHandler → Callback
    ↓
LED alterna → Retorna → Restaura contexto
```

**Tempo total:** ~5-10μs

## 🔧 Modificações Sugeridas

### Para Iniciários:
1. **Rising edge:** Configurar trigger na subida
2. **Both edges:** Trigger em ambas as bordas
3. **Contador:** Contar pressões de botão

### Para Intermediários:
1. **Prioridades:** Diferentes prioridades para cada IRQ
2. **Debounce software:** Filtro adicional no callback
3. **Long press:** Detectar pressão longa

### Para Avançados:
1. **Nested interrupts:** Interrupções aninhadas
2. **Wake from sleep:** Acordar MCU de low-power
3. **DMA trigger:** Usar EXTI para trigger DMA

## 🐛 Possíveis Problemas

### Interrupção não dispara:
- Verificar configuração EXTI no CubeMX
- Confirmar NVIC habilitado
- Testar com multímetro se há transição

### Múltiplas interrupções por pressão:
- **Bouncing:** Botão gera múltiplos pulsos
- **Solução:** Debounce por software ou hardware
- Usar capacitor 100nF em paralelo com botão

### Sistema trava:
- Verificar se callback está executando corretamente
- Evitar delays longos dentro da interrupção
- Verificar stack overflow

## 🔍 Debug de Interrupções

### Técnicas de Debug:
```c
volatile uint32_t interrupt_count = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    interrupt_count++;  // Contador para debug
    
    if (GPIO_Pin == k_up_Pin)
    {
        HAL_GPIO_TogglePin(led0_GPIO_Port, led0_Pin);
    }
}
```

### Ferramentas:
- **Debugger:** Breakpoint no callback
- **Logic analyzer:** Analisar timing
- **Osciloscópio:** Verificar bouncing

## 📈 Otimizações

### Callback Otimizado:
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // Lookup table para otimizar
    switch(GPIO_Pin)
    {
        case k_up_Pin:
            HAL_GPIO_TogglePin(led0_GPIO_Port, led0_Pin);
            break;
        case k1_Pin:
            HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
            break;
        default:
            break;
    }
}
```

## 📚 Próximos Passos

Continue aprendendo sobre sistemas orientados a eventos:
- **[Projeto08 - Timers](../Projeto08%20-%20Timers/):** Timer hardware básico
- **[Projeto10 - Timer Interrupt](../Projeto10%20-%20Timer%20Interrupt/):** Timer com interrupção
- **[Projeto41 - RTOS 01](../Projeto41%20-%20RTOS%2001/):** Sistemas em tempo real

## 📖 Referências

- [STM32F4 EXTI Reference](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [ARM Cortex-M4 Interrupt Handling](https://developer.arm.com/documentation/dui0553/a/handling-processor-exceptions/interrupt-handlers)
- [STM32 NVIC Programming](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)

---

⚡ **Dica:** Interrupções são a alma dos sistemas embarcados! Domine este conceito para criar sistemas responsivos e eficientes.