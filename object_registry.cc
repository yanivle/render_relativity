#include "object_registry.h"

#include "sdf.h"

namespace registry {
ObjectRegistry::~ObjectRegistry() {
    for (SDF* object : objects) {
        delete object;
    }
}

ObjectRegistry registry;
}


