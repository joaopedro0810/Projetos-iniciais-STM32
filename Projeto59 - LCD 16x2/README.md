# Projeto59 - LCD 16x2

## Descrição do Projeto
Este projeto demonstra interface com display **LCD 16x2** (HD44780 compatible) usando STM32F407VET6. O sistema implementa uma biblioteca personalizada para controle do LCD em modo **4-bit**, exibindo informações estáticas e dinâmicas como contador incremental, ideal para aplicações de interface homem-máquina (HMI).

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Display:** LCD 16x2 (HD44780 compatible)

## Display LCD 16x2

### Características do LCD HD44780
- **Resolução:** 16 colunas × 2 linhas
- **Controlador:** HD44780 ou compatível
- **Interface:** Paralela 4-bit ou 8-bit
- **Alimentação:** 5V típico (alguns 3.3V)
- **Backlight:** LED opcional
- **Caracteres:** ASCII + caracteres especiais

### Vantagens do LCD 16x2
1. **Baixo custo:** Display muito acessível
2. **Fácil interface:** Protocolo bem documentado
3. **Baixo consumo:** ~1-5mA sem backlight
4. **Robustez:** Resistente a interferências
5. **Visibilidade:** Boa legibilidade
6. **Padronização:** Interface universal

## Mapeamento de Pinos

### Conexões LCD → STM32F407VET6
```c
// Pinos de controle
#define RS_Pin GPIO_PIN_0        // PA0 - Register Select
#define RW_Pin GPIO_PIN_1        // PA1 - Read/Write
#define EN_Pin GPIO_PIN_4        // PA4 - Enable

// Pinos de dados (modo 4-bit)
#define D4_Pin GPIO_PIN_0        // PB0 - Data bit 4
#define D5_Pin GPIO_PIN_15       // PA15 - Data bit 5
#define D6_Pin GPIO_PIN_12       // PC12 - Data bit 6
#define D7_Pin GPIO_PIN_10       // PC10 - Data bit 7
```

### Pinout Físico LCD 16x2
```
LCD Pin | Nome | Função              | STM32 Pin
--------|------|--------------------|-----------
1       | VSS  | Ground             | GND
2       | VDD  | Power (+5V)        | +5V
3       | V0   | Contrast           | Potenciômetro
4       | RS   | Register Select    | PA0
5       | RW   | Read/Write         | PA1
6       | EN   | Enable             | PA4
7       | D0   | Data 0 (não usado) | -
8       | D1   | Data 1 (não usado) | -
9       | D2   | Data 2 (não usado) | -
10      | D3   | Data 3 (não usado) | -
11      | D4   | Data 4             | PB0
12      | D5   | Data 5             | PA15
13      | D6   | Data 6             | PC12
14      | D7   | Data 7             | PC10
15      | A    | Backlight Anode    | +5V (opcional)
16      | K    | Backlight Cathode  | GND (opcional)
```

## Protocolo de Comunicação HD44780

### Sinais de Controle
- **RS (Register Select):** 0=Comando, 1=Dados
- **RW (Read/Write):** 0=Escrita, 1=Leitura (sempre 0 neste projeto)
- **EN (Enable):** Pulso para latch dos dados

### Modo 4-bit vs 8-bit
```c
// 8-bit: Usa D0-D7 (8 pinos dados)
// 4-bit: Usa apenas D4-D7 (4 pinos dados)
// Economia: 4 pinos do microcontrolador
// Desvantagem: Duas transferências por byte
```

## Timer TIM1 - Delay Microsegundos

### Configuração para Delay Preciso
```c
TIM_HandleTypeDef htim1;
htim1.Instance = TIM1;
htim1.Init.Prescaler = 16-1;         // Prescaler = 16
htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
htim1.Init.Period = 65535-1;         // Período máximo
htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
```

### Função de Delay Microsegundos
```c
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);              // Reset contador
    while(__HAL_TIM_GET_COUNTER(&htim1) < us);     // Aguarda us microssegundos
}
```

**Cálculo do Timing:**
- Clock Timer = HSI / Prescaler = 16MHz / 16 = **1MHz**
- Resolução = 1/1MHz = **1µs por tick**
- Range = 0 a 65535µs ≈ **65ms máximo**

## Biblioteca LCD1602 - Análise Detalhada

### Função de Baixo Nível
```c
void send_to_lcd(char data, int rs)
{
    HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs);    // Set RS (comando ou dado)
    
    // Enviar 4 bits (modo nibble)
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data>>3)&0x01));  // Bit 3
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data>>2)&0x01));  // Bit 2
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data>>1)&0x01));  // Bit 1
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data>>0)&0x01));  // Bit 0
    
    // Pulso Enable
    HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);     // EN = 1
    delay_us(20);                                   // Setup time
    HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);     // EN = 0
    delay_us(20);                                   // Hold time
}
```

