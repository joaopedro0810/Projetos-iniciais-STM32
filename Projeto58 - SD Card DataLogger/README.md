# Projeto58 - SD Card DataLogger

## Descrição do Projeto
Este projeto implementa um **sistema completo de data logger** usando STM32F407VET6, cartão SD e RTC. O sistema coleta dados simulados continuamente, armazena em memória com timestamp, e grava tudo no cartão SD quando o usuário pressiona um botão. É um exemplo prático de sistema de aquisição de dados industriais.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 100MHz (PLL configurado)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Periféricos:** SDIO, RTC, Timer, GPIO

## Arquitetura do Sistema Data Logger

### Componentes Principais
1. **Timer TIM7:** Trigger de aquisição (1kHz)
2. **RTC:** Timestamp das amostras
3. **Memória RAM:** Buffer circular para dados
4. **SD Card:** Armazenamento permanente
5. **Interface usuário:** Botão para salvar

### Fluxo de Operação
```
Timer TIM7 (1ms) → Callback → Aquisição de dados → Buffer RAM
                                      ↓
                              Timestamp (RTC)
                                      ↓
Botão pressionado → Montar SD → Formar arquivo → Gravar → Desmontar
```

## Timer TIM7 - Trigger de Aquisição

### Configuração do Timer
```c
TIM_HandleTypeDef htim7;
htim7.Instance = TIM7;
htim7.Init.Prescaler = 5-1;          // Prescaler = 5
htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
htim7.Init.Period = 10000-1;         // Period = 10000
htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
```

### Cálculo da Frequência
```
Clock Timer = APB1_Clock / Prescaler = 50MHz / 5 = 10MHz
Período Timer = Period + 1 = 10000
Frequência = Clock Timer / Período = 10MHz / 10000 = 1kHz
Intervalo = 1ms
```

### Callback de Interrupção
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim7) {
        // Atualizar RTC
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        
        segundos = sTime.Seconds;
        minutos = sTime.Minutes;
        
        // Aquisição a cada 1000 callbacks (1 segundo)
        cont++;
        if (cont == 1000) {
            vetor[i] = rand();        // Dado simulado
            min[i] = minutos;         // Timestamp minutos
            seg[i] = segundos;        // Timestamp segundos
            
            i++; j++;
            if ((i >= 20) || (j >= 20)) {
                i = 0; j = 0;         // Buffer circular
            }
            cont = 0;                 // Reset contador
        }
    }
}
```

## Sistema RTC - Real Time Clock

### Configuração RTC
```c
RTC_HandleTypeDef hrtc;
hrtc.Instance = RTC;
hrtc.Init.HourFormat = RTC_HOURFORMAT_24;     // Formato 24h
hrtc.Init.AsynchPrediv = 127;                 // Prediv assíncrono
hrtc.Init.SynchPrediv = 255;                  // Prediv síncrono
hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;        // Sem saída
```

### Clock Source - LSI
```c
RCC_OscInitStruct.LSIState = RCC_LSI_ON;      // LSI habilitado
// LSI ≈ 32kHz (oscilador interno de baixa velocidade)
```

### Inicialização de Data/Hora
```c
// Configuração inicial (00:00:00, 01/01/2000)
sTime.Hours = 0x0;
sTime.Minutes = 0x0;
sTime.Seconds = 0x0;
HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

sDate.WeekDay = RTC_WEEKDAY_MONDAY;
sDate.Month = RTC_MONTH_JANUARY;
sDate.Date = 0x1;
sDate.Year = 0x0;                             // Ano 2000
HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
```

## Sistema de Buffers

### Estruturas de Dados
```c
uint16_t vetor[20] = {0};     // Dados principais (valores simulados)
uint8_t min[20] = {0};        // Timestamp: minutos
uint8_t seg[20] = {0};        // Timestamp: segundos
uint16_t i, j;                // Índices de controle
uint16_t cont;                // Contador auxiliar

uint8_t bufferG[1000];        // Buffer geral para arquivo
uint8_t bufferT[50];          // Buffer temporário para formatação
```

### Buffer Circular
- **Capacidade:** 20 amostras
- **Overwrap:** Automático quando buffer lotado
- **Índices:** `i` e `j` resetam para 0 após 20 amostras
- **Thread-safe:** Operação em interrupção

## Interface do Usuário

### Botão de Gravação
```c
if (HAL_GPIO_ReadPin(SW_PB0_GPIO_Port, SW_PB0_Pin)) {
    while (HAL_GPIO_ReadPin(SW_PB0_GPIO_Port, SW_PB0_Pin)){} // Debounce
    
    // Processo de gravação...
}
```

### LED de Status
```c
HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);  // Liga durante gravação
// ... processo de gravação ...
HAL_Delay(500);
HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);  // Desliga ao finalizar
```

## Processo de Gravação no SD Card

### 1. Montagem do Sistema de Arquivos
```c
if (f_mount(&meuFATFS, SDPath, 1) == FR_OK) {
    // SD Card montado com sucesso
}
```

### 2. Criação e Abertura do Arquivo
```c
char caminho[] = "DLG.txt";
f_open(&meuArquivo, caminho, FA_WRITE | FA_CREATE_ALWAYS);
```

### 3. Formatação do Cabeçalho
```c
char info[] = "Data Logger Curso STM32\r\n\r\n";
strcat(bufferG, info);
```

### 4. Formatação dos Dados
```c
for (i = 0, j = 0; i < 19; i++, j++) {
    sprintf(bufferT, "Valor[%02u]->Hs:%02u:%02u->Leitura:%05u\r\n", 
            i, min[j], seg[j], vetor[i]);
    strcat(bufferG, bufferT);
}
```

### 5. Gravação e Fechamento
```c
f_write(&meuArquivo, bufferG, sizeof(bufferG), &testeByte);
f_close(&meuArquivo);
```

## Formato do Arquivo Gerado

### Estrutura do Arquivo "DLG.txt"
```
Data Logger Curso STM32

