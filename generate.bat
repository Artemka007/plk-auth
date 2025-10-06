@echo off
setlocal EnableDelayedExpansion

:: Пути для отдельных репозиториев
set ODB_EXE=D:\tools\odb-2.5.0-x86_64-windows10\bin\odb.exe
set MODEL_DIR=src\models
set OUTPUT_DIR=src\models\odb-2.5.0
set ODB_INCLUDE=libodb-2.5.0

:: Создаем выходную директорию
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: Проверяем что ODB компилятор существует
if not exist "%ODB_EXE%" (
    echo Error: ODB compiler not found at %ODB_EXE%
    echo Please build ODB compiler first
    pause
    exit /b 1
)

:: Проверяем что core.hxx существует
if not exist "%ODB_INCLUDE%\odb\core.hxx" (
    echo Error: core.hxx not found at %ODB_INCLUDE%\odb\core.hxx
    pause
    exit /b 1
)

echo ODB compiler version:
"%ODB_EXE%" --version

:: Список моделей для генерации
set MODELS=user.hpp user_role.hpp user_role_assignment.hpp access_permission.hpp user_role.hpp system_log.hpp

:: Генерируем код для каждой модели
for %%M in (%MODELS%) do (
    echo.
    echo Generating ODB code for %%M...
    
    "%ODB_EXE%" ^
        -d pgsql ^
        --std c++17 ^
        --generate-query ^
        --generate-schema ^
        --output-dir "%OUTPUT_DIR%" ^
        --hxx-suffix .hpp ^
        --ixx-suffix .ipp ^
        --cxx-suffix .cpp ^
        -I "%ODB_INCLUDE%" ^
        "%MODEL_DIR%\%%M"
    
    if !errorlevel! neq 0 (
        echo ERROR: Failed to generate code for %%M
        pause
        exit /b 1
    ) else (
        echo Success: Generated code for %%M
    )
)

echo.
echo ========================================
echo ODB code generation completed successfully!
echo Generated files are in: %OUTPUT_DIR%
echo ========================================
pause