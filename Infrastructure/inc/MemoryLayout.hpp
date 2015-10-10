//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_MEMORYLAYOUT_HPP
#define BREAK_0_1_MEMORYLAYOUT_HPP

#include "Globals.hpp"
#include "MemoryElement.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
        /**
		 * \brief represnets memory input layout
		 * \author Moustapha Saad
		 */
        class BREAK_API MemoryLayout{
        protected:
            ///size of full data
            u32 m_size;

            ///calculates the size of the memory layout
            u32 calcSize();
        public:
            ///elements vector
            std::vector<MemoryElement> elements;

            ///default constructor
            MemoryLayout();
            ///vector init constructor
            MemoryLayout(std::vector<MemoryElement>& e);
            ///raw pointer init constructor
            MemoryLayout(MemoryElement* e, u32 count);
            ///copy constrcutor
            MemoryLayout(const MemoryLayout& val);

            /**
             * \brief appends element to layout
             * \param v element to be appended
             * \author Moustapha Saad
             */
            void append(MemoryElement v);

            ///size getter
            u32 getSize();

            ///count getter
            u32 getElementCount();

            ///equality check function
            bool equals(const MemoryLayout& val);

            ///destrcutor
            ~MemoryLayout();

        };
    }
}
#endif //BREAK_0_1_MEMORYLAYOUT_HPP
