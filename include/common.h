#pragma once
#include <string>
#include "URLDepth.h"

struct Page {
    std::string url;
    int depth;
    std::string html;
};