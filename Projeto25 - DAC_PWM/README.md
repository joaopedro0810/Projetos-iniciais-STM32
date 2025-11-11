# Projeto25 - DAC_PWM

## Descrição do Projeto
Este projeto implementa um **PWM variável com DMA** usando Timer 2 do STM32F407VET6. O sistema gera um sinal PWM que varia ciclicamente o duty cycle de 0% a 100%, demonstrando controle analógico através de modulação por largura de pulso com atualização automática via DMA.

## Plataforma de Desenvolvimento
- **Microcontrolador:** STM32F407VET6
- **Core:** ARM Cortex-M4 @ 16MHz (HSI)
- **Ambiente:** STM32CubeIDE
- **HAL:** STM32 HAL Library
- **Pino PWM:** PA0 (TIM2_CH1)
- **Recursos:** Timer 2 + DMA1 Stream 5

## PWM como DAC Analógico

### Conceito Fundamental
O **PWM (Pulse Width Modulation)** pode funcionar como um **DAC de baixo custo** quando seguido por um filtro passa-baixas. A tensão média da saída é proporcional ao duty cycle:

```
Vout_avg = Vcc × (Duty Cycle / 100%)
```

### Vantagens do PWM-DAC
- **Custo zero:** Usa timer já disponível
- **Alta resolução:** Limitada apenas pelo timer
- **Linear:** Relação direta duty cycle ↔ tensão
- **Sem drift:** Não sofre deriva térmica como DAC real
- **Flexível:** Fácil controle via software

### Limitações
- **Ripple:** Necessita filtro para tensão DC limpa
- **Frequência:** Limitada pela frequência PWM
- **Resposta:** Mais lenta que DAC dedicado
- **EMI:** Pode gerar interferência eletromagnética

## Configuração do Timer 2

### Parâmetros do PWM
```c
static void MX_TIM2_Init(void)
{
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 2-1;         // Prescaler = 2
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 100-1;          // Período = 99 (ARR)
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    
    // Configuração PWM Channel 1
    sConfigOC.OCMode = TIM_OCMODE_PWM1;    // PWM Mode 1
    sConfigOC.Pulse = 0;                   // Duty cycle inicial = 0
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;  // Polaridade alta
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
}
```

### Cálculo da Frequência PWM
**Com HSI @ 16MHz:**
```
Timer Clock = PCLK1 = 16MHz (sem PLL)
PWM Frequency = Timer Clock / (Prescaler × Period)
PWM Frequency = 16MHz / (2 × 100) = 80kHz
Período PWM = 12.5µs
```

### Resolução do Duty Cycle
```c
Período = 100 ticks (0 a 99)
Resolução = 1/100 = 1% por step
Duty Cycle Range = 0% a 99%
```

## Controle Dinâmico com DMA

### Variável de Controle
```c
uint32_t dutyCycle = 0;  // Valor do duty cycle (0 a 499)
```

### Loop Principal
```c
while (1)
{
    dutyCycle++;                           // Incrementar duty cycle
    dutyCycle = (dutyCycle > 499) ? 0 : dutyCycle;  // Reset após máximo
    
    HAL_Delay(20);  // Aguardar 20ms entre incrementos
}
```

### Análise do Controle
- **Range:** 0 a 499 (500 níveis)
- **Mapeamento:** dutyCycle valor → CCR register
- **Resolução efetiva:** 500 níveis em 100 ticks = interpolação
- **Tempo total do ciclo:** 500 × 20ms = 10 segundos
- **Taxa de variação:** 50 níveis por segundo

## DMA para PWM

### Inicialização DMA
```c
HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, &dutyCycle, 1);
```

### Função HAL_TIM_PWM_Start_DMA
```c
HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim,
                                        uint32_t Channel,
                                        uint32_t *pData,
                                        uint16_t Length);
```

**Parâmetros:**
- **htim:** Handle do timer
- **Channel:** Canal PWM (TIM_CHANNEL_1)
- **pData:** Ponteiro para dados (dutyCycle)
- **Length:** Número de transferências (1)

### Operação DMA
1. **DMA monitora** a variável `dutyCycle`
2. **A cada update** do timer, DMA transfere o valor
3. **CCR register** é atualizado automaticamente
4. **Duty cycle** muda sem intervenção da CPU

## Mapeamento do Duty Cycle

### Problema de Resolução
O timer tem período 100, mas `dutyCycle` vai até 499:
```c
Timer Period: 0 a 99   (100 valores)
dutyCycle:    0 a 499  (500 valores)
```

### Solução de Mapeamento
O **hardware do timer** mapeia automaticamente:
```c
Effective_Duty = (dutyCycle * Period) / Max_DutyCycle
Effective_Duty = (dutyCycle * 100) / 500 = dutyCycle / 5
```

