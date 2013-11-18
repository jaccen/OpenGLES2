#include "Audio.h"
#include "ResourceLoader.h"

#include <iostream>

Audio* Audio::sInstance = NULL;

Audio* Audio::get()
{
	if ( !sInstance ) {
		sInstance = new Audio();
	}
	return sInstance;
}

Audio::Audio()
{
	mDevice = alcOpenDevice( NULL );
	mContext = alcCreateContext( mDevice, NULL );
	alcMakeContextCurrent( mContext );
}

bool Audio::createSound( Sound* sound )
{
	alGenSources( 1, &sound->outputSource );
	
	alSourcef( sound->outputSource, AL_PITCH, 1.0f );
	alSourcef( sound->outputSource, AL_GAIN, 1.0f );
	
	alGenBuffers( 1, &sound->outputBuffer );
	
	ALenum error = alGetError();
	if ( AL_NO_ERROR != error ) {
		std::cout << "Audio::createSound -> Error " << error << " when attemping to open mDevice." << std::endl;
		return false;
	}
	
	if ( sound->data ) {
		alBufferData( sound->outputBuffer, AL_FORMAT_STEREO16, sound->data, sound->sizeBytes, 44100 );
		alSourcei( sound->outputSource, AL_BUFFER, sound->outputBuffer );
		
		free( sound->data );
		sound->data = NULL;
	}
	else {
		std::cout << "Error buffering sound data." << std::endl;
		return false;
	}
		
	return true;
}

void Audio::playSound( Sound* sound )
{
	alSourcePlay( sound->outputSource );
}

void Audio::destroySound( Sound* sound )
{
	alDeleteSources( 1, &sound->outputSource );
	alDeleteBuffers( 1, &sound->outputBuffer );
}

Audio::~Audio()
{
	alcDestroyContext( mContext );
	alcCloseDevice( mDevice );
}