#include "Sound.h"
#include <iostream>


using namespace std;

Sound::Sound(){
	listOrientation[0] = 0.0f;
	listOrientation[1] = 0.0f;
	listOrientation[2] = 1.0f;
	listOrientation[3] = 0.0f;
	listOrientation[4] = 1.0f;
	listOrientation[5] = 0.0f;
	
	listPosition[0] = 0.0f;
	listPosition[1] = 0.0f;
	listPosition[2] = 0.0f;
}

bool Sound::openDevice(){
	device = alcOpenDevice(NULL);
	
	if(!device){
		std::cerr << "No audio devices present." << std::endl;
		return false;
	}
	
	return true;
}

void Sound::devicesList(){
	const ALCchar *device = alcGetString(NULL, ALC_DEVICE_SPECIFIER), *next = devices + 1;
    size_t len = 0;

	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	
	if(enumeration == ALC_FALSE){
		std::clog << "Enumeration not supported." << std::endl;
		return ;
	}else{
		while (device && *device != '\0' && next && *next != '\0') {
            devices.push_back(device);
            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
    	}	
	}
}

bool Sound::errorCheck(std::string error){
	if(alGetError() != AL_NO_ERROR){
		std::cerr << error << std::endl;
		return true;
	}
	
	return false;	
}

bool Sound::createContext(){
	context = alcCreateContext(device, NULL);
	
	if(!alcMakeContextCurrent(context)){
		std::cerr << "Failed to make context current." << std::endl;		
		return false;
	}
	
	if(this->errorCheck("Failed to create context.")){
		return false;
	}
	
	return true;
}

bool Sound::createListener(ALfloat* position, ALfloat* velocity, ALfloat* orientation){
	int i;
		
	if(orientation != NULL){
		for(i = 0; i < 3; i++){
			listOrientation[i] = orientation[i];
		}
	}	
	if(position != NULL){
		for(i = 0; i < 3; i++){
			listPosition[i] = position[i];
		}
	}
	if(velocity != NULL){
		for(i = 0; i < 3; i++){
			listVelocity[i] = velocity[i];
		}
	}	
		
	alListener3f(AL_POSITION, listOrientation[0], listOrientation[1], listenerOrientation[2]);
	if(this->errorCheck("Failed to set listener position.")) return false;
	
	alListener3f(AL_VELOCITY, listVelocity[0], listVelocity[1], listVelocity[2]);
	if(this->errorCheck("Failed to set listener position.")) return false;
	
	alListenerfv(AL_ORIENTATION, listenerOrientation);
	if(this->errorCheck("Failed to set listener orientation.")) return false;
	
	return true;
}

bool Sound::createSource(ALfloat* position, ALfloat* velocity, float pitch, float gain){
	int i;
		
	if(position != NULL){
		for(i = 0; i < 3; i++){
			srcPosition[i] = position[i];
		}
	}
	if(velocity != NULL){
		for(i = 0; i < 3; i++){
			srcVelocity[i] = velocity[i];
		}
	}	

	alGenSources((ALuint) 1, &source);
	if(this->errorCheck("Failed to generate audio source.")) return false;

	alSourcef(source, AL_PITCH, pitch);
	if(this->errorCheck("Failed to set audio pitch.")) return false;
	
	alSourcef(source, AL_GAIN, gain);
	if(this->errorCheck("Failed to set audio gain.")) return false;
	
	alSourcef(source, AL_POSITION, srcPosition[0], srcPosition[1], srcPosition[2]);
	if(this->errorCheck("Failed to set audio position.")) return false;
	
	alSourcef(source, AL_VELOCITY, srcVelocity[0], srcVelocity[1], srcVelocity[2]);
	if(this->errorCheck("Failed to set audio velocity.")) return false;
	
	alSourcei(source, AL_LOOPING, AL_FALSE);
	if(this->errorCheck("Failed to set audio loop to false.")) return false;
	
	return true;
}

bool Sound::loadAudioStream(const char* audioFile){
	alutLoadWAVFile(audioFile, &format, &data, &size, &freq, &loop);
	if(this->errorCheck("Failed to load audio stream.")) return false;
	
	return true;
}

void Sound::clean(){
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void Sound::play(const char* audioFile){
	ALboolean source_state;
	
	this->createContext();
	this->createListener(NULL, NULL, NULL);
	this->createSource(NULL, NULL, 1, 1);
	this->loadAudioStream(audioFile);
	
	alSourcei(source, AL_BUFFER, buffer);
	if(this->errorCheck("Failed to bind the source to the buffer.")) return false;
	
	alSourcePlay(source);
	if(this->errorCheck("Failed to play the audio.")) return false;
	
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	if(this->errorCheck("Failed to get source state.")) return false;
	
	while(source_state == AL_PLAYING){
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
		if(this->errorCheck("Failed to get source state.")) return false;
	}
	
//	this->clean();
}

Sound::~Sound(){
	this->clean();
}
