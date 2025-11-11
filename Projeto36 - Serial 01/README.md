# Projeto 36 - Serial 01

## Descrição do Projeto
Este projeto implementa comunicação serial básica usando UART (Universal Asynchronous Receiver-Transmitter) no microcontrolador STM32F407VET6. O sistema demonstra os fundamentos da comunicação serial síncrona, incluindo transmissão e recepção de dados em modo polling (bloqueante), configuração de baudrate e parâmetros de comunicação.

## 🎯 Objetivos de Aprendizado

- Configurar periférico UART
- Implementar transmissão serial
## Funcionalidades
- **UART Básica**: Configuração e uso da USART1 para comunicação serial
- **Modo Polling**: Transmissão e recepção bloqueante de dados
- **Echo Function**: Sistema de eco que retorna dados recebidos
- **Baudrate 9600**: Taxa de comunicação padrão para compatibilidade
- **8N1 Configuration**: 8 bits de dados, sem paridade, 1 stop bit
- **Buffer Management**: Controle básico de buffers de recepção

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- Conversor USB-Serial (FTDI, CP2102, CH340, etc.)
- Cabos jumper para conexões
- Terminal serial (PuTTY, Tera Term, Arduino IDE Serial Monitor)
- Computador com porta USB

## Configuração dos Pinos
| Pino STM32 | Função UART | Conversor USB-Serial |
|------------|-------------|----------------------|
| PA9 | USART1_TX | RX |
| PA10 | USART1_RX | TX |
| GND | Ground | GND |

## Análise Técnica

### Configuração UART
O projeto utiliza a USART1 configurada com:
- **Baudrate**: 9600 bps (bits por segundo)
- **Word Length**: 8 bits de dados
- **Stop Bits**: 1 bit de parada
- **Parity**: Nenhuma (UART_PARITY_NONE)
- **Flow Control**: Nenhum (UART_HWCONTROL_NONE)
- **Oversampling**: 16x para melhor precisão

### Cálculos de Baudrate
```
Clock USART1 = APB2 Clock = 16MHz (HSI)
Baudrate = 9600 bps
Oversampling = 16

USARTDIV = Clock / (Oversampling × Baudrate)
USARTDIV = 16000000 / (16 × 9600) = 104.1667

BRR Register = 104 (0x68)
Erro real ≈ 0.16% (aceitável para comunicação serial)
```

### Clock Configuration
```c
// Sistema usando HSI (16MHz)
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
RCC_OscInitStruct.HSIState = RCC_HSI_ON;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

```

## Código Principal

### Inicialização UART
```c
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}
```

### Loop Principal com Echo
```c
uint8_t recebe[10] = {0};

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    while (1)
    {
        // Recebe dados com timeout de 1000ms
        HAL_UART_Receive(&huart1, recebe, sizeof(recebe), 1000);
        
        // Retorna os dados recebidos (echo)
        HAL_UART_Transmit(&huart1, recebe, sizeof(recebe), 100);
        
        // Alternativa: envio de string fixa
        // HAL_UART_Transmit(&huart1, (const uint8_t *) "Joao Pedro", 10, 100);
        // HAL_Delay(1000);
    }
}
```

### Funções HAL UART Utilizadas

#### HAL_UART_Transmit
```c
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, 
                                   const uint8_t *pData, 
                                   uint16_t Size, 
                                   uint32_t Timeout);
```
- **huart**: Handle da UART
- **pData**: Ponteiro para dados a transmitir
- **Size**: Número de bytes a transmitir
- **Timeout**: Timeout em millisegundos

```c
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, 
                                  uint8_t *pData, 
                                  uint16_t Size, 
                                  uint32_t Timeout);
```
- **huart**: Handle da UART
- **pData**: Buffer para armazenar dados recebidos
- **Size**: Número máximo de bytes a receber
- **Timeout**: Timeout em millisegundos

