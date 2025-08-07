@echo off
REM KOTOR.ai UE5 Installation Finder
REM Searches all drives for UE5 installations

echo 🔍 Searching for Unreal Engine 5 installations...
echo.

REM Check all available drives
for %%d in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist %%d:\ (
        echo Checking drive %%d:\...
        
        REM Check Epic Games folder
        if exist "%%d:\Program Files\Epic Games" (
            for /d %%f in ("%%d:\Program Files\Epic Games\UE_*") do (
                if exist "%%f\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
                    echo ✅ FOUND: %%f
                    echo    UnrealEditor-Cmd.exe: %%f\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
                    echo.
                )
            )
        )
        
        REM Check Epic Games folder (without Program Files)
        if exist "%%d:\Epic Games" (
            for /d %%f in ("%%d:\Epic Games\UE_*") do (
                if exist "%%f\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
                    echo ✅ FOUND: %%f
                    echo    UnrealEditor-Cmd.exe: %%f\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
                    echo.
                )
            )
        )
        
        REM Check UnrealEngine folder
        if exist "%%d:\UnrealEngine" (
            if exist "%%d:\UnrealEngine\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
                echo ✅ FOUND: %%d:\UnrealEngine
                echo    UnrealEditor-Cmd.exe: %%d:\UnrealEngine\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
                echo.
            )
        )
    )
)

echo.
echo 🔍 Search complete!
echo.
echo If you found UE5 installations above, you can:
echo 1. Set environment variable: set UE_ENGINE_DIR=^<path^>
echo 2. Or edit Scripts\run_game.cmd and set UE_ENGINE_DIR manually
echo.
pause
