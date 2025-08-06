@echo off
setlocal enabledelayedexpansion

:: KOTOR.ai - Automated Build and Launch Script
:: Copyright Epic Games, Inc. All Rights Reserved.

echo ========================================
echo KOTOR.ai - Build and Launch Script
echo ========================================
echo.

:: Configuration
set PROJECT_NAME=KOTOR_Clone
set FAST_PROJECT_NAME=KOTOR_Clone_FastDev
set UE_VERSION=5.1
set BUILD_CONFIG=Development
set PLATFORM=Win64
set USE_FAST_BUILD=false
set LAUNCH_EDITOR=false
set LAUNCH_GAME=false
set AUTO_LOAD_CAMPAIGN=false
set CAMPAIGN_FILE=""
set PLANET_NAME=""
set SKIP_BUILD=false
set VERBOSE=false
set CLEAN_FIRST=false
set USE_LIVE_CODING=false

:: Parse command line arguments
:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="--fast" (
    set USE_FAST_BUILD=true
    echo [INFO] Using fast build configuration
)
if /i "%~1"=="--editor" (
    set LAUNCH_EDITOR=true
    echo [INFO] Will launch editor after build
)
if /i "%~1"=="--game" (
    set LAUNCH_GAME=true
    echo [INFO] Will launch game after build
)
if /i "%~1"=="--campaign" (
    set AUTO_LOAD_CAMPAIGN=true
    set CAMPAIGN_FILE=%~2
    shift
    echo [INFO] Will auto-load campaign: !CAMPAIGN_FILE!
)
if /i "%~1"=="--planet" (
    set PLANET_NAME=%~2
    shift
    echo [INFO] Will start on planet: !PLANET_NAME!
)
if /i "%~1"=="--skip-build" (
    set SKIP_BUILD=true
    echo [INFO] Skipping build step
)
if /i "%~1"=="--verbose" (
    set VERBOSE=true
    echo [INFO] Verbose output enabled
)
if /i "%~1"=="--clean" (
    set CLEAN_FIRST=true
    echo [INFO] Will clean before building
)
if /i "%~1"=="--live-coding" (
    set USE_LIVE_CODING=true
    echo [INFO] Live coding enabled
)
if /i "%~1"=="--help" (
    goto :show_help
)
shift
goto :parse_args

:args_done

:: Determine project file to use
if "%USE_FAST_BUILD%"=="true" (
    set PROJECT_FILE=%FAST_PROJECT_NAME%.uproject
    echo [INFO] Using fast development project: !PROJECT_FILE!
) else (
    set PROJECT_FILE=%PROJECT_NAME%.uproject
    echo [INFO] Using standard project: !PROJECT_FILE!
)

:: Check if project file exists
if not exist "%PROJECT_FILE%" (
    echo [ERROR] Project file not found: %PROJECT_FILE%
    echo [ERROR] Make sure you're running this script from the project root directory
    exit /b 1
)

:: Find Unreal Engine installation
echo [INFO] Locating Unreal Engine %UE_VERSION%...
set UE_ROOT=""
for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%UE_VERSION%" /v "InstalledDirectory" 2^>nul') do set UE_ROOT=%%b
if "%UE_ROOT%"=="" (
    echo [ERROR] Unreal Engine %UE_VERSION% not found in registry
    echo [ERROR] Please ensure Unreal Engine %UE_VERSION% is installed
    exit /b 1
)

set UBT_PATH="%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
set UE_EDITOR_PATH="%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe"

echo [INFO] Found Unreal Engine at: %UE_ROOT%

:: Clean if requested
if "%CLEAN_FIRST%"=="true" (
    echo [INFO] Cleaning build directories...
    call Scripts\clean_kotor_build.bat --quiet
    if errorlevel 1 (
        echo [ERROR] Clean failed
        exit /b 1
    )
)

