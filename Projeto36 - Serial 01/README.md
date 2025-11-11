# Projeto36 - Serial 01 📡

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-UART-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-30%20min-orange.svg)

## 📋 Descrição

Este projeto introduz a comunicação serial através do protocolo UART. Demonstra como enviar e receber dados através da porta serial, estabelecendo comunicação entre o microcontrolador e um terminal no PC.

## 🎯 Objetivos de Aprendizado

- Configurar periférico UART
- Implementar transmissão serial
- Trabalhar com strings e dados
- Usar terminal serial para debug
- Compreender protocolos de comunicação assíncrona

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Conversor USB-Serial:** FTDI, CH340 ou similar
- **Jumpers:** Para conexões
- **Terminal Serial:** PuTTY, TeraTerm ou serial monitor do Arduino IDE

## 📐 Esquema de Ligação

```
STM32F407VET6    |    Conversor USB-Serial
==========================================
PA9 (USART1_TX) ----> RX
PA10(USART1_RX) ----> TX
GND             ----> GND
```

### Configuração de Pinos
- **PA9:** USART1_TX (Transmissão)
- **PA10:** USART1_RX (Recepção)

## 💻 Principais Conceitos

### 1. Configuração UART
```c
UART_HandleTypeDef huart1;

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}
```

### 2. Transmissão de Dados
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    
    char mensagem[] = "Olá do STM32!\r\n";
    uint8_t contador = 0;
    
    while (1)
    {
        // Enviar mensagem
        HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, strlen(mensagem), HAL_MAX_DELAY);
        
        // Enviar contador
        char buffer[50];
        sprintf(buffer, "Contador: %d\r\n", contador++);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        
        HAL_Delay(1000);
    }
}
```

### 3. Recepção de Dados
```c
uint8_t dadoRecebido;

// No loop principal
if (HAL_UART_Receive(&huart1, &dadoRecebido, 1, 100) == HAL_OK)
{
    // Eco - reenviar o dado recebido
    HAL_UART_Transmit(&huart1, &dadoRecebido, 1, HAL_MAX_DELAY);
}
```

## 🔍 Funcionalidades

### Funções HAL Utilizadas:
- `HAL_UART_Init()` - Inicializa UART
- `HAL_UART_Transmit()` - Transmite dados
- `HAL_UART_Receive()` - Recebe dados
- `sprintf()` - Formatação de strings

### Parâmetros UART:
- **Baud Rate:** 115200 bps
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

## ⚙️ Configuração STM32CubeMX

### UART Configuration:
| Parâmetro | Valor |
|-----------|-------|
| Baud Rate | 115200 |
| Word Length | 8 Bits |
| Parity | None |
| Stop Bits | 1 |
| Data Direction | Receive and Transmit |
| Over Sampling | 16 Samples |

### Clock Configuration:
- **APB2 Timer clocks:** 84 MHz
- **USART1 Clock:** 84 MHz

### GPIO Configuration:
| Pino | Função | Mode | Pull | Speed |
|------|--------|------|------|-------|
| PA9  | USART1_TX | Alternate Function | Pull-up | High |
| PA10 | USART1_RX | Alternate Function | Pull-up | High |

## 🚀 Como Executar

1. **Hardware:**
   - Conecte o conversor USB-Serial conforme esquema
   - Conecte o STM32 ao PC via ST-Link

2. **Software:**
   - Abra terminal serial (115200, 8N1)
   - Compile e execute o projeto
   - Observe mensagens no terminal

3. **Teste de Recepção:**
   - Digite caracteres no terminal
   - Verifique eco dos caracteres

## 📊 Análise de Performance

### Throughput:
- **Taxa máxima teórica:** 115200 bps
- **Taxa efetiva:** ~95% (considerando overhead)
- **Latência:** ~87μs por byte

### Recursos Utilizados:
- **RAM:** ~100 bytes (buffers)
- **Flash:** ~2KB (código UART)
- **CPU:** ~1% (em 115200 bps)

## 🔧 Modificações Sugeridas

### Para Iniciantes:
1. **Alterar Baud Rate:** Teste diferentes velocidades
2. **Mensagens personalizadas:** Mude o conteúdo transmitido
3. **LED de status:** Indique transmissão com LED

### Para Intermediários:
1. **Buffer circular:** Implemente buffer para recepção
2. **Protocolo simples:** Crie comandos específicos
3. **Múltiplas UARTs:** Use USART2, USART3

### Para Avançados:
1. **DMA:** Use DMA para transmissão/recepção
2. **Interrupções:** Implemente comunicação por interrupção
3. **Flow Control:** Adicione controle de fluxo RTS/CTS

## 🐛 Possíveis Problemas

### Não recebe dados no terminal:
- Verificar baud rate (deve ser igual em ambos os lados)
- Confirmar ligação TX/RX (cruzada)
- Verificar GND comum

### Caracteres corrompidos:
- Clock incorreto do UART
- Interferência nos cabos
- Configuração de paridade errada

### Travamento na transmissão:
- Timeout muito baixo
- Problema no clock do periférico
- Buffer overflow

## 🔍 Debug e Monitoramento

### Ferramentas Recomendadas:
- **PuTTY:** Terminal gratuito e confiável
- **TeraTerm:** Alternativa com macros
- **CoolTerm:** Interface gráfica intuitiva

### Configurações do Terminal:
```
Baud Rate: 115200
Data Bits: 8
Parity: None
Stop Bits: 1
Flow Control: None
```

## 📚 Próximos Passos

Projetos recomendados após este:
- **[Projeto37 - Serial 02](../Projeto37%20-%20Serial%2002/):** UART com interrupções
- **[Projeto38 - Serial 03](../Projeto38%20-%20Serial%2003/):** UART com DMA
- **[Projeto39 - I2C ADXL345](../Projeto39%20-%20I2C%20ADXL345/):** Comunicação I2C

## 📖 Referências

- [UART Protocol Overview](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
- [STM32F4 UART Documentation](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Serial Communication Tutorial](https://www.sparkfun.com/tutorials/8)

---

💡 **Dica:** A comunicação serial é fundamental para debug. Domine este projeto antes de prosseguir para protocolos mais complexos!