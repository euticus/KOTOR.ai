#!/usr/bin/env python3
"""
🤖 KOTOR.ai MCP Automation Master Script
Orchestrates all MCP integration tasks: CI/CD, auto-fix, documentation, and README updates
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path
from datetime import datetime

class MCPAutomationMaster:
    """Master automation controller for KOTOR.ai MCP integration"""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.scripts_dir = self.project_root / "Scripts"
        
    def run_command(self, command: str, cwd: str = None) -> bool:
        """Run a command and return success status"""
        try:
            result = subprocess.run(
                command, 
                shell=True, 
                cwd=cwd or self.project_root,
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                print(f"✅ Command succeeded: {command}")
                if result.stdout:
                    print(f"📝 Output: {result.stdout.strip()}")
                return True
            else:
                print(f"❌ Command failed: {command}")
                if result.stderr:
                    print(f"🚨 Error: {result.stderr.strip()}")
                return False
                
        except Exception as e:
            print(f"💥 Exception running command '{command}': {e}")
            return False
    
    def setup_github_workflows(self) -> bool:
        """Setup GitHub workflows for CI/CD"""
        print("🔧 Setting up GitHub workflows...")
        
        workflows_dir = self.project_root / ".github" / "workflows"
        
        # Check if workflows exist
        build_workflow = workflows_dir / "ue5_build_test.yml"
        autofix_workflow = workflows_dir / "mcp_auto_fix.yml"
        
        if build_workflow.exists() and autofix_workflow.exists():
            print("✅ GitHub workflows already configured")
            return True
        else:
            print("❌ GitHub workflows not found - please ensure they are committed")
            return False
    
    def run_build_and_tests(self) -> bool:
        """Run build and test pipeline locally"""
        print("🏗️ Running build and test pipeline...")
        
        # Check for build script
        build_script = self.scripts_dir / "Build.bat" if os.name == 'nt' else self.scripts_dir / "build.sh"
        
        if build_script.exists():
            print("🔨 Running build script...")
            success = self.run_command(str(build_script))
            if not success:
                print("❌ Build failed")
                return False
        else:
            print("⚠️ Build script not found, skipping build")
        
        # Run tests if available
        test_script = self.scripts_dir / "run_tests.bat" if os.name == 'nt' else self.scripts_dir / "run_tests.sh"
        
        if test_script.exists():
            print("🧪 Running tests...")
            success = self.run_command(str(test_script))
            if not success:
                print("❌ Tests failed - MCP auto-fix may be triggered")
                return False
        else:
            print("⚠️ Test script not found, skipping tests")
        
        print("✅ Build and test pipeline completed successfully")
        return True
    
    def generate_documentation(self) -> bool:
        """Generate comprehensive documentation"""
        print("📚 Generating documentation...")
        
        doc_generator = self.scripts_dir / "generate_docs.py"
        
        if doc_generator.exists():
            print("🤖 Running documentation generator...")
            success = self.run_command(f"python {doc_generator}")
            if success:
                print("✅ Documentation generated successfully")
                return True
            else:
                print("❌ Documentation generation failed")
                return False
        else:
            print("❌ Documentation generator not found")
            return False
    
    def update_readme(self) -> bool:
        """Update README.md with latest information"""
        print("📝 Updating README.md...")
        
        readme_updater = self.scripts_dir / "update_readme.py"
        
        if readme_updater.exists():
            print("🤖 Running README updater...")
            success = self.run_command(f"python {readme_updater}")
            if success:
                print("✅ README.md updated successfully")
                return True
            else:
                print("❌ README update failed")
                return False
        else:
            print("❌ README updater not found")
            return False
    
    def check_mcp_configuration(self) -> bool:
        """Check MCP configuration"""
        print("🔍 Checking MCP configuration...")
        
        codexrc = self.project_root / ".codexrc"
        
        if codexrc.exists():
            print("✅ MCP configuration found (.codexrc)")
            
            # Check for required environment variables
            required_vars = ["ANTHROPIC_API_KEY", "OPENAI_API_KEY"]
            missing_vars = []
            
            for var in required_vars:
                if not os.getenv(var):
                    missing_vars.append(var)
            
            if missing_vars:
                print(f"⚠️ Missing environment variables: {', '.join(missing_vars)}")
                print("💡 Set these for full MCP functionality")
            else:
                print("✅ All required environment variables set")
            
            return True
        else:
            print("❌ MCP configuration not found (.codexrc)")
            return False
    
    def run_full_automation(self) -> bool:
        """Run complete MCP automation pipeline"""
        print("🚀 Starting full MCP automation pipeline...")
        print("=" * 60)
        
        success_count = 0
        total_tasks = 5
        
        # Task 1: Check MCP configuration
        if self.check_mcp_configuration():
            success_count += 1
        
        # Task 2: Setup GitHub workflows
        if self.setup_github_workflows():
            success_count += 1
        
        # Task 3: Run build and tests
        if self.run_build_and_tests():
            success_count += 1
        
        # Task 4: Generate documentation
        if self.generate_documentation():
            success_count += 1
        
        # Task 5: Update README
        if self.update_readme():
            success_count += 1
        
        # Summary
        print("=" * 60)
        print(f"🎯 MCP Automation Summary: {success_count}/{total_tasks} tasks completed")
        
        if success_count == total_tasks:
            print("🎉 Full MCP automation pipeline completed successfully!")
            return True
        else:
            print(f"⚠️ {total_tasks - success_count} tasks failed or skipped")
            return False
    
    def trigger_github_workflow(self, workflow_name: str) -> bool:
        """Trigger a GitHub workflow manually"""
        print(f"🚀 Triggering GitHub workflow: {workflow_name}")
        
        # This would use GitHub CLI or API to trigger workflow
        # For now, just show the command
        command = f"gh workflow run {workflow_name}"
        print(f"💡 Run this command to trigger workflow: {command}")
        
        return True
    
    def simulate_mcp_auto_fix(self) -> bool:
        """Simulate MCP auto-fix process"""
        print("🤖 Simulating MCP auto-fix process...")
        
        # This would normally be triggered by failed tests
        # For demonstration, we'll show what would happen
        
        print("🔍 Analyzing failed tests...")
        print("🤖 Generating fixes with MCP Codex...")
        print("🔧 Applying automated fixes...")
        print("🧪 Re-running tests...")
        print("🚀 Creating auto-fix PR...")
        
        print("✅ MCP auto-fix simulation completed")
        return True

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(description="KOTOR.ai MCP Automation Master")
    parser.add_argument("--project-root", default=".", help="Project root directory")
    parser.add_argument("--task", choices=[
        "full", "build", "test", "docs", "readme", "workflows", "mcp-check", "auto-fix"
    ], default="full", help="Specific task to run")
    parser.add_argument("--trigger-workflow", help="Trigger specific GitHub workflow")
    
    args = parser.parse_args()
    
    print("🤖 KOTOR.ai MCP Automation Master")
    print(f"📅 {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 60)
    
    automation = MCPAutomationMaster(args.project_root)
    
    success = False
    
    if args.trigger_workflow:
        success = automation.trigger_github_workflow(args.trigger_workflow)
    elif args.task == "full":
        success = automation.run_full_automation()
    elif args.task == "build":
        success = automation.run_build_and_tests()
    elif args.task == "docs":
        success = automation.generate_documentation()
    elif args.task == "readme":
        success = automation.update_readme()
    elif args.task == "workflows":
        success = automation.setup_github_workflows()
    elif args.task == "mcp-check":
        success = automation.check_mcp_configuration()
    elif args.task == "auto-fix":
        success = automation.simulate_mcp_auto_fix()
    
    print("=" * 60)
    if success:
        print("🎉 MCP automation completed successfully!")
        sys.exit(0)
    else:
        print("❌ MCP automation encountered errors")
        sys.exit(1)

if __name__ == "__main__":
    main()