:: Build the project
if "%SKIP_BUILD%"=="false" (
    echo [INFO] Building project...
    echo [INFO] Configuration: %BUILD_CONFIG%
    echo [INFO] Platform: %PLATFORM%
    
    if "%USE_FAST_BUILD%"=="true" (
        set BUILD_TARGET=%PROJECT_NAME%FastEditor
    ) else (
        set BUILD_TARGET=%PROJECT_NAME%Editor
    )
    
    if "%VERBOSE%"=="true" (
        set BUILD_VERBOSITY=-verbose
    ) else (
        set BUILD_VERBOSITY=
    )
    
    echo [INFO] Building target: !BUILD_TARGET!
    %UBT_PATH% !BUILD_TARGET! %PLATFORM% %BUILD_CONFIG% -project="%CD%\%PROJECT_FILE%" %BUILD_VERBOSITY%
    
    if errorlevel 1 (
        echo [ERROR] Build failed
        exit /b 1
    )
    
    echo [SUCCESS] Build completed successfully
) else (
    echo [INFO] Skipping build as requested
)

:: Prepare launch arguments
set LAUNCH_ARGS=""
if "%AUTO_LOAD_CAMPAIGN%"=="true" (
    set LAUNCH_ARGS=!LAUNCH_ARGS! -AutoLoadCampaign="%CAMPAIGN_FILE%"
)
if not "%PLANET_NAME%"=="" (
    set LAUNCH_ARGS=!LAUNCH_ARGS! -StartPlanet="%PLANET_NAME%"
)
if "%USE_LIVE_CODING%"=="true" (
    set LAUNCH_ARGS=!LAUNCH_ARGS! -LiveCoding
)
if "%USE_FAST_BUILD%"=="true" (
    set LAUNCH_ARGS=!LAUNCH_ARGS! -FastBuild
)

:: Launch editor if requested
if "%LAUNCH_EDITOR%"=="true" (
    echo [INFO] Launching Unreal Editor...
    if "%VERBOSE%"=="true" (
        echo [INFO] Launch command: %UE_EDITOR_PATH% "%CD%\%PROJECT_FILE%" !LAUNCH_ARGS!
    )
    start "" %UE_EDITOR_PATH% "%CD%\%PROJECT_FILE%" !LAUNCH_ARGS!
    echo [INFO] Editor launched
)

:: Launch game if requested
if "%LAUNCH_GAME%"=="true" (
    echo [INFO] Launching game...
    set GAME_EXE_PATH="%CD%\Binaries\Win64\%PROJECT_NAME%.exe"
    if exist !GAME_EXE_PATH! (
        if "%VERBOSE%"=="true" (
            echo [INFO] Launch command: !GAME_EXE_PATH! !LAUNCH_ARGS!
        )
        start "" !GAME_EXE_PATH! !LAUNCH_ARGS!
        echo [INFO] Game launched
    ) else (
        echo [WARNING] Game executable not found: !GAME_EXE_PATH!
        echo [WARNING] You may need to build the game target first
    )
)

echo.
echo [SUCCESS] Script completed successfully
goto :end

:show_help
echo.
echo KOTOR.ai Build and Launch Script
echo.
echo Usage: run_kotor_game.bat [options]
echo.
echo Options:
echo   --fast              Use fast build configuration
echo   --editor            Launch editor after build
echo   --game              Launch game after build
echo   --campaign FILE     Auto-load specified campaign file
echo   --planet NAME       Start on specified planet
echo   --skip-build        Skip the build step
echo   --verbose           Enable verbose output
echo   --clean             Clean before building
echo   --live-coding       Enable live coding
echo   --help              Show this help message
echo.
echo Examples:
echo   run_kotor_game.bat --fast --editor
echo   run_kotor_game.bat --campaign "TestCampaign.json" --planet "Taris" --editor
echo   run_kotor_game.bat --clean --fast --live-coding --editor
echo   run_kotor_game.bat --skip-build --game
echo.

:end
endlocal