## Aplicações Práticas
1. **Debug e Logs**: Envio de informações de debug via serial
2. **Comunicação com PC**: Interface entre microcontrolador e computador
3. **Configuração Remota**: Recebimento de comandos de configuração
4. **Telemetria**: Envio de dados de sensores para monitoramento
5. **Bootloader Serial**: Atualização de firmware via UART
6. **Comunicação com Módulos**: GPS, GSM, WiFi, Bluetooth
7. **Interface Humano-Máquina**: Terminal de comandos simples

## Vantagens da Comunicação Serial
- **Simplicidade**: Apenas 2 fios para comunicação full-duplex
- **Compatibilidade**: Padrão amplamente suportado
- **Distância**: Comunicação de média distância com RS232/RS485
- **Velocidade Ajustável**: Baudrate configurável conforme necessidade
- **Baixo Custo**: Hardware simples e barato
- **Confiabilidade**: Protocolo robusto e bem estabelecido

## Limitações do Modo Polling
- **Blocking Operation**: CPU fica bloqueada durante transmissão/recepção
- **Timeout Dependency**: Operação depende de timeout para completar
- **Ineficiência**: CPU não pode executar outras tarefas durante comunicação
- **Buffer Fixo**: Tamanho do buffer definido em tempo de compilação
## Troubleshooting

### Sem comunicação
- Verificar conexões TX/RX (cruzadas entre dispositivos)
- Confirmar baudrate idêntico em ambos os lados
- Testar continuidade dos cabos
- Verificar alimentação do conversor USB-Serial

### Caracteres corrompidos
- Verificar configuração de baudrate
- Confirmar parâmetros 8N1 em ambos os lados
- Testar com baudrate mais baixo (1200, 2400)
- Verificar clock do microcontrolador

### Timeout nas operações
- Aumentar valor de timeout
- Verificar se há dados sendo enviados
- Testar com buffer menor
- Verificar configuração do terminal

### Echo não funciona
- Confirmar recepção de dados no buffer
- Verificar tamanho do buffer de recepção
- Testar transmissão separadamente
- Verificar timeout de recepção

## Melhorias Sugeridas
1. **Modo Interrupt**: Implementar comunicação não-bloqueante
2. **DMA**: Usar DMA para transferências automáticas
3. **Circular Buffer**: Buffer circular para dados contínuos
4. **Flow Control**: Implementar RTS/CTS
5. **Error Handling**: Tratamento de erros de comunicação
6. **Printf Support**: Redirecionamento de printf para UART
7. **Command Parser**: Parser de comandos recebidos
8. **Higher Baudrates**: Testar velocidades maiores (115200, 921600)

## Configuração do Terminal
Para testar o projeto, configure seu terminal serial com:
- **Porta**: Conforme conversor USB-Serial
- **Baudrate**: 9600
- **Data bits**: 8
- **Parity**: None
- **Stop bits**: 1
- **Flow control**: None

### Exemplo PuTTY
1. Connection type: Serial
2. Serial line: COM3 (ou conforme sistema)
3. Speed: 9600
4. Connection → Serial:
   - Data bits: 8
   - Stop bits: 1
   - Parity: None
   - Flow control: None

## Testes Funcionais
1. **Teste de Echo**: Digite caracteres e verifique retorno
2. **Teste de Buffer**: Envie exatamente 10 caracteres
3. **Teste de Timeout**: Aguarde timeout de recepção
4. **Teste de Velocidade**: Meça taxa de transferência real
5. **Teste de Caracteres Especiais**: ASCII extendido e controle

## Recursos de Aprendizado
- **Conceitos**: UART, Serial Communication, Polling, Baudrate
- **Periféricos**: USART1, GPIO (PA9/PA10), RCC
- **HAL Functions**: HAL_UART_Init, HAL_UART_Transmit, HAL_UART_Receive
- **Ferramentas**: Terminal Serial, Conversor USB-Serial
- **Protocolos**: RS232, Comunicação Assíncrona

Este projeto estabelece a base fundamental para comunicação serial em projetos STM32, sendo o primeiro passo para implementações mais avançadas com interrupções e DMA.

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