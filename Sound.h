#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vector>


class Sound {
	public:
		Sound();
		
		bool play(const char* audioFile);
		void clean();
		~Sound();
	private:
		ALCdevice *device;
		ALCcontext *context;
		ALuint source;
		ALboolean enumeration;
		std::vector<ALChar*> devices;
		
		//Listener properties
		ALfloat listVelocity[] = {0.0f, 0.0f, 0.0f}, listPosition[] = {0.0f, 0.0f, 1.0f}; 
		ALfloat listOrientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		
		//Source properties
		ALfloat srcVelocity[] = {0.0f, 0.0f, 0.0f}, srcPosition[] = {0.0f, 0.0f, 0.0f};
		float pitch, gain;
		
		//Audio stream properties
		ALsizei size, freq;
		ALenum format;
		ALvoid *data;
		ALboolean loop = AL_FALSE;
		
		bool openDevice();
		bool createContext();
		bool createListener(ALfloat* position, ALfloat* velocity, ALfloat* orientation);
		bool createSource(ALfloat* position, ALfloat* velocity, float pitch, float gain);
		bool loadAudioStream(const char* audioFile);
		
		void devicesList();
		bool errorCheck();
};

#endif
