# Projeto19 - ADC Simultâneo 🔄

![Dificuldade](https://img.shields.io/badge/Dificuldade-Avançado-red.svg)
![Periféricos](https://img.shields.io/badge/Periféricos-Multiple%20ADC%20Sync-blue.svg)
![Tempo](https://img.shields.io/badge/Tempo-50%20min-orange.svg)

## 📋 Descrição

Este projeto demonstra técnicas avançadas de **ADC simultâneo** para aquisição sincronizada de múltiplos canais. Explora configurações de múltiplos ADCs para maximizar throughput e sincronização temporal, essencial para aplicações que exigem amostragem simultânea precisa.

## 🎯 Objetivos de Aprendizado

- Configurar múltiplos ADCs simultâneos
- Sincronizar aquisição entre diferentes ADCs
- Maximizar throughput de aquisição
- Entender modos simultaneous/interleaved
- Aplicações de measurement de alta precisão

## 🔧 Hardware Necessário

- **Microcontrolador:** STM32F407VET6
- **Múltiplos sensores** ou **geradores de sinal**
- **Osciloscópio multicanal** (verificar sincronização)

## 💻 Principais Conceitos

### ADC Simultaneous Mode:
```c
// Configuração para ADC1 + ADC2 simultâneos
// ADC1: Master ADC
// ADC2: Slave ADC (sincronizado com ADC1)

ADC_HandleTypeDef hadc1;  // Master
// ADC2 configurado como slave no CubeMX

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init(); 
    MX_ADC1_Init();  // Master ADC
    // ADC2 configurado automaticamente como slave
    
    while (1)
    {
        // Aquisição simultânea em múltiplos canais
        // Timing crítico para aplicações de medição
    }
}
```

## ⚙️ Características

### Modos Disponíveis:
- **Simultaneous mode:** ADC1 e ADC2 convertem ao mesmo tempo
- **Interleaved mode:** ADCs alternados para dobrar sampling rate  
- **Combined mode:** Diferentes combinações de operação

### Vantagens:
- **Sincronização perfeita:** Amostragem simultânea real
- **Alto throughput:** Múltiplos canais sem delay
- **Precisão temporal:** Crítico para medições diferenciais

## 🚀 Aplicações

- **Medição diferencial:** Sinais que devem ser amostrados simultaneamente
- **Análise de fase:** Comparação entre sinais
- **High-speed acquisition:** Máximo throughput possível
- **Multi-sensor systems:** Sincronização de múltiplos sensores

## 📊 Performance

- **Throughput:** Até 2x sampling rate de ADC único
- **Sincronização:** <1 ADC clock cycle de skew
- **Channels:** Até 16 canais simultâneos (ADC1+ADC2)

## 📚 Próximos Passos

Avance para projetos de conversores digitais-analógicos:
- **[Projeto20 - DAC 01](../Projeto20%20-%20DAC%2001/):** Conversão digital-analógica
- **[Projeto17 - ADC com DMA](../Projeto17%20-%20ADC%20com%20DMA/)** (revisitar): Comparar single vs multi-ADC

---

🔄 **Dica:** ADC simultâneo é essencial quando timing relativo entre canais é crítico. Use para medições diferenciais e análise de fase!