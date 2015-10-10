//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_COMPONENT_HPP
#define BREAK_0_1_COMPONENT_HPP

#include <Globals.hpp>
#include <memory>

namespace Break{
    namespace Graphics{
        class Entity;
        class Component: public Object{
        public:
            RTTI(Component);

            Entity* gameEntity;

            Component(std::string id):Object(id,Component::Type){
                gameEntity = nullptr;
            }

            Component():Object("Component",Component::Type)
            {
                gameEntity = nullptr;
            }

            virtual ~Component(){
                gameEntity = nullptr;
            }
        };
        typedef std::shared_ptr<Component> ComponentPtr;
    }
}
#endif //BREAK_0_1_COMPONENT_HPP
