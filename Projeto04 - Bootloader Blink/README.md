# Projeto04 - Bootloader Blink 🔴

![Dificuldade](https://img.shields.io/badge/Dificuldade-Básico-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-GPIO-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-20%20min-orange.svg)

## 📋 Descrição

Este projeto demonstra um sistema de LED piscante duplo, funcionando como uma evolução do projeto anterior. Dois LEDs piscam simultaneamente, criando um padrão visual que pode ser usado em sistemas de inicialização (bootloader) ou sinalização de status.

## 🎯 Objetivos de Aprendizado

- Controlar múltiplos LEDs simultaneamente
- Trabalhar com nomenclatura específica de pinos (led1, led2)
- Implementar padrões de sinalização visual
- Compreender aplicações em sistemas de boot
- Usar GPIO para múltiplas saídas

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LEDs:** 2 LEDs externos (vermelho e verde recomendados)
- **Resistores:** 2x 330Ω (limitadores de corrente)
- **Protoboard e jumpers**

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Hardware
================================
led1_Pin  -----> |+| LED1 (Red) --|> GND
                      |
                    330Ω

led2_Pin  -----> |+| LED2 (Green) --|> GND  
                      |
                    330Ω
```

### Configuração de Pinos
- **led1_Pin:** GPIO_Output (LED1 - Status principal)
- **led2_Pin:** GPIO_Output (LED2 - Status secundário)

## 💻 Principais Conceitos

### 1. Controle Simultâneo de LEDs
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    while (1)
    {
        // Alternar ambos os LEDs simultaneamente
        HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
        HAL_GPIO_TogglePin(led2_GPIO_Port, led2_Pin);
        
        HAL_Delay(500);  // Período de 500ms
    }
}
```

### 2. Configuração de Múltiplos GPIOs
```c
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Habilitar clocks dos GPIOs
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configurar ambos os LEDs
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_RESET);
    
    // Configuração idêntica para ambos
    GPIO_InitStruct.Pin = led1_Pin | led2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
```

## 🔍 Funcionalidades

### Comportamento do Sistema:
- **Inicialização:** Ambos os LEDs iniciam apagados
- **Loop principal:** LEDs alternam estado simultaneamente a cada 500ms
- **Padrão:** ON → OFF → ON → OFF (ciclo contínuo)
- **Frequência:** 1Hz (2 mudanças por segundo)

### Aplicações Práticas:
- **Bootloader indicator:** Sinaliza processo de inicialização
- **Status duplo:** Dois estados diferentes do sistema
- **Heartbeat:** Indicação de sistema funcionando
- **Debug visual:** Verificação de funcionamento

## ⚙️ Configuração STM32CubeMX

### GPIO Configuration:
| Pino | Label | Mode | Pull | Speed | Initial State |
|------|--------|------|------|-------|---------------|
| PAx  | led1   | Output Push Pull | No Pull | Low | Reset |
| PAy  | led2   | Output Push Pull | No Pull | Low | Reset |

### Clock Configuration:
- **System Clock:** 16 MHz (HSI default)
- **AHB:** 16 MHz
- **APB1/APB2:** 16 MHz

## 🚀 Como Executar

1. **Hardware Setup:**
   - Conecte LED1 (vermelho) ao pino led1
   - Conecte LED2 (verde) ao pino led2
   - Use resistores limitadores de corrente

2. **Software:**
   - Abra projeto no STM32CubeIDE
   - Compile (Ctrl+B)
   - Execute/Debug (F11)

3. **Comportamento Esperado:**
   - Ambos os LEDs piscam juntos
   - Período de 500ms ligado, 500ms desligado
   - Padrão contínuo e sincronizado

## 📊 Análise Temporal

### Timing do Sistema:
```
Tempo:  0ms   500ms  1000ms  1500ms  2000ms
LED1:   OFF    ON     OFF     ON      OFF
LED2:   OFF    ON     OFF     ON      OFF
Status: ▪▪     ▪▪     ▪▪      ▪▪      ▪▪
```

### Características:
- **Duty Cycle:** 50%
- **Frequência:** 1Hz
- **Sincronização:** Perfeita entre LEDs
- **Consumo:** ~30mA (ambos LEDs ligados)

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Frequências diferentes:** Um LED mais rápido que outro
2. **Padrão alternado:** Quando um liga, outro desliga
3. **Sequência:** LED1 → LED2 → ambos → apagados

### Para Intermediários:
1. **Controle por botão:** Iniciar/parar com botão
2. **Velocidade variável:** Alterar período dinamicamente
3. **Mais LEDs:** Adicionar LED3, LED4 em sequência

### Para Avançados:
1. **Timer-based:** Usar timer em vez de HAL_Delay()
2. **Interrupção:** Controle via interrupção externa
3. **Estado máquina:** Múltiplos padrões de piscada

## 🐛 Possíveis Problemas

### LEDs não piscam:
- Verificar configuração dos pinos no CubeMX
- Confirmar labels (led1, led2) nos pinos
- Verificar conexões físicas e resistores

### Apenas um LED funciona:
- Verificar configuração GPIO de ambos os pinos
- Testar continuidade dos circuitos
- Verificar se ambos os LEDs estão funcionais

### Frequência incorreta:
- Verificar valor do HAL_Delay(500)
- Confirmar clock do sistema
- Verificar se há delays adicionais no código

## 🎨 Variações de Padrão

### Código para Padrão Alternado:
```c
while (1)
{
    // LED1 ligado, LED2 desligado
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
    
    // LED1 desligado, LED2 ligado
    HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
}
```

## 📚 Próximos Passos

Após dominar este projeto, continue com:
- **[Projeto05 - Clock1](../Projeto05%20-%20Clock1/):** Configuração avançada de clock
- **[Projeto06 - GPIO1](../Projeto06%20-%20GPIO1/):** GPIO com entrada (botões)
- **[Projeto07 - GPIO Interrupt](../Projeto07%20-%20GPIO%20Interrupt/):** Interrupções de GPIO

## 📖 Referências

- [STM32F4 GPIO Documentation](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [LED Control Best Practices](https://www.electronics-tutorials.ws/blog/led-resistor-calculator.html)
- [STM32 Bootloader Guide](https://www.st.com/resource/en/application_note/an2606-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf)

---

🔴🟢 **Dica:** Este projeto é ideal para entender sistemas de sinalização visual e pode ser facilmente adaptado para indicadores de status em projetos maiores!