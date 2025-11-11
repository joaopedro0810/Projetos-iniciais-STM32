# Projeto06 - GPIO1 🔘

![Dificuldade](https://img.shields.io/badge/Dificuldade-Básico-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-GPIO%20Input%2FOutput-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-30%20min-orange.svg)

## 📋 Descrição

Este projeto introduz o conceito de entrada digital (input) combinado com saída (output). Demonstra controle de LEDs através de botões, implementando leitura de estado de pinos GPIO e resposta interativa do sistema.

## 🎯 Objetivos de Aprendizado

- Configurar GPIO como entrada e saída
- Implementar leitura de botões (polling)
- Trabalhar com pull-up/pull-down resistors
- Debouncing básico de botões
- Controle interativo de LEDs
- Lógica condicional em sistemas embarcados

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 2 LEDs externos (led0, led1)  
- **Botões:** 1 botão (k_up)
- **Resistores:** 2x 330Ω (LEDs) + 1x 10kΩ (pull-up botão)
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

k_up_Pin  <----- |   BTN   |
                      |     |
                    10kΩ   GND
                      |
                    VCC
```

### Configuração de Pinos
- **led0_Pin:** GPIO_Output (LED principal)
- **led1_Pin:** GPIO_Output (LED secundário)  
- **k_up_Pin:** GPIO_Input (Botão com pull-up)

## 💻 Principais Conceitos

### 1. Configuração Mista GPIO
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
    
    // Configurar botão (entrada)
    GPIO_InitStruct.Pin = k_up_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // Pull-up interno
    HAL_GPIO_Init(k_up_GPIO_Port, &GPIO_InitStruct);
}
```

### 2. Lógica de Controle Principal
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    while (1)
    {
        // LEDs piscam continuamente
        HAL_GPIO_TogglePin(led0_GPIO_Port, led0_Pin);
        HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
        
        // Verificar estado do botão
        if (HAL_GPIO_ReadPin(k_up_GPIO_Port, k_up_Pin) == GPIO_PIN_SET)
        {
            HAL_Delay(200);  // Delay quando botão pressionado
        }
        else
        {
            HAL_Delay(1000); // Delay padrão quando botão não pressionado
        }
    }
}
```

## 🔍 Funcionalidades

### Comportamento Interativo:
- **Estado normal:** LEDs piscam a cada 1 segundo
- **Botão pressionado:** LEDs piscam mais rápido (200ms)
- **Debouncing:** Evita múltiplas leituras
- **Polling contínuo:** Verifica botão a cada ciclo

### Lógica do Sistema:
```
Botão Solto (HIGH)  → Delay 1000ms → LEDs piscam devagar
Botão Pressionado (LOW) → Delay 200ms  → LEDs piscam rápido
```

## ⚙️ Configuração STM32CubeMX

### GPIO Configuration:
| Pino | Label | Mode | Pull | Speed | Descrição |
|------|--------|------|------|-------|-----------|
| PAx | led0 | Output Push Pull | No Pull | Low | LED principal |
| PAy | led1 | Output Push Pull | No Pull | Low | LED secundário |
| PAz | k_up | Input | Pull-up | - | Botão controle |

### Características do Pull-up:
- **Resistor interno:** ~40kΩ
- **Estado padrão:** HIGH (3.3V)
- **Botão pressionado:** LOW (0V - GND)

## 🚀 Como Executar

1. **Montagem do Hardware:**
   - Conecte LEDs com resistores limitadores
   - Conecte botão entre pino e GND
   - Configure pull-up interno no CubeMX

2. **Teste do Sistema:**
   - LEDs devem piscar lentamente (1s) inicialmente
   - Pressione e mantenha botão
   - LEDs devem piscar rapidamente (200ms)
   - Solte botão - volta ao padrão lento

3. **Debug:**
   - Use debugger para monitorar estado do botão
   - Observe variável de estado em tempo real

## 📊 Análise de Comportamento

### Estados do Sistema:
```
Estado      | Botão | Delay | Freq LEDs | Descrição
------------|-------|-------|-----------|------------------
NORMAL      | HIGH  | 1000ms| 0.5 Hz   | Operação padrão
FAST_BLINK  | LOW   | 200ms | 2.5 Hz   | Botão pressionado
```

### Consumo de Corrente:
- **LEDs OFF:** ~5mA (microcontrolador)
- **LEDs ON:** ~35mA (LEDs + MCU)
- **Botão:** Desprezível (<1μA)

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Botão liga/desliga:** Pressionar para ligar/desligar LEDs
2. **Contagem de pressões:** Contar quantas vezes foi pressionado
3. **LED de status do botão:** LED específico para indicar botão

### Para Intermediários:
1. **Múltiplos botões:** Adicionar mais botões com funções diferentes
2. **Debouncing por software:** Implementar filtro de debounce
3. **Estados múltiplos:** Diferentes padrões de piscar

### Para Avançados:
1. **Máquina de estados:** Implementar FSM (Finite State Machine)
2. **Interrupção:** Usar EXTI em vez de polling
3. **Long press:** Detectar pressão longa vs. curta

## 🐛 Possíveis Problemas

### Botão não funciona:
- Verificar configuração pull-up
- Confirmar conexão do botão ao GND
- Testar continuidade elétrica

### Comportamento errático:
- **Bouncing:** Botão pode "quicar" eletricamente
- **Solução:** Adicionar capacitor (100nF) ou debounce software
- Verificar qualidade do botão

### LEDs não respondem:
- Verificar lógica condicional no código
- Confirmar leitura correta do pino
- Testar com voltímetro estado do pino

## 🔍 Debug e Medições

### Variáveis para Monitorar:
```c
GPIO_PinState buttonState = HAL_GPIO_ReadPin(k_up_GPIO_Port, k_up_Pin);
uint32_t pressCount = 0;  // Contador de pressões
```

### Testes Recomendados:
- **Voltagem botão solto:** ~3.3V
- **Voltagem botão pressionado:** ~0V  
- **Tempo de response:** <10ms
- **Debounce necessário:** Depende do botão

## 📈 Melhorias de Debouncing

### Debounce por Software:
```c
#define DEBOUNCE_DELAY 50  // 50ms

