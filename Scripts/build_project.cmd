@echo off
REM KOTOR.ai Project Builder
REM Builds the KOTOR_Clone project for UE5

echo 🏗️ KOTOR.ai Project Builder
echo ========================

REM Set project details
set PROJECT_NAME=KOTOR_Clone
set PROJECT_DIR=%~dp0..
set PROJECT_FILE=%PROJECT_DIR%\%PROJECT_NAME%.uproject

REM Check if UE_ENGINE_DIR is set
if "%UE_ENGINE_DIR%"=="" (
    echo ❌ UE_ENGINE_DIR not set!
    echo Please run: set UE_ENGINE_DIR=D:\Epic Games\UE_5.6
    echo (Replace with your actual UE5 path)
    pause
    exit /b 1
)

echo 📁 Project: %PROJECT_FILE%
echo 🎮 UE5 Path: %UE_ENGINE_DIR%
echo.

REM Check if UE5 exists
if not exist "%UE_ENGINE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" (
    echo ❌ UnrealBuildTool not found at: %UE_ENGINE_DIR%
    echo Please check your UE_ENGINE_DIR path
    pause
    exit /b 1
)

echo 🔧 Step 1: Generating project files...
"%UE_ENGINE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="%PROJECT_FILE%" -game -rocket -progress

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Failed to generate project files
    pause
    exit /b %ERRORLEVEL%
)

echo ✅ Project files generated successfully!
echo.

echo 🏗️ Step 2: Building project...
"%UE_ENGINE_DIR%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" %PROJECT_NAME%Editor Win64 Development -Project="%PROJECT_FILE%" -WaitMutex -FromMsBuild

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed with error code %ERRORLEVEL%
    echo.
    echo 💡 Try these solutions:
    echo 1. Open %PROJECT_NAME%.sln in Visual Studio and build there
    echo 2. Check for missing dependencies
    echo 3. Verify UE5 installation is complete
    pause
    exit /b %ERRORLEVEL%
)

echo ✅ Build completed successfully!
echo.
echo 🎮 You can now:
echo 1. Double-click %PROJECT_NAME%.uproject to open in UE5
echo 2. Or run: Scripts\run_game.cmd --map TestLevel --windowed
echo.
pause
