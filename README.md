# Projetos Iniciais STM32 🚀

[![STM32](https://img.shields.io/badge/STM32-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html)
[![HAL](https://img.shields.io/badge/HAL-STM32-green.svg)](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html)
[![FreeRTOS](https://img.shields.io/badge/FreeRTOS-Real--Time-orange.svg)](https://www.freertos.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Uma coleção abrangente de projetos educacionais para aprender desenvolvimento em microcontroladores STM32, cobrindo desde conceitos básicos até implementações avançadas com sistemas operacionais em tempo real.

## 📋 Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Placas Utilizadas](#placas-utilizadas)
- [Ferramentas e Software](#ferramentas-e-software)
- [Estrutura dos Projetos](#estrutura-dos-projetos)
- [Guia de Projetos](#guia-de-projetos)
- [Como Usar](#como-usar)
- [Pré-requisitos](#pré-requisitos)
- [Instalação](#instalação)
- [Contribuição](#contribuição)
- [Licença](#licença)
- [Contato](#contato)

## 🎯 Sobre o Projeto

Este repositório contém uma série progressiva de projetos desenvolvidos para ensinar os fundamentos e conceitos avançados de programação em microcontroladores STM32. Os projetos estão organizados de forma didática, partindo de conceitos básicos como controle de GPIO até implementações complexas envolvendo protocolos de comunicação, sistemas em tempo real e aplicações industriais.

### Objetivos Educacionais:
- 📚 Aprender os fundamentos dos microcontroladores STM32
- 🔧 Dominar a HAL (Hardware Abstraction Layer) da STMicroelectronics
- ⚡ Implementar sistemas em tempo real com FreeRTOS
- 🌐 Trabalhar com protocolos de comunicação (UART, SPI, I2C, CAN)
- 📊 Desenvolver sistemas de aquisição de dados e controle
- 🔒 Implementar sistemas de baixo consumo e gerenciamento de energia

## 🛠️ Placas Utilizadas

### Principal
- **STM32F407VET6** - Utilizada na maioria dos projetos
  - ARM Cortex-M4 @ 168MHz
  - 512KB Flash, 192KB RAM
  - Amplo conjunto de periféricos
  - Suporte completo para desenvolvimento

### Secundária
- **NUCLEO-H7A3ZI-Q** - Projetos específicos com FDCAN
  - ARM Cortex-M7 @ 280MHz
  - 2MB Flash, 1MB RAM
  - Suporte para FDCAN (CAN FD)

## 🔧 Ferramentas e Software

- **STM32CubeIDE** - IDE principal para desenvolvimento
- **STM32CubeMX** - Configuração gráfica de periféricos
- **HAL Library** - Biblioteca de abstração de hardware
- **FreeRTOS** - Sistema operacional em tempo real
- **Git** - Controle de versão

## 📁 Estrutura dos Projetos

Cada projeto segue uma estrutura padrão do STM32CubeIDE:

```
ProjetoXX - Nome/
├── Core/
│   ├── Inc/           # Headers (.h)
│   └── Src/           # Código fonte (.c)
├── Drivers/           # Drivers HAL/LL
├── Debug/             # Arquivos de debug
├── *.ioc              # Arquivo de configuração STM32CubeMX
├── *.ld               # Linker scripts
└── README.md          # Documentação específica do projeto
```

## 📖 Guia de Projetos

### 🟢 Projetos Básicos (01-12)
Fundamentos de GPIO, timers e controle básico.

- **[Projeto01](./Projeto01/)** - Primeiro projeto
- **[Projeto02 - Debugger](./Projeto02%20-%20Debugger/)** - Uso do debugger
- **[Projeto03 - Toggle LED](./Projeto03%20-%20Toggle%20led/)** - Controle básico de LED
- **[Projeto04 - Bootloader Blink](./Projeto04%20-%20Bootloader%20Blink/)** - LED piscante ([📖 README](./Projeto04%20-%20Bootloader%20Blink/README.md))
- **[Projeto05 - Clock1](./Projeto05%20-%20Clock1/)** - Configuração de clock ([📖 README](./Projeto05%20-%20Clock1/README.md))
- **[Projeto06 - GPIO1](./Projeto06%20-%20GPIO1/)** - Manipulação de GPIO ([📖 README](./Projeto06%20-%20GPIO1/README.md))
- **[Projeto07 - GPIO Interrupt](./Projeto07%20-%20GPIO%20Interrupt/)** - Interrupções de GPIO ([📖 README](./Projeto07%20-%20GPIO%20Interrupt/README.md))
- **[Projeto08 - Timers](./Projeto08%20-%20Timers/)** - Uso básico de timers ([📖 README](./Projeto08%20-%20Timers/README.md))
- **[Projeto09 - Timer](./Projeto09%20-%20Timer/)** - Timer avançado ([📖 README](./Projeto09%20-%20Timer/README.md))
- **[Projeto10 - Timer Interrupt](./Projeto10%20-%20Timer%20Interrupt/)** - Interrupções de timer ([📖 README](./Projeto10%20-%20Timer%20Interrupt/README.md))
- **[Projeto11 - Output Compare](./Projeto11%20-%20Output%20Compare%20OC/)** - Comparação de saída ([📖 README](./Projeto11%20-%20Output%20Compare%20OC/README.md))
- **[Projeto12 - Input Capture](./Projeto12%20-%20Input%20Capture%20IC/)** - Captura de entrada ([📖 README](./Projeto12%20-%20Input%20Capture%20IC/README.md))

### 🔵 Conversores Analógico-Digitais (13-19)
ADC, sensores e aquisição de dados.

- **[Projeto13 - ADC Simples](./Projeto13%20-%20ADC%20Simples/)** - ADC básico ([📖 README](./Projeto13%20-%20ADC%20Simples/README.md))
- **[Projeto14 - ADC Simples 2](./Projeto14%20-%20ADC%20Simples%202/)** - ADC múltiplos canais ([📖 README](./Projeto14%20-%20ADC%20Simples%202/README.md))
- **[Projeto15 - Sensor Temperatura](./Projeto15%20-%20Sensor%20de%20temperatura%20interno/)** - Sensor interno ([📖 README](./Projeto15%20-%20Sensor%20de%20temperatura%20interno/README.md))
- **[Projeto16 - ADC Interrupção](./Projeto16%20-%20ADC%20Interrupcao/)** - ADC com interrupções ([📖 README](./Projeto16%20-%20ADC%20Interrupcao/README.md))
- **[Projeto17 - ADC com DMA](./Projeto17%20-%20ADC%20com%20DMA/)** - ADC com DMA ([📖 README](./Projeto17%20-%20ADC%20com%20DMA/README.md))
- **[Projeto18 - Frequência Amostragem](./Projeto18%20-%20Frequencia%20de%20amostrage%201/)** - Taxa de amostragem ([📖 README](./Projeto18%20-%20Frequencia%20de%20amostrage%201/README.md))
- **[Projeto19 - ADC Simultâneo](./Projeto19%20-%20Simult%20ADC/)** - ADC simultâneo ([📖 README](./Projeto19%20-%20Simult%20ADC/README.md))

### 🟡 Conversores Digital-Analógicos (20-25)
DAC e geração de sinais.

- **[Projeto20 - DAC 01](./Projeto20%20-%20DAC%2001/)** - DAC básico ([📖 README](./Projeto20%20-%20DAC%2001/README.md))
- **[Projeto21 - DAC 02](./Projeto21%20-%20DAC%2002/)** - DAC dual-channel ([📖 README](./Projeto21%20-%20DAC%2002/README.md))
- **[Projeto22 - DAC 03](./Projeto22%20-%20DAC%2003/)** - DAC com DMA ([📖 README](./Projeto22%20-%20DAC%2003/README.md))
- **[Projeto23 - DAC 04](./Projeto23%20-%20DAC%2004/)** - Gerador senoidal ([📖 README](./Projeto23%20-%20DAC%2004/README.md))
- **[Projeto24 - DAC 05](./Projeto24%20-%20DAC%2005/)** - Onda triangular ([📖 README](./Projeto24%20-%20DAC%2005/README.md))
- **[Projeto25 - DAC PWM](./Projeto25%20-%20DAC_PWM/)** - PWM como DAC ([📖 README](./Projeto25%20-%20DAC_PWM/README.md))

### 🔴 Gerenciamento de Energia (26-35)
Watchdog, modos de baixo consumo e RTC.

- **[Projeto26 - IWDG](./Projeto26%20-%20WatchDog%20IWDG/)** - Independent Watchdog ([📖 README](./Projeto26%20-%20WatchDog%20IWDG/README.md))
- **[Projeto27 - WWDG](./Projeto27%20-%20WatchDog%20WWDG/)** - Window Watchdog ([📖 README](./Projeto27%20-%20WatchDog%20WWDG/README.md))
- **[Projeto28 - Sleep Mode](./Projeto28%20-%20Sleep%20Mode%2001/)** - Modo sleep ([📖 README](./Projeto28%20-%20Sleep%20Mode%2001/README.md))
- **[Projeto29 - Stop Mode](./Projeto29%20-%20Stop%20Mode/)** - Modo stop
- **[Projeto30 - Shutdown Mode](./Projeto30%20-%20Shutdown%20Mode/)** - Modo shutdown ([📖 README](./Projeto30%20-%20Shutdown%20Mode/README.md))
- **[Projeto31 - Standby Mode](./Projeto31%20-%20Standby%20Mode/)** - Modo standby ([📖 README](./Projeto31%20-%20Standby%20Mode/README.md))
- **[Projeto32 - RTC 1](./Projeto32%20-%20RTC%201/)** - Real Time Clock básico ([📖 README](./Projeto32%20-%20RTC%201/README.md))
- **[Projeto33 - RTC 2](./Projeto33%20-%20RTC%202/)** - RTC com LSE e MCO ([📖 README](./Projeto33%20-%20RTC%202/README.md))
- **[Projeto34 - RTC 3](./Projeto34%20-%20RTC%203/)** - RTC com datetime específico ([📖 README](./Projeto34%20-%20RTC%203/README.md))
- **[Projeto35 - RTC 4](./Projeto35%20-%20RTC%204/)** - RTC com sistema de alarme ([📖 README](./Projeto35%20-%20RTC%204/README.md))

### 🟣 Comunicação Serial (36-40)
UART, SPI, I2C e sensores.

- **[Projeto36 - Serial 01](./Projeto36%20-%20Serial%2001/)** - UART básica ([📖 README](./Projeto36%20-%20Serial%2001/README.md))
- **[Projeto37 - Serial 02](./Projeto37%20-%20Serial%2002/)** - UART com interrupção ([📖 README](./Projeto37%20-%20Serial%2002/README.md))
- **[Projeto38 - Serial 03](./Projeto38%20-%20Serial%2003/)** - UART com DMA ([📖 README](./Projeto38%20-%20Serial%2003/README.md))
- **[Projeto39 - I2C ADXL345](./Projeto39%20-%20I2C%20ADXL345/)** - Acelerômetro I2C ([📖 README](./Projeto39%20-%20I2C%20ADXL345/README.md))
- **[Projeto40 - SPI ADXL345](./Projeto40%20-%20SPI%20ADXL345/)** - Acelerômetro SPI ([📖 README](./Projeto40%20-%20SPI%20ADXL345/README.md))

### 🟠 Sistemas em Tempo Real (41-48)
FreeRTOS e programação concorrente.

- **[Projeto41 - RTOS 01](./Projeto41%20-%20RTOS%2001/)** - Introdução ao FreeRTOS ([📖 README](./Projeto41%20-%20RTOS%2001/README.md))
- **[Projeto42 - RTOS 02](./Projeto42%20-%20RTOS%2002/)** - Tarefas múltiplas ([📖 README](./Projeto42%20-%20RTOS%2002/README.md))
- **[Projeto43 - RTOS 03](./Projeto43%20-%20RTOS%2003/)** - Semáforos binários ([📖 README](./Projeto43%20-%20RTOS%2003/README.md))
- **[Projeto44 - RTOS 04](./Projeto44%20-%20RTOS%2004/)** - Semáforos contadores ([📖 README](./Projeto44%20-%20RTOS%2004/README.md))
- **[Projeto45 - RTOS 05](./Projeto45%20-%20RTOS%2005/)** - Mutex ([📖 README](./Projeto45%20-%20RTOS%2005/README.md))
- **[Projeto46 - RTOS 06](./Projeto46%20-%20RTOS%2006/)** - Filas de mensagens ([📖 README](./Projeto46%20-%20RTOS%2006/README.md))
- **[Projeto47 - RTOS 07](./Projeto47%20-%20RTOS%2007/)** - Software timers ([📖 README](./Projeto47%20-%20RTOS%2007/README.md))
- **[Projeto48 - RTOS 08](./Projeto48%20-%20RTOS%2008/)** - Event flags ([📖 README](./Projeto48%20-%20RTOS%2008/README.md))

### 🔶 Comunicação CAN (49-56)
Protocolos CAN e CAN FD.

- **[Projeto49 - CAN Loopback](./Projeto49%20-%20CAN%20Loopback/)** - CAN modo loopback ([📖 README](./Projeto49%20-%20CAN%20Loopback/README.md))
- **[Projeto50 - CAN Normal](./Projeto50%20-%20CAN%20Normal%20mode/)** - CAN modo normal ([📖 README](./Projeto50%20-%20CAN%20Normal%20mode/README.md))
- **[Projeto51 - CAN Interrupt](./Projeto51%20-%20CAN%20Loopback%20Interrupt/)** - CAN com interrupção ([📖 README](./Projeto51%20-%20CAN%20Loopback%20Interrupt/README.md))
- **[Projeto52 - CAN Filter](./Projeto52%20-%20CAN%20Loopback%20Filter%20ID%20Mask/)** - Filtros CAN ([📖 README](./Projeto52%20-%20CAN%20Loopback%20Filter%20ID%20Mask/README.md))
- **[Projeto53 - CAN Filter Advanced](./Projeto53%20-%20CAN%20Filter/)** - Filtros avançados ([📖 README](./Projeto53%20-%20CAN%20Filter/README.md))
- **[Projeto54 - FDCAN 1](./Projeto54%20-%20FDCAN%201%20Loopback/)** - FDCAN loopback ([📖 README](./Projeto54%20-%20FDCAN%201%20Loopback/README.md))
- **[Projeto55 - FDCAN 2](./Projeto55%20-%20FDCAN%202%20Loopback/)** - FDCAN avançado ([📖 README](./Projeto55%20-%20FDCAN%202%20Loopback/README.md))
- **[Projeto56 - FDCAN Normal](./Projeto56%20-%20FDCAN%20in%20Normal%20Mode/)** - FDCAN normal ([📖 README](./Projeto56%20-%20FDCAN%20in%20Normal%20Mode/README.md))

### 🔷 Projetos Avançados (57-63)
Aplicações complexas e industriais.

- **[Projeto57 - SD Card 1](./Projeto57%20-%20SD%20Card%201/)** - Interface SD Card ([📖 README](./Projeto57%20-%20SD%20Card%201/README.md))
- **[Projeto58 - SD DataLogger](./Projeto58%20-%20SD%20Card%20DataLogger/)** - Logger de dados ([📖 README](./Projeto58%20-%20SD%20Card%20DataLogger/README.md))
- **[Projeto59 - LCD 16x2](./Projeto59%20-%20LCD%2016x2/)** - Display LCD ([📖 README](./Projeto59%20-%20LCD%2016x2/README.md))
- **[Projeto60 - CRC Modbus](./Projeto60%20-%20CRC%20Modbus/)** - Cálculo CRC ([📖 README](./Projeto60%20-%20CRC%20Modbus/README.md))
- **[Projeto61 - Modbus RTU](./Projeto61%20-%20Modbus%20RTU%20Serial/)** - Protocolo Modbus
- **[Projeto62 - TCP](./Projeto62%20-%20TCP/)** - Comunicação TCP/IP
- **[Projeto63 - Reset Reason](./Projeto63%20-%20Reset%20Reason/)** - Análise de reset

### 🟪 Quantum Platform (QP)
Framework para sistemas de eventos.

- **[QP Projeto01 - Blink](./QP%20-%20Projeto01%20-%20Bink/)** - LED com QP
- **[QP Projeto02 - Timer](./QP%20-%20Projeto02%20-%20Bink%20timer/)** - Timer com QP
- **[QP Projeto03 - Pub-Sub](./QP%20-%20Projeto03%20-%20Publish-Subscribe/)** - Publish-Subscribe
- **[QP Projeto04 - Eventos](./QP%20-%20Projeto04%20-%20Eventos%20dinamicos/)** - Eventos dinâmicos
- **[QP Projeto05 - FreeRTOS](./QP%20-%20Projeto05%20-%20Port%20para%20FreeRTOS/)** - QP com FreeRTOS

## 🚀 Como Usar

### Pré-requisitos

- **STM32CubeIDE** versão 1.10.0 ou superior
- **STM32CubeMX** (incluído no CubeIDE)
- **Placa STM32F407VET6** ou compatível
- **ST-Link** ou programador compatível
- **Cabo USB** para programação/debug
- **Git** para controle de versão

### Instalação

1. **Clone o repositório:**
   ```bash
   git clone https://github.com/joaopedro0810/Projetos-iniciais-STM32.git
   cd Projetos-iniciais-STM32
   ```

2. **Limpeza inicial (opcional):**
   ```bash
   # Windows
   cleanup-workspace.bat
   
   # Linux/macOS
   ./cleanup-workspace.sh
   ```

3. **Abra o STM32CubeIDE:**
   - File → Import → Existing Projects into Workspace
   - Selecione a pasta do projeto desejado

4. **Configure o target:**
   - Verifique se a placa está conectada
   - Configure o debugger (ST-Link)

5. **Compile e execute:**
   - Build Project (Ctrl+B)
   - Run/Debug (F11)

### Navegação dos Projetos

Recomendamos seguir a ordem numérica dos projetos, pois cada um constrói conhecimento sobre o anterior. Cada projeto possui:

1. **README.md específico** com documentação detalhada
2. **Código comentado** explicando cada funcionalidade
3. **Arquivo .ioc** para visualizar configurações no CubeMX
4. **Esquemas de ligação** quando necessário

## 📁 Estrutura e Versionamento

### .gitignore Otimizado
Este repositório utiliza um `.gitignore` otimizado que:
- **Ignora arquivos de build** (Debug/, *.elf, *.bin, etc.)
- **Remove metadados** do STM32CubeIDE (.metadata/, *.launch)
- **Exclui arquivos temporários** (*.tmp, *.bak, logs)
- **Mantém apenas código essencial** para o projeto

📖 **Veja o [Guia do .gitignore](GITIGNORE_GUIDE.md)** para detalhes completos.

### Scripts de Limpeza
Use os scripts fornecidos para limpar arquivos desnecessários:

```bash
# Windows
cleanup-workspace.bat

# Linux/macOS  
./cleanup-workspace.sh
```

## 🤝 Contribuição

Contribuições são sempre bem-vindas! Para contribuir:

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. **Execute limpeza:** `cleanup-workspace.bat` (Windows) ou `./cleanup-workspace.sh` (Linux/macOS)
4. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
5. Push para a branch (`git push origin feature/AmazingFeature`)
6. Abra um Pull Request

### Diretrizes para Contribuição:
- Mantenha o padrão de nomenclatura dos projetos
- Documente adequadamente seu código
- Inclua README.md específico para novos projetos
- **Execute scripts de limpeza** antes de commits
- Teste em hardware real antes de submeter

📖 **Leia o [Guia de Contribuição](CONTRIBUTING.md)** para informações detalhadas.

## 📄 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## 📞 Contato

**João Pedro** - [@joaopedro0810](https://github.com/joaopedro0810)

Link do Projeto: [https://github.com/joaopedro0810/Projetos-iniciais-STM32](https://github.com/joaopedro0810/Projetos-iniciais-STM32)

---

⭐ **Se este projeto foi útil para você, considere dar uma estrela!**

💡 **Sugestões e melhorias são sempre bem-vindas através das Issues!**
