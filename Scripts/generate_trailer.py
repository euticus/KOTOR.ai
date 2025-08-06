#!/usr/bin/env python3
"""
KOTOR.ai - Procedural Trailer Generator
Copyright Epic Games, Inc. All Rights Reserved.

This script automatically generates campaign trailers from gameplay footage and cinematics.
"""

import json
import os
import sys
import argparse
import random
import time
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple

class TrailerGenerator:
    """Procedural Trailer Generator for KOTOR.ai campaigns"""
    
    def __init__(self):
        self.trailer_templates = {
            'epic': {
                'duration': 90,  # seconds
                'segments': [
                    {'type': 'title_card', 'duration': 3, 'text': 'A long time ago...'},
                    {'type': 'story_setup', 'duration': 15, 'focus': 'narrative'},
                    {'type': 'character_showcase', 'duration': 20, 'focus': 'companions'},
                    {'type': 'action_montage', 'duration': 25, 'focus': 'combat'},
                    {'type': 'dramatic_moment', 'duration': 12, 'focus': 'climax'},
                    {'type': 'force_powers', 'duration': 10, 'focus': 'abilities'},
                    {'type': 'title_reveal', 'duration': 5, 'text': 'KOTOR.ai'}
                ],
                'music_style': 'orchestral_epic',
                'pacing': 'building'
            },
            'action': {
                'duration': 60,
                'segments': [
                    {'type': 'hook', 'duration': 5, 'focus': 'explosive_start'},
                    {'type': 'combat_montage', 'duration': 20, 'focus': 'lightsaber_combat'},
                    {'type': 'force_showcase', 'duration': 15, 'focus': 'force_powers'},
                    {'type': 'enemy_reveal', 'duration': 10, 'focus': 'antagonists'},
                    {'type': 'final_clash', 'duration': 8, 'focus': 'boss_battle'},
                    {'type': 'title_slam', 'duration': 2, 'text': 'KOTOR.ai'}
                ],
                'music_style': 'intense_action',
                'pacing': 'fast'
            },
            'story': {
                'duration': 120,
                'segments': [
                    {'type': 'prologue', 'duration': 10, 'focus': 'backstory'},
                    {'type': 'character_intro', 'duration': 25, 'focus': 'protagonist'},
                    {'type': 'world_building', 'duration': 20, 'focus': 'planets'},
                    {'type': 'companion_bonds', 'duration': 20, 'focus': 'relationships'},
                    {'type': 'moral_choices', 'duration': 15, 'focus': 'decisions'},
                    {'type': 'stakes_reveal', 'duration': 15, 'focus': 'consequences'},
                    {'type': 'emotional_climax', 'duration': 10, 'focus': 'drama'},
                    {'type': 'title_fade', 'duration': 5, 'text': 'KOTOR.ai'}
                ],
                'music_style': 'emotional_orchestral',
                'pacing': 'measured'
            }
        }
        
        self.content_categories = {
            'narrative': ['dialogue_scenes', 'story_moments', 'character_development'],
            'combat': ['lightsaber_duels', 'force_battles', 'group_combat', 'boss_fights'],
            'exploration': ['planet_vistas', 'ancient_ruins', 'space_travel', 'discovery_moments'],
            'characters': ['companion_interactions', 'character_reveals', 'emotional_moments'],
            'force_powers': ['force_lightning', 'force_push', 'mind_tricks', 'lightsaber_throws'],
            'cinematics': ['cutscenes', 'dramatic_reveals', 'plot_twists', 'endings']
        }
        
        self.music_tracks = {
            'orchestral_epic': ['epic_main_theme.ogg', 'battle_symphony.ogg', 'force_awakening.ogg'],
            'intense_action': ['combat_intensity.ogg', 'lightsaber_clash.ogg', 'dark_side_rising.ogg'],
            'emotional_orchestral': ['character_theme.ogg', 'love_theme.ogg', 'sacrifice_melody.ogg'],
            'ambient_mysterious': ['ancient_mysteries.ogg', 'force_whispers.ogg', 'jedi_meditation.ogg']
        }

    def generate_campaign_trailer(self, campaign_file: str, template_type: str = 'epic',
                                output_dir: str = 'Generated/Trailers',
                                footage_dir: str = 'Saved/Screenshots',
                                music_dir: str = 'Content/Audio/Music') -> Dict[str, Any]:
        """Generate a trailer for a campaign"""
        
        print(f"🎬 Generating {template_type} trailer for campaign: {campaign_file}")
        print(f"📁 Output directory: {output_dir}")
        print()
        
        # Load campaign data
        try:
            with open(campaign_file, 'r', encoding='utf-8') as f:
                campaign_data = json.load(f)
        except Exception as e:
            print(f"❌ Error loading campaign file: {e}")
            return {'success': False, 'error': str(e)}
        
        # Get trailer template
        if template_type not in self.trailer_templates:
            print(f"❌ Unknown trailer template: {template_type}")
            return {'success': False, 'error': f'Unknown template: {template_type}'}
        
        template = self.trailer_templates[template_type]
        
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
        # Generate trailer script
        trailer_script = self._generate_trailer_script(campaign_data, template)
        
        # Collect required footage
        required_footage = self._collect_required_footage(trailer_script, footage_dir)
        
        # Generate video editing script
        editing_script = self._generate_editing_script(trailer_script, required_footage, music_dir)
        
        # Save trailer files
        campaign_name = campaign_data.get('campaign_name', 'Unknown Campaign')
        safe_name = self._make_safe_filename(campaign_name)
        
        script_file = os.path.join(output_dir, f"{safe_name}_{template_type}_trailer_script.json")
        editing_file = os.path.join(output_dir, f"{safe_name}_{template_type}_editing_script.py")
        
        # Save trailer script
        with open(script_file, 'w', encoding='utf-8') as f:
            json.dump(trailer_script, f, indent=2, ensure_ascii=False)
        
        # Save editing script
        with open(editing_file, 'w', encoding='utf-8') as f:
            f.write(editing_script)
        
        results = {
            'success': True,
            'campaign_name': campaign_name,
            'template_type': template_type,
            'duration': template['duration'],
            'segments': len(trailer_script['segments']),
            'script_file': script_file,
            'editing_file': editing_file,
            'required_footage': len(required_footage),
            'missing_footage': len([f for f in required_footage if not f.get('available', False)])
        }
        
        print(f"✅ Trailer script generated successfully!")
        print(f"   📋 Script: {script_file}")
        print(f"   🎞️ Editing: {editing_file}")
        print(f"   ⏱️ Duration: {template['duration']} seconds")
        print(f"   🎬 Segments: {len(trailer_script['segments'])}")
        print(f"   📸 Required footage: {results['required_footage']}")
        print(f"   ❓ Missing footage: {results['missing_footage']}")
        
        return results

    def _generate_trailer_script(self, campaign_data: Dict[str, Any], 
                               template: Dict[str, Any]) -> Dict[str, Any]:
        """Generate the trailer script from campaign data and template"""
        
        script = {
            'title': f"{campaign_data.get('campaign_name', 'Unknown Campaign')} - Trailer",
            'duration': template['duration'],
            'music_style': template['music_style'],
            'pacing': template['pacing'],
            'segments': []
        }
        
        current_time = 0.0
        
        for segment_template in template['segments']:
            segment = {
                'start_time': current_time,
                'duration': segment_template['duration'],
                'end_time': current_time + segment_template['duration'],
                'type': segment_template['type'],
                'content': self._generate_segment_content(campaign_data, segment_template),
                'transitions': self._get_segment_transitions(segment_template),
                'effects': self._get_segment_effects(segment_template)
            }
            
            script['segments'].append(segment)
            current_time += segment_template['duration']
        
        return script

    def _generate_segment_content(self, campaign_data: Dict[str, Any], 
                                segment_template: Dict[str, Any]) -> Dict[str, Any]:
        """Generate content for a specific segment"""
        
        segment_type = segment_template['type']
        focus = segment_template.get('focus', 'general')
        
        content = {
            'type': segment_type,
            'focus': focus,
            'shots': [],
            'text_overlays': [],
            'voice_over': '',
            'music_cues': []
        }
        
        if segment_type == 'title_card':
            content['text_overlays'] = [
                {
                    'text': segment_template.get('text', 'KOTOR.ai'),
                    'style': 'title_large',
                    'duration': segment_template['duration'],
                    'fade_in': 1.0,
                    'fade_out': 1.0
                }
            ]
            content['shots'] = [{'type': 'star_field', 'duration': segment_template['duration']}]
        
        elif segment_type == 'story_setup':
            content['shots'] = self._get_story_shots(campaign_data)
            content['voice_over'] = self._generate_story_narration(campaign_data)
        
        elif segment_type == 'character_showcase':
            content['shots'] = self._get_character_shots(campaign_data)
            content['text_overlays'] = self._get_character_names(campaign_data)
        
        elif segment_type == 'action_montage':
            content['shots'] = self._get_action_shots(campaign_data)
            content['music_cues'] = [{'type': 'intensity_build', 'time': 0}]
        
        elif segment_type == 'combat_montage':
            content['shots'] = self._get_combat_shots(campaign_data)
            content['effects'] = ['quick_cuts', 'impact_flashes']
        
        elif segment_type == 'force_showcase':
            content['shots'] = self._get_force_power_shots(campaign_data)
            content['effects'] = ['force_glow', 'energy_trails']
        
        elif segment_type == 'dramatic_moment':
            content['shots'] = self._get_dramatic_shots(campaign_data)
            content['voice_over'] = self._generate_dramatic_narration(campaign_data)
        
        elif segment_type == 'title_reveal' or segment_type == 'title_slam' or segment_type == 'title_fade':
            content['text_overlays'] = [
                {
                    'text': segment_template.get('text', campaign_data.get('campaign_name', 'KOTOR.ai')),
                    'style': 'title_massive',
                    'duration': segment_template['duration'],
                    'animation': segment_type.replace('title_', '')
                }
            ]
        
        return content

    def _get_story_shots(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get shots for story segments"""
        shots = []
        
        # Planet establishing shots
        for planet in campaign_data.get('planets', [])[:3]:
            shots.append({
                'type': 'planet_vista',
                'location': planet.get('planet_name', 'Unknown'),
                'duration': 3.0,
                'camera_movement': 'slow_pan'
            })
        
        # Character introduction shots
        for npc in campaign_data.get('npcs', [])[:2]:
            if npc.get('role') in ['companion', 'quest_giver']:
                shots.append({
                    'type': 'character_close_up',
                    'character': npc.get('name', 'Unknown'),
                    'duration': 2.0,
                    'expression': 'determined'
                })
        
        return shots

    def _get_character_shots(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get shots for character showcase"""
        shots = []
        
        companions = [npc for npc in campaign_data.get('npcs', []) if npc.get('role') == 'companion']
        
        for companion in companions[:4]:  # Limit to 4 main companions
            shots.append({
                'type': 'character_action',
                'character': companion.get('name', 'Unknown'),
                'duration': 4.0,
                'action': self._get_character_signature_action(companion),
                'camera_angle': 'heroic_low'
            })
        
        return shots

    def _get_action_shots(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get shots for action sequences"""
        shots = []
        
        # Lightsaber combat
        shots.extend([
            {'type': 'lightsaber_ignition', 'duration': 1.0, 'effect': 'dramatic_close_up'},
            {'type': 'lightsaber_clash', 'duration': 2.0, 'effect': 'sparks_flying'},
            {'type': 'force_push', 'duration': 1.5, 'effect': 'shockwave'},
            {'type': 'acrobatic_flip', 'duration': 1.0, 'effect': 'slow_motion'},
        ])
        
        # Enemy encounters
        for i, enemy_type in enumerate(['sith_lord', 'dark_jedi', 'bounty_hunter'][:3]):
            shots.append({
                'type': 'enemy_reveal',
                'enemy_type': enemy_type,
                'duration': 1.5,
                'camera_movement': 'dramatic_zoom'
            })
        
        return shots

    def _get_combat_shots(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get shots for combat montage"""
        return [
            {'type': 'group_combat', 'duration': 2.0, 'style': 'wide_shot'},
            {'type': 'lightsaber_duel', 'duration': 3.0, 'style': 'close_combat'},
            {'type': 'force_lightning', 'duration': 1.0, 'style': 'dramatic'},
            {'type': 'blaster_deflection', 'duration': 1.5, 'style': 'bullet_time'},
            {'type': 'finishing_move', 'duration': 2.0, 'style': 'cinematic'}
        ]

    def _get_force_power_shots(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get shots for Force power showcase"""
        return [
            {'type': 'force_meditation', 'duration': 2.0, 'effect': 'ethereal_glow'},
            {'type': 'force_jump', 'duration': 1.5, 'effect': 'gravity_defying'},
            {'type': 'mind_trick', 'duration': 2.0, 'effect': 'psychic_waves'},
            {'type': 'force_choke', 'duration': 1.5, 'effect': 'dark_energy'},
            {'type': 'force_storm', 'duration': 3.0, 'effect': 'lightning_cascade'}
        ]

    def _get_dramatic_shots(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get shots for dramatic moments"""
        return [
            {'type': 'character_revelation', 'duration': 3.0, 'style': 'close_up_reaction'},
            {'type': 'moral_choice', 'duration': 2.0, 'style': 'internal_conflict'},
            {'type': 'sacrifice_moment', 'duration': 3.0, 'style': 'emotional_wide'},
            {'type': 'destiny_fulfilled', 'duration': 4.0, 'style': 'epic_reveal'}
        ]

    def _get_character_signature_action(self, character: Dict[str, Any]) -> str:
        """Get signature action for a character based on their role"""
        role = character.get('role', 'companion')
        species = character.get('species', 'human')
        
        if species.lower() == 'wookiee':
            return 'bowcaster_shot'
        elif role == 'jedi':
            return 'lightsaber_flourish'
        elif role == 'pilot':
            return 'ship_maneuver'
        elif role == 'soldier':
            return 'tactical_shot'
        else:
            return 'heroic_pose'

    def _generate_story_narration(self, campaign_data: Dict[str, Any]) -> str:
        """Generate narration for story segments"""
        campaign_name = campaign_data.get('campaign_name', 'this galaxy')
        theme = campaign_data.get('theme', 'adventure')
        
        narrations = {
            'redemption': f"In {campaign_name}, the path to redemption is fraught with danger...",
            'ancient_mysteries': f"Ancient secrets lie buried in {campaign_name}...",
            'war_aftermath': f"In the aftermath of war, {campaign_name} struggles to find peace...",
            'galactic_conflict': f"The galaxy burns as {campaign_name} becomes the battleground..."
        }
        
        return narrations.get(theme, f"A new adventure awaits in {campaign_name}...")

    def _generate_dramatic_narration(self, campaign_data: Dict[str, Any]) -> str:
        """Generate narration for dramatic moments"""
        return "The fate of the galaxy hangs in the balance. Will you choose the light... or embrace the darkness?"

    def _get_character_names(self, campaign_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Get character name overlays"""
        overlays = []
        companions = [npc for npc in campaign_data.get('npcs', []) if npc.get('role') == 'companion']
        
        for i, companion in enumerate(companions[:4]):
            overlays.append({
                'text': companion.get('name', 'Unknown'),
                'style': 'character_name',
                'start_time': i * 4.0,
                'duration': 2.0,
                'position': 'lower_third'
            })
        
        return overlays

    def _get_segment_transitions(self, segment_template: Dict[str, Any]) -> List[str]:
        """Get transitions for a segment"""
        segment_type = segment_template['type']
        
        transitions = {
            'title_card': ['fade_in'],
            'story_setup': ['cross_fade'],
            'character_showcase': ['wipe_left'],
            'action_montage': ['quick_cut'],
            'combat_montage': ['impact_cut'],
            'dramatic_moment': ['slow_fade'],
            'title_reveal': ['dramatic_zoom']
        }
        
        return transitions.get(segment_type, ['cross_fade'])

    def _get_segment_effects(self, segment_template: Dict[str, Any]) -> List[str]:
        """Get effects for a segment"""
        segment_type = segment_template['type']
        
        effects = {
            'action_montage': ['speed_ramp', 'motion_blur'],
            'combat_montage': ['impact_flash', 'screen_shake'],
            'force_showcase': ['energy_glow', 'particle_effects'],
            'dramatic_moment': ['color_grade_dramatic', 'depth_of_field'],
            'title_reveal': ['lens_flare', 'epic_glow']
        }
        
        return effects.get(segment_type, [])

    def _collect_required_footage(self, trailer_script: Dict[str, Any], 
                                footage_dir: str) -> List[Dict[str, Any]]:
        """Collect required footage for the trailer"""
        required_footage = []
        
        for segment in trailer_script['segments']:
            for shot in segment['content'].get('shots', []):
                footage_item = {
                    'type': shot['type'],
                    'duration': shot['duration'],
                    'requirements': shot,
                    'available': False,
                    'file_path': ''
                }
                
                # Check if footage exists
                footage_file = self._find_footage_file(shot, footage_dir)
                if footage_file:
                    footage_item['available'] = True
                    footage_item['file_path'] = footage_file
                
                required_footage.append(footage_item)
        
        return required_footage

    def _find_footage_file(self, shot: Dict[str, Any], footage_dir: str) -> Optional[str]:
        """Find footage file for a shot"""
        # This would implement actual file searching logic
        # For now, return a placeholder
        shot_type = shot['type']
        possible_files = [
            f"{shot_type}.mp4",
            f"{shot_type}.avi",
            f"{shot_type}_001.mp4"
        ]
        
        for filename in possible_files:
            file_path = os.path.join(footage_dir, filename)
            if os.path.exists(file_path):
                return file_path
        
        return None

    def _generate_editing_script(self, trailer_script: Dict[str, Any], 
                               required_footage: List[Dict[str, Any]], 
                               music_dir: str) -> str:
        """Generate video editing script"""
        
        script_lines = [
            "#!/usr/bin/env python3",
            "# Auto-generated trailer editing script",
            "# This script can be used with video editing libraries like MoviePy",
            "",
            "import os",
            "from moviepy.editor import *",
            "",
            "def create_trailer():",
            "    clips = []",
            "    "
        ]
        
        # Add clips for each segment
        for i, segment in enumerate(trailer_script['segments']):
            script_lines.append(f"    # Segment {i+1}: {segment['type']}")
            
            for shot in segment['content'].get('shots', []):
                script_lines.append(f"    # Shot: {shot['type']}")
                script_lines.append(f"    # Duration: {shot['duration']} seconds")
                script_lines.append("")
        
        # Add music
        music_style = trailer_script.get('music_style', 'orchestral_epic')
        music_tracks = self.music_tracks.get(music_style, ['default_music.ogg'])
        script_lines.extend([
            f"    # Add background music: {music_style}",
            f"    music_file = os.path.join('{music_dir}', '{music_tracks[0]}')",
            "    if os.path.exists(music_file):",
            "        background_music = AudioFileClip(music_file)",
            "        # Set music duration and fade",
            "",
            "    # Combine all clips",
            "    final_video = concatenate_videoclips(clips)",
            "    ",
            "    # Export trailer",
            "    final_video.write_videofile('trailer_output.mp4', fps=24)",
            "",
            "if __name__ == '__main__':",
            "    create_trailer()"
        ])
        
        return "\n".join(script_lines)

    def _make_safe_filename(self, name: str) -> str:
        """Make a safe filename from a string"""
        import re
        # Remove invalid characters
        safe_name = re.sub(r'[<>:"/\\|?*]', '', name)
        # Replace spaces with underscores
        safe_name = safe_name.replace(' ', '_')
        # Limit length
        return safe_name[:50]

def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="Generate procedural trailers for KOTOR.ai campaigns")
    parser.add_argument("campaign_file", help="Path to campaign JSON file")
    parser.add_argument("--template", "-t", choices=['epic', 'action', 'story'], 
                       default='epic', help="Trailer template to use")
    parser.add_argument("--output", "-o", default="Generated/Trailers", help="Output directory")
    parser.add_argument("--footage", "-f", default="Saved/Screenshots", help="Footage directory")
    parser.add_argument("--music", "-m", default="Content/Audio/Music", help="Music directory")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    if args.verbose:
        print("🎬 KOTOR.ai Procedural Trailer Generator")
        print("=" * 50)
        print(f"Campaign: {args.campaign_file}")
        print(f"Template: {args.template}")
        print(f"Output: {args.output}")
        print()
    
    # Check if campaign file exists
    if not os.path.exists(args.campaign_file):
        print(f"❌ Campaign file not found: {args.campaign_file}")
        sys.exit(1)
    
    # Generate trailer
    generator = TrailerGenerator()
    results = generator.generate_campaign_trailer(
        args.campaign_file,
        args.template,
        args.output,
        args.footage,
        args.music
    )
    
    if results['success']:
        print(f"✅ Trailer generation completed successfully!")
        sys.exit(0)
    else:
        print(f"❌ Trailer generation failed: {results.get('error', 'Unknown error')}")
        sys.exit(1)

if __name__ == "__main__":
    main()
