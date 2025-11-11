# Guia de Contribuição 🤝

Obrigado por seu interesse em contribuir com este projeto! Este guia contém informações sobre como contribuir de forma efetiva.

## 🎯 Como Contribuir

### Tipos de Contribuição Aceitas

- 🐛 **Correção de bugs** em código ou documentação
- 📝 **Melhoria da documentação** existente
- ✨ **Novos projetos** seguindo o padrão estabelecido
- 🔧 **Otimizações** de código ou configurações
- 📊 **Testes** e validação em diferentes placas
- 🌐 **Traduções** para outros idiomas

### Não Aceitos

- Projetos não relacionados a STM32
- Código sem documentação adequada
- Alterações que quebrem compatibilidade sem justificativa
- Contribuições que violem direitos autorais

## 📋 Processo de Contribuição

### 1. Preparação

1. **Fork** o repositório
2. **Clone** seu fork localmente
3. **Crie uma branch** para sua contribuição:
   ```bash
   git checkout -b feature/nome-da-feature
   # ou
   git checkout -b fix/nome-do-bug
   # ou  
   git checkout -b docs/melhoria-documentacao
   ```

### 2. Desenvolvimento

1. **Siga os padrões** estabelecidos no projeto
2. **Teste** thoroughly em hardware real
3. **Documente** adequadamente suas mudanças
4. **Commit** com mensagens descritivas

### 3. Submissão

1. **Push** sua branch para seu fork
2. **Abra um Pull Request** com descrição detalhada
3. **Aguarde review** e responda aos comentários
4. **Faça ajustes** se necessário

## 📝 Padrões de Código

### Nomenclatura de Projetos

```
ProjetoXX - Nome Descritivo/
```

- **XX:** Número sequencial (01, 02, 03...)
- **Nome:** Descrição clara e concisa
- Use hífens para separar palavras
- Evite caracteres especiais

### Estrutura de Diretórios

Mantenha a estrutura padrão do STM32CubeIDE:

```
ProjetoXX - Nome/
├── Core/
│   ├── Inc/
│   └── Src/
├── Drivers/
├── Debug/ (pode ser ignorado no git)
├── README.md
├── *.ioc
└── *.ld
```

### Padrões de Código C

```c
/* Comentários de cabeçalho */
/**
 * @brief Descrição breve da função
 * @param param1 Descrição do parâmetro
 * @retval Descrição do retorno
 */

// Use nomes descritivos
uint16_t valorADC;
float temperaturaCelsius;

// Indentação: 4 espaços (não tabs)
if (condicao) {
    // código aqui
    funcao();
}

// Constantes em MAIÚSCULO
#define LED_PIN GPIO_PIN_0
#define DELAY_MS 1000
```

### Comentários

- **Português ou Inglês** são aceitos
- **Explique o "porquê"**, não apenas o "como"
- **Documente configurações** importantes
- **Inclua referências** quando relevante

## 📚 Padrões de Documentação

### README de Projeto

Siga o template em `TEMPLATE_README.md`:

1. **Badges informativos** (dificuldade, periféricos, tempo)
2. **Descrição clara** do projeto
3. **Objetivos de aprendizado** específicos
4. **Lista de hardware** necessário
5. **Esquema de ligação** (ASCII art aceitável)
6. **Código exemplo** comentado
7. **Instruções de uso** passo-a-passo
8. **Troubleshooting** comum
9. **Referências** técnicas

### Mensagens de Commit

Use o padrão conventional commits:

```bash
feat: adiciona Projeto65 - Ethernet TCP/IP
fix: corrige configuração de clock no Projeto32
docs: melhora README do Projeto41 - RTOS
refactor: otimiza código ADC no Projeto17
test: valida Projeto25 na placa H7A3ZI-Q
```

Tipos aceitos:
- `feat:` nova funcionalidade
- `fix:` correção de bug  
- `docs:` documentação
- `refactor:` refatoração
- `test:` testes
- `chore:` tarefas auxiliares

## 🧪 Testes e Validação

### Antes de Submeter

1. **Compile** sem erros no STM32CubeIDE
2. **Teste** em hardware real (STM32F407VET6 preferencialmente)
3. **Valide** comportamento descrito no README
4. **Verifique** se arquivos de configuração (.ioc) estão corretos

### Documentação de Testes

Inclua no PR:
- **Placa utilizada** para teste
- **Versão do CubeIDE** utilizada
- **Comportamento observado**
- **Screenshots** se relevante

## 🔍 Review Process

### O que Verificamos

1. **Funcionalidade:** Código funciona conforme esperado
2. **Documentação:** README claro e completo
3. **Padrões:** Seguimento dos padrões estabelecidos
4. **Qualidade:** Código limpo e bem estruturado
5. **Compatibilidade:** Não quebra projetos existentes

### Timeline

- **Primeira resposta:** 2-3 dias úteis
- **Review completo:** 1 semana
- **Merge:** Após aprovação e testes

## 🚀 Sugestões de Contribuição

### Projetos Prioritários

1. **Ethernet/TCP-IP:** Comunicação de rede
2. **USB:** Host/Device/OTG
3. **Bootloader:** Sistema de atualização
4. **Criptografia:** AES, SHA, etc.
5. **Machine Learning:** TensorFlow Lite
6. **Protocolos industriais:** Modbus TCP, EtherCAT

### Melhorias na Documentação

1. **Tradução para inglês** dos READMEs
2. **Diagramas de tempo** para protocolos
3. **Videos tutoriais** (links)
4. **Troubleshooting** expandido
5. **Benchmarks** de performance

### Testes e Validação

1. **Outras placas STM32** (F4, H7, G4, etc.)
2. **Diferentes versões** do CubeIDE
3. **Sistemas operacionais** (Windows, Linux, macOS)
4. **Compiladores** alternativos

## 📞 Contato

- **Issues:** Para bugs e sugestões
- **Discussions:** Para perguntas gerais
- **Pull Requests:** Para contribuições de código

### Dúvidas Frequentes

**P: Posso contribuir sem hardware STM32?**
R: Sim! Documentação, reviews e melhorias são sempre bem-vindas.

**P: Preciso seguir exatamente o template?**
R: O template é uma orientação. Adaptações são aceitas se justificadas.

**P: Posso usar bibliotecas externas?**
R: Sim, desde que sejam open source e bem documentadas.

**P: Como sugerir um novo projeto?**
R: Abra uma Issue com tag "enhancement" descrevendo a proposta.

---

🎉 **Obrigado por contribuir! Sua ajuda torna este projeto melhor para toda a comunidade STM32!**