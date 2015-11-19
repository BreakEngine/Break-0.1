#ifndef BREAK_0_1_SOUNDDEVICE_HPP
#define BREAK_0_1_SOUNDDEVICE_HPP

#include "Globals.hpp"
#include "SoundEffect.hpp"
#include <vector>

namespace Break{
	namespace Infrastructure{

		struct AudioFormat
		{
			u16 Channels;
			u32 SamplesPerSec;
			u16 SampleSize;

			AudioFormat()
			{
				Channels = 2;
				SamplesPerSec = 48000;
				SampleSize = 2;
			}
		};

		typedef void (*GetAudioCallback)(byte*,u32,void*);

		class BREAK_API ISoundDevice{
		public:
			ISoundDevice();
			virtual ~ISoundDevice();

			virtual void play(SoundEffect* track)=0;

			virtual void stop(SoundEffect* track)=0;

			virtual void setVolume(real64 volume)=0;
			virtual real64 getVolume()=0;

			virtual void setFormat(AudioFormat format)=0;
			virtual AudioFormat getFormat()=0;

			virtual GetAudioCallback getAudioFeedCallback()=0;
		};
		typedef std::shared_ptr<ISoundDevice> ISoundDevicePtr;

		class BREAK_API Engine;

		class BREAK_API SoundDevice : public ISoundDevice{
			friend class Engine;

			static void FeedAudio(byte* buffer, u32 framesPerBuffer, void* userData);
		public:

			SoundDevice();

			virtual ~SoundDevice();

			void setFormat(AudioFormat format) override;

			AudioFormat getFormat() override;

			void play(SoundEffect* track) override;

			void stop(SoundEffect* track) override;

			void setVolume(real64 volume) override;

			real64 getVolume() override;

			GetAudioCallback getAudioFeedCallback() override;
		private:
			AudioFormat m_format;
			real64 m_volume;
			std::vector<std::pair<SoundEffect*,bool> > m_sounds;

			byte* m_buffer;
			u32 m_bufferOffset;
			u32 m_bufferSize;
			u32 m_written;
		};
		typedef std::shared_ptr<SoundDevice> SoundDevicePtr;
	}
}
#endif
