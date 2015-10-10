//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_ASSETMANAGER_HPP
#define BREAK_0_1_ASSETMANAGER_HPP

#include "Globals.hpp"
#include "Asset.hpp"
#include <memory>
#include <map>
#include <string>

namespace Break{
    namespace Infrastructure{
        class BREAK_API AssetManager{
        protected:

            std::map<std::string, AssetPtr> m_assetTable;

            /*
			 * \brief populates the table with the default assets
			 * \author Moustapha Saad
			 */
            void populateDefaultAssets();
            /**
			 * \brief performs memory cleanup before destruction
			 * \author Moustapha Saad
			 */
            void cleanUp();

        public:
            AssetManager();

            virtual ~AssetManager();

            /**
			 * \brief search the table for an asset
			 * \param id name of the asset
			 * \return Asset pointer
			 * \author Moustapha Saad
			 */
            Asset* find(std::string id);

            /**
			 * \brief adds an asset to asset table
			 * \param asset asset shared pointer
			 * \author Moustapha Saad
			 */
            void add(AssetPtr asset);
        };
        typedef std::shared_ptr<AssetManager> AssetManagerPtr;
    }
}
#endif //BREAK_0_1_ASSETMANAGER_HPP
