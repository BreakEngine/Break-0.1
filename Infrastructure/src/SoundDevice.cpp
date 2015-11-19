#include "SoundDevice.hpp"
#include <Services.hpp>
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

void SoundDevice::FeedAudio(byte* buffer, u32 framesPerBuffer, void* userData)
{
	auto IDevice = Services::getSoundDevice();
	SoundDevice* this_ptr = static_cast<SoundDevice*>(IDevice);

//	if(!this_ptr->m_buffer)
//		return;
//
//	if(this_ptr->m_bufferOffset >= this_ptr->m_bufferSize)
//		this_ptr->m_bufferOffset = 0;

	s16* output = reinterpret_cast<s16*>(buffer);

//	for(int j=0;j<this_ptr->m_sounds.size();++j)
//	{
//		auto playCursor = this_ptr->m_sounds[j]->getPlayingCursor();
//		s16* input = reinterpret_cast<s16*>(this_ptr->m_sounds[j]->getData()+playCursor);
//		s16 r_val = 0, l_val = 0;
//		for(int i=0;i<framesPerBuffer;++i)
//		{
//			l_val = static_cast<s16>(static_cast<real32>(*input++) * this_ptr->m_volume);
//			r_val = static_cast<s16>(static_cast<real32>(*input++) * this_ptr->m_volume);
//
//			playCursor+=this_ptr->getFormat().SampleSize;
//			if(playCursor>=this_ptr->m_sounds[j]->getBufferSize())
//			{
//				if(this_ptr->m_sounds[j]->isLooping())
//				{
//					this_ptr->m_sounds[j]->setPlayingCursor(0);
//					input = reinterpret_cast<s16*>(this_ptr->m_sounds[j]->getData());
//				}else{
//					break;
//				}
//
//			}else{
//				this_ptr->m_sounds[j]->setPlayingCursor(playCursor);
//			}
//		}
//	}

	for(int i=0;i<this_ptr->m_sounds.size();i++)
	{
		if(this_ptr->m_sounds[i].second)
		{
			this_ptr->m_sounds.erase(
				std::remove(this_ptr->m_sounds.begin(),
							this_ptr->m_sounds.end(),
							this_ptr->m_sounds[i]),
				this_ptr->m_sounds.end());
		}
	}

	for(int i=0;i<framesPerBuffer;i++)
	{
		s16 r_val=0,l_val=0;
		s16* input = nullptr;
		for(auto sound_it = this_ptr->m_sounds.begin();
			sound_it != this_ptr->m_sounds.end();
			++sound_it,input=nullptr)
		{
			auto sound = sound_it->first;

			if(!sound){
				sound_it->second = true;
				continue;
			}

			if(!sound->isPlaying())
				continue;

			if(!input)
				input = reinterpret_cast<s16*>(sound->getData()+sound->getPlayingCursor());

			auto playCursor = sound->getPlayingCursor();

			if(playCursor < sound->getBufferSize()){
				l_val += static_cast<s16>(static_cast<real32>(*input++) * this_ptr->m_volume * sound->getVolume());
				r_val += static_cast<s16>(static_cast<real32>(*input++) * this_ptr->m_volume * sound->getVolume());
			}

			playCursor += this_ptr->getFormat().SampleSize*2;
			if(playCursor>=sound->getBufferSize())
			{
				if(sound->isLooping())
				{
					sound->setPlayingCursor(4);
					input = nullptr;
				}else
				{
					//delete
					sound_it->second = true;
					break;
				}
			}else
			{
				sound->setPlayingCursor(playCursor);
			}
		}

		*output++ = l_val;
		*output++ = r_val;
	}

//	for(u32 i = 0; i < framesPerBuffer; ++i)
//	{
//		*output++ = static_cast<s16>(static_cast<real32>(*input++) * this_ptr->m_volume);
//		*output++ = static_cast<s16>(static_cast<real32>(*input++) * this_ptr->m_volume);
//	}
//
//	this_ptr->m_bufferOffset += framesPerBuffer*this_ptr->getFormat().SampleSize*2;

	return;
}

ISoundDevice::ISoundDevice(){

}

ISoundDevice::~ISoundDevice(){

}

SoundDevice::SoundDevice()
{
	m_volume = 1;
	m_written = 0;
	m_buffer = nullptr;
}

SoundDevice::~SoundDevice()
{
	m_sounds.clear();
}

void SoundDevice::setFormat(AudioFormat format)
{
	m_format = format;
}

AudioFormat SoundDevice::getFormat()
{
	return m_format;
}

void SoundDevice::stop(SoundEffect* track)
{
	for(int i=0;i<m_sounds.size();i++)
	{
		if(m_sounds[i].first == track)
		{
			m_sounds[i].second = true;
			break;
		}
	}
}

void SoundDevice::play(SoundEffect* track)
{
	m_sounds.push_back(make_pair(track,false));

	m_buffer = track->getData();
	m_bufferOffset = 0;
	m_bufferSize = track->getBufferSize();
}

void SoundDevice::setVolume(real64 volume)
{
	m_volume = volume;
}

real64 SoundDevice::getVolume()
{
	return m_volume;
}

GetAudioCallback SoundDevice::getAudioFeedCallback(){
	return &SoundDevice::FeedAudio;
}
