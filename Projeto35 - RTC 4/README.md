# Projeto 35 - RTC 4

## Descrição do Projeto
Este projeto implementa um sistema de alarme RTC (Real-Time Clock) baseado em interrupções usando o microcontrolador STM32F407VET6. O sistema configura um alarme RTC que é acionado em horário específico, demonstrando funcionalidades avançadas de tempo real como alarmes com callback, controle preciso de timing e indicação visual através de LED.

## Funcionalidades
- **Alarme RTC com Interrupção**: Configuração de alarme com callback automático
- **Inicialização Temporal Específica**: Data/hora inicial configurada para teste de alarme
- **Indicação Visual**: LED1 pisca quando o alarme é acionado
- **Reinicialização Manual**: Botão K1 para reset do sistema RTC
- **Clock Source LSI**: Utiliza oscilador interno de baixa velocidade
- **MCO Output**: Saída do clock HSI em PA8 para debug

## Hardware Necessário
- Placa de desenvolvimento STM32F407VET6
- LED conectado ao pino PA6 (LED1)
- Botão push-button no pino PE3 (K1)
- Resistor de pull-up para o botão (10kΩ recomendado)
- Multímetro ou osciloscópio para verificar saída MCO (opcional)

## Configuração dos Pinos
| Pino | Função | Configuração |
|------|--------|-------------|
| PA6 | LED1 | GPIO Output Push-Pull |
| PE3 | K1 (Botão) | GPIO Input com Pull-up interno |
| PA8 | MCO1 | MCO Output (HSI) |

## Análise Técnica

### Sistema RTC com Alarme
O projeto utiliza o RTC interno do STM32F407 configurado com:
- **Clock Source**: LSI (~32kHz) com prescaler síncrono 127 e assíncrono 255
- **Frequência RTC**: 1Hz para operação de relógio
- **Formato**: 24 horas com data completa

### Configuração do Alarme
```c
// Configuração inicial: 23:59:56
RTC_TimeTypeDef sTime = {
    .Hours = 23,
    .Minutes = 59,
    .Seconds = 56,
    .DayLightSaving = RTC_DAYLIGHTSAVING_NONE,
    .StoreOperation = RTC_STOREOPERATION_RESET
};

// Alarme configurado para: 00:00:00
RTC_AlarmTypeDef sAlarm = {
    .AlarmTime.Hours = 0,
    .AlarmTime.Minutes = 0,
    .AlarmTime.Seconds = 0,
    .AlarmMask = RTC_ALARMMASK_NONE,  // Sem máscara - alarme exato
    .AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL,
    .AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE,
    .AlarmDateWeekDay = 1,
    .Alarm = RTC_ALARM_A
};
```

### Timing do Alarme
- **Hora inicial**: 23:59:56
- **Hora do alarme**: 00:00:00 (próximo dia)
- **Tempo para alarme**: 4 segundos
- **Transição**: Meia-noite com mudança de data

### Sistema de Callback
```c
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}
```

### Cálculos de Prescaler RTC
```
LSI_FREQ ≈ 32000 Hz
PREDIV_A = 127 (prescaler assíncrono)
PREDIV_S = 255 (prescaler síncrono)

f_ck_spre = LSI_FREQ / ((PREDIV_A + 1) × (PREDIV_S + 1))
f_ck_spre = 32000 / (128 × 256) = 32000 / 32768 ≈ 0.977 Hz

Para ajuste mais preciso:
PREDIV_A = 127, PREDIV_S = 249
Resultado: ~1.000 Hz (mais próximo do ideal)
```

## Código Principal

### Inicialização RTC
```c
static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    // Configurar data inicial
    sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
    sDate.Month = RTC_MONTH_OCTOBER;
    sDate.Date = 8;
    sDate.Year = 23;
    
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }

    // Configurar hora inicial (4 segundos antes do alarme)
    sTime.Hours = 23;
    sTime.Minutes = 59;
    sTime.Seconds = 56;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }

    // Configurar alarme para meia-noite
    sAlarm.AlarmTime.Hours = 0;
    sAlarm.AlarmTime.Minutes = 0;
    sAlarm.AlarmTime.Seconds = 0;
    sAlarm.AlarmTime.SubSeconds = 0;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 1;
    sAlarm.Alarm = RTC_ALARM_A;
    
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
}
```

### Loop Principal
```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_RTC_Init();

    while (1)
    {
        // Verificar botão de reset
        if (HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET)
        {
            HAL_Delay(50); // Debounce
            if (HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET)
            {
                // Reset do sistema RTC
                MX_RTC_Init();
                HAL_Delay(500);
            }
        }
        HAL_Delay(100);
    }
}
```

## Aplicações Práticas
1. **Sistemas de Agendamento**: Alarmes para tarefas programadas
2. **Controle de Processos**: Acionamento em horários específicos
3. **Automação Residencial**: Timer para equipamentos
4. **Sistemas de Segurança**: Ativação em períodos determinados
5. **Data Loggers**: Coleta de dados em intervalos precisos
6. **Sistemas de Irrigação**: Acionamento automático
7. **Controle de Iluminação**: Liga/desliga programado

## Vantagens do Sistema de Alarme RTC
- **Precisão**: Alarme baseado em crystal ou LSI calibrado
- **Baixo Consumo**: RTC opera em modos de baixa potência
- **Autonomia**: Mantém funcionamento com backup de bateria
- **Flexibilidade**: Múltiplos alarmes e configurações
- **Interrupt-driven**: Não bloqueia processamento principal

## Troubleshooting

### LED não pisca no alarme
- Verificar conexão do LED no pino PA6
- Confirmar configuração do alarme com HAL_RTC_SetAlarm_IT()
- Verificar habilitação das interrupções RTC no NVIC
- Testar callback com breakpoint

### Alarme não dispara
- Verificar configuração de data/hora inicial
- Confirmar máscara do alarme (RTC_ALARMMASK_NONE)
- Verificar prescalers do RTC para timing correto
- Testar com alarme em segundos ao invés de horas

### Botão K1 não funciona
- Verificar pull-up interno habilitado
- Confirmar lógica invertida (GPIO_PIN_RESET para pressionado)
- Adicionar debounce adequado (50ms mínimo)
- Testar continuidade do botão

### Timing impreciso
- Calibrar prescalers para frequência LSI real
- Considerar usar LSE (32.768kHz) para maior precisão
- Verificar temperatura ambiente (afeta LSI)
- Implementar calibração automática

## Melhorias Sugeridas
1. **Múltiplos Alarmes**: Configurar Alarme A e B
2. **Interface Serial**: Exibir data/hora via UART
3. **Alarme Recorrente**: Repetição automática
4. **Snooze Function**: Função de soneca programável
5. **Backup Domain**: Preservar RTC em reset
6. **LSE Crystal**: Maior precisão temporal
7. **LCD Display**: Mostrar relógio em tempo real
8. **Configuração Dinâmica**: Alterar alarme via interface

## Recursos de Aprendizado
- **Conceitos**: RTC, Alarmes, Interrupções, Timing, Callbacks
- **Periféricos**: RTC, GPIO, NVIC, Clock System
- **HAL Functions**: HAL_RTC_SetAlarm_IT, HAL_RTC_AlarmAEventCallback
- **Debug**: MCO output, LED indicators, Button interface
- **Aplicações**: Sistemas de tempo real, agendamento, automação

Este projeto demonstra implementação avançada de sistema RTC com alarmes programáveis, ideal para aplicações que necessitam de controle temporal preciso e acionamentos automáticos baseados em horário específico.