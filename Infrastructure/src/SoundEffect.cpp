#include "SoundEffect.hpp"
#include <Services.hpp>

using namespace Break;
using namespace Break::Infrastructure;
using namespace std;
SoundEffect::SoundEffect(byte* dataRecived,u32 dataSize, real64 volume ){
	m_data=dataRecived;
	m_volume=volume;
	m_playingCursor=0;
	m_dataSize=dataSize;
	m_loop = false;
	m_playing = false;
	m_pausing = false;
	m_SampleRate = 0;
	m_SampleSize = 0;
}
SoundEffect::~SoundEffect(){
    delete m_data;
}
real64 SoundEffect::getVolume(){
    return m_volume;
}
void SoundEffect::setVolume(real64 newVolume){
    m_volume=newVolume;
}
u32 SoundEffect::getPlayingCursor(){
    return m_playingCursor;
}

void SoundEffect::setPlayingCursor(u32 newCursor){
    m_playingCursor=newCursor;
}

byte* SoundEffect::getData(){
    return m_data;
}

u32 SoundEffect::getBufferSize(){
    return m_dataSize;
}

bool SoundEffect::isLooping()
{
	return m_loop;
}

void SoundEffect::setLooping(bool val)
{
	m_loop = val;
}

bool SoundEffect::isPlaying(){
	return m_playing;
}

void SoundEffect::play(bool looping){
	m_playing = true;
	m_pausing = false;
	m_loop = looping;
	Services::getSoundDevice()->play(this);
}

void SoundEffect::pause(){
	m_playing = false;
	m_pausing = true;
	Services::getSoundDevice()->stop(this);
}

void SoundEffect::stop(){
	m_playing = false;
	m_pausing = false;
	m_playingCursor = 0;
	Services::getSoundDevice()->stop(this);
}

void SoundEffect::setSampleRate(u32 val)
{
	m_SampleRate = val;
}

u32 SoundEffect::getSampleRate(){
	return m_SampleRate;
}

void SoundEffect::setSampleSize(u32 val){
	m_SampleSize = val;
}

u32 SoundEffect::getSampleSize(){
	return m_SampleSize;
}
