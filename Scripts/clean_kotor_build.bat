@echo off
setlocal enabledelayedexpansion

:: KOTOR.ai - Build Cleanup Script
:: Copyright Epic Games, Inc. All Rights Reserved.

echo ========================================
echo KOTOR.ai - Build Cleanup Script
echo ========================================
echo.

:: Configuration
set PROJECT_NAME=KOTOR_Clone
set QUIET_MODE=false
set DEEP_CLEAN=false
set CLEAN_CACHE=false
set CLEAN_LOGS=false
set CLEAN_SAVED=false
set CLEAN_INTERMEDIATE=true
set CLEAN_BINARIES=true

:: Parse command line arguments
:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="--quiet" (
    set QUIET_MODE=true
)
if /i "%~1"=="--deep" (
    set DEEP_CLEAN=true
    set CLEAN_CACHE=true
    set CLEAN_LOGS=true
    set CLEAN_SAVED=true
    echo [INFO] Deep clean mode enabled
)
if /i "%~1"=="--cache" (
    set CLEAN_CACHE=true
    echo [INFO] Will clean cache directories
)
if /i "%~1"=="--logs" (
    set CLEAN_LOGS=true
    echo [INFO] Will clean log files
)
if /i "%~1"=="--saved" (
    set CLEAN_SAVED=true
    echo [INFO] Will clean saved data (WARNING: This includes save games!)
)
if /i "%~1"=="--no-intermediate" (
    set CLEAN_INTERMEDIATE=false
    echo [INFO] Will NOT clean intermediate files
)
if /i "%~1"=="--no-binaries" (
    set CLEAN_BINARIES=false
    echo [INFO] Will NOT clean binary files
)
if /i "%~1"=="--help" (
    goto :show_help
)
shift
goto :parse_args

:args_done

if "%QUIET_MODE%"=="false" (
    echo [INFO] Starting cleanup process...
    echo.
)

:: Function to safely remove directory
:remove_directory
set DIR_PATH=%~1
set DIR_NAME=%~2
if exist "%DIR_PATH%" (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] Removing %DIR_NAME%: %DIR_PATH%
    )
    rmdir /s /q "%DIR_PATH%" 2>nul
    if exist "%DIR_PATH%" (
        echo [WARNING] Could not completely remove %DIR_NAME%: %DIR_PATH%
        echo [WARNING] Some files may be in use. Try closing all applications and running again.
    ) else (
        if "%QUIET_MODE%"=="false" (
            echo [SUCCESS] Removed %DIR_NAME%
        )
    )
) else (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] %DIR_NAME% not found: %DIR_PATH%
    )
)
goto :eof

:: Function to safely remove file
:remove_file
set FILE_PATH=%~1
set FILE_NAME=%~2
if exist "%FILE_PATH%" (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] Removing %FILE_NAME%: %FILE_PATH%
    )
    del /f /q "%FILE_PATH%" 2>nul
    if exist "%FILE_PATH%" (
        echo [WARNING] Could not remove %FILE_NAME%: %FILE_PATH%
    ) else (
        if "%QUIET_MODE%"=="false" (
            echo [SUCCESS] Removed %FILE_NAME%
        )
    )
) else (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] %FILE_NAME% not found: %FILE_PATH%
    )
)
goto :eof

:: Clean Intermediate files
if "%CLEAN_INTERMEDIATE%"=="true" (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] Cleaning intermediate files...
    )
    call :remove_directory "Intermediate" "Intermediate directory"
    call :remove_directory "Source\%PROJECT_NAME%\Intermediate" "Source intermediate directory"
    call :remove_directory ".vs" "Visual Studio cache"
    call :remove_directory ".vscode" "VS Code cache"
    
    :: Clean intermediate files in subdirectories
    for /d %%d in (Plugins\*) do (
        call :remove_directory "%%d\Intermediate" "Plugin intermediate (%%d)"
    )
)

:: Clean Binary files
if "%CLEAN_BINARIES%"=="true" (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] Cleaning binary files...
    )
    call :remove_directory "Binaries" "Binaries directory"
    call :remove_directory "Source\%PROJECT_NAME%\Binaries" "Source binaries directory"
    
    :: Clean binary files in subdirectories
    for /d %%d in (Plugins\*) do (
        call :remove_directory "%%d\Binaries" "Plugin binaries (%%d)"
    )
)

:: Clean Cache files
if "%CLEAN_CACHE%"=="true" (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] Cleaning cache files...
    )
    call :remove_directory "DerivedDataCache" "Derived Data Cache"
    call :remove_directory "Saved\Cooked" "Cooked content cache"
    call :remove_directory "Saved\StagedBuilds" "Staged builds cache"
    call :remove_directory "Saved\Automation" "Automation cache"
    call :remove_directory "Saved\MaterialStats" "Material stats cache"
    
    :: Clean shader cache
    call :remove_directory "%LOCALAPPDATA%\UnrealEngine\%PROJECT_NAME%\Saved\Shaders" "Local shader cache"
    call :remove_directory "Saved\ShaderDebugInfo" "Shader debug info"
)

