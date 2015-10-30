#ifndef BREAK_0_1_SOUNDDEVICE_HPP
#define BREAK_0_1_SOUNDDEVICE_HPP

#include "Globals.hpp"

namespace Break{
	namespace Infrastructure{

		struct AudioFormat
		{
			u16 FormatTag;
			u16 Channels;
			u32 SamplesPerSec;
			u32 AvgBytePerSec;
			u16 BlockAlign;
			u16 BitsPerSample;
			u16 Size;
		};

		class SoundDevice;
		typedef void (*GetAudioCallback)(byte*,u32,SoundDevice*);

		class BREAK_API SoundDevice{
		public:
			static void FeedAudio(byte* buffer, u32 buffer_size, SoundDevice* this_ptr)
			{
				printf("%d\n",buffer_size);
				static s32 samplesPerSec=  this_ptr->getFormat().SamplesPerSec;
				static u32 RunningSampleIndex = 0;
				static s32 Hz = 256;
				static s32 SquareWavePeriod = samplesPerSec/Hz;

				s16 *SampleOut = (s16*)buffer;
				u32 region1SampleCount = buffer_size/this_ptr->m_format.BlockAlign;
				for(u32 sampleIndex = 0;
					sampleIndex < region1SampleCount;
					++sampleIndex)
				{
					s16 SampleValue = (RunningSampleIndex / (SquareWavePeriod/2))%2 ? 1000: -1000;
					//cout<<SampleValue<<endl;
					//printf("%d\n",SampleValue);
					*SampleOut++ = SampleValue;
					*SampleOut++ = SampleValue/2;
					++RunningSampleIndex;
				}
			}

			SoundDevice(){}

			virtual ~SoundDevice(){}

			void setFormat(AudioFormat format)
			{
				m_format = format;
			}

			AudioFormat getFormat()
			{
				return m_format;
			}
			
		private:
			AudioFormat m_format;
		};
		typedef std::shared_ptr<SoundDevice> SoundDevicePtr;
	}
}
#endif