### Envio de Comando
```c
void lcd_send_cmd(char cmd)
{
    char datatosend;
    
    // Enviar nibble alto (bits 7-4)
    datatosend = ((cmd>>4)&0x0f);
    send_to_lcd(datatosend, 0);  // RS=0 para comando
    
    // Enviar nibble baixo (bits 3-0)
    datatosend = ((cmd)&0x0f);
    send_to_lcd(datatosend, 0);  // RS=0 para comando
}
```

### Envio de Dados
```c
void lcd_send_data(char data)
{
    char datatosend;
    
    // Enviar nibble alto (bits 7-4)
    datatosend = ((data>>4)&0x0f);
    send_to_lcd(datatosend, 1);  // RS=1 para dados
    
    // Enviar nibble baixo (bits 3-0)
    datatosend = ((data)&0x0f);
    send_to_lcd(datatosend, 1);  // RS=1 para dados
}
```

## Inicialização do LCD

### Sequência de Inicialização HD44780
```c
void lcd_init(void)
{
    HAL_Delay(50);          // Aguardar estabilização (>40ms)
    
    // Sequência de reset específica do HD44780
    lcd_send_cmd(0x30);     // Inicialização forçada
    HAL_Delay(5);
    lcd_send_cmd(0x30);     // Repetir
    HAL_Delay(1);
    lcd_send_cmd(0x30);     // Confirmar
    HAL_Delay(10);
    lcd_send_cmd(0x20);     // Configurar modo 4-bit
    HAL_Delay(10);
    
    // Configuração funcional
    lcd_send_cmd(0x28);     // 4-bit, 2 linhas, 5x8 dots
    HAL_Delay(1);
    lcd_send_cmd(0x08);     // Display OFF
    HAL_Delay(1);
    lcd_send_cmd(0x01);     // Clear display
    HAL_Delay(1);
    lcd_send_cmd(0x06);     // Entry mode (increment, no shift)
    HAL_Delay(1);
    lcd_send_cmd(0x0C);     // Display ON, cursor OFF
}
```

### Comandos HD44780 Principais
```c
0x01    // Clear Display
0x02    // Return Home
0x06    // Entry Mode Set (increment)
0x0C    // Display ON, Cursor OFF, Blink OFF
0x0E    // Display ON, Cursor ON, Blink OFF
0x0F    // Display ON, Cursor ON, Blink ON
0x28    // Function Set: 4-bit, 2-line, 5x8
0x80    // Set DDRAM Address (linha 0)
0xC0    // Set DDRAM Address (linha 1)
```

## Funcionalidades Implementadas

### 1. Posicionamento do Cursor
```c
void lcd_put_cur(int row, int col)
{
    switch (row) {
        case 0:
            col |= 0x80;    // Linha 0: endereço 0x80-0x8F
            break;
        case 1:
            col |= 0xC0;    // Linha 1: endereço 0xC0-0xCF
            break;
    }
    lcd_send_cmd(col);
}
```

### 2. Envio de String
```c
void lcd_send_string(char *str)
{
    while (*str) lcd_send_data(*str++);  // Enviar cada caractere
}
```

### 3. Limpeza do Display
```c
void lcd_clear(void)
{
    lcd_send_cmd(0x01);     // Comando Clear Display
    HAL_Delay(2);           // Aguardar execução
}
```

## Aplicação Principal

### Tela de Apresentação
```c
lcd_init();
lcd_put_cur(0, 0);          // Linha 0, coluna 0
lcd_send_string("Curso ");
lcd_send_string("STM32 ");
lcd_send_string("PRO");

lcd_put_cur(1, 0);          // Linha 1, coluna 0
lcd_send_string("Prof. Gabriel ");
HAL_Delay(3000);            // Exibir por 3 segundos

lcd_clear();                // Limpar para próxima tela
```

### Loop Principal - Contador
```c
while (1) {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);  // LED de vida
    HAL_Delay(500);
    
    contagem++;                                   // Incrementar contador
    
    sprintf((char *)dadosBuf, "%04d", (int)contagem);  // Formatar número
    lcd_put_cur(0, 0);                           // Posicionar cursor
    lcd_send_string("Cont.: ");                  // Label
    lcd_put_cur(0, 6);                           // Posição do número
    lcd_send_string((char *)dadosBuf);           // Exibir contador
}
```

### Layout da Tela
```
Linha 0: |Cont.: 0001     |
Linha 1: |                |
         0123456789ABCDEF
```

