//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_RAMBUFFER_HPP
#define BREAK_0_1_RAMBUFFER_HPP

#include "Globals.hpp"
#include <memory>
namespace Break{
    namespace Infrastructure{
        ///represents a chunck of bytes in ram
        class BREAK_API RAMBuffer{
        protected:

            ///pointer to data
            byte* m_data;

            ///data capacity
            u32 m_capacity;

            ///current size of data
            u32 m_size;

            ///indicate if it's the owner and should free memory of not
            bool m_owner;

            ///changed flag
            bool m_changed;
        public:

            RAMBuffer();

            /**
			 * \brief size init constructor
			 * \param size size of the buffer
			 * \author Moustapha Saad
			 */
            RAMBuffer(u32 size);

            ///copy constructor
            RAMBuffer(const RAMBuffer& val);

            ///virtual destructor
            virtual ~RAMBuffer();

            ///reallocates memory
            void reallocate(u32 size);

            ///changed getter and setter
            bool isChanged(){
                return m_changed;
            }

            void setChanged(bool val){
                m_changed = val;
            }

            /**
			 * \brief appends data to buffer
			 * \param data pointer to data to be appended
			 * \param size size of the data
			 * \author Moustapha Saad
			 */
            void append(void* data,u32 size);

            ///appends two RAM buffers
            void append(RAMBuffer&);

            /**
             * \brief overwrites a certain amount of data in ram
             * \param data pointer to data in memory
             * \param size size of the data
             * \param start offset inside the buffer
             */
            void map(void* data,u32 size,u32 start);

            ///cleans the data
            void clear();

            ///returns pointer to the data
            byte* getData(u32 offset=0);

            ///returns size of the buffer
            u32 getCapacity();

            ///returns used size
            u32 getDataSize();

            ///deletes current buffer
            void deleteBuffer();

            /*
             * \brief shallow copy buffer from meory
             * \param ptr pointer to buffer in memory
             * \param size size of the buffer in bytes
             */
            void copyHandle(void* ptr,u32 size);

            /*
             * \brief deep copy buffer from meory
             * \param ptr pointer to buffer in memory
             * \param size size of the buffer in bytes
             */
            void copyBuffer(void* ptr,u32 size);

            /*
             * \brief shallow copy buffer from raw pointer to buffer
             * \param ptr pointer to buffer in memory
             * \param size size of the buffer in bytes
             * \return RAMBuffer smart pointer
             */
            static RAMBuffer* asHandle(void* ptr,u32 size);

            /*
             * \brief deep copy buffer from raw pointer to buffer
             * \param ptr pointer to buffer in memory
             * \param size size of the buffer in bytes
             * \return RAMBuffer smart pointer
             */
            static std::shared_ptr<RAMBuffer> asBuffer(void* ptr,u32 size);


            ///returns a deep copy of the buffer
            std::shared_ptr<RAMBuffer> clone();
        };
        typedef std::shared_ptr<RAMBuffer> RAMBufferPtr;
    }
}
#endif //BREAK_0_1_RAMBUFFER_HPP
