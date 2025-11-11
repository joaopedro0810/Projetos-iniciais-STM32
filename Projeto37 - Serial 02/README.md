# Projeto 37 - Serial 02

## Descrição do Projeto
Este projeto implementa comunicação serial avançada usando UART com interrupções no microcontrolador STM32F407VET6. O sistema demonstra comunicação não-bloqueante através de callbacks de interrupção, permitindo que o microcontrolador execute outras tarefas enquanto aguarda dados seriais, com processamento de dados numéricos e conversão de strings.

## Funcionalidades
- **UART com Interrupção**: Comunicação serial não-bloqueante usando HAL_UART_Receive_IT()
- **Callback Processing**: Processamento de dados através de HAL_UART_RxCpltCallback()
- **String to Number**: Conversão de strings recebidas para números inteiros usando atol()
- **Counter System**: Sistema de contagem de recepções para monitoramento
- **Buffer Management**: Gerenciamento automático de buffer de recepção
- **Non-blocking Operation**: CPU livre para outras tarefas durante comunicação

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

### Configuração UART com Interrupções
O projeto utiliza a USART1 com as mesmas configurações básicas do Projeto 36, mas implementando comunicação por interrupção:
- **Baudrate**: 9600 bps (configuração padrão)
- **Mode**: UART_MODE_TX_RX (transmissão e recepção)
- **Interrupt Mode**: HAL_UART_Receive_IT() para recepção não-bloqueante
- **NVIC**: Interrupção USART1 habilitada automaticamente pelo HAL

### Sistema de Interrupção UART
```c
// Configuração automática da interrupção no HAL_UART_Init()
// NVIC_EnableIRQ(USART1_IRQn) é chamado internamente
// Priority configurada automaticamente pelo CubeMX
```

### Buffer e Variáveis Globais
```c
uint16_t x = 0;          // Contador de recepções
long int dados = 0;      // Dados convertidos para inteiro
uint8_t buffer[4] = {0}; // Buffer de recepção (4 bytes)
```

## Código Principal

### Loop Principal Simplificado
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    while (1)
    {
        // Inicia recepção por interrupção
        HAL_UART_Receive_IT(&huart1, buffer, sizeof(buffer));
        
        // CPU livre para outras tarefas
        // Não há bloqueio esperando dados
    }
}
```

### Callback de Recepção Completa
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        x++;  // Incrementa contador de recepções
        
        // Converte string recebida para número inteiro
        dados = atol((const char *) buffer);
        
        // Aqui poderia processar os dados recebidos
        // Exemplo: transmitir de volta, armazenar, etc.
    }
}
```

### Função atol() para Conversão
A função `atol()` (ASCII to Long) converte uma string para um número inteiro longo:
```c
// Exemplo de conversões:
// Buffer: "1234" → dados = 1234
// Buffer: "0567" → dados = 567
// Buffer: "9999" → dados = 9999
// Buffer: "abcd" → dados = 0 (caracteres inválidos)
```

## Vantagens do Modo Interrupt

### 1. **Não-bloqueante**
- CPU não fica parada esperando dados
- Permite multitarefa mesmo sem RTOS
- Melhor eficiência energética

### 2. **Responsividade**
- Resposta imediata aos dados recebidos
- Processamento em tempo real
- Sem perda de dados por timeout

### 3. **Flexibilidade**
- Buffer de tamanho configurável
- Processamento customizado no callback
- Integração fácil com outras funcionalidades

## Aplicações Práticas
1. **Sistemas de Comando**: Recepção de comandos numéricos remotos
2. **Aquisição de Dados**: Coleta de valores de sensores externos
3. **Controle Remoto**: Recepção de parâmetros de configuração
4. **Monitoramento**: Recepção de dados de telemetria
5. **Interface Homem-Máquina**: Terminal de comandos interativo
6. **Datalogger**: Recepção e armazenamento de dados
7. **Sistemas de Calibração**: Recepção de valores de ajuste

## Fluxo de Funcionamento

