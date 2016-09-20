#include "ResourceLoader.hpp"
#include "Image.hpp"
#include "SoundEffect.hpp"
#include "Utils.hpp"
#include <FreeImage.h>
#include "File.hpp"
#include <ServiceException.hpp>
#include <iostream>

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

namespace Break
{
	namespace Infrastructure
	{
		std::shared_ptr<SoundEffect> loadWAV(std::string filePath){
			details::WAVHeader* musicHeader;
			SoundEffectPtr music = nullptr;
			File file;
			file.open(filePath);

			byte* buffer_header = new byte[sizeof(details::WAVHeader)];
			std::memset(buffer_header,0,sizeof(details::WAVHeader));
			file.read(sizeof(details::WAVHeader),buffer_header);

			musicHeader = reinterpret_cast<details::WAVHeader*>(buffer_header);

			musicHeader->ChunckID = Utils::reverseBytes(musicHeader->ChunckID);
			musicHeader->Format = Utils::reverseBytes(musicHeader->Format);
			musicHeader->subChunck1ID = Utils::reverseBytes(musicHeader->subChunck1ID);
			musicHeader->SubChunck2ID = Utils::reverseBytes(musicHeader->SubChunck2ID);

			byte* music_buffer = new byte[musicHeader->ChunckSize];
			std::memset(music_buffer,0,musicHeader->ChunckSize);
			file.read(musicHeader->ChunckSize,music_buffer);

			file.close();

			u32 ByteSize = musicHeader->BitsPerSample/8;
			if(ByteSize == 2){
				music = std::make_shared<SoundEffect>(music_buffer,musicHeader->ChunckSize);
				music->setSampleRate(musicHeader->SampleRate);
				music->setSampleSize(musicHeader->BitsPerSample/8);
			}else if(ByteSize > 2){
				u32 sample_count = (musicHeader->ChunckSize/ByteSize);
				byte* new_music_buffer = new byte[sample_count*2];
				std::memset(new_music_buffer,0,sample_count*2);
				u32 original_it = ByteSize, new_it = 0;

				for(u32 i=0;i<sample_count;i++){
					new_music_buffer[new_it] = music_buffer[original_it-2];
					new_it++;
					new_music_buffer[new_it] = music_buffer[original_it-1];
					new_it++;

					original_it += (ByteSize);
				}

				music = std::make_shared<SoundEffect>(new_music_buffer,sample_count*2);
				music->setSampleRate(musicHeader->SampleRate);
				music->setSampleSize(2);
				delete music_buffer;
			}
			return music;
		}

		template<>
		ImagePtr ResourceLoader<Image>::load(std::string file)
		{
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

			FIBITMAP* dib(0);

			BYTE* bits(0);

			unsigned int width(0), height(0);

			fif = FreeImage_GetFileType(file.c_str(),0);

			if(fif==FIF_UNKNOWN)
			{
				fif = FreeImage_GetFIFFromFilename(file.c_str());
			}

			if(fif == FIF_UNKNOWN)
			{
				throw ServiceException("Cannot load file named: "+file);
			}

			if(FreeImage_FIFSupportsReading(fif))
				dib = FreeImage_Load(fif,file.c_str());

			if(!dib)
				throw ServiceException("Cannot load file named: "+file);

			//FreeImage_ConvertToRGBAF()
			dib = FreeImage_ConvertTo32Bits(dib);
			FreeImage_FlipVertical(dib);
			bits = FreeImage_GetBits(dib);
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);

			if((bits == 0)||(width == 0)||(height == 0))
				throw ServiceException("Cannot load file named: "+file);

			Pixel* p = reinterpret_cast<Pixel*>(bits);
			ImagePtr res = make_shared<Image>(p,width,height);
			FreeImage_Unload(dib);
			return res;
		}

		template<>
		SoundEffectPtr ResourceLoader<SoundEffect>::load(std::string file){
			SoundEffectPtr res = nullptr;
			vector<char> delimiters(3);
			delimiters.push_back('.');
			delimiters.push_back('/');
			delimiters.push_back('\\');
			string file_ext = Utils::split(file,delimiters).back();

			if(file_ext == "wav"){
				res = loadWAV(file);
			}else{
				throw ServiceException("Unimplemented resource loader");
			}

			return res;
		}
	}
}
