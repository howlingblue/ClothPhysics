#ifndef INCLUDED_FMOD_MIXER_HPP
#define INCLUDED_FMOD_MIXER_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
//Disable warning for warning C4505: 'FMOD_ErrorString' : unreferenced local function has been removed
#include <iostream>
#pragma warning( disable: 4505 )
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment( lib, "fmodex_vc.lib" ) //Link in FMod's static library
#include "../Math/EngineMath.hpp"
#include "Mixer.hpp"


//-----------------------------------------------------------------------------------------------
STATIC class FMODMixer : Mixer
{
public:
	class FMODSound : public Mixer::Sound
	{
		FMOD::Channel* m_channel;
		FMOD::DSP*		m_dsp;
		FMOD::Sound* m_sound;
		FMOD::System* m_soundSystem;
	public:
		FMODSound( FMODMixer& mixer, const std::string& soundLocation )
			: m_soundSystem( mixer.m_audioSystem )
			, m_channel( nullptr )
			, m_dsp( nullptr )
		{
			m_soundSystem->createSound( soundLocation.c_str(), FMOD_DEFAULT, false, &m_sound );
			mixer.s_soundRegistry[ soundLocation ] = this;
		}
		~FMODSound()
		{
			m_sound->release();
		}

		bool IsPaused()
		{
			bool isPaused;
			if( m_channel == nullptr ) return true;
			m_channel->getPaused( &isPaused );
			return isPaused;
		}
		bool IsPlaying()
		{
			bool isPlaying;
			if( m_channel == nullptr ) return false;
			m_channel->isPlaying (&isPlaying );
			return isPlaying;
		}
		void Play() 
		{ 
			FMOD_CHANNELINDEX channelProperty = FMOD_CHANNEL_REUSE;
			if( m_channel == nullptr )
				channelProperty = FMOD_CHANNEL_FREE;

			m_soundSystem->playSound( channelProperty, m_sound, false, &m_channel );
			ShiftPitch( ( GetRandomFloatBetweenZeroandOne() * 1.f ) + 0.75f ); 
		}
		void Stop() { }
		void ShiftPitch( float valueBetweenHalfandTwo )
		{
				m_soundSystem->createDSPByType( FMOD_DSP_TYPE_PITCHSHIFT, &m_dsp );
				m_channel->addDSP( m_dsp, 0);
			m_dsp->setParameter( FMOD_DSP_PITCHSHIFT_PITCH, valueBetweenHalfandTwo );
		}
	};
private:
	friend class Mixer;

	FMOD::System* m_audioSystem;

	FMODMixer() { }
	~FMODMixer() 
	{ 
		m_audioSystem->release(); 
	}

	//Copy and assign are not allowed
	FMODMixer( const FMODMixer& );
	void operator=( const FMODMixer& );

	void FMODErrorCheck( FMOD_RESULT result );
	void Initialize();

public:
	Sound* GetOrLoadSound( const std::string& fileLocation );
	void Update() { m_audioSystem->update(); }
};

//-----------------------------------------------------------------------------------------------
inline void FMODMixer::FMODErrorCheck( FMOD_RESULT result )
{
	switch( result )
	{
	case FMOD_OK:
		return;
	default:
		std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
		exit(-1);
	}
}

#endif //INCLUDED_FMOD_MIXER_HPP
