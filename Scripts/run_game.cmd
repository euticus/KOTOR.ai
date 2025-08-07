@echo off
REM KOTOR.ai Game Launch Script for Windows
REM Direct map launch bypassing editor

setlocal enabledelayedexpansion

REM ============================================================================
REM Configuration
REM ============================================================================

REM Project settings
set PROJECT_NAME=KOTOR_Clone
set PROJECT_DIR=%~dp0..
set PROJECT_FILE=%PROJECT_DIR%\%PROJECT_NAME%.uproject

REM Default launch settings
set DEFAULT_MAP=TestLevel
set DEFAULT_GAME_MODE=BP_KOTORGameMode
set DEFAULT_RESOLUTION=1920x1080
set DEFAULT_WINDOWED=true
set DEFAULT_LOG_LEVEL=Log

REM Engine paths (auto-detect or set manually)
set UE_ENGINE_DIR=
set UE_EDITOR_CMD=

REM ============================================================================
REM Command Line Arguments
REM ============================================================================

set MAP_NAME=%DEFAULT_MAP%
set GAME_MODE=%DEFAULT_GAME_MODE%
set RESOLUTION=%DEFAULT_RESOLUTION%
set WINDOWED=%DEFAULT_WINDOWED%
set LOG_LEVEL=%DEFAULT_LOG_LEVEL%
set ADDITIONAL_ARGS=
set SHOW_HELP=false
set VERBOSE=false

:parse_args
if "%~1"=="" goto :args_done
if "%~1"=="-h" set SHOW_HELP=true
if "%~1"=="--help" set SHOW_HELP=true
if "%~1"=="-m" (
    set MAP_NAME=%~2
    shift
)
if "%~1"=="--map" (
    set MAP_NAME=%~2
    shift
)
if "%~1"=="-g" (
    set GAME_MODE=%~2
    shift
)
if "%~1"=="--gamemode" (
    set GAME_MODE=%~2
    shift
)
if "%~1"=="-r" (
    set RESOLUTION=%~2
    shift
)
if "%~1"=="--resolution" (
    set RESOLUTION=%~2
    shift
)
if "%~1"=="-w" set WINDOWED=true
if "%~1"=="--windowed" set WINDOWED=true
if "%~1"=="-f" set WINDOWED=false
if "%~1"=="--fullscreen" set WINDOWED=false
if "%~1"=="-v" set VERBOSE=true
if "%~1"=="--verbose" set VERBOSE=true
if "%~1"=="--log" (
    set LOG_LEVEL=%~2
    shift
)
if "%~1"=="--args" (
    set ADDITIONAL_ARGS=%~2
    shift
)
shift
goto :parse_args

:args_done

REM ============================================================================
REM Help Display
REM ============================================================================

if "%SHOW_HELP%"=="true" (
    echo KOTOR.ai Game Launch Script
    echo.
    echo Usage: run_game.cmd [OPTIONS]
    echo.
    echo Options:
    echo   -h, --help              Show this help message
    echo   -m, --map MAP           Map to load ^(default: %DEFAULT_MAP%^)
    echo   -g, --gamemode MODE     Game mode to use ^(default: %DEFAULT_GAME_MODE%^)
    echo   -r, --resolution RES    Screen resolution ^(default: %DEFAULT_RESOLUTION%^)
    echo   -w, --windowed          Run in windowed mode
    echo   -f, --fullscreen        Run in fullscreen mode
    echo   -v, --verbose           Enable verbose logging
    echo   --log LEVEL             Set log level ^(Log, Warning, Error^)
    echo   --args ARGS             Additional command line arguments
    echo.
    echo Examples:
    echo   run_game.cmd --map Taris --windowed
    echo   run_game.cmd --map Dantooine --resolution 2560x1440 --fullscreen
    echo   run_game.cmd --verbose --log Warning
    echo.
    goto :end
)

REM ============================================================================
REM Engine Detection
REM ============================================================================

if "%VERBOSE%"=="true" echo [INFO] Detecting Unreal Engine installation...

