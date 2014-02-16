#include "FMODMixer.hpp"

//-----------------------------------------------------------------------------------------------
void FMODMixer::Initialize()
{
	FMOD::System* system;
	FMOD_RESULT result;
	unsigned int version;
	int numberOfSoundCards;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS soundCardCapabilities;
	static const unsigned int SOUND_CARD_NAME_BUFFER_SIZE = 256;
	char name[ SOUND_CARD_NAME_BUFFER_SIZE ];

	//create FMOD's interface
	result = FMOD::System_Create( &system );
	FMODErrorCheck( result );

	//Check fmod's libary version against our dll
	result = system->getVersion( &version );
	FMODErrorCheck( result );
	
	if( version < FMOD_VERSION )
	{
		std::cout << "FMOD error! FMOD version does not match DLL!" << std::endl;
		exit(-1);
	}

	//See how many sound cards are available for output.
	result = system->getNumDrivers( &numberOfSoundCards );
	FMODErrorCheck( result );
	if ( numberOfSoundCards == 0 )
	{
		//If we have no sound card, set output type to nothing.
		result = system->setOutput( FMOD_OUTPUTTYPE_NOSOUND );
		FMODErrorCheck( result );
	}
	else
	{
		//Get the name of the sound card
		static FMOD_GUID* guiInfo = NULL;
		result = system->getDriverInfo( 0, name, SOUND_CARD_NAME_BUFFER_SIZE, guiInfo );

		//Get the capabilities of sound card zero.
		result = system->getDriverCaps( 0, &soundCardCapabilities, 0, &speakerMode );
		FMODErrorCheck( result );

		//Match speaker mode to the mode in the control panel
		result = system->setSpeakerMode( speakerMode );
		FMODErrorCheck( result );

		//If sound card hardware acceleration is disabled, make the software buffer larger.
		if( soundCardCapabilities & FMOD_CAPS_HARDWARE_EMULATED )
		{
			static const unsigned int SOUND_BUFFER_SIZE = 1024;
			static const int NUMBER_OF_SOUND_BUFFERS = 10;
			result = system->setDSPBufferSize( SOUND_BUFFER_SIZE, NUMBER_OF_SOUND_BUFFERS );
			FMODErrorCheck( result );
		}

		if (strstr(name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			PCM floating point output seems to solve it.
			*/
			result = system->setSoftwareFormat( 48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR );
			FMODErrorCheck( result );
		}
	}

	//Initialize FMOD
	static const int MAXIMUM_SOUND_CHANNELS = 100;
	static void* NO_EXTRA_DRIVER_FLAGS = NULL;
	result = system->init( MAXIMUM_SOUND_CHANNELS, FMOD_INIT_NORMAL, NO_EXTRA_DRIVER_FLAGS );

	//If our sound mode setting caused an error during initialization, set it back to stereo and reinit
	if ( result == FMOD_ERR_OUTPUT_CREATEBUFFER )
	{
		result = system->setSpeakerMode( FMOD_SPEAKERMODE_STEREO );
		FMODErrorCheck( result );
		result = system->init( MAXIMUM_SOUND_CHANNELS, FMOD_INIT_NORMAL, NO_EXTRA_DRIVER_FLAGS );
		FMODErrorCheck( result );
	}

	m_audioSystem = system;
}

//-----------------------------------------------------------------------------------------------
Mixer::Sound* FMODMixer::GetOrLoadSound( const std::string& fileLocation )
{
	std::map< std::string, Sound* >::iterator soundIterator = s_soundRegistry.find( fileLocation );

	if( soundIterator == s_soundRegistry.end() )
	{
		Sound* sound = new FMODSound( *this, fileLocation );
		return sound;
	}

	return soundIterator->second;
}
