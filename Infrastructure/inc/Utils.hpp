#ifndef BREAK_0_1_UTILS_HPP
#define BREAK_0_1_UTILS_HPP

#include "Globals.hpp"
#include <string>
#include <vector>

namespace Break
{
	class BREAK_API Utils
	{
	public:
		template<class T>
		static int find(const std::vector<T>& list,T element)
		{
			for(int i=0;i<list.size();i++)
			{
				if(list[i] == element)
					return i;
			}
			return -1;
		}

		static std::vector<std::string> split(const std::string& str, std::vector<char> delimiter)
		{
			std::vector<std::string> result;
			std::string word = "";
			for(int i=0;i<str.size();i++)
			{
				if(find(delimiter,str[i]) != -1)
				{
					if(word.size()>0){
						result.push_back(word);
						word.clear();
					}
				}else
				{
					word.push_back(str[i]);
				}
			}
			if(word.size()>0)
			{
				result.push_back(word);
				word.clear();
			}
			return result;
		}

		static u16 reverseBytes(u16 val)
		{
			u16 __x = val;
			__x = (
				((__x >> 8) & 0xFF) | ((__x & 0xFF) << 8)
			);
			return __x;
		}

		static u32 reverseBytes(u32 val)
		{
			u32 __x = val;
			__x = (
				((__x & 0xFF000000) >> 24) |
				((__x & 0x00FF0000) >> 8) |
				((__x & 0x0000FF00) << 8) |
				((__x & 0x000000FF) << 24)
			);
			return __x;
		}
	};
}

#endif
