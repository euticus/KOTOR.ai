# KOTOR.ai Project Builder (PowerShell)
# Builds the KOTOR_Clone project for UE5

Write-Host "🏗️ KOTOR.ai Project Builder" -ForegroundColor Cyan
Write-Host "========================" -ForegroundColor Cyan

# Set project details
$ProjectName = "KOTOR_Clone"
$ProjectDir = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectDir "$ProjectName.uproject"

# Set UE5 path
$UEEnginePath = "D:\Epic Games\UE_5.6"

Write-Host "📁 Project: $ProjectFile" -ForegroundColor Yellow
Write-Host "🎮 UE5 Path: $UEEnginePath" -ForegroundColor Yellow
Write-Host ""

# Check if UE5 exists
$UnrealBuildTool = Join-Path $UEEnginePath "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

if (-not (Test-Path $UnrealBuildTool)) {
    Write-Host "❌ UnrealBuildTool not found at: $UEEnginePath" -ForegroundColor Red
    Write-Host "Please check your UE5 installation path" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "🔧 Step 1: Generating project files..." -ForegroundColor Green

try {
    & $UnrealBuildTool -projectfiles -project="$ProjectFile" -game -rocket -progress
    
    if ($LASTEXITCODE -ne 0) {
        throw "Project file generation failed with exit code $LASTEXITCODE"
    }
    
    Write-Host "✅ Project files generated successfully!" -ForegroundColor Green
    Write-Host ""
    
    Write-Host "🏗️ Step 2: Building project..." -ForegroundColor Green
    
    & $UnrealBuildTool "$($ProjectName)Editor" Win64 Development -Project="$ProjectFile" -WaitMutex -FromMsBuild
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
    
    Write-Host "✅ Build completed successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "🎮 You can now:" -ForegroundColor Cyan
    Write-Host "1. Double-click $ProjectName.uproject to open in UE5" -ForegroundColor White
    Write-Host "2. Or run: Scripts\run_game.cmd --map TestLevel --windowed" -ForegroundColor White
    Write-Host ""
    
} catch {
    Write-Host "❌ Build failed: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "💡 Try these solutions:" -ForegroundColor Yellow
    Write-Host "1. Open $ProjectName.sln in Visual Studio and build there" -ForegroundColor White
    Write-Host "2. Check for missing dependencies" -ForegroundColor White
    Write-Host "3. Verify UE5 installation is complete" -ForegroundColor White
    Write-Host ""
}

Read-Host "Press Enter to continue"
