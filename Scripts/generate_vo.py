#!/usr/bin/env python3
"""
KOTOR.ai - Voice Over Cache Generator
Copyright Epic Games, Inc. All Rights Reserved.

This script generates voice over files for AIDM campaigns using various TTS providers.
"""

import json
import os
import sys
import argparse
import hashlib
import time
import requests
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple

# Add the AIDM path to sys.path
script_dir = os.path.dirname(os.path.abspath(__file__))
aidm_path = os.path.join(script_dir, '..', 'AIDM')
sys.path.insert(0, aidm_path)

class VOCacheGenerator:
    """Voice Over Cache Generator for KOTOR.ai campaigns"""
    
    def __init__(self):
        self.supported_providers = {
            'elevenlabs': self._generate_elevenlabs_audio,
            'azure': self._generate_azure_audio,
            'opentts': self._generate_opentts_audio,
            'mock': self._generate_mock_audio
        }
        
        # Voice mappings for different character types
        self.voice_mappings = {
            # Species-based voices
            'human_male': 'voice_human_male_01',
            'human_female': 'voice_human_female_01',
            'twi_lek_male': 'voice_twilek_male_01',
            'twi_lek_female': 'voice_twilek_female_01',
            'rodian': 'voice_alien_01',
            'wookiee': 'voice_wookiee_01',
            'droid': 'voice_droid_01',
            
            # Role-based voices
            'jedi': 'voice_wise_01',
            'sith': 'voice_menacing_01',
            'soldier': 'voice_authoritative_01',
            'merchant': 'voice_friendly_01',
            'criminal': 'voice_rough_01',
            'noble': 'voice_refined_01',
            'pilot': 'voice_casual_01',
            'scientist': 'voice_intellectual_01',
            
            # Special characters
            'narrator': 'voice_narrator_01',
            'computer': 'voice_computer_01',
            'force_ghost': 'voice_ethereal_01'
        }
        
        # Default voice parameters
        self.default_voice_params = {
            'pitch': 1.0,
            'speed': 1.0,
            'stability': 0.75,
            'clarity': 0.75,
            'emotion': 0.5
        }

    def generate_campaign_vo(self, campaign_file: str, output_dir: str, 
                           provider: str = 'mock', api_key: str = '', 
                           force_regenerate: bool = False) -> Dict[str, Any]:
        """Generate voice over files for an entire campaign"""
        
        print(f"🎤 Generating VO for campaign: {campaign_file}")
        print(f"📁 Output directory: {output_dir}")
        print(f"🔊 Provider: {provider}")
        print()
        
        # Load campaign data
        try:
            with open(campaign_file, 'r', encoding='utf-8') as f:
                campaign_data = json.load(f)
        except Exception as e:
            print(f"❌ Error loading campaign file: {e}")
            return {'success': False, 'error': str(e)}
        
        # Create output directory structure
        os.makedirs(output_dir, exist_ok=True)
        
        # Extract all dialogue lines
        dialogue_lines = self._extract_dialogue_lines(campaign_data)
        
        print(f"📝 Found {len(dialogue_lines)} dialogue lines to process")
        
        # Generate VO for each line
        results = {
            'success': True,
            'total_lines': len(dialogue_lines),
            'generated': 0,
            'cached': 0,
            'failed': 0,
            'files': []
        }
        
        for i, (line_id, line_data) in enumerate(dialogue_lines.items()):
            print(f"🎙️ Processing {i+1}/{len(dialogue_lines)}: {line_data['speaker']}")
            
            # Generate cache key
            cache_key = self._generate_cache_key(line_data['text'], line_data['voice_profile'])
            output_file = os.path.join(output_dir, f"{cache_key}.wav")
            
            # Check if file already exists
            if os.path.exists(output_file) and not force_regenerate:
                print(f"   ✅ Using cached file: {os.path.basename(output_file)}")
                results['cached'] += 1
                results['files'].append(output_file)
                continue
            
            # Generate audio
            try:
                audio_data = self._generate_audio(
                    line_data['text'], 
                    line_data['voice_profile'], 
                    provider, 
                    api_key
                )
                
                if audio_data:
                    # Save audio file
                    with open(output_file, 'wb') as f:
                        f.write(audio_data)
                    
                    print(f"   ✅ Generated: {os.path.basename(output_file)}")
                    results['generated'] += 1
                    results['files'].append(output_file)
                else:
                    print(f"   ❌ Failed to generate audio")
                    results['failed'] += 1
                    
            except Exception as e:
                print(f"   ❌ Error generating audio: {e}")
                results['failed'] += 1
            
            # Small delay to avoid rate limiting
            if provider != 'mock':
                time.sleep(0.1)
        
        # Generate manifest file
        manifest_file = os.path.join(output_dir, 'vo_manifest.json')
        self._generate_manifest(dialogue_lines, results['files'], manifest_file)
        
        print()
        print(f"✅ VO generation complete!")
        print(f"   Generated: {results['generated']}")
        print(f"   Cached: {results['cached']}")
        print(f"   Failed: {results['failed']}")
        print(f"   Manifest: {manifest_file}")
        
        return results

    def _extract_dialogue_lines(self, campaign_data: Dict[str, Any]) -> Dict[str, Dict[str, Any]]:
        """Extract all dialogue lines from campaign data"""
        dialogue_lines = {}
        line_counter = 0
        
        # Extract from NPCs
        for npc in campaign_data.get('npcs', []):
            npc_id = npc.get('npc_id', f'npc_{line_counter}')
            
            # Get voice profile for NPC
            voice_profile = self._get_voice_profile_for_npc(npc)
            
            # Extract dialogue lines
            for dialogue in npc.get('dialogue_lines', []):
                line_counter += 1
                line_id = f"{npc_id}_line_{line_counter}"
                
                dialogue_lines[line_id] = {
                    'text': dialogue.get('text', ''),
                    'speaker': npc.get('name', 'Unknown'),
                    'speaker_id': npc_id,
                    'voice_profile': voice_profile,
                    'context': dialogue.get('context', ''),
                    'emotion': dialogue.get('emotion', 'neutral')
                }
        
        # Extract from quests
        for quest in campaign_data.get('quests', []):
            quest_id = quest.get('quest_id', f'quest_{line_counter}')
            
            # Quest giver dialogue
            if 'quest_giver_dialogue' in quest:
                for dialogue in quest['quest_giver_dialogue']:
                    line_counter += 1
                    line_id = f"{quest_id}_giver_line_{line_counter}"
                    
                    # Use default quest giver voice
                    voice_profile = self._get_voice_profile_for_role('quest_giver')
                    
                    dialogue_lines[line_id] = {
                        'text': dialogue.get('text', ''),
                        'speaker': dialogue.get('speaker', 'Quest Giver'),
                        'speaker_id': f"{quest_id}_giver",
                        'voice_profile': voice_profile,
                        'context': 'quest_dialogue',
                        'emotion': dialogue.get('emotion', 'neutral')
                    }
            
            # Completion dialogue
            if 'completion_dialogue' in quest:
                for dialogue in quest['completion_dialogue']:
                    line_counter += 1
                    line_id = f"{quest_id}_completion_line_{line_counter}"
                    
                    voice_profile = self._get_voice_profile_for_role('quest_giver')
                    
                    dialogue_lines[line_id] = {
                        'text': dialogue.get('text', ''),
                        'speaker': dialogue.get('speaker', 'Quest Giver'),
                        'speaker_id': f"{quest_id}_completion",
                        'voice_profile': voice_profile,
                        'context': 'quest_completion',
                        'emotion': dialogue.get('emotion', 'satisfied')
                    }
        
        # Extract narrator lines
        for event in campaign_data.get('timeline_events', []):
            if 'narration' in event:
                line_counter += 1
                line_id = f"narrator_line_{line_counter}"
                
                voice_profile = self._get_voice_profile_for_role('narrator')
                
                dialogue_lines[line_id] = {
                    'text': event['narration'],
                    'speaker': 'Narrator',
                    'speaker_id': 'narrator',
                    'voice_profile': voice_profile,
                    'context': 'narration',
                    'emotion': 'neutral'
                }
        
        return dialogue_lines

    def _get_voice_profile_for_npc(self, npc: Dict[str, Any]) -> Dict[str, Any]:
        """Get voice profile for an NPC"""
        species = npc.get('species', 'human').lower()
        gender = npc.get('gender', 'neutral').lower()
        role = npc.get('role', 'civilian').lower()
        
        # Try species + gender combination first
        species_gender_key = f"{species}_{gender}"
        if species_gender_key in self.voice_mappings:
            voice_id = self.voice_mappings[species_gender_key]
        elif species in self.voice_mappings:
            voice_id = self.voice_mappings[species]
        elif role in self.voice_mappings:
            voice_id = self.voice_mappings[role]
        else:
            voice_id = self.voice_mappings['human_male']  # Default fallback
        
        # Create voice profile
        voice_profile = self.default_voice_params.copy()
        voice_profile['voice_id'] = voice_id
        
        # Adjust parameters based on character traits
        personality = npc.get('personality_traits', [])
        if 'aggressive' in personality:
            voice_profile['pitch'] = 0.9
            voice_profile['emotion'] = 0.8
        elif 'gentle' in personality:
            voice_profile['pitch'] = 1.1
            voice_profile['emotion'] = 0.3
        elif 'wise' in personality:
            voice_profile['speed'] = 0.9
            voice_profile['stability'] = 0.9
        
        return voice_profile

    def _get_voice_profile_for_role(self, role: str) -> Dict[str, Any]:
        """Get voice profile for a specific role"""
        voice_id = self.voice_mappings.get(role, self.voice_mappings['human_male'])
        
        voice_profile = self.default_voice_params.copy()
        voice_profile['voice_id'] = voice_id
        
        return voice_profile

    def _generate_cache_key(self, text: str, voice_profile: Dict[str, Any]) -> str:
        """Generate a cache key for a text + voice combination"""
        # Create a string representation of voice profile
        voice_str = f"{voice_profile.get('voice_id', 'default')}_" \
                   f"{voice_profile.get('pitch', 1.0)}_" \
                   f"{voice_profile.get('speed', 1.0)}_" \
                   f"{voice_profile.get('emotion', 0.5)}"
        
        # Combine text and voice profile
        combined = f"{text}_{voice_str}"
        
        # Generate hash
        return hashlib.md5(combined.encode('utf-8')).hexdigest()

    def _generate_audio(self, text: str, voice_profile: Dict[str, Any], 
                       provider: str, api_key: str) -> Optional[bytes]:
        """Generate audio for a text line"""
        if provider not in self.supported_providers:
            print(f"❌ Unsupported provider: {provider}")
            return None
        
        return self.supported_providers[provider](text, voice_profile, api_key)

    def _generate_elevenlabs_audio(self, text: str, voice_profile: Dict[str, Any], 
                                  api_key: str) -> Optional[bytes]:
        """Generate audio using ElevenLabs API"""
        if not api_key:
            print("❌ ElevenLabs API key required")
            return None
        
        url = f"https://api.elevenlabs.io/v1/text-to-speech/{voice_profile.get('voice_id', 'default')}"
        
        headers = {
            'Accept': 'audio/mpeg',
            'Content-Type': 'application/json',
            'xi-api-key': api_key
        }
        
        data = {
            'text': text,
            'model_id': 'eleven_monolingual_v1',
            'voice_settings': {
                'stability': voice_profile.get('stability', 0.75),
                'similarity_boost': voice_profile.get('clarity', 0.75)
            }
        }
        
        try:
            response = requests.post(url, json=data, headers=headers, timeout=30)
            if response.status_code == 200:
                return response.content
            else:
                print(f"❌ ElevenLabs API error: {response.status_code}")
                return None
        except Exception as e:
            print(f"❌ ElevenLabs request error: {e}")
            return None

    def _generate_azure_audio(self, text: str, voice_profile: Dict[str, Any], 
                             api_key: str) -> Optional[bytes]:
        """Generate audio using Azure Speech API"""
        # Placeholder for Azure implementation
        print("⚠️ Azure TTS not implemented yet")
        return self._generate_mock_audio(text, voice_profile, api_key)

    def _generate_opentts_audio(self, text: str, voice_profile: Dict[str, Any], 
                               api_key: str) -> Optional[bytes]:
        """Generate audio using OpenTTS"""
        # Placeholder for OpenTTS implementation
        print("⚠️ OpenTTS not implemented yet")
        return self._generate_mock_audio(text, voice_profile, api_key)

    def _generate_mock_audio(self, text: str, voice_profile: Dict[str, Any], 
                            api_key: str) -> Optional[bytes]:
        """Generate mock audio for testing"""
        # Create a simple WAV file with silence
        # This is just for testing - replace with actual TTS in production
        
        # WAV header for 1 second of silence at 22050 Hz, 16-bit mono
        sample_rate = 22050
        duration = max(0.5, len(text) * 0.05)  # Rough estimate based on text length
        num_samples = int(sample_rate * duration)
        
        # WAV header
        wav_header = bytearray([
            0x52, 0x49, 0x46, 0x46,  # "RIFF"
            0x00, 0x00, 0x00, 0x00,  # File size (will be filled)
            0x57, 0x41, 0x56, 0x45,  # "WAVE"
            0x66, 0x6D, 0x74, 0x20,  # "fmt "
            0x10, 0x00, 0x00, 0x00,  # Subchunk1Size (16)
            0x01, 0x00,              # AudioFormat (PCM)
            0x01, 0x00,              # NumChannels (1)
            0x22, 0x56, 0x00, 0x00,  # SampleRate (22050)
            0x44, 0xAC, 0x00, 0x00,  # ByteRate
            0x02, 0x00,              # BlockAlign
            0x10, 0x00,              # BitsPerSample (16)
            0x64, 0x61, 0x74, 0x61,  # "data"
            0x00, 0x00, 0x00, 0x00   # Subchunk2Size (will be filled)
        ])
        
        # Generate silence (or simple tone for testing)
        audio_data = bytearray(num_samples * 2)  # 2 bytes per sample (16-bit)
        
        # Fill file size and data size
        file_size = len(wav_header) + len(audio_data) - 8
        data_size = len(audio_data)
        
        wav_header[4:8] = file_size.to_bytes(4, 'little')
        wav_header[-4:] = data_size.to_bytes(4, 'little')
        
        return bytes(wav_header + audio_data)

    def _generate_manifest(self, dialogue_lines: Dict[str, Dict[str, Any]], 
                          generated_files: List[str], manifest_file: str):
        """Generate a manifest file for the VO cache"""
        manifest = {
            'version': '1.0',
            'generated_at': time.strftime('%Y-%m-%d %H:%M:%S'),
            'total_lines': len(dialogue_lines),
            'generated_files': len(generated_files),
            'lines': {}
        }
        
        # Map dialogue lines to generated files
        for line_id, line_data in dialogue_lines.items():
            cache_key = self._generate_cache_key(line_data['text'], line_data['voice_profile'])
            audio_file = f"{cache_key}.wav"
            
            manifest['lines'][line_id] = {
                'text': line_data['text'],
                'speaker': line_data['speaker'],
                'audio_file': audio_file,
                'voice_profile': line_data['voice_profile'],
                'context': line_data['context']
            }
        
        # Save manifest
        with open(manifest_file, 'w', encoding='utf-8') as f:
            json.dump(manifest, f, indent=2, ensure_ascii=False)

