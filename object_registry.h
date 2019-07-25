#ifndef OBJECT_REGISTRY_H
#define OBJECT_REGISTRY_H

#include <vector>

class SDF;

namespace registry {

class ObjectRegistry {
  public:
    void registerObject(SDF* s) {
        objects.push_back(s);
    }

    ~ObjectRegistry();

    int numObjects() const {
        return objects.size();
    }
  private:
    std::vector<SDF*> objects;
};

extern ObjectRegistry registry;

}

#endif
