#ifndef BREAK_0_1_RESOURCELOADER_HPP
#define BREAK_0_1_RESOURCELOADER_HPP

#include "Globals.hpp"
#include <string>
#include <memory>
#include "ServiceException.hpp"

namespace Break
{
	namespace Infrastructure
	{

		namespace details{
			struct WAVHeader{
			    u32 ChunckID; //big
			    u32 ChunckSize; //little
			    u32 Format; //big
			    u32 subChunck1ID; //big
			    u32 subChunck1Size; //little
			    u16 AudioFormat; //little
			    u16 NumChannels; //little
			    u32 SampleRate; //little
			    u32 ByteRate; //little
			    u16 BlockAlign; //little
			    u16 BitsPerSample; //little
			    u32 SubChunck2ID; //big
			    u32 SubChunck2Size; //big
			};
		}

		class Image;
		class SoundEffect;

		template<class T>
		class BREAK_API ResourceLoader
		{		
		protected:

//			static std::shared_ptr<SoundEffect> loadWAV(std::string file);
		public:
			static std::shared_ptr<T> load(std::string file)
			{
				throw ServiceException("Unimplemented resource loader");
				return nullptr;
			}

//			
//			template<>
//			static std::shared_ptr<Image> load<Image>(std::string file);
//
//			template<>
//			static std::shared_ptr<SoundEffect> load<SoundEffect>(std::string file);
		};

		/*class BREAK_API ResourceLoader
		{

		public:

			template<class T>
			static std::shared_ptr<T> load(std::string file)
			{
				throw ServiceException("Unimplemented resource loader");
				return nullptr;
			}

			template<>
			static std::shared_ptr<Image> load<Image>(std::string file);

			template<>
			static std::shared_ptr<SoundEffect> load<SoundEffect>(std::string file);
		};*/
	}
}

#endif
