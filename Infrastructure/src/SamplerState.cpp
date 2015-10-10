//
// Created by Moustapha on 04/10/2015.
//

#include "SamplerState.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

SamplerState::SamplerState(){
    filter = TextureFilter::LINEAR;
    addressU = TextureAddressMode::WRAP;
    addressV = TextureAddressMode::WRAP;
    addressW = TextureAddressMode::WRAP;
    borderColor = Color(255,255,255,255);
    compareFunction = CompareFunction::NEVER;
}

SamplerState::SamplerState(const SamplerState& val){
    addressU = val.addressU;
    addressV = val.addressV;
    addressW = val.addressW;
    borderColor = val.borderColor;
    compareFunction = val.compareFunction;
}

SamplerState::~SamplerState(){
    //delete sampler state
}