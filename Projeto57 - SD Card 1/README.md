# Projeto57 - SD Card 1

## Descrição do Projeto
Este projeto demonstra interface básica com cartão SD usando STM32F407VET6 e o sistema de arquivos **FATFS**. O projeto implementa operações fundamentais de leitura de arquivos em cartão SD, incluindo montagem do sistema de arquivos, leitura de dados específicos e manipulação de valores numéricos extraídos de arquivos de texto.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 168MHz
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Sistema de Arquivos:** FATFS (File Allocation Table)

## Sistema de Arquivos FATFS

### O que é FATFS?
FATFS é uma implementação **genérica do sistema de arquivos FAT** para microcontroladores:
- **FAT12/FAT16/FAT32:** Compatível com todos os formatos
- **LFN (Long File Names):** Suporte a nomes longos
- **Unicode:** Suporte a caracteres internacionais
- **Múltiplas partições:** Vários drives lógicos
- **Thread-safe:** Operação segura em multitarefa

### Vantagens do FATFS
1. **Compatibilidade universal:** Qualquer PC pode ler/escrever
2. **Robustez:** Sistema maduro e testado
3. **Flexibilidade:** Configurável para diferentes necessidades
4. **Performance:** Otimizado para sistemas embarcados
5. **Portabilidade:** Funciona em qualquer microcontrolador

## Interface SDIO (SD Input/Output)

### Hardware Configuration
```c
SD_HandleTypeDef hsd;
hsd.Instance = SDIO;                              // Periférico SDIO
hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;      // Borda de subida
hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;  // Clock interno
hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE; // Sem economia
hsd.Init.BusWide = SDIO_BUS_WIDE_1B;              // Bus de 1 bit
hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
hsd.Init.ClockDiv = 0;                            // Divisor de clock
```

### Pinos SDIO do STM32F407VET6
```
PC8  - SDIO_D0   (Data 0)
PC9  - SDIO_D1   (Data 1) 
PC10 - SDIO_D2   (Data 2)
PC11 - SDIO_D3   (Data 3)
PC12 - SDIO_CK   (Clock)
PD2  - SDIO_CMD  (Command)
```

**Observação:** Este projeto usa **1-bit mode** (apenas D0), mais simples e confiável.

## Configuração DMA

### DMA Streams para SDIO
```c
DMA_HandleTypeDef hdma_sdio_rx;  // DMA para recepção
DMA_HandleTypeDef hdma_sdio_tx;  // DMA para transmissão

// DMA2 Stream 3 - SDIO RX
// DMA2 Stream 6 - SDIO TX
```

### Vantagens do DMA para SD Card
1. **Performance:** Transferência sem CPU
2. **Eficiência:** CPU livre para outras tarefas
3. **Confiabilidade:** Menos chance de timeout
4. **Throughput:** Velocidade máxima de transferência

## Clock Configuration

### PLL para SDIO
```c
RCC_OscInitStruct.PLL.PLLM = 16;    // Divisor entrada (HSI/16)
RCC_OscInitStruct.PLL.PLLN = 192;   // Multiplicador (×192)
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // Divisor P (/2)
RCC_OscInitStruct.PLL.PLLQ = 4;     // Divisor Q (/4) - SDIO Clock
```

**Cálculo do Clock SDIO:**
- HSI = 16MHz
- VCO = 16MHz / 16 × 192 = 192MHz
- SDIO_CK = VCO / PLLQ = 192MHz / 4 = **48MHz**

### Clock SDIO Otimizado
48MHz é ideal para SD Cards:
- **SD Standard:** Máximo 50MHz
- **SDHC/SDXC:** Compatível
- **Margem de segurança:** Operação confiável

## Operações FATFS Implementadas

### Estruturas FATFS
```c
FATFS meuFATFS;    // Sistema de arquivos
FIL   meuArquivo;  // Handle do arquivo
UINT  testeByte;   // Bytes transferidos
```

### 1. Montagem do Sistema de Arquivos
```c
if (f_mount(&meuFATFS, SDPath, 1) == FR_OK) {
    HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);  // Sucesso
    // Operações com SD Card...
}
```

**Parâmetros:**
- `&meuFATFS`: Estrutura do sistema de arquivos
- `SDPath`: Caminho do drive (definido pelo FATFS)
- `1`: Montagem imediata (vs lazy mount)

### 2. Operações de Escrita (Comentadas)
```c
// CRIAR E ESCREVER ARQUIVO
f_open(&meuArquivo, "ARQUIVO.txt", (FA_WRITE | FA_CREATE_ALWAYS));
char meusDados[] = "Escreveu linha 1\r\nEscreveu linha 2\0";
f_write(&meuArquivo, meusDados, sizeof(meusDados), &testeByte);
f_close(&meuArquivo);

// CRIAR PASTA E ARQUIVO
f_mkdir("PASTA");
f_open(&meuArquivo, "/PASTA/File.txt", (FA_WRITE | FA_CREATE_ALWAYS));
char dadosFile[] = "Teste curso stm32 pasta";
f_write(&meuArquivo, dadosFile, sizeof(dadosFile), &testeByte);
f_close(&meuArquivo);
```