uint32_t lastButtonTime = 0;
GPIO_PinState lastButtonState = GPIO_PIN_SET;

// No loop principal:
uint32_t currentTime = HAL_GetTick();
GPIO_PinState currentButtonState = HAL_GPIO_ReadPin(k_up_GPIO_Port, k_up_Pin);

if (currentButtonState != lastButtonState) 
{
    if ((currentTime - lastButtonTime) > DEBOUNCE_DELAY) 
    {
        // Botão mudou de estado validamente
        lastButtonState = currentButtonState;
        lastButtonTime = currentTime;
    }
}
```

## 🎯 Aplicações Práticas

### Casos de Uso:
- **Interface usuário:** Botões de controle
- **Sistemas de menu:** Navegação simples
- **Controle industrial:** Start/Stop de processos
- **Debug:** Trigger manual para testes

## 📚 Próximos Passos

Continue aprendendo com:
- **[Projeto07 - GPIO Interrupt](../Projeto07%20-%20GPIO%20Interrupt/):** Interrupções de GPIO
- **[Projeto08 - Timers](../Projeto08%20-%20Timers/):** Controle temporal preciso
- **[Projeto10 - Timer Interrupt](../Projeto10%20-%20Timer%20Interrupt/):** Combinação timer + interrupção

## 📖 Referências

- [STM32F4 GPIO Reference Manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Button Debouncing Techniques](https://www.ganssle.com/debouncing.htm)
- [Pull-up/Pull-down Resistors Guide](https://learn.sparkfun.com/tutorials/pull-up-resistors)

---

🔘 **Dica:** Domine entrada e saída GPIO - é a base para interfaces humano-máquina em sistemas embarcados!