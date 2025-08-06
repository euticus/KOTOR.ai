#!/usr/bin/env python3
"""
🤖 KOTOR.ai Auto-Documentation Generator
Automatically generates comprehensive documentation from code comments and structure
"""

import os
import re
import json
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass, asdict
from datetime import datetime

@dataclass
class ClassInfo:
    """Information about a C++ class"""
    name: str
    namespace: str
    base_classes: List[str]
    description: str
    file_path: str
    methods: List[Dict]
    properties: List[Dict]
    enums: List[Dict]
    delegates: List[Dict]

@dataclass
class ModuleInfo:
    """Information about a UE5 module"""
    name: str
    description: str
    classes: List[ClassInfo]
    file_count: int
    line_count: int

class KOTORAIDocGenerator:
    """Main documentation generator for KOTOR.ai"""
    
    def __init__(self, source_dir: str, output_dir: str):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.modules = {}
        self.classes = {}
        
        # Ensure output directory exists
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
    def parse_cpp_file(self, file_path: Path) -> Optional[ClassInfo]:
        """Parse a C++ header file and extract class information"""
        try:
            content = file_path.read_text(encoding='utf-8', errors='ignore')
            
            # Extract class information
            class_match = re.search(r'class\s+(\w+_API\s+)?(\w+)\s*:\s*public\s+([^\{]+)', content)
            if not class_match:
                return None
                
            class_name = class_match.group(2)
            base_classes = [cls.strip() for cls in class_match.group(3).split(',')]
            
            # Extract class description from comments
            description = self._extract_class_description(content, class_name)
            
            # Extract methods
            methods = self._extract_methods(content)
            
            # Extract properties
            properties = self._extract_properties(content)
            
            # Extract enums
            enums = self._extract_enums(content)
            
            # Extract delegates
            delegates = self._extract_delegates(content)
            
            return ClassInfo(
                name=class_name,
                namespace=self._extract_namespace(content),
                base_classes=base_classes,
                description=description,
                file_path=str(file_path.relative_to(self.source_dir)),
                methods=methods,
                properties=properties,
                enums=enums,
                delegates=delegates
            )
            
        except Exception as e:
            print(f"❌ Error parsing {file_path}: {e}")
            return None
    
    def _extract_class_description(self, content: str, class_name: str) -> str:
        """Extract class description from comments"""
        # Look for comment block before class declaration
        class_pattern = rf'class\s+\w*\s*{re.escape(class_name)}'
        class_match = re.search(class_pattern, content)
        
        if class_match:
            before_class = content[:class_match.start()]
            
            # Find the last comment block
            comment_blocks = re.findall(r'/\*\*(.*?)\*/', before_class, re.DOTALL)
            if comment_blocks:
                description = comment_blocks[-1].strip()
                # Clean up comment formatting
                description = re.sub(r'^\s*\*\s?', '', description, flags=re.MULTILINE)
                return description.strip()
                
            # Try single-line comments
            lines = before_class.split('\n')
            comment_lines = []
            for line in reversed(lines):
                line = line.strip()
                if line.startswith('//'):
                    comment_lines.insert(0, line[2:].strip())
                elif line and not line.startswith('//'):
                    break
                    
            if comment_lines:
                return ' '.join(comment_lines)
        
        return f"UE5 class: {class_name}"
    
    def _extract_namespace(self, content: str) -> str:
        """Extract namespace from content"""
        namespace_match = re.search(r'namespace\s+(\w+)', content)
        return namespace_match.group(1) if namespace_match else ""
    
    def _extract_methods(self, content: str) -> List[Dict]:
        """Extract method information from content"""
        methods = []
        
        # Pattern for UFUNCTION methods
        ufunction_pattern = r'UFUNCTION\([^)]*\)\s*([^;{]+(?:;|\{))'
        
        for match in re.finditer(ufunction_pattern, content, re.MULTILINE):
            method_decl = match.group(1).strip()
            
            # Parse method signature
            method_info = self._parse_method_signature(method_decl)
            if method_info:
                # Extract method description
                before_ufunction = content[:match.start()]
                description = self._extract_preceding_comment(before_ufunction)
                method_info['description'] = description
                methods.append(method_info)
        
        return methods
    
    def _extract_properties(self, content: str) -> List[Dict]:
        """Extract property information from content"""
        properties = []
        
        # Pattern for UPROPERTY
        uproperty_pattern = r'UPROPERTY\([^)]*\)\s*([^;]+;)'
        
        for match in re.finditer(uproperty_pattern, content, re.MULTILINE):
            prop_decl = match.group(1).strip()
            
            # Parse property
            prop_info = self._parse_property_declaration(prop_decl)
            if prop_info:
                # Extract property description
                before_uproperty = content[:match.start()]
                description = self._extract_preceding_comment(before_uproperty)
                prop_info['description'] = description
                properties.append(prop_info)
        
        return properties
    
    def _extract_enums(self, content: str) -> List[Dict]:
        """Extract enum information from content"""
        enums = []
        
        # Pattern for UENUM
        uenum_pattern = r'UENUM\([^)]*\)\s*enum\s+class\s+(\w+)[^{]*\{([^}]+)\}'
        
        for match in re.finditer(uenum_pattern, content, re.DOTALL):
            enum_name = match.group(1)
            enum_body = match.group(2)
            
            # Extract enum values
            values = []
            for value_match in re.finditer(r'(\w+)\s*(?:UMETA[^,]*)?(?:,|$)', enum_body):
                values.append(value_match.group(1))
            
            enums.append({
                'name': enum_name,
                'values': values,
                'description': f"Enumeration: {enum_name}"
            })
        
        return enums
    
    def _extract_delegates(self, content: str) -> List[Dict]:
        """Extract delegate information from content"""
        delegates = []
        
        # Pattern for DECLARE_DYNAMIC_MULTICAST_DELEGATE
        delegate_pattern = r'DECLARE_DYNAMIC_MULTICAST_DELEGATE[^(]*\(([^,)]+)(?:,([^)]*))?\)'
        
        for match in re.finditer(delegate_pattern, content):
            delegate_name = match.group(1)
            params = match.group(2) if match.group(2) else ""
            
            delegates.append({
                'name': delegate_name,
                'parameters': params.strip(),
                'description': f"Multicast delegate: {delegate_name}"
            })
        
        return delegates
    
    def _parse_method_signature(self, signature: str) -> Optional[Dict]:
        """Parse a method signature"""
        # Remove semicolon and opening brace
        signature = signature.replace(';', '').replace('{', '').strip()
        
        # Basic parsing (can be enhanced)
        parts = signature.split('(')
        if len(parts) < 2:
            return None
            
        return_and_name = parts[0].strip().split()
        if len(return_and_name) < 2:
            return None
            
        return_type = ' '.join(return_and_name[:-1])
        method_name = return_and_name[-1]
        
        # Extract parameters
        param_part = parts[1].split(')')[0]
        parameters = [p.strip() for p in param_part.split(',') if p.strip()]
        
        return {
            'name': method_name,
            'return_type': return_type,
            'parameters': parameters,
            'signature': signature
        }
    
    def _parse_property_declaration(self, declaration: str) -> Optional[Dict]:
        """Parse a property declaration"""
        declaration = declaration.replace(';', '').strip()
        parts = declaration.split()
        
        if len(parts) < 2:
            return None
            
        prop_type = ' '.join(parts[:-1])
        prop_name = parts[-1]
        
        return {
            'name': prop_name,
            'type': prop_type
        }
    
    def _extract_preceding_comment(self, before_text: str) -> str:
        """Extract comment immediately preceding current position"""
        lines = before_text.split('\n')
        comment_lines = []
        
        for line in reversed(lines):
            line = line.strip()
            if line.startswith('//'):
                comment_lines.insert(0, line[2:].strip())
            elif line.startswith('*'):
                comment_lines.insert(0, line[1:].strip())
            elif line and not line.startswith('//') and not line.startswith('*'):
                break
        
        return ' '.join(comment_lines) if comment_lines else ""
    
    def scan_source_directory(self):
        """Scan the source directory and parse all files"""
        print(f"🔍 Scanning source directory: {self.source_dir}")
        
        # Find all header files
        header_files = list(self.source_dir.rglob("*.h"))
        print(f"📁 Found {len(header_files)} header files")
        
        for header_file in header_files:
            # Determine module from path
            relative_path = header_file.relative_to(self.source_dir)
            module_name = relative_path.parts[1] if len(relative_path.parts) > 1 else "Core"
            
            # Parse the file
            class_info = self.parse_cpp_file(header_file)
            if class_info:
                self.classes[class_info.name] = class_info
                
                # Add to module
                if module_name not in self.modules:
                    self.modules[module_name] = ModuleInfo(
                        name=module_name,
                        description=f"KOTOR.ai {module_name} Module",
                        classes=[],
                        file_count=0,
                        line_count=0
                    )
                
                self.modules[module_name].classes.append(class_info)
                self.modules[module_name].file_count += 1
                
                print(f"✅ Parsed: {class_info.name} ({module_name})")
    
    def generate_markdown_docs(self):
        """Generate comprehensive markdown documentation"""
        print("📝 Generating markdown documentation...")
        
        # Generate main README
        self._generate_main_readme()
        
        # Generate module documentation
        for module_name, module_info in self.modules.items():
            self._generate_module_docs(module_name, module_info)
        
        # Generate class reference
        self._generate_class_reference()
        
        # Generate API index
        self._generate_api_index()
        
        print(f"✅ Documentation generated in: {self.output_dir}")
    
    def _generate_main_readme(self):
        """Generate main README.md"""
        readme_content = f"""# 🎮 KOTOR.ai API Documentation

**Generated on:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

## 🎯 Overview

KOTOR.ai is the most advanced AI-driven RPG framework ever created, combining Python AIDM (AI Director & Manager) with Unreal Engine 5 for unprecedented gaming experiences.

## 📊 Project Statistics

- **Total Classes:** {len(self.classes)}
- **Total Modules:** {len(self.modules)}
- **Documentation Coverage:** Auto-generated from source code

## 🏗️ Architecture Overview

KOTOR.ai consists of {len(self.modules)} main modules:

"""
        
        for module_name, module_info in self.modules.items():
            readme_content += f"- **[{module_name}](modules/{module_name}.md)**: {module_info.description} ({len(module_info.classes)} classes)\n"
        
        readme_content += f"""

## 🔗 Quick Links

- [📚 API Reference](api_reference.md)
- [🏗️ Class Index](class_index.md)
- [🧪 Integration Tests](../Tests/KOTOR_AI_INTEGRATION_TESTS_README.md)

## 🚀 Getting Started

1. **Setup**: Follow the build instructions in the main README
2. **Integration**: See module documentation for specific integration guides
3. **Testing**: Run the comprehensive integration test suite
4. **Development**: Use the auto-generated API reference for development

---

*This documentation is automatically generated from source code comments and structure.*
"""
        
        (self.output_dir / "README.md").write_text(readme_content)
    
    def _generate_module_docs(self, module_name: str, module_info: ModuleInfo):
        """Generate documentation for a specific module"""
        modules_dir = self.output_dir / "modules"
        modules_dir.mkdir(exist_ok=True)
        
        module_content = f"""# 🏗️ {module_name} Module

{module_info.description}

## 📊 Module Statistics

- **Classes:** {len(module_info.classes)}
- **Files:** {module_info.file_count}

## 🎯 Classes

"""
        
        for class_info in module_info.classes:
            module_content += f"""### {class_info.name}

**File:** `{class_info.file_path}`  
**Base Classes:** {', '.join(class_info.base_classes)}

{class_info.description}

#### Methods ({len(class_info.methods)})
"""
            
            for method in class_info.methods:
                module_content += f"- `{method['name']}()`: {method.get('description', 'No description')}\n"
            
            module_content += f"\n#### Properties ({len(class_info.properties)})\n"
            
            for prop in class_info.properties:
                module_content += f"- `{prop['type']} {prop['name']}`: {prop.get('description', 'No description')}\n"
            
            if class_info.enums:
                module_content += f"\n#### Enums ({len(class_info.enums)})\n"
                for enum in class_info.enums:
                    module_content += f"- `{enum['name']}`: {', '.join(enum['values'])}\n"
            
            module_content += "\n---\n\n"
        
        (modules_dir / f"{module_name}.md").write_text(module_content)
    
    def _generate_class_reference(self):
        """Generate complete class reference"""
        ref_content = f"""# 📚 KOTOR.ai Class Reference

Complete reference for all {len(self.classes)} classes in KOTOR.ai.

"""
        
        # Sort classes alphabetically
        sorted_classes = sorted(self.classes.items())
        
        for class_name, class_info in sorted_classes:
            ref_content += f"""## {class_name}

**Module:** {self._get_module_for_class(class_name)}  
**File:** `{class_info.file_path}`  
**Inherits:** {', '.join(class_info.base_classes)}

{class_info.description}

### Public Methods

"""
            
            for method in class_info.methods:
                ref_content += f"""#### {method['name']}

```cpp
{method['signature']}
```

{method.get('description', 'No description available.')}

"""
            
            ref_content += "### Properties\n\n"
            
            for prop in class_info.properties:
                ref_content += f"- **{prop['name']}** (`{prop['type']}`): {prop.get('description', 'No description')}\n"
            
            ref_content += "\n---\n\n"
        
        (self.output_dir / "class_reference.md").write_text(ref_content)
    
    def _generate_api_index(self):
        """Generate API index"""
        index_content = f"""# 🔍 KOTOR.ai API Index

Quick reference index for all APIs in KOTOR.ai.

## Classes by Module

"""
        
        for module_name, module_info in sorted(self.modules.items()):
            index_content += f"### {module_name}\n\n"
            
            for class_info in sorted(module_info.classes, key=lambda c: c.name):
                index_content += f"- [{class_info.name}](class_reference.md#{class_info.name.lower()})\n"
            
            index_content += "\n"
        
        index_content += """## Methods by Category

### AI & Director
"""
        
        # Categorize methods
        ai_methods = []
        animation_methods = []
        audio_methods = []
        
        for class_info in self.classes.values():
            for method in class_info.methods:
                if 'AI' in class_info.name or 'Director' in class_info.name:
                    ai_methods.append((class_info.name, method['name']))
                elif 'Animation' in class_info.name or 'Performance' in class_info.name:
                    animation_methods.append((class_info.name, method['name']))
                elif 'Audio' in class_info.name or 'Music' in class_info.name:
                    audio_methods.append((class_info.name, method['name']))
        
        for class_name, method_name in sorted(ai_methods):
            index_content += f"- `{class_name}::{method_name}()`\n"
        
        index_content += "\n### Animation & Performance\n\n"
        for class_name, method_name in sorted(animation_methods):
            index_content += f"- `{class_name}::{method_name}()`\n"
        
        index_content += "\n### Audio & Music\n\n"
        for class_name, method_name in sorted(audio_methods):
            index_content += f"- `{class_name}::{method_name}()`\n"
        
        (self.output_dir / "api_index.md").write_text(index_content)
    
    def _get_module_for_class(self, class_name: str) -> str:
        """Get module name for a class"""
        for module_name, module_info in self.modules.items():
            for class_info in module_info.classes:
                if class_info.name == class_name:
                    return module_name
        return "Unknown"

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(description="Generate KOTOR.ai documentation")
    parser.add_argument("--source", default="Source/KOTOR_Clone", help="Source directory")
    parser.add_argument("--output", default="Documentation", help="Output directory")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    print("🤖 KOTOR.ai Auto-Documentation Generator")
    print("=" * 50)
    
    # Create generator
    generator = KOTORAIDocGenerator(args.source, args.output)
    
    # Scan and parse
    generator.scan_source_directory()
    
    # Generate documentation
    generator.generate_markdown_docs()
    
    print("🎉 Documentation generation complete!")

if __name__ == "__main__":
    main()
