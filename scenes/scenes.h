#include "../scene.h"

#define DEFINE_SCENE(name) class name : public Scene {\
 public:\
  name();\
};

namespace scenes {

DEFINE_SCENE(Spheres);
DEFINE_SCENE(Capsules);
DEFINE_SCENE(ChoppedCube);
DEFINE_SCENE(Stars);

}  // namespace scenes
