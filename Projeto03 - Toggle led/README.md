# Projeto03 - Toggle LED 💡

![Dificuldade](https://img.shields.io/badge/Dificuldade-Básico-green.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-GPIO-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-15%20min-orange.svg)

## 📋 Descrição

Este projeto demonstra o controle básico de GPIO para fazer um LED piscar. É um dos primeiros projetos para aprender manipulação de pinos digitais no STM32, utilizando a função `HAL_GPIO_TogglePin()`.

## 🎯 Objetivos de Aprendizado

- Configurar GPIO como saída digital
- Utilizar funções de delay
- Implementar um loop infinito
- Compreender a estrutura básica de um programa STM32

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **LED:** LED externo conectado ao pino ou LED onboard
- **Resistor:** 330Ω (se usar LED externo)
- **Protoboard e jumpers** (opcional)

## 📐 Esquema de Ligação

```
STM32F407VET6    |    LED
===============================
PA0    --------> |+| LED --|> GND
                      |
                    330Ω
```

### Configuração de Pinos
- **PA0:** GPIO_Output (LED)

## 💻 Principais Conceitos

### 1. Configuração GPIO
```c
// Configuração automática via CubeMX
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
```

### 2. Loop Principal
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);  // Inverte estado do LED
        HAL_Delay(500);                         // Delay de 500ms
    }
}
```

## 🔍 Funcionalidades

### Funções HAL Utilizadas:
- `HAL_GPIO_TogglePin()` - Alterna estado do pino
- `HAL_Delay()` - Delay em milissegundos
- `HAL_GPIO_WritePin()` - Escreve estado específico no pino

### Comportamento Esperado:
- LED pisca a cada 500ms
- Estado HIGH por 500ms, depois LOW por 500ms
- Ciclo se repete infinitamente

## ⚙️ Configuração STM32CubeMX

### Clock Configuration:
- **System Clock:** 168 MHz (default)
- **AHB:** 168 MHz
- **APB1:** 42 MHz
- **APB2:** 84 MHz

### GPIO Configuration:
| Pino | Função | Mode | Pull | Speed |
|------|--------|------|------|-------|
| PA0  | GPIO_Output | Output Push Pull | No Pull | Low |

## 🚀 Como Executar

1. **Abra o projeto no STM32CubeIDE**
2. **Conecte a placa STM32F407VET6**
3. **Compile o projeto** (Ctrl+B)
4. **Execute/Debug** (F11)
5. **Observe o LED piscando**

## 📊 Análise do Consumo

- **Corrente típica:** ~15mA (com LED ligado)
- **Frequência de chaveamento:** 1Hz
- **Duty cycle:** 50%

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Alterar frequência:** Mude o valor do delay
2. **Múltiplos LEDs:** Adicione mais pinos GPIO
3. **Padrões diferentes:** Implemente sequências específicas

### Para Avançados:
1. **Usar Timer:** Substitua `HAL_Delay()` por timer
2. **Interrupções:** Controle via botão externo
3. **PWM:** Varie intensidade do LED

## 🐛 Possíveis Problemas

### LED não pisca:
- Verificar conexões de hardware
- Confirmar configuração do pino no CubeMX
- Verificar se o clock do GPIO está habilitado

### Frequência incorreta:
- Verificar valor do delay
- Confirmar configuração do SysTick

## 📚 Próximos Passos

Após dominar este projeto, continue com:
- **[Projeto04 - Bootloader Blink](../Projeto04%20-%20Bootloader%20Blink/):** LED com configuração de bootloader
- **[Projeto05 - Clock1](../Projeto05%20-%20Clock1/):** Configuração avançada de clock
- **[Projeto06 - GPIO1](../Projeto06%20-%20GPIO1/):** Manipulação avançada de GPIO

## 📖 Referências

- [STM32F4 HAL Documentation](https://www.st.com/resource/en/user_manual/um1725-description-of-stm32f4-hal-and-lowlayer-drivers-stmicroelectronics.pdf)
- [STM32F407 Datasheet](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf)
- [GPIO Configuration Guide](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_GPIO)

---

💡 **Dica:** Este é um projeto fundamental! Certifique-se de entender completamente antes de avançar para projetos mais complexos.