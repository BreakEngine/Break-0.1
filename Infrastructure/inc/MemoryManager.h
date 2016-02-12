#pragma once

#include "Globals.hpp"
#include "Block.h"
#include <memory>
#include "Services.hpp"

namespace Break
{
	namespace Infrastructure
	{

		class BREAK_API Memory
		{
		public:

			Memory(Block mem_handle);
			~Memory();

			Block allocateBlock(size_t size);
			void deallocateBlock(Block mem);

			bool owns(void* ptr);

			void deallocateAll();
		private:
			Block Allocation_Algorithm(size_t size);

			struct MemoryNode
			{
				Block block;
				bool free;
				MemoryNode *next, *prev;

				MemoryNode()
				{
					next = nullptr;
					prev = nullptr;
					free = true;
				}
				~MemoryNode()
				{
					if (next)
						delete next;
				}
			};

			//free node linked list holds the free space data
			MemoryNode* m_memoryRoot;
			//holds the memory allocated for the system
			Block m_memory;
			//allocated size
			size_t m_allocated;
			//free size
			size_t m_free;
		public:

			template<class T>
			static T* allocate()
			{
				Memory* mem = Services::getMemory();
				Block blk;

				try {
					blk = mem->allocateBlock(sizeof(T));
				}
				catch (...)
				{
					throw;
				}

				return ::new(blk.ptr) T();
			}

			template<class T, typename ... TArgs>
			static T* allocate(TArgs ... args)
			{
				Memory* mem = Services::getMemory();
				Block blk;

				try {
					blk = mem->allocateBlock(sizeof(T));
				}
				catch (...)
				{
					throw;
				}

				return ::new(blk.ptr) T(args...);
			}


			template<class T>
			static void deallocate(T* ptr)
			{
				Memory* mem = Services::getMemory();
				Block blk;
				blk.size = sizeof(T);
				blk.ptr = ptr;
				mem->deallocateBlock(blk);
			}

			template<class T>
			static std::shared_ptr<T> make_shared()
			{
				return std::shared_ptr<T>(allocate<T>());
			}

			template<class T, typename ... T_Args>
			static std::shared_ptr<T> make_shared(T_Args ... args)
			{
				return std::shared_ptr<T>(allocate<T>(args...));
			}

			template<class T>
			static std::shared_ptr<T> make_unique()
			{
				return std::unique_ptr<T>(allocate<T>());
			}

			template<class T, typename ... T_Args>
			static std::shared_ptr<T> make_unique(T_Args ... args)
			{
				return std::unique_ptr<T>(allocate<T>(args...));
			}

			static constexpr int MEM_START_ADDRESS = 0x10000000;
		};
	}
}