## Clock Configuration Simples

### HSI sem PLL
```c
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
RCC_OscInitStruct.HSIState = RCC_HSI_ON;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // PLL desabilitado

RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // HSI direto
```

**Clocks resultantes:**
- **SYSCLK:** 16MHz (HSI)
- **HCLK:** 16MHz
- **APB1:** 16MHz  
- **APB2:** 16MHz

### Vantagens da Configuração Simples
1. **Baixo consumo:** Sem PLL
2. **Startup rápido:** HSI liga rapidamente
3. **Simplicidade:** Menos configuração
4. **Estabilidade:** Menos componentes ativos

## Mapeamento de Memória LCD

### DDRAM (Display Data RAM)
```
Linha 0: 0x00-0x0F (endereços 0x80-0x8F)
Linha 1: 0x40-0x4F (endereços 0xC0-0xCF)

Posição física vs endereço lógico:
[0][0] = 0x80  [0][8] = 0x88
[1][0] = 0xC0  [1][8] = 0xC8
```

### CGRAM (Character Generator RAM)
- **8 caracteres customizáveis:** 0x00-0x07
- **Resolução:** 5×8 pixels por caractere
- **Programável:** Via comandos específicos

## Aplicações Práticas

### 1. Instrumentação
```c
lcd_put_cur(0, 0);
lcd_send_string("Temp: 25.6C");
lcd_put_cur(1, 0);
lcd_send_string("Humi: 60.2%");
```

### 2. Menu de Configuração
```c
lcd_put_cur(0, 0);
lcd_send_string("Menu Principal");
lcd_put_cur(1, 0);
lcd_send_string("1-Config 2-Test");
```

### 3. Status de Sistema
```c
lcd_put_cur(0, 0);
lcd_send_string("Sistema: OK");
lcd_put_cur(1, 0);
lcd_send_string("Uptime: 123h45m");
```

### 4. Data Logger Display
```c
lcd_put_cur(0, 0);
lcd_send_string("Log: 1024 pts");
lcd_put_cur(1, 0);
lcd_send_string("SD: 75% usado");
```

## Otimizações e Melhorias

### 1. Buffer de Display
```c
char lcd_buffer[2][17];  // Buffer para cada linha
void lcd_update_buffer(void);  // Atualizar apenas mudanças
```

### 2. Caracteres Customizados
```c
void lcd_create_char(uint8_t location, uint8_t charmap[]);
// Criar símbolos especiais (graus, setas, etc.)
```

### 3. Scroll Automático
```c
void lcd_scroll_text(char *text, int line, int delay_ms);
// Rolar texto longo horizontalmente
```

### 4. Backlight Control
```c
#define BACKLIGHT_Pin GPIO_PIN_5
void lcd_backlight(uint8_t state);  // ON/OFF do backlight
```

## Timing Crítico

### Tempos Mínimos HD44780
- **Enable pulse width:** >450ns
- **Enable cycle time:** >1µs  
- **Data setup time:** >80ns
- **Data hold time:** >10ns
- **Clear display time:** ~2ms

### Implementação no Projeto
```c
delay_us(20);  // 20µs >> 450ns (margem de segurança)
```

**Observação:** 20µs é conservativo, funciona mesmo com clocks altos.

## Conceitos Aprendidos
1. **Interface paralela LCD** - Protocolo HD44780
2. **Modo 4-bit** - Economia de pinos
3. **Timing preciso** - Delays em microssegundos
4. **Display controller** - Comandos e dados
5. **Character mapping** - DDRAM e CGRAM
6. **User interface** - Display como HMI

## Melhorias Sugeridas
1. **I2C LCD:** Usar PCF8574 para 2 pinos apenas
2. **Printf redirect:** Redirecionar printf para LCD
3. **Menu system:** Implementar navegação com botões
4. **Caracteres especiais:** Criar símbolos customizados
5. **Backlight dimming:** Controle PWM do backlight

## Hardware Recomendado
- **LCD 16x2:** HD44780 compatible
- **Potenciômetro:** 10kΩ para contraste
- **Resistor:** 330Ω para backlight
- **Capacitores:** 100nF para filtro
- **Conectores:** Headers ou protoboard

## Observações Importantes
- **Nível lógico:** LCD 5V, STM32 3.3V (usar level shifters se necessário)
- **Consumo:** ~1mA sem backlight, ~20mA com backlight
- **Contraste:** Ajustar potenciômetro para melhor visibilidade
- **Timing:** Delays são críticos para funcionamento correto
- **Pinout:** Verificar datasheet específico do LCD
- **Alimentação:** 5V estável para operação confiável