:: Clean Log files
if "%CLEAN_LOGS%"=="true" (
    if "%QUIET_MODE%"=="false" (
        echo [INFO] Cleaning log files...
    )
    call :remove_directory "Saved\Logs" "Log files"
    call :remove_file "*.log" "Root log files"
    call :remove_file "Saved\*.log" "Saved log files"
    
    :: Clean crash reports
    call :remove_directory "Saved\Crashes" "Crash reports"
    call :remove_directory "Saved\Profiling" "Profiling data"
)

:: Clean Saved data (WARNING: Includes save games!)
if "%CLEAN_SAVED%"=="true" (
    if "%QUIET_MODE%"=="false" (
        echo [WARNING] Cleaning saved data (including save games)...
    )
    call :remove_directory "Saved\SaveGames" "Save games"
    call :remove_directory "Saved\Config" "Saved configuration"
    call :remove_directory "Saved\Screenshots" "Screenshots"
    call :remove_directory "Saved\Backup" "Backup files"
)

:: Clean project-specific files
if "%QUIET_MODE%"=="false" (
    echo [INFO] Cleaning project-specific files...
)
call :remove_file "%PROJECT_NAME%.sln" "Visual Studio solution"
call :remove_file "*.tmp" "Temporary files"
call :remove_file "*.temp" "Temporary files"

:: Clean Unreal Engine generated files
call :remove_file "*.uproject.bak" "Project backup files"
call :remove_file "*.uplugin.bak" "Plugin backup files"

:: Clean fast development project files if they exist
if exist "KOTOR_Clone_FastDev.sln" (
    call :remove_file "KOTOR_Clone_FastDev.sln" "Fast dev solution file"
)

:: Clean build artifacts
if "%QUIET_MODE%"=="false" (
    echo [INFO] Cleaning build artifacts...
)
for %%f in (*.pdb *.ilk *.exp *.lib) do (
    if exist "%%f" (
        call :remove_file "%%f" "Build artifact (%%f)"
    )
)

:: Clean temporary directories that might be left behind
call :remove_directory "Temp" "Temporary directory"
call :remove_directory "tmp" "Temporary directory"

:: Clean plugin-specific intermediate and binary files
if "%QUIET_MODE%"=="false" (
    echo [INFO] Cleaning plugin files...
)
for /d %%d in (Plugins\*) do (
    if exist "%%d" (
        call :remove_directory "%%d\Intermediate" "Plugin intermediate (%%d)"
        call :remove_directory "%%d\Binaries" "Plugin binaries (%%d)"
    )
)

:: Summary
echo.
if "%QUIET_MODE%"=="false" (
    echo [SUCCESS] Cleanup completed!
    echo.
    echo Summary of cleaned items:
    if "%CLEAN_INTERMEDIATE%"=="true" echo   - Intermediate files
    if "%CLEAN_BINARIES%"=="true" echo   - Binary files
    if "%CLEAN_CACHE%"=="true" echo   - Cache files
    if "%CLEAN_LOGS%"=="true" echo   - Log files
    if "%CLEAN_SAVED%"=="true" echo   - Saved data (including save games!)
    echo.
    echo You can now run a fresh build of the project.
    echo Use 'run_kotor_game.bat --fast --editor' for a quick development build.
) else (
    echo [SUCCESS] Cleanup completed
)

goto :end

:show_help
echo.
echo KOTOR.ai Build Cleanup Script
echo.
echo Usage: clean_kotor_build.bat [options]
echo.
echo Options:
echo   --quiet             Run in quiet mode (minimal output)
echo   --deep              Deep clean (includes cache, logs, and saved data)
echo   --cache             Clean cache directories
echo   --logs              Clean log files
echo   --saved             Clean saved data (WARNING: includes save games!)
echo   --no-intermediate   Do NOT clean intermediate files
echo   --no-binaries       Do NOT clean binary files
echo   --help              Show this help message
echo.
echo Default behavior:
echo   - Cleans intermediate files
echo   - Cleans binary files
echo   - Does NOT clean cache, logs, or saved data
echo.
echo Examples:
echo   clean_kotor_build.bat
echo   clean_kotor_build.bat --deep
echo   clean_kotor_build.bat --cache --logs
echo   clean_kotor_build.bat --quiet --no-binaries
echo.
echo WARNING: --saved option will delete save games and screenshots!
echo.

:end
endlocal
