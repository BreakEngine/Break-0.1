//
// Created by Moustapha on 01/10/2015.
//

#include "IGXDevice.hpp"
#include "Engine.hpp"
#include "Services.hpp"
using namespace Break;
using namespace Break::Infrastructure;

void IGXDevice::gameloop() {
    Services::getEngine()->gameloop();
}