### Exemplos de Mapeamento
```c
dutyCycle =   0  → CCR =  0   → 0%   duty
dutyCycle =  50  → CCR = 10   → 10%  duty  
dutyCycle = 100  → CCR = 20   → 20%  duty
dutyCycle = 250  → CCR = 50   → 50%  duty
dutyCycle = 499  → CCR = 99.8 → 99.8% duty
```

## Forma de Onda Gerada

### PWM com Duty Cycle Variável
```
0% Duty:  ___________________  (0V avg)

25% Duty: ■■■■■_______________  (0.8V avg)

50% Duty: ■■■■■■■■■■__________  (1.65V avg)

75% Duty: ■■■■■■■■■■■■■■■_____  (2.5V avg)

99% Duty: ■■■■■■■■■■■■■■■■■■■_  (3.27V avg)

■ = High (3.3V)    _ = Low (0V)
```

### Tensão Média vs Duty Cycle
```c
Duty =  0% → Vavg = 3.3V × 0.00 = 0.00V
Duty = 25% → Vavg = 3.3V × 0.25 = 0.83V
Duty = 50% → Vavg = 3.3V × 0.50 = 1.65V
Duty = 75% → Vavg = 3.3V × 0.75 = 2.48V
Duty = 99% → Vavg = 3.3V × 0.99 = 3.27V
```

## Filtro Passa-Baixas

### Necessidade do Filtro
Para converter PWM em tensão DC, é necessário **filtro RC**:
```
PWM → [R] → [C] → Vout(DC)
      ↓     
     GND
```

### Cálculo do Filtro
```c
Frequência PWM: 80kHz
Frequência de corte recomendada: fc = fpwm / 10 = 8kHz

Para fc = 8kHz:
RC = 1 / (2π × fc) = 1 / (2π × 8000) = 19.9µs

Exemplo com C = 100nF:
R = 19.9µs / 100nF = 199Ω ≈ 220Ω (valor padrão)
```

### Ripple Residual
```c
Ripple ≈ Vpp × (fc / fpwm)
Ripple ≈ 3.3V × (8kHz / 80kHz) = 0.33V pp

Para menor ripple: fc = fpwm / 20 → Ripple ≈ 0.165V pp
```

## Clock Configuration HSI

### Configuração Simples
```c
void SystemClock_Config(void)
{
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // PLL desabilitado
    
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // HSI direto
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     // 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;      // 16MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;      // 16MHz
}
```

**Clocks resultantes:**
- **SYSCLK:** 16MHz
- **AHB:** 16MHz
- **APB1:** 16MHz (Timer 2 clock)
- **APB2:** 16MHz

## Aplicações Práticas

### 1. Controle de Brilho LED
```c
// Controlar brilho de LED de potência
void set_led_brightness(uint8_t brightness_percent)
{
    if (brightness_percent <= 100)
    {
        dutyCycle = (brightness_percent * 499) / 100;
    }
}

// Fade in/out automático
void led_fade_effect(void)
{
    static uint8_t brightness = 0;
    static int8_t direction = 1;
    
    brightness += direction;
    if (brightness >= 100 || brightness <= 0)
        direction = -direction;
        
    set_led_brightness(brightness);
    HAL_Delay(10);
}
```

### 2. Controle de Motor DC
```c
// Controlar velocidade de motor DC
void set_motor_speed(uint8_t speed_percent)
{
    if (speed_percent <= 100)
    {
        dutyCycle = (speed_percent * 499) / 100;
    }
}

// Aceleração suave
void motor_soft_start(uint8_t target_speed)
{
    for (uint8_t speed = 0; speed <= target_speed; speed++)
    {
        set_motor_speed(speed);
        HAL_Delay(50);  // 50ms por step
    }
}
```

### 3. Aquecimento Proporcional
```c
// Controle de aquecimento proporcional
typedef struct {
    float setpoint;
    float current_temp;
    float kp;           // Ganho proporcional
} heater_control_t;

void heater_control(heater_control_t *ctrl)
{
    float error = ctrl->setpoint - ctrl->current_temp;
    float output = error * ctrl->kp;
    
    // Limitar saída a 0-100%
    if (output > 100) output = 100;
    if (output < 0) output = 0;
    
    dutyCycle = (uint32_t)(output * 499 / 100);
}
```

### 4. Controle de Servo
```c
// Gerar sinal para servo motor (modificação necessária)
void servo_control_init(void)
{
    // Reconfigurar para 50Hz (20ms período)
    htim2.Init.Prescaler = 160-1;    // 100kHz
    htim2.Init.Period = 2000-1;      // 20ms período
    HAL_TIM_Base_Init(&htim2);
}

void set_servo_angle(uint8_t angle_degrees)
{
    // 1ms = 0°, 1.5ms = 90°, 2ms = 180°
    uint32_t pulse_width = 100 + (angle_degrees * 100) / 180;  // 100-200 (1-2ms)
    dutyCycle = pulse_width;
}
```

