//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_OBJECT_HPP
#define BREAK_0_1_OBJECT_HPP

#include "Globals.hpp"
#include <string>
#include <functional>
#include <memory>
#include <functional>

namespace Break{

    class BREAK_API Object{
    protected:
        ///object types
        type_id m_type;
    public:


        static const type_id None = GENERATE_TYPE_ID;
        ///Type code of Object class
        RTTI(Object);

        ///object id
        std::string id;

        Object(std::string name="Object", type_id type = Object::Type)
                :id(name), m_type(type)
        {

        }

        Object(const Object& val){
            m_type = val.m_type;
        }

        virtual ~Object(){id.clear(); m_type=Object::None;}

        type_id getType(){
            return m_type;
        }

        u64 getHashID(){
            static std::hash<std::string> hash_fn;
            return hash_fn(id);
        }
    };
    typedef std::shared_ptr<Object> ObjectPtr;
}
#endif //BREAK_0_1_OBJECT_HPP
