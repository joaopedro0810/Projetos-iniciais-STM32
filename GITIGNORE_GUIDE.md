# Guia do .gitignore para Projetos STM32 📁

Este documento explica as regras do arquivo `.gitignore` utilizadas neste repositório de projetos STM32CubeIDE.

## 🎯 Objetivo

O `.gitignore` é configurado para:
- **Reduzir o tamanho** do repositório
- **Evitar conflitos** entre diferentes máquinas/usuários
- **Manter apenas arquivos essenciais** para o projeto
- **Facilitar clonagem** e setup em novos ambientes

## 📋 Categorias de Arquivos Ignorados

### 🔨 Arquivos de Build e Compilação
```gitignore
Debug/
Release/
*.o
*.elf
*.bin
*.hex
*.map
*.list
```
**Motivo:** Gerados automaticamente durante compilação, específicos da máquina.

### ⚙️ Metadados do Eclipse/STM32CubeIDE
```gitignore
.metadata/
.settings/
*.launch
.project
.cproject
```
**Motivo:** Configurações específicas do workspace/usuário, podem causar conflitos.

### 🗂️ Arquivos Temporários
```gitignore
*.tmp
*.bak
*.log
*.cache
```
**Motivo:** Arquivos temporários que não agregam valor ao projeto.

### 💻 Arquivos do Sistema Operacional
```gitignore
# Windows
Thumbs.db
Desktop.ini

# macOS  
.DS_Store

# Linux
*~
```
**Motivo:** Específicos do OS, não relacionados ao projeto.

## ✅ Arquivos que DEVEM ser Versionados

### Essenciais para o Projeto:
- **`*.ioc`** - Arquivos de configuração do CubeMX
- **`*.ld`** - Linker scripts
- **`Core/Src/`** - Código fonte principal
- **`Core/Inc/`** - Headers do projeto
- **`README.md`** - Documentação
- **`Makefile`** - Se usado

### Drivers e Middlewares:
Por padrão, **incluímos** os drivers HAL no repositório para garantir compatibilidade. Se preferir usar como submódulos:

```gitignore
# Descomente para excluir drivers:
# Drivers/STM32F4xx_HAL_Driver/
# Drivers/CMSIS/
```

## 🔧 Configuração Local

### Para projetos existentes:
Se já tem arquivos que deveriam ser ignorados:

```bash
# Remove arquivos já trackados
git rm -r --cached Debug/
git rm -r --cached .metadata/
git rm *.launch

# Commit as mudanças
git add .gitignore
git commit -m "chore: update .gitignore and remove tracked build files"
```

### Para novos projetos:
O `.gitignore` funciona automaticamente para novos arquivos.

## 🚨 Casos Especiais

### Arquivos de Configuração de Debug
```gitignore
*.launch
*Debug.launch
```
**Exceção:** Se sua configuração de debug for específica e importante para o projeto, adicione:
```gitignore
!projeto-especifico.launch
```

### Bibliotecas Customizadas
Se você tem bibliotecas próprias em `Middlewares/`:
```gitignore
# Ignora bibliotecas third-party
Middlewares/Third_Party/

# Mas mantém suas bibliotecas
!Middlewares/Custom/
```

## 📊 Comparação de Tamanhos

### Sem .gitignore:
- **Repositório completo:** ~500MB+
- **Cada clone:** ~500MB+
- **Conflitos frequentes** em .metadata

### Com .gitignore otimizado:
- **Repositório essencial:** ~50MB
- **Clone inicial:** ~50MB
- **Sem conflitos** de configuração

## 🔍 Verificação

### Comandos úteis para verificar o .gitignore:

```bash
# Verificar arquivos ignorados
git status --ignored

# Verificar se arquivo específico está sendo ignorado
git check-ignore Debug/projeto.elf

# Listar todos os arquivos trackados
git ls-files

# Verificar tamanho do repositório
git count-objects -vH
```

## 🛠️ Customização

### Para seu ambiente específico:
Crie um `.gitignore` local (não versionado):

```bash
# .git/info/exclude
# Configurações pessoais que não devem ir para o repositório
meu-script-pessoal.sh
notas-privadas.txt
```

### Para diferentes tipos de projeto:
- **Projeto com FreeRTOS:** Pode incluir configurações específicas
- **Projeto com Ethernet:** Ignorar logs de rede
- **Projeto comercial:** Incluir configurações de criptografia

## 📚 Recursos Adicionais

- [Git .gitignore Documentation](https://git-scm.com/docs/gitignore)
- [GitHub .gitignore Templates](https://github.com/github/gitignore)
- [STM32CubeIDE User Guide](https://www.st.com/resource/en/user_manual/um2609-stm32cubeide-user-guide-stmicroelectronics.pdf)

## ❓ FAQ

**P: Por que não ignorar os arquivos .ioc?**
R: São essenciais para recriar a configuração do projeto em outras máquinas.

**P: Posso ignorar todo o diretório Drivers/?**
R: Sim, se usar drivers como submódulos git. Caso contrário, mantenha para compatibilidade.

**P: O que fazer se acidentalmente commitei arquivos do Debug/?**
R: Use `git rm -r --cached Debug/` para remover do tracking sem deletar localmente.

**P: Como ignorar arquivos já trackados?**
R: Use `git rm --cached <arquivo>` e depois adicione ao .gitignore.

---

💡 **Dica:** Mantenha o .gitignore atualizado conforme seu projeto evolui!