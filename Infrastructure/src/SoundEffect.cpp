#include "SoundEffect.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace std;
SoundEffect::SoundEffect(byte* dataRecived,u32 dataSize, real64 volume ){
  m_data=dataRecived;
  m_volume=volume;
  m_playingCursor=0;
  m_dataSize=dataSize;
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