REM Try to find UE5 installation
if "%UE_ENGINE_DIR%"=="" (
    REM Check common installation paths across multiple drives
    for %%d in (
        "C:\Program Files\Epic Games\UE_5.1"
        "C:\Program Files\Epic Games\UE_5.2"
        "C:\Program Files\Epic Games\UE_5.3"
        "C:\Program Files\Epic Games\UE_5.4"
        "C:\UnrealEngine"
        "D:\Program Files\Epic Games\UE_5.1"
        "D:\Program Files\Epic Games\UE_5.2"
        "D:\Program Files\Epic Games\UE_5.3"
        "D:\Program Files\Epic Games\UE_5.4"
        "D:\UnrealEngine"
        "D:\Epic Games\UE_5.1"
        "D:\Epic Games\UE_5.2"
        "D:\Epic Games\UE_5.3"
        "D:\Epic Games\UE_5.4"
        "E:\Program Files\Epic Games\UE_5.1"
        "E:\Program Files\Epic Games\UE_5.2"
        "E:\Program Files\Epic Games\UE_5.3"
        "E:\Program Files\Epic Games\UE_5.4"
        "E:\UnrealEngine"
        "E:\Epic Games\UE_5.1"
        "E:\Epic Games\UE_5.2"
        "E:\Epic Games\UE_5.3"
        "E:\Epic Games\UE_5.4"
        "F:\Program Files\Epic Games\UE_5.1"
        "F:\Program Files\Epic Games\UE_5.2"
        "F:\Program Files\Epic Games\UE_5.3"
        "F:\Program Files\Epic Games\UE_5.4"
        "F:\UnrealEngine"
        "F:\Epic Games\UE_5.1"
        "F:\Epic Games\UE_5.2"
        "F:\Epic Games\UE_5.3"
        "F:\Epic Games\UE_5.4"
    ) do (
        if exist "%%~d\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
            set UE_ENGINE_DIR=%%~d
            if "%VERBOSE%"=="true" echo [INFO] Found UE5 at: %%~d
            goto :engine_found
        )
    )
    
    REM Check registry for launcher installations
    for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine" /s /v "InstalledDirectory" 2^>nul') do (
        if exist "%%b\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
            set UE_ENGINE_DIR=%%b
            goto :engine_found
        )
    )
)

:engine_found
if "%UE_ENGINE_DIR%"=="" (
    echo [ERROR] Unreal Engine 5 installation not found!
    echo Please install UE5 or set UE_ENGINE_DIR manually in this script.
    goto :error
)

set UE_EDITOR_CMD=%UE_ENGINE_DIR%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe

if "%VERBOSE%"=="true" echo [INFO] Found Unreal Engine at: %UE_ENGINE_DIR%

REM ============================================================================
REM Project Validation
REM ============================================================================

if "%VERBOSE%"=="true" echo [INFO] Validating project...

if not exist "%PROJECT_FILE%" (
    echo [ERROR] Project file not found: %PROJECT_FILE%
    echo Please run this script from the project root directory.
    goto :error
)

if "%VERBOSE%"=="true" echo [INFO] Project file found: %PROJECT_FILE%

REM ============================================================================
REM Build Launch Arguments
REM ============================================================================

if "%VERBOSE%"=="true" echo [INFO] Building launch arguments...

set LAUNCH_ARGS=%PROJECT_FILE%

REM Add map
if not "%MAP_NAME%"=="" (
    set LAUNCH_ARGS=%LAUNCH_ARGS% /Game/Maps/%MAP_NAME%
)

REM Add game mode
if not "%GAME_MODE%"=="" (
    set LAUNCH_ARGS=%LAUNCH_ARGS% -game -GameMode=%GAME_MODE%
)

REM Add resolution
if not "%RESOLUTION%"=="" (
    set LAUNCH_ARGS=%LAUNCH_ARGS% -ResX=%RESOLUTION:x= -ResY=%
)

REM Add windowed/fullscreen
if "%WINDOWED%"=="true" (
    set LAUNCH_ARGS=%LAUNCH_ARGS% -windowed
) else (
    set LAUNCH_ARGS=%LAUNCH_ARGS% -fullscreen
)

REM Add logging
set LAUNCH_ARGS=%LAUNCH_ARGS% -log -LogCmds="LogTemp %LOG_LEVEL%"

REM Add performance settings
set LAUNCH_ARGS=%LAUNCH_ARGS% -NoVerifyGC -NoSplash -NoLoadingScreen

REM Add additional arguments
if not "%ADDITIONAL_ARGS%"=="" (
    set LAUNCH_ARGS=%LAUNCH_ARGS% %ADDITIONAL_ARGS%
)

REM ============================================================================
REM Launch Game
REM ============================================================================

echo [INFO] Launching KOTOR.ai...
echo [INFO] Map: %MAP_NAME%
echo [INFO] Game Mode: %GAME_MODE%
echo [INFO] Resolution: %RESOLUTION%
echo [INFO] Windowed: %WINDOWED%

if "%VERBOSE%"=="true" (
    echo [DEBUG] Full command line:
    echo "%UE_EDITOR_CMD%" %LAUNCH_ARGS%
    echo.
)

echo [INFO] Starting game... Press Ctrl+C to stop.

REM Launch the game
"%UE_EDITOR_CMD%" %LAUNCH_ARGS%

set LAUNCH_RESULT=%ERRORLEVEL%

if %LAUNCH_RESULT% equ 0 (
    echo [INFO] Game exited normally.
) else (
    echo [WARNING] Game exited with code: %LAUNCH_RESULT%
)

goto :end

REM ============================================================================
REM Error Handling
REM ============================================================================

:error
echo [ERROR] Failed to launch game.
set LAUNCH_RESULT=1
goto :end

:end
if "%VERBOSE%"=="true" echo [INFO] Script completed with exit code: %LAUNCH_RESULT%
exit /b %LAUNCH_RESULT%
