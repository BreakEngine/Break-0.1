//
// Created by Moustapha on 02/10/2015.
//

#include "RAMBuffer.hpp"
#include <cstring>
#include <ServiceException.hpp>

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

RAMBuffer::RAMBuffer(){
    m_size = 0;
    m_capacity = 0;
    m_data = nullptr;
    m_owner = false;
    m_changed = false;
}

RAMBuffer::RAMBuffer(u32 size){
    m_capacity = size;
    m_size = 0;
    m_data = new byte[m_capacity];
    m_owner = true;
    m_changed = false;
}

RAMBuffer::RAMBuffer(const RAMBuffer& val){
    m_capacity = val.m_capacity;
    m_size = val.m_size;
    m_data = val.m_data;
    m_owner = false;
    m_changed = false;
}

RAMBuffer::~RAMBuffer(){
    if(m_data && m_owner)
        delete[] m_data;
}

void RAMBuffer::clear(){
    if(m_data){
        m_size = 0;
        //set bytes to zero
        memset(m_data,0,m_capacity);
        m_changed = true;
    }
}

void RAMBuffer::append(void* data, u32 size){
    if(m_size + size <= m_capacity){
        memcpy(m_data+m_size,data,size);
        m_size += size;
        m_changed = true;
    }else{
        throw ServiceException("Memory out of capacity");
    }
}

void RAMBuffer::append(RAMBuffer& buffer){
    if(m_size + buffer.m_size <= m_capacity){
        memcpy(m_data+m_size,buffer.m_data,buffer.m_size);
        m_size += buffer.m_size;
        m_changed = true;
    }else{
        throw ServiceException("Memory out of capacity");
    }
}

void RAMBuffer::map(void* data,u32 size,u32 start){
    if(start+size<=m_capacity){
        memcpy(m_data+start,data,size);
        m_changed = true;
    }else{
        throw ServiceException("Memory out of bound");
    }
}

void RAMBuffer::reallocate(u32 size){
    if(m_owner)
        delete[] m_data;

    m_capacity = size;
    m_size = 0;
    m_data = new byte[m_size];
    m_changed = true;
}

byte* RAMBuffer::getData(u32 offset){
    if(offset<=m_capacity)
        return (byte*)(m_data+offset);
    else
        return nullptr;
}

u32 RAMBuffer::getCapacity(){
    return m_capacity;
}

u32 RAMBuffer::getDataSize(){
    return m_size;
}

void RAMBuffer::deleteBuffer(){
    if(m_data)
        delete[] m_data;
}

void RAMBuffer::copyHandle(void* ptr, u32 size){
    m_data = (byte*)ptr;
    m_owner = false;
    m_capacity = size;
    m_size = size;
}

void RAMBuffer::copyBuffer(void* ptr, u32 size){
    m_capacity = size;
    m_size = size;
    m_data = new byte[m_size];
    m_owner = true;
    memcpy(m_data,ptr,size);
}

RAMBufferPtr RAMBuffer::clone(){
    auto ret = make_shared<RAMBuffer>();
    ret->copyBuffer(m_data,m_capacity);
    return ret;
}

RAMBuffer* RAMBuffer::asHandle(void* ptr,u32 size){
    RAMBuffer* ret = new RAMBuffer();
    ret->copyHandle(ptr,size);
    return ret;
}

std::shared_ptr<RAMBuffer> RAMBuffer::asBuffer(void* ptr,u32 size){
    RAMBufferPtr ret = make_shared<RAMBuffer>();
    ret->copyBuffer(ptr,size);
    return ret;
}
