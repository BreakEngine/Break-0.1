//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_ENTITY_HPP
#define BREAK_0_1_ENTITY_HPP

#include <Globals.hpp>
#include "Component.hpp"
#include <memory>
#include <vector>
#include <TimeStep.hpp>

namespace Break{
    namespace Graphics{
        class BREAK_API Entity: public Object{
        protected:
            ///children list
            std::vector<std::shared_ptr<Entity> > m_children;

            ///components list
            std::vector<ComponentPtr> m_components;

            ///parent pointer
            Entity* m_parent;

        public:
            RTTI(Entity);

            ///default constructor
            Entity():Object("Entity",Entity::Type){
                m_parent = nullptr;
            }

            /**
             * \brief naming constructor that assigns a name to entity
             * \param id name of the entity
             * \author Moustapha Saad
             */
            Entity(std::string id):Object(id,Entity::Type)
            {
                m_parent = nullptr;
            }

            ///virtual destructor
            virtual ~Entity()
            {
                m_children.clear();
                m_components.clear();
                m_parent = nullptr;
            }

            /**
             * \brief returns the first child that has the sent id
             * \param id name of the entity
             * \return Entity pointer
             * \author Moustapha Saad
             */
            Entity* find(std::string id){
                for(auto child : m_children){
                    if(child->id == id)
                        return child.get();
                }
                return nullptr;
            }

            /**
			 * \brief finds a child with a specific name
			 * \param name name of the entity that will be searched for
			 * \return list of the found children
			 * \author Moustapha Saad
			 */
            std::vector<Entity*> find_all(std::string id){
                std::vector<Entity*> res;
                for(auto child : m_children){
                    if(child->id == id)
                        res.push_back(child.get());
                }
                return res;
            }

            /**
			 * \brief performs DFS search for a child entity with a name
			 * \param name name of the entity that will be searched for
			 * \return pointer to Entity if found otherwise NULL
			 * \author Moustapha Saad
			 */
            Entity* search(std::string id)
            {
                for(auto child :m_children){
                    if(child->id == id){
                        return child.get();
                    }else{
                        auto res = child->search(id);
                        if(res != NULL)
                            return res;
                    }
                }
                return NULL;
            }

            template<class T>
            void addComponent()
            {
                ComponentPtr component = std::make_shared<T>();
                component->gameEntity = this;
                m_components.push_back(component);
            }

            std::vector<Component*> getComponents(std::string id){
                std::vector<Component*> res;
                for(auto c : m_components)
                    if(c->id == id)
                        res.push_back(c.get());
                return res;
            }

            std::vector<Component*> getComponents(type_id id){
                std::vector<Component*> res;
                for(auto c : m_components)
                    if(c->getType() == id)
                        res.push_back(c.get());
                return res;
            }

            Component* getComponent(std::string id){
                for(auto c : m_components)
                    if(c->id == id)
                        return c.get();
                return nullptr;
            }

            Component* getComponent(type_id id){
                for(auto c : m_components)
                    if(c->getType() == id)
                        return c.get();
                return nullptr;
            }

            /**
			 * \brief input function that invokes children input function
			 *
			 * \author Moustapha Saad
			 */
            virtual void input()
            {
                for(auto child:m_children)
                    child->input();
            }

            /**
             * \brief update function that invokes children update function
             * \param tick time step that holds delta time and elapsed time
             * \author Moustapha Saad
             */
            virtual void update(Infrastructure::TimeStep tick){
                for(auto child:m_children)
                    child->update(tick);
            }

            /**
             * \brief draw function that invokes children draw function
             *
             * \author Moustapha Saad
             */
            virtual void draw(){
                for(auto child:m_children)
                    child->draw();
            }

            /**
             * \brief add child to children list
             * \param child shared_ptr of Entity
             *
             * \author Moustapha Saad
             */
            void addChild(std::shared_ptr<Entity> child)
            {
                m_children.push_back(child);
                child->m_parent = this;
            }

        };
        typedef std::shared_ptr<Entity> EntityPtr;
    }
}
#endif //BREAK_0_1_ENTITY_HPP
