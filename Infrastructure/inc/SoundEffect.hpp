#pragma once
#include "Globals.hpp"
#include "Object.hpp"
#include <memory>

namespace Break {
	namespace Infrastructure{
		class BREAK_API SoundEffect:public Object {
		protected:
			u32 m_dataSize;
			byte* m_data;
			real64 m_volume;
			u32 m_playingCursor;
			u32 m_SampleRate;
			u32 m_SampleSize;
			bool m_loop;
			bool m_playing, m_pausing;

		public:
			RTTI(SoundEffect);

			SoundEffect(byte* dataRecived,u32 dataSize, real64 volume = 1);
			~SoundEffect();

			real64 getVolume();
			void setVolume(real64 newVolume);

			u32 getPlayingCursor();
			void setPlayingCursor(u32 newCursor);

			byte* getData();
			u32 getBufferSize();

			bool isLooping();
			void setLooping(bool val);

			void setSampleRate(u32 val);
			u32 getSampleRate();

			void setSampleSize(u32 val);
			u32 getSampleSize();

			bool isPlaying();

			void play(bool looping = false);
			void pause();
			void stop();

		};
		typedef std::shared_ptr<SoundEffect> SoundEffectPtr;
	}
}
