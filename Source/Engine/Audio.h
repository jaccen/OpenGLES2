#pragma once

#include <string>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "Resources.h"

class Audio {
public:
	static Audio* get();
	
	bool createSound( Sound* sound );
	void playSound( Sound* sound );
	void destroySound( Sound* sound );
	
private:
	static Audio* sInstance;
	Audio();
	virtual ~Audio();
	
	ALCdevice* mDevice;
	ALCcontext* mContext;
};