Valor[00]->Hs:00:01->Leitura:12345
Valor[01]->Hs:00:02->Leitura:23456
Valor[02]->Hs:00:03->Leitura:34567
...
Valor[18]->Hs:00:19->Leitura:98765
```

### Explicação dos Campos
- **Valor[XX]:** Índice da amostra (00-18)
- **Hs:MM:SS:** Timestamp (minutos:segundos)
- **Leitura:XXXXX:** Valor simulado (5 dígitos)

## Clock Configuration

### PLL Otimizada
```c
RCC_OscInitStruct.PLL.PLLM = 8;     // Divisor entrada
RCC_OscInitStruct.PLL.PLLN = 50;    // Multiplicador
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // Divisor P
RCC_OscInitStruct.PLL.PLLQ = 4;     // Divisor Q (SDIO)
```

**Cálculo dos Clocks:**
- HSI = 16MHz
- VCO = 16MHz / 8 × 50 = 100MHz
- SYSCLK = VCO / 2 = **50MHz**
- APB1 = SYSCLK / 2 = **25MHz**
- APB2 = SYSCLK / 1 = **50MHz**
- SDIO_CK = VCO / 4 = **25MHz**

### Clocks Otimizados Para:
- **Performance moderada:** 50MHz SYSCLK
- **Baixo consumo:** Clock reduzido
- **SDIO funcional:** 25MHz adequado
- **Timers precisos:** APB1 estável

## Vantagens do Sistema

### 1. Aquisição Contínua
- **Background operation:** Dados coletados continuamente
- **Sem perda:** Buffer circular previne overflow
- **Timestamp preciso:** RTC garante referência temporal

### 2. Armazenamento Sob Demanda
- **Usuário controla:** Grava apenas quando necessário
- **Preserva SD Card:** Evita escritas desnecessárias
- **Feedback visual:** LED indica operação

### 3. Formato Legível
- **Texto puro:** Readable em qualquer editor
- **CSV-like:** Facilmente importável
- **Timestamp:** Rastreabilidade temporal

### 4. Robustez
- **Error handling:** Verificação de montagem SD
- **Debounce:** Tratamento de botão robusto
- **Buffer overflow:** Proteção automática

## Aplicações Práticas

### 1. Monitoramento Ambiental
```c
// Substituir rand() por:
temperatura = ler_sensor_temperatura();
umidade = ler_sensor_umidade();
pressao = ler_sensor_pressao();
```

### 2. Monitoramento Industrial
```c
// Dados de processo:
corrente_motor = ler_ADC_corrente();
vibracao = ler_acelerometro();
temperatura_bearing = ler_termopar();
```

### 3. Data Logger Veicular
```c
// Parâmetros do veículo:
rpm_motor = ler_CAN_rpm();
velocidade = ler_GPS_speed();
combustivel = ler_nivel_tanque();
```

### 4. Instrumentação Científica
```c
// Experimentos:
voltagem = ler_multimetro();
corrente = ler_amperimetro();
timestamp_preciso = ler_GPS_time();
```

## Otimizações Possíveis

### 1. Compressão de Dados
```c
// Formato binário mais eficiente
typedef struct {
    uint32_t timestamp;
    uint16_t valor;
} amostra_t;
```

### 2. Multi-threading (com FreeRTOS)
```c
// Task separada para SD Card
xTaskCreate(task_sdcard_write, "SD_Write", 512, NULL, 1, NULL);
```

### 3. Buffering Avançado
```c
// Double buffering
uint8_t buffer_A[1000], buffer_B[1000];
bool using_buffer_A = true;
```

### 4. Compactação Temporal
```c
// Armazenar apenas mudanças significativas
if (abs(novo_valor - ultimo_valor) > threshold) {
    // Armazenar amostra
}
```

## Análise de Performance

### Timing Crítico
- **Aquisição:** 1ms (interrupção TIM7)
- **Processamento:** <100µs (cálculos simples)
- **Gravação SD:** ~500ms (operação bloqueante)
- **Overhead:** <1% CPU em operação normal

### Consumo de Memória
- **RAM:** ~1.5KB para buffers
- **Flash:** Dependente do FATFS (~50KB típico)
- **SD Card:** Limitado apenas pela capacidade

## Conceitos Aprendidos
1. **Data logging architecture** - Sistema completo de aquisição
2. **Real-time timestamping** - RTC para referência temporal
3. **Circular buffering** - Gestão eficiente de memória
4. **User interface** - Botão e LED para controle
5. **File formatting** - Estruturação de dados para arquivo
6. **Background processing** - Operação contínua em interrupção

## Melhorias Sugeridas
1. **RTC externa:** DS3231 para maior precisão
2. **Bateria backup:** Manter RTC durante power-off
3. **Compressão:** Algoritmos de compressão para economizar espaço
4. **Interface:** Display LCD para status
5. **Comunicação:** WiFi/Bluetooth para acesso remoto

## Hardware Recomendado
- **SD Card:** Class 10, 8-32GB
- **RTC Battery:** CR2032 para backup
- **Botão:** Momentary push button
- **LED:** Indicação visual clara
- **Enclosure:** Proteção industrial se necessário

## Observações Importantes
- **Clock LSI:** Menos preciso que cristal externo
- **Buffer size:** Limitado a 20 amostras (configurável)
- **SD Card wear:** Minimizar escritas desnecessárias
- **Power management:** Considerar consumo em aplicações battery-powered
- **File system:** FATFS é thread-safe com configuração adequada