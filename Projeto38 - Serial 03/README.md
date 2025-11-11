# Projeto 38 - Serial 03

## Descrição do Projeto
Este projeto implementa comunicação serial de alta performance usando DMA (Direct Memory Access) com UART no microcontrolador STM32F407VET6. O sistema demonstra transferência de dados automatizada sem intervenção da CPU, permitindo comunicação serial eficiente com processamento paralelo de outras tarefas, incluindo controle de LED baseado em comando específico.

## Funcionalidades
- **UART com DMA**: Transferência automática de dados usando DMA2 Stream 2
- **Recepção Contínua**: HAL_UART_Receive_DMA() para recepção sem bloqueio da CPU
- **Command Recognition**: Reconhecimento do comando "muda" para controle de LED
- **LED Control**: Toggle de LED2 (PA7) baseado em comando recebido
- **Transmissão Periódica**: Envio automático de mensagem "SMT32Curso" a cada segundo
- **Zero CPU Overhead**: DMA gerencia transferências sem uso de CPU

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- LED conectado ao pino PA7 (LED_2)
- Resistor limitador para LED (220Ω recomendado)
- Conversor USB-Serial (FTDI, CP2102, CH340, etc.)
- Cabos jumper para conexões
- Terminal serial (PuTTY, Tera Term, Arduino IDE Serial Monitor)
- Computador com porta USB

## Configuração dos Pinos
| Pino STM32 | Função | Descrição |
|------------|--------|-----------|
| PA9 | USART1_TX | Transmissão serial |
| PA10 | USART1_RX | Recepção serial |
| PA7 | LED_2 | LED de controle |
| GND | Ground | Terra comum |

## Análise Técnica

### Configuração DMA
O projeto utiliza DMA2 Stream 2 para USART1 RX:
- **DMA Controller**: DMA2 (USART1 está conectado ao DMA2)
- **Stream**: Stream 2 (canal específico para USART1_RX)
- **Channel**: Canal 4 (mapeamento USART1_RX)
- **Direction**: Peripheral to Memory
- **Priority**: Média (configurável)
- **Mode**: Circular (recepção contínua)

### Mapeamento DMA-UART
```c
// Configuração automática pelo STM32CubeMX
DMA_HandleTypeDef hdma_usart1_rx;

// Vinculação DMA-UART
huart1.hdmarx = &hdma_usart1_rx;
hdma_usart1_rx.Parent = &huart1;
```

### Interrupção DMA
```c
// Configuração da interrupção DMA2_Stream2
HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
```

## Código Principal

### Inicialização com DMA
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();          // Inicialização DMA antes da UART
    MX_USART1_UART_Init();

    // Inicia recepção DMA contínua
    HAL_UART_Receive_DMA(&huart1, buffer, sizeof(buffer));

    while (1)
    {
        // Transmissão periódica (não usa DMA neste exemplo)
        HAL_UART_Transmit(&huart1, (const uint8_t *) "SMT32Curso\r\n", 
                         strlen("SMT32Curso\r\n"), 100);
        HAL_Delay(1000);
    }
}
```

### Callback de Recepção Completa
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        // Verifica comando "muda" (4 bytes)
        if ((buffer[0] == 'm') && (buffer[1] == 'u') && 
            (buffer[2] == 'd') && (buffer[3] == 'a'))
        {
            HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
        }
        
        // DMA automaticamente reinicia recepção (modo circular)
    }
}
```

### Configuração DMA Detalhada
```c
static void MX_DMA_Init(void)
{
    // Habilita clock do DMA2
    __HAL_RCC_DMA2_CLK_ENABLE();

    // Configuração da interrupção
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}
```

## Vantagens do DMA

### 1. **Zero CPU Overhead**
- CPU completamente livre durante transferências
- Melhor performance para outras tarefas
- Redução significativa de uso de CPU

### 2. **Alta Velocidade**
- Transferência direta memory-to-peripheral
- Sem overhead de interrupções frequentes
- Suporte a altas taxas de baudrate

### 3. **Modo Circular**
- Recepção contínua automática
- Sem necessidade de reiniciar recepção
- Buffer automaticamente gerenciado

### 4. **Multitarefa Natural**
- CPU disponível para outras operações
- Ideal para sistemas complexos
- Facilita implementação de RTOS

## Funcionamento do Sistema

### Fluxo de Operação
```
1. Sistema inicializado (HAL, Clock, GPIO, DMA, UART)
2. HAL_UART_Receive_DMA() inicia recepção contínua
3. DMA configura transferência Peripheral → Memory
4. CPU executa outras tarefas no loop principal
5. Dados chegam pela UART
6. DMA transfere automaticamente para buffer
7. Ao completar 4 bytes, DMA gera interrupção
8. HAL_UART_RxCpltCallback() é executado
9. Comando verificado e LED controlado
10. DMA reinicia automaticamente (modo circular)
```

