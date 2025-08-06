#!/bin/bash
# KOTOR.ai Game Launch Script for Mac/Linux
# Direct map launch bypassing editor

set -e  # Exit on any error

# ============================================================================
# Configuration
# ============================================================================

# Project settings
PROJECT_NAME="KOTOR_Clone"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
PROJECT_FILE="$PROJECT_DIR/$PROJECT_NAME.uproject"

# Default launch settings
DEFAULT_MAP="TestLevel"
DEFAULT_GAME_MODE="BP_KOTORGameMode"
DEFAULT_RESOLUTION="1920x1080"
DEFAULT_WINDOWED="true"
DEFAULT_LOG_LEVEL="Log"

# Engine paths (auto-detect or set manually)
UE_ENGINE_DIR=""
UE_EDITOR_CMD=""

# ============================================================================
# Command Line Arguments
# ============================================================================

MAP_NAME="$DEFAULT_MAP"
GAME_MODE="$DEFAULT_GAME_MODE"
RESOLUTION="$DEFAULT_RESOLUTION"
WINDOWED="$DEFAULT_WINDOWED"
LOG_LEVEL="$DEFAULT_LOG_LEVEL"
ADDITIONAL_ARGS=""
SHOW_HELP=false
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            SHOW_HELP=true
            shift
            ;;
        -m|--map)
            MAP_NAME="$2"
            shift 2
            ;;
        -g|--gamemode)
            GAME_MODE="$2"
            shift 2
            ;;
        -r|--resolution)
            RESOLUTION="$2"
            shift 2
            ;;
        -w|--windowed)
            WINDOWED="true"
            shift
            ;;
        -f|--fullscreen)
            WINDOWED="false"
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --log)
            LOG_LEVEL="$2"
            shift 2
            ;;
        --args)
            ADDITIONAL_ARGS="$2"
            shift 2
            ;;
        *)
            echo "[ERROR] Unknown option: $1"
            exit 1
            ;;
    esac
done

# ============================================================================
# Help Display
# ============================================================================

if [ "$SHOW_HELP" = true ]; then
    cat << EOF
KOTOR.ai Game Launch Script

Usage: run_game.sh [OPTIONS]

Options:
  -h, --help              Show this help message
  -m, --map MAP           Map to load (default: $DEFAULT_MAP)
  -g, --gamemode MODE     Game mode to use (default: $DEFAULT_GAME_MODE)
  -r, --resolution RES    Screen resolution (default: $DEFAULT_RESOLUTION)
  -w, --windowed          Run in windowed mode
  -f, --fullscreen        Run in fullscreen mode
  -v, --verbose           Enable verbose logging
  --log LEVEL             Set log level (Log, Warning, Error)
  --args ARGS             Additional command line arguments

Examples:
  ./run_game.sh --map Taris --windowed
  ./run_game.sh --map Dantooine --resolution 2560x1440 --fullscreen
  ./run_game.sh --verbose --log Warning

EOF
    exit 0
fi

# ============================================================================
# Utility Functions
# ============================================================================

log_info() {
    echo "[INFO] $1"
}

log_error() {
    echo "[ERROR] $1" >&2
}

log_debug() {
    if [ "$VERBOSE" = true ]; then
        echo "[DEBUG] $1"
    fi
}

# ============================================================================
# Engine Detection
# ============================================================================

detect_engine() {
    log_debug "Detecting Unreal Engine installation..."
    
    # Detect platform
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        PLATFORM="Mac"
        ENGINE_BINARY="UnrealEditor"
        
        # Common macOS installation paths
        ENGINE_PATHS=(
            "/Users/Shared/Epic Games/UE_5.1"
            "/Users/Shared/Epic Games/UE_5.2"
            "/Users/Shared/Epic Games/UE_5.3"
            "/Users/Shared/Epic Games/UE_5.4"
            "/Applications/Epic Games/UE_5.1"
            "/Applications/Epic Games/UE_5.2"
            "/Applications/Epic Games/UE_5.3"
            "/Applications/Epic Games/UE_5.4"
            "$HOME/UnrealEngine"
        )
        
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux
        PLATFORM="Linux"
        ENGINE_BINARY="UnrealEditor"
        
        # Common Linux installation paths
        ENGINE_PATHS=(
            "$HOME/UnrealEngine"
            "/opt/UnrealEngine"
            "/usr/local/UnrealEngine"
            "$HOME/Epic Games/UE_5.1"
            "$HOME/Epic Games/UE_5.2"
            "$HOME/Epic Games/UE_5.3"
            "$HOME/Epic Games/UE_5.4"
        )
    else
        log_error "Unsupported platform: $OSTYPE"
        exit 1
    fi
    
    # Try to find engine installation
    for engine_path in "${ENGINE_PATHS[@]}"; do
        candidate="$engine_path/Engine/Binaries/$PLATFORM/$ENGINE_BINARY"
        log_debug "Checking: $candidate"
        
        if [ -f "$candidate" ] && [ -x "$candidate" ]; then
            UE_ENGINE_DIR="$engine_path"
            UE_EDITOR_CMD="$candidate"
            log_debug "Found Unreal Engine at: $UE_ENGINE_DIR"
            return 0
        fi
    done
    
    log_error "Unreal Engine 5 installation not found!"
    log_error "Please install UE5 or set UE_ENGINE_DIR manually in this script."
    log_error "Searched paths:"
    for path in "${ENGINE_PATHS[@]}"; do
        log_error "  $path"
    done
    exit 1
}

