//
// Created by autoboat on 21/12/25.
//

#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "external/crow_websvr.hpp"
#include "movement_controller.h"

std::future<void> start_webserver(std::shared_ptr<movement_controller> movement_controller_ptr);

#endif //WEBSERVER_H
