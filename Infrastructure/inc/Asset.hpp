//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_ASSET_HPP
#define BREAK_0_1_ASSET_HPP

#include "Globals.hpp"
#include "Object.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
        /**
		 * \brief represents a Asset class
		 * \author Moustapha Saad
		 */
        class BREAK_API Asset: public Object{
        public:

            RTTI(Asset);

            /**
			 * \brief defualt constructor
			 * \param name name of asset (OPTIONAL) default is empty string
			 * \param type type of the asset (OPTIONAL) default is UNDEFINED
			 * \author Moustapha Saad
			 */
            Asset(std::string name="Asset", type_id type = Asset::Type):Object(name,type){}
        };
        typedef std::shared_ptr<Asset> AssetPtr;
    }
}
#endif //BREAK_0_1_ASSET_HPP