# ============================================================================
# Project Validation
# ============================================================================

validate_project() {
    log_debug "Validating project..."
    
    if [ ! -f "$PROJECT_FILE" ]; then
        log_error "Project file not found: $PROJECT_FILE"
        log_error "Please run this script from the project root directory."
        exit 1
    fi
    
    log_debug "Project file found: $PROJECT_FILE"
}

# ============================================================================
# Launch Arguments Builder
# ============================================================================

build_launch_args() {
    log_debug "Building launch arguments..."
    
    LAUNCH_ARGS=("$PROJECT_FILE")
    
    # Add map
    if [ -n "$MAP_NAME" ]; then
        LAUNCH_ARGS+=("/Game/Maps/$MAP_NAME")
    fi
    
    # Add game mode
    if [ -n "$GAME_MODE" ]; then
        LAUNCH_ARGS+=("-game" "-GameMode=$GAME_MODE")
    fi
    
    # Add resolution
    if [ -n "$RESOLUTION" ]; then
        IFS='x' read -ra RES_PARTS <<< "$RESOLUTION"
        if [ ${#RES_PARTS[@]} -eq 2 ]; then
            LAUNCH_ARGS+=("-ResX=${RES_PARTS[0]}" "-ResY=${RES_PARTS[1]}")
        fi
    fi
    
    # Add windowed/fullscreen
    if [ "$WINDOWED" = "true" ]; then
        LAUNCH_ARGS+=("-windowed")
    else
        LAUNCH_ARGS+=("-fullscreen")
    fi
    
    # Add logging
    LAUNCH_ARGS+=("-log" "-LogCmds=LogTemp $LOG_LEVEL")
    
    # Add performance settings
    LAUNCH_ARGS+=("-NoVerifyGC" "-NoSplash" "-NoLoadingScreen")
    
    # Add additional arguments
    if [ -n "$ADDITIONAL_ARGS" ]; then
        IFS=' ' read -ra ADDITIONAL_ARRAY <<< "$ADDITIONAL_ARGS"
        LAUNCH_ARGS+=("${ADDITIONAL_ARRAY[@]}")
    fi
}

# ============================================================================
# Main Execution
# ============================================================================

main() {
    log_info "KOTOR.ai Game Launcher"
    
    # Detect engine
    detect_engine
    
    # Validate project
    validate_project
    
    # Build launch arguments
    build_launch_args
    
    # Display launch info
    log_info "Map: $MAP_NAME"
    log_info "Game Mode: $GAME_MODE"
    log_info "Resolution: $RESOLUTION"
    log_info "Windowed: $WINDOWED"
    
    if [ "$VERBOSE" = true ]; then
        log_debug "Full command line:"
        log_debug "\"$UE_EDITOR_CMD\" ${LAUNCH_ARGS[*]}"
        echo
    fi
    
    log_info "Starting game... Press Ctrl+C to stop."
    
    # Launch the game
    "$UE_EDITOR_CMD" "${LAUNCH_ARGS[@]}"
    
    LAUNCH_RESULT=$?
    
    if [ $LAUNCH_RESULT -eq 0 ]; then
        log_info "Game exited normally."
    else
        log_error "Game exited with code: $LAUNCH_RESULT"
    fi
    
    exit $LAUNCH_RESULT
}

# ============================================================================
# Error Handling
# ============================================================================

# Trap errors and cleanup
trap 'log_error "Script interrupted"; exit 130' INT
trap 'log_error "Script failed at line $LINENO"; exit 1' ERR

# ============================================================================
# Execute Main
# ============================================================================

main "$@"
