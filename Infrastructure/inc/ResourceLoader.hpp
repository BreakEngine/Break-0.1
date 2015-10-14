#ifndef BREAK_0_1_RESOURCELOADER_HPP
#define BREAK_0_1_RESOURCELOADER_HPP

#include "Globals.hpp"
#include <string>
#include <memory>

namespace Break
{
	namespace Infrastructure
	{
		class Image;

		class BREAK_API ResourceLoader
		{
		public:
			static std::shared_ptr<Image> load(std::string file);

		};
	}
}

#endif