def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="Generate VO cache for KOTOR.ai campaigns")
    parser.add_argument("campaign_file", help="Path to campaign JSON file")
    parser.add_argument("--output", "-o", default="Saved/VO", help="Output directory for VO files")
    parser.add_argument("--provider", "-p", choices=['elevenlabs', 'azure', 'opentts', 'mock'], 
                       default='mock', help="TTS provider to use")
    parser.add_argument("--api-key", "-k", default="", help="API key for TTS provider")
    parser.add_argument("--force", "-f", action="store_true", help="Force regenerate existing files")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    if args.verbose:
        print("🎤 KOTOR.ai VO Cache Generator")
        print("=" * 40)
        print(f"Campaign: {args.campaign_file}")
        print(f"Output: {args.output}")
        print(f"Provider: {args.provider}")
        print(f"Force regenerate: {args.force}")
        print()
    
    # Check if campaign file exists
    if not os.path.exists(args.campaign_file):
        print(f"❌ Campaign file not found: {args.campaign_file}")
        sys.exit(1)
    
    # Generate VO cache
    generator = VOCacheGenerator()
    results = generator.generate_campaign_vo(
        args.campaign_file,
        args.output,
        args.provider,
        args.api_key,
        args.force
    )
    
    if results['success']:
        print(f"✅ VO cache generation completed successfully!")
        sys.exit(0)
    else:
        print(f"❌ VO cache generation failed: {results.get('error', 'Unknown error')}")
        sys.exit(1)

if __name__ == "__main__":
    main()