## Medição e Caracterização

### Equipamentos Necessários
- **Osciloscópio:** Para ver PWM e tensão filtrada
- **Multímetro:** Para medir tensão média
- **Frequency counter:** Para verificar frequência PWM
- **Spectrum analyzer:** Para análise de ripple
- **Carga resistiva:** Para teste de drive

### Parâmetros de Medição
```c
Frequência PWM: 80kHz ±1%
Duty cycle range: 0% a 99.8%
Tensão saída (filtrada): 0V a 3.27V
Ripple: <5% com filtro adequado
Linearidade: Excelente (PWM é inerentemente linear)
Resolução: ~500 níveis (0.2% per step)
```

### Verificações Importantes
1. **PWM frequency:** Conferir com contador
2. **Duty cycle variation:** Verificar com scope
3. **Filtered output:** Medir tensão DC
4. **Ripple amplitude:** Análise AC
5. **Linearity:** Plotar Duty vs Vout

## Otimizações e Melhorias

### 1. Maior Resolução
```c
// Aumentar período do timer para mais resolução
htim2.Init.Period = 1000-1;     // 1000 níveis
htim2.Init.Prescaler = 16-1;    // Manter 1kHz PWM

// Ajustar range do dutyCycle
dutyCycle = (dutyCycle > 999) ? 0 : dutyCycle;
```

### 2. Frequência Otimizada
```c
// Para aplicações de áudio: ~44kHz
htim2.Init.Prescaler = 4-1;     // Prescaler = 4
htim2.Init.Period = 91-1;       // ~44kHz PWM

// Para controle de motor: ~20kHz (acima do audível)
htim2.Init.Prescaler = 8-1;     // Prescaler = 8  
htim2.Init.Period = 100-1;      // 20kHz PWM
```

### 3. Filtro Ativo
```c
// Usar op-amp para filtro ativo
// Melhor resposta e menor ripple
// Buffer para drive de cargas baixas
```

### 4. Controle Automático
```c
// Rampa automática via interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)  // Timer auxiliar para controle
    {
        static uint32_t counter = 0;
        dutyCycle = (counter * 499) / 10000;  // Rampa de 10 segundos
        counter++;
        if (counter > 10000) counter = 0;
    }
}
```

## Troubleshooting

### Problemas Comuns

#### 1. PWM Não Aparece
**Sintomas:** Saída sempre low ou high
**Soluções:**
```c
// Verificar configuração do pino
// Verificar se timer está iniciado
HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, &dutyCycle, 1);

// Verificar configuração PWM mode
sConfigOC.OCMode = TIM_OCMODE_PWM1;
```

#### 2. Duty Cycle Incorreto
**Sintomas:** Duty diferente do esperado
**Soluções:**
```c
// Verificar mapeamento de valores
// dutyCycle deve estar dentro do range válido
if (dutyCycle >= 100) dutyCycle = 99;

// Verificar configuração do período
htim2.Init.Period = 100-1;  // ARR = 99
```

#### 3. Frequência Errada
**Sintomas:** PWM muito rápido ou lento
**Soluções:**
```c
// Recalcular prescaler e período
uint32_t desired_freq = 20000;  // 20kHz
uint32_t timer_clock = 16000000;  // 16MHz
uint32_t prescaler = 8;
uint32_t period = timer_clock / (prescaler * desired_freq);
```

## Conceitos Aprendidos
1. **PWM como DAC** - Conversão PWM para tensão analógica
2. **DMA PWM control** - Controle automático via DMA
3. **Duty cycle mapping** - Mapeamento de valores de controle
4. **Filter design** - Projeto de filtro passa-baixas
5. **Real-time control** - Controle em tempo real
6. **Timer PWM modes** - Modos de operação PWM
7. **Analog approximation** - Aproximação de sinal analógico

## Melhorias Sugeridas
1. **Higher resolution** - Maior resolução do duty cycle
2. **Multiple channels** - PWM multi-canal
3. **Feedback control** - Controle em malha fechada
4. **Variable frequency** - Frequência PWM variável
5. **Dead time control** - Para aplicações de potência
6. **Complementary PWM** - Para controle de ponte H

## Hardware Recomendado
- **RC Filter:** 220Ω + 100nF para conversão DC
- **Op-amp buffer:** Para drive de cargas baixas
- **Power MOSFET:** Para controle de cargas altas
- **Isolador óptico:** Para isolação galvânica
- **Current sensor:** Para feedback de corrente

## Observações Importantes
- **Filter requirement:** PWM sempre precisa filtro para DC
- **Frequency trade-off:** Frequência alta → menos ripple, mais EMI
- **Resolution vs frequency:** Trade-off entre resolução e frequência
- **Load consideration:** Carga afeta a filtragem necessária
- **EMI awareness:** PWM pode gerar interferência
- **Thermal management:** Para aplicações de potência considerar dissipação