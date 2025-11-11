# Projeto18 - Frequência de Amostragem 1 📈

![Dificuldade](https://img.shields.io/badge/Dificuldade-Intermediário-yellow.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-ADC1%20+%20DMA%20Sampling-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-40%20min-orange.svg)

## 📋 Descrição

Este projeto explora o controle preciso da **frequência de amostragem** do ADC usando DMA com buffers de tamanho definido. Demonstra como implementar aquisição de dados por lotes (burst sampling) com controle de timing, essencial para aplicações de processamento de sinais e análise espectral.

## 🎯 Objetivos de Aprendizado

- Controlar frequência de amostragem ADC
- Implementar sampling em lotes (burst mode)
- Usar buffers de tamanho fixo para DMA
- Sincronizar aquisição com processamento
- Entender relação entre sampling rate e aplicação

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Gerador de função** ou **potenciômetro**
- **Osciloscópio** (para verificar timing)

## 💻 Principais Conceitos

### Configuração de Sampling Controlado:
```c
uint32_t adc[32];        // Buffer de 32 amostras
uint32_t leitura[1];     // Buffer para single sample
uint8_t i;
bool flag = false;

int main(void)
{
    // Inicialização
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    
    // Iniciar aquisição com DMA
    HAL_ADC_Start_DMA(&hadc1, adc, 32);  // 32 samples
    
    while (1)
    {
        if (flag)  // Quando buffer completo
        {
            flag = false;
            // Processar 32 amostras
            process_samples();
            
            // Reiniciar próxima aquisição
            HAL_ADC_Start_DMA(&hadc1, adc, 32);
        }
    }
}
```

### Callback de Controle:
```c
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    flag = true;  // Sinalizar buffer completo
    HAL_ADC_Stop_DMA(&hadc1);  // Parar para processar
}
```

## ⚙️ Características

- **Buffer size:** 32 amostras por lote
- **Modo:** Burst sampling com pause para processamento
- **DMA:** Transferência automática para buffer
- **Sincronização:** Flag-based entre aquisição e processamento

## 🚀 Aplicações

- **FFT Analysis:** Buffer de tamanho potência de 2
- **Signal Processing:** Processamento em lotes
- **Data Logging:** Aquisição periódica controlada

## 📚 Próximos Passos

- **[Projeto19 - ADC Simultâneo](../Projeto19%20-%20Simult%20ADC/):** Multi-ADC sampling
- **[Projeto17 - ADC com DMA](../Projeto17%20-%20ADC%20com%20DMA/)** (revisitar): Comparar continuous vs burst

---

📈 **Dica:** Controle de sampling rate é crucial para análise de sinais. Escolha buffer size adequado para sua aplicação!