### 3. Leitura de Arquivo (Ativa)
```c
char caminho[] = "Joao.txt";
f_open(&meuArquivo, caminho, (FA_READ | FA_OPEN_ALWAYS));
f_read(&meuArquivo, rxDados, sizeof(rxDados), &testeByte);
f_close(&meuArquivo);
```

## Processamento de Dados

### Extração de Valor ADC
```c
uint8_t rxDados[30] = {0};    // Buffer de leitura
uint8_t buffer1[4] = {0};     // Buffer para valor ADC
uint16_t adc1;                // Valor manipulável

// Extração de 4 dígitos específicos do arquivo
buffer1[0] = rxDados[5];      // Posição 5
buffer1[1] = rxDados[6];      // Posição 6
buffer1[2] = rxDados[7];      // Posição 7
buffer1[3] = rxDados[8];      // Posição 8

adc1 = atoi(buffer1);         // Conversão string → número
adc1++;                       // Incremento
```

### Estrutura Presumida do Arquivo "Joao.txt"
```
ADC1:1234,ADC2:5678,TEMP:25.6
     ^^^^
     |Posições 5-8 extraídas
```

## Modos de Abertura FATFS

### Flags de Abertura
```c
FA_READ          // Somente leitura
FA_WRITE         // Somente escrita
FA_OPEN_EXISTING // Abrir arquivo existente
FA_CREATE_NEW    // Criar novo (falha se existe)
FA_CREATE_ALWAYS // Criar sempre (sobrescreve)
FA_OPEN_ALWAYS   // Abrir ou criar
FA_OPEN_APPEND   // Abrir para anexar
```

### Combinações Comuns
```c
(FA_READ | FA_OPEN_ALWAYS)     // Ler, criar se não existe
(FA_WRITE | FA_CREATE_ALWAYS)  // Escrever, sempre criar novo
(FA_WRITE | FA_OPEN_APPEND)    // Anexar ao final
```

## Indicação Visual

### LED de Status
```c
HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);  // Toggle no mount
HAL_Delay(500);                                    // Delay processing
HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);  // Toggle no final
```

**Comportamento:**
- **LED acende:** SD Card montado com sucesso
- **Processing:** 500ms de delay
- **LED apaga:** Operação concluída

## Tratamento de Erros FATFS

### Códigos de Retorno Principais
```c
FR_OK                // Operação bem-sucedida
FR_DISK_ERR         // Erro de hardware
FR_INT_ERR          // Erro interno
FR_NOT_READY        // Drive não ready
FR_NO_FILE          // Arquivo não encontrado
FR_NO_PATH          // Caminho não encontrado
FR_INVALID_NAME     // Nome inválido
FR_DENIED           // Acesso negado
FR_EXIST            // Arquivo já existe
FR_INVALID_OBJECT   // Objeto inválido
FR_WRITE_PROTECTED  // Mídia protegida
FR_INVALID_DRIVE    // Drive inválido
FR_NOT_ENABLED      // Volume de trabalho não existe
FR_NO_FILESYSTEM    // Sistema de arquivos inválido
```

## Aplicações Práticas

### 1. Data Logger
```c
// Ler configurações do SD Card
// Gravar dados de sensores
// Timestamp com RTC
```

### 2. Calibração Externa
```c
// Carregar parâmetros de calibração
// Tabelas de lookup
// Configurações de usuário
```

### 3. Firmware Update
```c
// Carregar novo firmware
// Verificação de integridade
// Update seguro
```

### 4. Interface Homem-Máquina
```c
// Menus configuráveis
// Textos em múltiplas línguas
// Logos e gráficos
```

## Vantagens do SD Card

### Hardware
1. **Capacidade:** Até 2TB (SDXC)
2. **Velocidade:** Até 104MB/s (UHS-I)
3. **Durabilidade:** 10,000+ ciclos erase
4. **Temperatura:** -25°C a +85°C
5. **Shock resistant:** Sem partes móveis

### Software
1. **Padrão universal:** FAT32 reconhecido globalmente
2. **Hot-swap:** Remoção/inserção a quente
3. **Baixo custo:** Commodity item
4. **Debugging fácil:** Readable em qualquer PC
5. **Backup simples:** Copy/paste direto

## Conceitos Aprendidos
1. **Sistema FATFS** - File system para microcontroladores
2. **Interface SDIO** - Comunicação de alta velocidade com SD
3. **DMA para storage** - Transferência eficiente de dados
4. **Processamento de arquivos** - Parsing de dados estruturados
5. **Clock configuration** - Otimização para SDIO
6. **File operations** - Open, read, write, close

## Próximos Passos
- **Projeto58:** SD Card DataLogger (logging avançado)
- **Implementar:** Escrita de dados estruturados
- **Adicionar:** RTC para timestamp
- **Otimizar:** Buffering e cache

## Hardware Recomendado
- **SD Card:** Class 10 ou superior
- **Capacidade:** 8GB-32GB (FAT32 nativo)
- **Marca:** SanDisk, Samsung, Kingston
- **Socket:** Micro SD com adaptador

## Observações Importantes
- **FATFS thread-safe:** Requer configuração específica
- **Power management:** SD Cards consomem energia significativa
- **Error handling:** Sempre verificar retornos das funções
- **File system corruption:** Usar f_sync() para commit
- **Path separator:** Use '/' (Unix style) no FATFS
- **Case sensitivity:** Depende da configuração FATFS