### Comando de Controle
O sistema reconhece especificamente o comando **"muda"**:
- **m**: buffer[0] = 0x6D
- **u**: buffer[1] = 0x75  
- **d**: buffer[2] = 0x64
- **a**: buffer[3] = 0x61

Qualquer outro comando de 4 bytes é ignorado.

## Aplicações Práticas
1. **Sistemas de Alto Throughput**: Comunicação serial de alta velocidade
2. **Data Logging**: Recepção contínua de dados de sensores
3. **Protocolo de Comunicação**: Implementação de protocolos complexos
4. **Streaming de Dados**: Recepção de fluxo contínuo de dados
5. **Controle Industrial**: Comandos de controle em tempo real
6. **Telemetria**: Recepção de dados de telemetria de alta frequência
7. **Instrumentação**: Aquisição de dados de instrumentos

## Comparação entre Modos UART

| Aspecto | Polling | Interrupt | DMA |
|---------|---------|-----------|-----|
| **CPU Usage** | 100% | Médio | ~0% |
| **Velocidade** | Baixa | Média | Alta |
| **Complexidade** | Simples | Moderada | Alta |
| **Throughput** | Baixo | Médio | Alto |
| **Latência** | Alta | Baixa | Muito Baixa |
| **Multitarefa** | Impossível | Possível | Excelente |

## Performance e Recursos

### Recursos de DMA Utilizados
- **DMA2 Stream 2**: Para USART1_RX
- **Memory**: 4 bytes de buffer
- **Bandwidth**: Até várias centenas de kB/s
- **Latency**: < 1μs para início de transferência

### Cálculos de Performance
```c
// Com DMA a 921600 baud (máximo teórico):
// Bytes por segundo: 921600/10 = ~92KB/s
// Overhead de CPU: ~0% (apenas callbacks)
// Latência: < 50μs (incluindo callback)
```

## Limitações e Considerações

### **Buffer Fixo**
- Tamanho fixo de 4 bytes para comando
- Comandos maiores requerem múltiplas recepções
- Necessário ajustar conforme aplicação

### **Modo Circular**
- Recepção contínua pode sobrescrever dados
- Necessário processar dados rapidamente
- Buffer circular mais complexo para implementar

### **Configuração Complexa**
- Requer configuração cuidadosa do DMA
- Mapeamento correto de streams e canais
- Debug mais complexo que outros modos

## Troubleshooting

### DMA não funciona
- Verificar habilitação do clock DMA2
- Confirmar configuração do stream e canal corretos
- Verificar vinculação DMA-UART (hdmarx)
- Testar com DMA mais simples primeiro

### Callback não é chamado
- Verificar se HAL_UART_Receive_DMA() foi chamado
- Confirmar configuração da interrupção DMA
- Verificar se dados têm exatamente 4 bytes
- Testar com buffer menor (1 byte)

### LED não responde ao comando
- Verificar string exata "muda" (case-sensitive)
- Confirmar configuração do pino PA7 como output
- Testar comando caractere por caractere
- Verificar callback com breakpoint

### Performance não melhora
- Verificar se DMA está realmente ativo
- Confirmar que outras tarefas estão executando
- Medir uso de CPU real
- Comparar com versão interrupt

## Melhorias Sugeridas
1. **DMA TX**: Implementar transmissão também com DMA
2. **Buffer Circular**: Implementar buffer circular real
3. **Multiple Commands**: Suporte a múltiplos comandos
4. **Error Recovery**: Recuperação automática de erros DMA
5. **Flow Control**: Implementar controle de fluxo
6. **Protocol Layer**: Camada de protocolo mais robusta
7. **Performance Monitoring**: Monitoramento de performance DMA
8. **Memory Pool**: Pool de buffers para alta performance

## Teste do Sistema
1. Configure terminal para 9600 baud, 8N1
2. Compile e execute o projeto
3. Observe mensagem "SMT32Curso" sendo enviada periodicamente
4. Digite exatamente "muda" (4 caracteres)
5. Verifique que LED2 alterna estado
6. Teste outros comandos para confirmar seletividade

## Recursos de Aprendizado
- **Conceitos**: DMA, High-performance I/O, Zero-copy transfers
- **Periféricos**: DMA2, USART1, GPIO, NVIC
- **HAL Functions**: HAL_UART_Receive_DMA, HAL_UART_RxCpltCallback
- **Architecture**: STM32 DMA architecture, Stream mapping
- **Performance**: CPU utilization, throughput optimization

Este projeto demonstra o poder do DMA para criar sistemas de comunicação serial de alta performance, essencial para aplicações que requerem throughput elevado e uso eficiente de recursos computacionais.