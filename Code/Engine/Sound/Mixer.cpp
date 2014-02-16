#include "Mixer.hpp"
#include "FMODMixer.hpp"
#include "../../Game/EngineSettings.hpp"

STATIC Mixer* Mixer::s_audioMixer = nullptr;
STATIC std::map< std::string, Mixer::Sound* > Mixer::s_soundRegistry;

STATIC void Mixer::CreateMixer()
{
#ifdef AUDIO_USE_FMOD
	s_audioMixer = new FMODMixer();
	s_audioMixer->Initialize();
#endif

	if( s_audioMixer == nullptr )
		exit( -1 );
}
