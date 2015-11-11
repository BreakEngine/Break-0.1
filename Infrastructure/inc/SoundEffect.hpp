#pragma once
#include "Globals.hpp"
namespace Break {
  namespace Infrastructure{
    class BREAK_API SoundEffect {
      private:
        u32 m_dataSize;
        byte* m_data;
        real64 m_volume;
        u32 m_playingCursor;
      public:
        SoundEffect(byte* dataRecived,u32 dataSize, real64 volume = 1);
        ~SoundEffect();
        real64 getVolume();
        void setVolume(real64 newVolume);
        u32 getPlayingCursor();
        void setPlayingCursor(u32 newCursor);
        byte* getData();
        u32 getBufferSize();

    };


  }
}
