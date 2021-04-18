#pragma once
#include <iostream>
#include <memory>
#define ASSERT(x, errorMSG) if(!(x)) { std::cout << errorMSG << std::endl; __debugbreak(); }