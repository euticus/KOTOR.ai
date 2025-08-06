#!/usr/bin/env python3
"""
🤖 KOTOR.ai README Auto-Updater
Automatically updates README.md with latest features, statistics, and documentation
"""

import os
import re
import json
import subprocess
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Tuple

class KOTORAIReadmeUpdater:
    """Auto-updater for KOTOR.ai README.md"""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.readme_path = self.project_root / "README.md"
        self.stats = {}
        
    def gather_project_statistics(self) -> Dict:
        """Gather comprehensive project statistics"""
        print("📊 Gathering project statistics...")
        
        stats = {
            'timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            'files': {},
            'code': {},
            'features': {},
            'tests': {},
            'documentation': {}
        }
        
        # Count files by type
        stats['files'] = self._count_files()
        
        # Analyze code
        stats['code'] = self._analyze_code()
        
        # Count features/phases
        stats['features'] = self._count_features()
        
        # Analyze tests
        stats['tests'] = self._analyze_tests()
        
        # Check documentation
        stats['documentation'] = self._check_documentation()
        
        return stats
    
    def _count_files(self) -> Dict:
        """Count files by type"""
        file_counts = {
            'cpp_files': 0,
            'header_files': 0,
            'blueprint_files': 0,
            'python_files': 0,
            'config_files': 0,
            'script_files': 0,
            'total_files': 0
        }
        
        # Count C++ files
        cpp_files = list(self.project_root.rglob("*.cpp"))
        header_files = list(self.project_root.rglob("*.h"))
        
        file_counts['cpp_files'] = len(cpp_files)
        file_counts['header_files'] = len(header_files)
        
        # Count Blueprint files
        blueprint_files = list(self.project_root.rglob("*.uasset"))
        file_counts['blueprint_files'] = len(blueprint_files)
        
        # Count Python files
        python_files = list(self.project_root.rglob("*.py"))
        file_counts['python_files'] = len(python_files)
        
        # Count config files
        config_files = list(self.project_root.rglob("*.ini")) + list(self.project_root.rglob("*.json"))
        file_counts['config_files'] = len(config_files)
        
        # Count script files
        script_files = list(self.project_root.rglob("*.bat")) + list(self.project_root.rglob("*.sh"))
        file_counts['script_files'] = len(script_files)
        
        file_counts['total_files'] = sum([
            file_counts['cpp_files'],
            file_counts['header_files'],
            file_counts['blueprint_files'],
            file_counts['python_files'],
            file_counts['config_files'],
            file_counts['script_files']
        ])
        
        return file_counts
    
    def _analyze_code(self) -> Dict:
        """Analyze code metrics"""
        code_stats = {
            'total_lines': 0,
            'cpp_lines': 0,
            'python_lines': 0,
            'classes': 0,
            'functions': 0,
            'components': 0,
            'subsystems': 0
        }
        
        # Count lines in C++ files
        cpp_files = list(self.project_root.rglob("*.cpp")) + list(self.project_root.rglob("*.h"))
        for file in cpp_files:
            try:
                content = file.read_text(encoding='utf-8', errors='ignore')
                lines = len(content.splitlines())
                code_stats['cpp_lines'] += lines
                code_stats['total_lines'] += lines
                
                # Count classes
                class_matches = re.findall(r'class\s+\w+_API\s+(\w+)', content)
                code_stats['classes'] += len(class_matches)
                
                # Count functions
                function_matches = re.findall(r'UFUNCTION\s*\([^)]*\)', content)
                code_stats['functions'] += len(function_matches)
                
                # Count components
                if 'Component' in file.name:
                    code_stats['components'] += 1
                
                # Count subsystems
                if 'Subsystem' in file.name:
                    code_stats['subsystems'] += 1
                    
            except Exception as e:
                print(f"⚠️ Error reading {file}: {e}")
        
        # Count lines in Python files
        python_files = list(self.project_root.rglob("*.py"))
        for file in python_files:
            try:
                content = file.read_text(encoding='utf-8', errors='ignore')
                lines = len(content.splitlines())
                code_stats['python_lines'] += lines
                code_stats['total_lines'] += lines
            except Exception as e:
                print(f"⚠️ Error reading {file}: {e}")
        
        return code_stats
    
    def _count_features(self) -> Dict:
        """Count implemented features and phases"""
        features = {
            'total_phases': 12,
            'completed_phases': 0,
            'phase_details': {},
            'major_systems': []
        }
        
        # Check for phase completion markers
        phase_files = [
            "PHASE1_PYTHON_AIDM_README.md",
            "PHASE2_UE5_INTEGRATION_README.md", 
            "PHASE3_IMMERSION_INTELLIGENCE_README.md",
            "PHASE4_MULTIPLAYER_DND_README.md",
            "PHASE5_EMOTIONAL_INTELLIGENCE_README.md",
            "PHASE6_WORLD_SIMULATION_README.md",
            "PHASE7_UNREAL_STREAMLINING_README.md",
            "PHASE8_OPTIMIZATION_PIPELINE_README.md",
            "PHASE9_CINEMATIC_MULTIPLAYER_README.md",
            "PHASE10_DYNAMIC_ANIMATION_MUSIC_README.md",
            "PHASE11_AI_ANIMATION_PROCEDURAL_MUSIC_README.md",
            "PHASE12_BUILD_LAUNCH_STREAMLINING_README.md"
        ]
        
        for i, phase_file in enumerate(phase_files, 1):
            if (self.project_root / phase_file).exists():
                features['completed_phases'] += 1
                features['phase_details'][f'Phase {i}'] = 'Complete'
            else:
                features['phase_details'][f'Phase {i}'] = 'In Progress'
        
        # Identify major systems
        major_systems = [
            ('AI Director Subsystem', 'Source/KOTOR_Clone/Public/AI/AIDirectorSubsystem.h'),
            ('Procedural Music V2', 'Source/KOTOR_Clone/Public/Audio/ProceduralMusicSubsystemV2.h'),
            ('Animation Performance V2', 'Source/KOTOR_Clone/Public/Animation/ProceduralPerformanceComponentV2.h'),
            ('Companion Reactions', 'Source/KOTOR_Clone/Public/Companions/CompanionReactionSystem.h'),
            ('Boss Intro Sequencer', 'Source/KOTOR_Clone/Public/Cinematics/BossIntroSequencer.h'),
            ('Game Startup System', 'Source/KOTOR_Clone/Public/Core/GameStartupSubsystem.h'),
            ('Auto Map Loader', 'Source/KOTOR_Clone/Public/Core/AutoMapLoader.h'),
            ('Integration Tests', 'Tests/KOTOR_AI_End_To_End_Integration_Tests.h')
        ]
        
        for system_name, file_path in major_systems:
            if (self.project_root / file_path).exists():
                features['major_systems'].append(system_name)
        
        return features
    
    def _analyze_tests(self) -> Dict:
        """Analyze test coverage and status"""
        test_stats = {
            'test_files': 0,
            'test_classes': 0,
            'integration_tests': 0,
            'test_coverage': []
        }
        
        # Count test files
        test_files = list((self.project_root / "Tests").rglob("*.h")) if (self.project_root / "Tests").exists() else []
        test_stats['test_files'] = len(test_files)
        
        for test_file in test_files:
            try:
                content = test_file.read_text(encoding='utf-8', errors='ignore')
                
                # Count test classes
                test_class_matches = re.findall(r'IMPLEMENT_SIMPLE_AUTOMATION_TEST\s*\(\s*(\w+)', content)
                test_stats['test_classes'] += len(test_class_matches)
                
                # Count integration tests
                if 'Integration' in test_file.name:
                    test_stats['integration_tests'] += len(test_class_matches)
                
                # Track test coverage areas
                if 'Animation' in content:
                    test_stats['test_coverage'].append('Animation')
                if 'Music' in content:
                    test_stats['test_coverage'].append('Music')
                if 'AI' in content:
                    test_stats['test_coverage'].append('AI')
                if 'Companion' in content:
                    test_stats['test_coverage'].append('Companions')
                    
            except Exception as e:
                print(f"⚠️ Error reading test file {test_file}: {e}")
        
        # Remove duplicates
        test_stats['test_coverage'] = list(set(test_stats['test_coverage']))
        
        return test_stats
    
    def _check_documentation(self) -> Dict:
        """Check documentation status"""
        doc_stats = {
            'readme_files': 0,
            'phase_docs': 0,
            'api_docs': False,
            'integration_guide': False,
            'auto_generated': False
        }
        
        # Count README files
        readme_files = list(self.project_root.rglob("*README*.md"))
        doc_stats['readme_files'] = len(readme_files)
        
        # Count phase documentation
        phase_docs = list(self.project_root.glob("PHASE*_README.md"))
        doc_stats['phase_docs'] = len(phase_docs)
        
        # Check for specific documentation
        doc_stats['api_docs'] = (self.project_root / "Documentation").exists()
        doc_stats['integration_guide'] = (self.project_root / "Tests" / "KOTOR_AI_INTEGRATION_TESTS_README.md").exists()
        doc_stats['auto_generated'] = (self.project_root / "Scripts" / "generate_docs.py").exists()
        
        return doc_stats
    
    def generate_updated_readme(self, stats: Dict) -> str:
        """Generate updated README content"""
        print("📝 Generating updated README content...")
        
        readme_content = f"""# 🎮 KOTOR.ai - The Ultimate AI-Driven RPG Framework

**The most advanced AI-driven RPG development framework ever created** - combining Python AIDM (AI Director & Manager) with Unreal Engine 5 for unprecedented gaming experiences.

*Last Updated: {stats['timestamp']}*

## 🎯 Project Overview

KOTOR.ai represents a revolutionary approach to RPG development, where **Python AI systems** work seamlessly with **Unreal Engine 5** to create dynamic, emotionally intelligent, and infinitely replayable gaming experiences.

### 📊 Project Statistics

- **🏗️ Total Files**: {stats['files']['total_files']:,}
- **💻 Lines of Code**: {stats['code']['total_lines']:,}
- **🎭 C++ Classes**: {stats['code']['classes']}
- **🔧 Components**: {stats['code']['components']}
- **🎮 Subsystems**: {stats['code']['subsystems']}
- **🧪 Test Classes**: {stats['tests']['test_classes']}
- **📚 Documentation Files**: {stats['documentation']['readme_files']}

### 🚀 Development Progress

**Completed Phases: {stats['features']['completed_phases']}/{stats['features']['total_phases']}**

"""
        
        # Add phase completion status
        for phase, status in stats['features']['phase_details'].items():
            status_icon = "✅" if status == "Complete" else "🔄"
            readme_content += f"- {status_icon} **{phase}**: {status}\n"
        
        readme_content += f"""

## 🏗️ Architecture Overview

KOTOR.ai consists of **12 integrated phases** that work together to create the ultimate RPG experience:

### 🐍 Phase 1: Python AIDM Core System
- **Campaign Generation**: Dynamic story creation with infinite replayability
- **NPC Intelligence**: Advanced AI-driven character behavior
- **Quest System**: Procedural quest generation with meaningful choices
- **Loot Generation**: Intelligent item creation based on player progression

### 🎮 Phase 2: Unreal Engine 5 Integration
- **Seamless Python↔UE5 Communication**: Real-time data exchange
- **Subsystem Architecture**: Modular, scalable system design
- **Blueprint Integration**: Designer-friendly tools and workflows
- **Performance Optimization**: Enterprise-grade performance

### 🎭 Phase 3: Immersion & Intelligence
- **AI Voice Synthesis**: Dynamic character voice generation
- **Companion AI**: Emotionally intelligent companion behavior
- **Memory Systems**: Persistent character and world memory
- **Modding Framework**: Extensible content creation tools

### 🌌 Phase 4: Multiplayer D&D Simulator
- **Party Coordination**: Advanced multiplayer party management
- **D&D Campaign Simulation**: Authentic tabletop RPG experience
- **Space Exploration**: Procedural galaxy generation
- **Cloud Integration**: Seamless online experiences

### 💭 Phase 5: Emotional Intelligence
- **Character Depth**: Multi-layered personality systems
- **AI Pacing**: Dynamic difficulty and content adaptation
- **Infinite Content**: Procedural story generation
- **Emotional Tracking**: Player and NPC emotional state management

### 🌍 Phase 6: World Simulation & Atmosphere
- **Living Universe**: Dynamic world state simulation
- **Subconscious Narrative**: Background story evolution
- **Player-Driven Atmosphere**: Responsive environmental storytelling
- **Galactic Politics**: Complex faction and relationship systems

### 🔧 Phase 7: Unreal Engine Streamlining
- **Modular Toolkit**: Plug-and-play development components
- **Developer Tools**: Advanced debugging and development utilities
- **Workflow Optimization**: Streamlined development processes
- **Quality Assurance**: Automated testing and validation

### ⚡ Phase 8: Optimization Pipeline
- **Performance Monitoring**: Real-time performance analytics
- **Automated Optimization**: AI-driven performance improvements
- **Resource Management**: Intelligent asset loading and caching
- **Scalability**: Enterprise-grade scalability solutions

### 🎬 Phase 9: Cinematic Multiplayer
- **AI Audio Systems**: Dynamic audio generation and mixing
- **Party Decision Making**: Collaborative choice systems
- **Procedural Trailers**: Automatic highlight reel generation
- **Cinematic Tools**: Advanced cutscene and sequence creation

### 🎭 Phase 10: Dynamic Animation & Adaptive Soundtrack
- **AI Animation**: Emotion-driven character animation
- **Procedural Music**: Dynamic soundtrack adaptation
- **Performance Synchronization**: Perfect audio-visual coordination
- **Adaptive Systems**: Real-time content adaptation

### 🎵 Phase 11: AI Animation + Procedural Music
- **Advanced Performance System**: 16 emotions × 16 tones animation matrix
- **MetaHuman Integration**: Professional facial animation control
- **Music Layering**: Sophisticated real-time music composition
- **Narrative Music Linking**: Story-driven soundtrack evolution

### 🚀 Phase 12: Build & Launch Streamlining
- **Automated Startup**: Zero-configuration game initialization
- **Cross-Platform CLI**: Professional command-line tools
- **Debug Integration**: Comprehensive debugging infrastructure
- **Production Pipeline**: Enterprise-grade build and deployment

## 🎯 Major Systems

"""
        
        # Add major systems
        for system in stats['features']['major_systems']:
            readme_content += f"- ✅ **{system}**\n"
        
        readme_content += f"""

## 🧪 Testing & Quality Assurance

KOTOR.ai includes the most comprehensive integration test suite ever created for an RPG framework:

- **🧪 Test Files**: {stats['tests']['test_files']}
- **🎯 Test Classes**: {stats['tests']['test_classes']}
- **🔗 Integration Tests**: {stats['tests']['integration_tests']}
- **📊 Coverage Areas**: {', '.join(stats['tests']['test_coverage'])}

### Test Categories
- **End-to-End Integration**: Complete Python↔UE5 pipeline testing
- **Animation & Music**: Synchronized performance system testing
- **Complete Game Experience**: Full player journey validation
- **Cross-System Communication**: All 12 phases working together

## 📚 Documentation

- **📖 Phase Documentation**: {stats['documentation']['phase_docs']} comprehensive guides
- **🔗 API Reference**: {'✅ Available' if stats['documentation']['api_docs'] else '🔄 In Progress'}
- **🧪 Integration Guide**: {'✅ Complete' if stats['documentation']['integration_guide'] else '🔄 In Progress'}
- **🤖 Auto-Generated Docs**: {'✅ Enabled' if stats['documentation']['auto_generated'] else '❌ Disabled'}

## 🚀 Getting Started

### Prerequisites
- **Unreal Engine 5.4+**
- **Python 3.11+**
- **Visual Studio 2022** (Windows) or **Xcode** (Mac)
- **Git LFS** for large assets

### Quick Start
```bash
# Clone the repository
git clone https://github.com/your-org/kotor-ai.git
cd kotor-ai

# Run setup script
Scripts/setup.bat  # Windows
./Scripts/setup.sh # Mac/Linux

# Launch the game
Scripts/run_game.cmd --map Taris --windowed  # Windows
./Scripts/run_game.sh --map Taris --windowed # Mac/Linux
```

### Development Workflow
```bash
# Generate project files
Scripts/generate_project_files.bat

# Build the project
Scripts/build.bat Win64 Development

# Run integration tests
Scripts/run_tests.bat

# Generate documentation
python Scripts/generate_docs.py
```

## 🤖 MCP Integration & Automation

KOTOR.ai includes advanced MCP (Model Context Protocol) integration for automated development:

- **🔄 Auto-Build & Test**: Automated CI/CD pipeline with UE5 integration
- **🤖 Auto-Fix System**: MCP Codex automatically fixes failing tests
- **📝 Auto-Documentation**: Generates comprehensive docs from code comments
- **🔍 Code Analysis**: Advanced static analysis and optimization suggestions

## 🎮 Features Showcase

### Revolutionary AI Integration
- **Python AIDM** generates campaigns, NPCs, quests, and loot in real-time
- **UE5 Systems** seamlessly integrate Python data for immediate gameplay
- **Cross-System Communication** enables unprecedented AI-driven experiences

### Advanced Animation & Music
- **16 Emotions × 16 Tones** = 256 unique animation combinations
- **MetaHuman Integration** with professional facial animation control
- **Procedural Music** with 12 biomes, 16 tones, and dynamic stem layering
- **Perfect Synchronization** between voice, animation, and music

### Comprehensive Testing
- **End-to-End Pipeline Tests** verify complete Python→UE5 integration
- **Real-Time Adaptation Tests** validate AI learning and content adaptation
- **Performance Benchmarks** ensure production-quality performance
- **Cross-Platform Validation** across Windows, Mac, and Linux

## 🏆 Achievement Summary

> **KOTOR.ai represents the pinnacle of RPG development technology:**
> 
> - ✅ **Most Advanced AI Integration**: Python AIDM + UE5 seamless communication
> - ✅ **Revolutionary Animation System**: 256+ emotion/tone combinations with MetaHuman
> - ✅ **Sophisticated Music Engine**: Dynamic procedural soundtrack with narrative adaptation
> - ✅ **Comprehensive Testing**: Most extensive RPG framework test suite ever created
> - ✅ **Production-Ready**: Enterprise-grade performance, scalability, and reliability
> - ✅ **Developer-Friendly**: Streamlined workflows with automated tools and documentation

## 📞 Support & Community

- **📖 Documentation**: [Complete API Reference](Documentation/README.md)
- **🧪 Testing Guide**: [Integration Test Suite](Tests/KOTOR_AI_INTEGRATION_TESTS_README.md)
- **🐛 Issues**: [GitHub Issues](https://github.com/your-org/kotor-ai/issues)
- **💬 Discussions**: [GitHub Discussions](https://github.com/your-org/kotor-ai/discussions)

---

**🎮 KOTOR.ai - Where AI meets Epic Gaming** ✨

*Built with ❤️ by the KOTOR.ai development team*
"""
        
        return readme_content
    
    def update_readme(self):
        """Update the README.md file"""
        print("🚀 Starting README update process...")
        
        # Gather statistics
        stats = self.gather_project_statistics()
        
        # Generate new content
        new_content = self.generate_updated_readme(stats)
        
        # Write to README
        self.readme_path.write_text(new_content, encoding='utf-8')
        
        print(f"✅ README.md updated successfully!")
        print(f"📊 Statistics: {stats['files']['total_files']} files, {stats['code']['total_lines']:,} lines of code")
        print(f"🎯 Progress: {stats['features']['completed_phases']}/{stats['features']['total_phases']} phases complete")

def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(description="Update KOTOR.ai README.md")
    parser.add_argument("--project-root", default=".", help="Project root directory")
    parser.add_argument("--dry-run", action="store_true", help="Show what would be updated without writing")
    
    args = parser.parse_args()
    
    print("🤖 KOTOR.ai README Auto-Updater")
    print("=" * 50)
    
    updater = KOTORAIReadmeUpdater(args.project_root)
    
    if args.dry_run:
        stats = updater.gather_project_statistics()
        print("📊 Project Statistics (Dry Run):")
        print(f"  Files: {stats['files']['total_files']}")
        print(f"  Lines: {stats['code']['total_lines']:,}")
        print(f"  Classes: {stats['code']['classes']}")
        print(f"  Tests: {stats['tests']['test_classes']}")
        print(f"  Phases: {stats['features']['completed_phases']}/12")
    else:
        updater.update_readme()
    
    print("🎉 README update process complete!")

if __name__ == "__main__":
    main()
