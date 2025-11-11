#!/bin/bash

# ==========================================================================
# Script para limpeza de arquivos desnecessários em projetos STM32CubeIDE
# ==========================================================================
# 
# Este script remove arquivos que não devem estar versionados no Git
# Execute este script na raiz do workspace antes de fazer commit
# 
# Uso: ./cleanup-workspace.sh
# 
# Autor: João Pedro
# Data: 2025-11-11
# ==========================================================================

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}"
echo "========================================"
echo "  Limpeza de Arquivos STM32CubeIDE"
echo "========================================"
echo -e "${NC}"

# Verificar se estamos na raiz do workspace
if [ ! -f ".gitignore" ]; then
    echo -e "${RED}ERRO: .gitignore não encontrado!${NC}"
    echo "Execute este script na raiz do workspace."
    exit 1
fi

echo "Iniciando limpeza de arquivos desnecessários..."
echo

# ==========================================================================
# Função para contar e remover arquivos
# ==========================================================================
remove_files() {
    local pattern="$1"
    local description="$2"
    local count=0
    
    echo -e "${YELLOW}$description${NC}"
    
    while IFS= read -r -d '' file; do
        echo "  Removendo: $file"
        rm -rf "$file"
        ((count++))
    done < <(find . -name "$pattern" -print0 2>/dev/null)
    
    echo "  Total removido: $count arquivos/diretórios"
    echo
}

# ==========================================================================
# Remover diretórios de build
# ==========================================================================
echo -e "${GREEN}[1/8] Removendo diretórios Debug e Release...${NC}"
remove_files "Debug" "Diretórios Debug:"
remove_files "Release" "Diretórios Release:"

# ==========================================================================
# Remover diretório .metadata
# ==========================================================================
echo -e "${GREEN}[2/8] Removendo diretório .metadata...${NC}"
if [ -d ".metadata" ]; then
    echo "  Removendo: .metadata"
    rm -rf ".metadata"
    echo "  Total removido: 1 diretório"
else
    echo "  .metadata não encontrado"
fi
echo

# ==========================================================================
# Remover arquivos compilados
# ==========================================================================
echo -e "${GREEN}[3/8] Removendo arquivos compilados...${NC}"
find . \( -name "*.elf" -o -name "*.bin" -o -name "*.hex" -o -name "*.map" -o -name "*.list" -o -name "*.o" -o -name "*.obj" \) -type f | while read file; do
    echo "  Removendo: $file"
    rm -f "$file"
done
echo

# ==========================================================================
# Remover arquivos de configuração do workspace
# ==========================================================================
echo -e "${GREEN}[4/8] Removendo arquivos .launch...${NC}"
find . -name "*Debug.launch" -type f | while read file; do
    echo "  Removendo: $file"
    rm -f "$file"
done
echo

# ==========================================================================
# Remover arquivos XML de configuração
# ==========================================================================
echo -e "${GREEN}[5/8] Removendo arquivos XML de configuração...${NC}"
find . -name "*.language.settings.xml" -type f | while read file; do
    echo "  Removendo: $file"
    rm -f "$file"
done
echo

# ==========================================================================
# Remover arquivos temporários e backup
# ==========================================================================
echo -e "${GREEN}[6/8] Removendo arquivos temporários...${NC}"
find . \( -name "*.tmp" -o -name "*.bak" -o -name "*.log" -o -name "*~" \) -type f | while read file; do
    echo "  Removendo: $file"
    rm -f "$file"
done
echo

# ==========================================================================
# Remover arquivos do sistema operacional
# ==========================================================================
echo -e "${GREEN}[7/8] Removendo arquivos do sistema...${NC}"
find . \( -name "Thumbs.db" -o -name "Desktop.ini" -o -name ".DS_Store" -o -name "._*" \) -type f | while read file; do
    echo "  Removendo: $file"
    rm -f "$file"
done
echo

# ==========================================================================
# Remover diretórios vazios
# ==========================================================================
echo -e "${GREEN}[8/8] Removendo diretórios vazios...${NC}"
find . -type d -empty | while read dir; do
    if [ "$dir" != "." ]; then
        echo "  Removendo diretório vazio: $dir"
        rmdir "$dir" 2>/dev/null
    fi
done
echo

# ==========================================================================
# Verificar git status
# ==========================================================================
echo -e "${BLUE}"
echo "========================================"
echo "  Limpeza Concluída!"
echo "========================================"
echo -e "${NC}"

# Verificar se o Git está disponível
if command -v git &> /dev/null; then
    echo "Status do Git após limpeza:"
    echo
    git status --porcelain
    echo
    echo -e "${YELLOW}Para aplicar as mudanças no Git:${NC}"
    echo "  git add ."
    echo "  git commit -m \"chore: remove build artifacts and temporary files\""
else
    echo -e "${YELLOW}Git não encontrado no PATH.${NC}"
    echo "Verifique manualmente os arquivos removidos."
fi

echo
echo -e "${GREEN}Script executado com sucesso!${NC}"