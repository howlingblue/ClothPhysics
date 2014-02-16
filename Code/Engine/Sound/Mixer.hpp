#ifndef INCLUDED_MIXER_HPP
#define INCLUDED_MIXER_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include "../EngineDefines.hpp"

//-----------------------------------------------------------------------------------------------
ABSTRACT STATIC class Mixer
{
public:
	ABSTRACT class Sound
	{
	public:
		virtual bool IsPaused() = 0;
		virtual bool IsPlaying() = 0;
		virtual void Play() = 0;
		virtual void ShiftPitch( float valueBetweenHalfandTwo ) = 0;
		virtual void Stop() = 0;
	};

private:
	//Member Variables
	static Mixer* s_audioMixer;

	virtual void Initialize() = 0;

protected:
	static std::map< std::string, Sound* >	s_soundRegistry;

	virtual ~Mixer();

public:
	//Creation and Getting
	static void CreateMixer();
	static Mixer* GetMixer() { return s_audioMixer; }

	virtual void Update() = 0;

	virtual Sound* GetOrLoadSound( const std::string& fileLocation ) = 0;
};

inline Mixer::~Mixer()
{
	Sound* sound;
	for( std::map< std::string, Sound* >::iterator pair = s_soundRegistry.begin();
		pair != s_soundRegistry.end();
		++pair )
	{
		sound = pair->second;
		delete sound;
	}
	s_soundRegistry.clear();
}

#endif //INCLUDED_MIXER_HPP
