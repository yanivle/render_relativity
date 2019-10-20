#include "scenes.h"

#include <map>
#include <memory>
#include <iostream>

#include "../logging.h"
#include "../singleton.h"

class SceneRegistry {
 public:
  void RegisterScene(Scene *scene) {
    std::cout << "Registering scene " << scene->name() << std::endl;
    registry_[scene->name()].reset(scene);
  }

  Scene *GetScene(const std::string &name) {
    auto it = registry_.find(name);
    CHECK(it != registry_.end());
    return it->second.get();
  }

 private:
  std::map<std::string, std::shared_ptr<Scene>> registry_;
};

SINGLETON(SceneRegistry, SceneRegistrySingleton);

namespace scenes {

void RegisterScene(Scene *scene) {
  SceneRegistrySingleton::instance().RegisterScene(scene);
}
Scene *GetScene(const std::string &name) {
  return SceneRegistrySingleton::instance().GetScene(name);
}

}  // namespace scenes
