@echo off
REM ==========================================================================
REM Script para limpeza de arquivos desnecessários em projetos STM32CubeIDE
REM ==========================================================================
REM 
REM Este script remove arquivos que não devem estar versionados no Git
REM Execute este script na raiz do workspace antes de fazer commit
REM 
REM Autor: João Pedro
REM Data: 2025-11-11
REM ==========================================================================

echo.
echo ========================================
echo  Limpeza de Arquivos STM32CubeIDE
echo ========================================
echo.

REM Verificar se estamos na raiz do workspace
if not exist ".gitignore" (
    echo ERRO: .gitignore não encontrado!
    echo Execute este script na raiz do workspace.
    pause
    exit /b 1
)

echo Iniciando limpeza de arquivos desnecessários...
echo.

REM ==========================================================================
REM Remover diretórios de build
REM ==========================================================================
echo [1/6] Removendo diretórios Debug e Release...

for /d /r . %%d in (Debug) do (
    if exist "%%d" (
        echo   Removendo: %%d
        rd /s /q "%%d" 2>nul
    )
)

for /d /r . %%d in (Release) do (
    if exist "%%d" (
        echo   Removendo: %%d
        rd /s /q "%%d" 2>nul
    )
)

REM ==========================================================================
REM Remover diretório .metadata
REM ==========================================================================
echo.
echo [2/6] Removendo diretório .metadata...
if exist ".metadata" (
    echo   Removendo: .metadata
    rd /s /q ".metadata" 2>nul
)

REM ==========================================================================
REM Remover arquivos compilados
REM ==========================================================================
echo.
echo [3/6] Removendo arquivos compilados (.elf, .bin, .hex, .map, .list)...

for /r . %%f in (*.elf *.bin *.hex *.map *.list *.o *.obj) do (
    echo   Removendo: %%f
    del "%%f" 2>nul
)

REM ==========================================================================
REM Remover arquivos de configuração do workspace
REM ==========================================================================
echo.
echo [4/6] Removendo arquivos .launch...

for /r . %%f in (*Debug.launch) do (
    echo   Removendo: %%f
    del "%%f" 2>nul
)

REM ==========================================================================
REM Remover arquivos temporários e backup
REM ==========================================================================
echo.
echo [5/6] Removendo arquivos temporários (.tmp, .bak, .log)...

for /r . %%f in (*.tmp *.bak *.log) do (
    echo   Removendo: %%f
    del "%%f" 2>nul
)

REM ==========================================================================
REM Remover arquivos do sistema
REM ==========================================================================
echo.
echo [6/6] Removendo arquivos do sistema (Thumbs.db, Desktop.ini)...

for /r . %%f in (Thumbs.db Desktop.ini .DS_Store) do (
    echo   Removendo: %%f
    del "%%f" 2>nul
)

REM ==========================================================================
REM Verificar git status
REM ==========================================================================
echo.
echo ========================================
echo  Limpeza Concluída!
echo ========================================
echo.

REM Verificar se o Git está disponível
git --version >nul 2>&1
if errorlevel 1 (
    echo Git não encontrado no PATH.
    echo Verifique manualmente os arquivos removidos.
) else (
    echo Status do Git após limpeza:
    echo.
    git status --porcelain
    echo.
    echo Para aplicar as mudanças no Git:
    echo   git add .
    echo   git commit -m "chore: remove build artifacts and temporary files"
)

echo.
echo Pressione qualquer tecla para continuar...
pause >nul