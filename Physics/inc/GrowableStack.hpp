#pragma once
#include "Globals.hpp"

namespace Break
{
	namespace Physics
	{

		/// This is a growable LIFO stack with an initial capacity of N.
		/// If the stack size exceeds the initial capacity, the heap is used
		/// to increase the size of the stack.

		template <typename T, s32 N>
		class BREAK_API GrowableStack
		{
		public:
			GrowableStack()
			{
				m_stack = m_array;
				m_count = 0;
				m_capacity = N;
			}

			~GrowableStack()
			{
				if (m_stack != m_array)
				{
					free(m_stack);
					m_stack = NULL;
				}
			}

			void Push(const T& element)
			{
				if (m_count == m_capacity)
				{
					T* old = m_stack;
					m_capacity *= 2;
					m_stack = (T*)malloc(m_capacity * sizeof(T));
					memcpy(m_stack, old, m_count * sizeof(T));
					if (old != m_array)
					{
						free(old);
					}
				}

				m_stack[m_count] = element;
				++m_count;
			}

			T Pop()
			{
				assert(m_count > 0);
				--m_count;
				return m_stack[m_count];
			}

			s32 GetCount()
			{
				return m_count;
			}

		private:
			T* m_stack;
			T m_array[N];
			s32 m_count;
			s32 m_capacity;
		};



	}
}