### 1. **Inicialização**
```
1. Sistema inicializado (HAL_Init, Clock, GPIO, UART)
2. Interrupção USART1 habilitada automaticamente
3. Buffer de recepção preparado
```

### 2. **Operação Normal**
```
1. HAL_UART_Receive_IT() inicia recepção
2. CPU executa outras tarefas no loop principal
3. Dados chegam pela UART
4. Interrupção USART1_IRQHandler é chamada
5. HAL processa interrupção internamente
6. HAL_UART_RxCpltCallback() é executado
7. Dados processados e convertidos
8. Loop reinicia com nova chamada HAL_UART_Receive_IT()
```

### 3. **Processamento de Dados**
```
1. Buffer recebido: ['1', '2', '3', '4']
2. atol() converte para: dados = 1234
3. Contador x incrementado
4. Sistema pronto para próxima recepção
```

## Limitações e Considerações

### **Buffer Fixo**
- Tamanho do buffer definido em compile-time (4 bytes)
- Dados maiores que buffer são truncados
- Necessário planejar tamanho adequado

### **Conversão atol()**
- Aceita apenas caracteres numéricos válidos
- Caracteres inválidos resultam em 0
- Sem validação de overflow

### **Single Reception**
- Cada chamada HAL_UART_Receive_IT() recebe apenas um buffer
- Necessário re-chamar após cada recepção completa
- Sem buffer circular automático

## Troubleshooting

### Callback não é chamado
- Verificar habilitação da interrupção USART1 no NVIC
- Confirmar que HAL_UART_Receive_IT() foi chamado
- Verificar se dados estão sendo enviados corretamente
- Testar com buffer menor (1 byte)

### Dados corrompidos
- Verificar baudrate em ambos os lados
- Confirmar configuração 8N1
- Testar com velocidade menor
- Verificar integridade das conexões

### Conversão atol() falha
- Verificar se buffer contém apenas números
- Testar com dados simples ("123")
- Adicionar terminador null no buffer
- Verificar endianness dos dados

### Perda de dados
- Buffer muito pequeno para dados recebidos
- Tempo entre recepções muito curto
- Processamento no callback muito lento
- Implementar buffer circular

## Melhorias Sugeridas
1. **Buffer Circular**: Implementar buffer circular para recepção contínua
2. **Validação de Dados**: Adicionar verificação de dados válidos
3. **Timeout Protection**: Implementar timeout para recepções incompletas
4. **Error Handling**: Tratamento de erros de comunicação
5. **Multiple Commands**: Parser de múltiplos comandos
6. **Transmission Response**: Resposta automática após recepção
7. **Data Logging**: Armazenamento de dados recebidos
8. **Protocol Implementation**: Implementar protocolo de comunicação

## Exemplo de Teste
Para testar o projeto:
1. Configure terminal para 9600 baud, 8N1
2. Envie números de 4 dígitos: "1234", "5678", "9999"
3. Monitore variáveis `x` (contador) e `dados` (valor convertido)
4. Verifique que cada envio incrementa o contador
5. Confirme que conversão numérica funciona corretamente

## Comparação com Projeto 36
| Aspecto | Projeto 36 (Polling) | Projeto 37 (Interrupt) |
|---------|----------------------|-------------------------|
| **Bloqueio** | CPU bloqueada | CPU livre |
| **Timeout** | Necessário | Não necessário |
| **Eficiência** | Baixa | Alta |
| **Complexidade** | Simples | Moderada |
| **Responsividade** | Limitada | Excelente |
| **Multitarefa** | Difícil | Fácil |

## Recursos de Aprendizado
- **Conceitos**: Interrupts, Callbacks, Non-blocking I/O, String Conversion
- **Periféricos**: USART1, NVIC, GPIO
- **HAL Functions**: HAL_UART_Receive_IT, HAL_UART_RxCpltCallback
- **C Functions**: atol(), string manipulation
- **Patterns**: Interrupt-driven programming, Event-driven architecture

Este projeto demonstra o poder das interrupções para criar sistemas responsivos e eficientes, estabelecendo a base para aplicações mais complexas que requerem comunicação serial não-bloqueante.