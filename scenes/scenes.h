#include "../scene.h"

#include <string>

#define DEFINE_SCENE(name)    \
  class name : public Scene { \
   public:                    \
    name();                   \
  };                          \
  static SceneRegisterer<name> registerer;

namespace scenes {

void RegisterScene(Scene* scene);
Scene* GetScene(const std::string& name);

template <class SceneClass>
class SceneRegisterer {
 public:
  SceneRegisterer() { RegisterScene(new SceneClass); }
};

}  // namespace scenes
