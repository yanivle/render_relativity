#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "filters.h"
#include "image.h"
#include "palette.h"
#include "progress.h"
#include "rand_utils.h"
#include "range.h"
#include "vec3.h"

ABSL_FLAG(int, iterations, 10000, "the number of iterations");
ABSL_FLAG(int, print_every, 1000,
          "will print the status every this number of iterations");
ABSL_FLAG(int, num_creatures, 1000, "the number of creatures");
ABSL_FLAG(int, num_predators, 1, "the number of predators");
ABSL_FLAG(int, num_foods, 10, "the number of foods");
ABSL_FLAG(float, initial_energy, 2.0, "the creature's initial energy");
ABSL_FLAG(float, reproduction_energy, 3.0,
          "the energy needed for reproduction");
ABSL_FLAG(float, velocity, 0.01, "the creature's velocity");
ABSL_FLAG(float, predator_velocity, 0.002, "the predators's velocity");
ABSL_FLAG(float, move_cost, 0.01, "the cost of moving");
ABSL_FLAG(float, living_cost, 0.001, "the cost of living");
ABSL_FLAG(float, eat_radius, 0.01, "can eat within this distance of the food");
ABSL_FLAG(float, eatten_radius, 0.01,
          "can eat within this distance of the food");
ABSL_FLAG(float, food_energy_gain, 0.001, "energy gained by eating");
ABSL_FLAG(float, food_energy, 1.0, "the starting energy in a new food");
ABSL_FLAG(float, food_spawn_prob, 0.01,
          "the probability of spawning a new food each iteration");
ABSL_FLAG(float, predator_spawn_prob, 0.01,
          "the probability of spawning a new predator each iteration");
ABSL_FLAG(float, view_radius, 0.1, "the view radius");
ABSL_FLAG(float, predator_energy, 0.5, "the initial energy of a predator");
ABSL_FLAG(float, mutation_amplitude, 0.01,
          "the max amount of noise to add to genes");

float rnd() { return float(rand()) / float(RAND_MAX - 1); }

float rnd(float min, float max) { return min + rnd() * (max - min); }

std::string to_fixed_string(const float f, const int n = 3) {
  std::ostringstream out;
  out.precision(n);
  out << std::fixed << f;
  return out.str();
}

struct Brain {
  Brain() : predator_weight(rnd(-1, 1)), food_weight(rnd(-1, 1)) {}

  float predator_weight;
  float food_weight;

  std::string str() const {
    return std::string("Brain(food:") + to_fixed_string(food_weight) + ")";
  }
};

struct Food {
  vec3 pos = vec3(rnd(), rnd(), 0);
  float energy = absl::GetFlag(FLAGS_food_energy);
};

struct Predator {
  vec3 pos = vec3(rnd(), rnd(), 0);
  vec3 dir = vec3(rnd(-1, 1), rnd(-1, 1), 0).normalize() *
             absl::GetFlag(FLAGS_predator_velocity);
  float energy = absl::GetFlag(FLAGS_predator_energy);

  void update() {
    energy -= absl::GetFlag(FLAGS_living_cost);
    pos += dir;
    if (pos.x > 1) {
      pos.x -= 2;
    } else if (pos.x < -1) {
      pos.x += 2;
    }

    if (pos.y > 1) {
      pos.y -= 2;
    } else if (pos.y < -1) {
      pos.y += 2;
    }
  }
};

struct Surroundings {
  std::vector<Food*> foods;
  std::vector<Predator*> predators;
};

struct Creature {
  Brain brain;
  static int global_id;
  int id = global_id++;
  vec3 pos = vec3(rnd(), rnd(), 0);
  float energy = absl::GetFlag(FLAGS_initial_energy);

  std::string str() const {
    return std::string("Creature(id:" + std::to_string(id) + ",brain:") +
           brain.str() + ",pos:" + to_fixed_string(pos.x) + ',' +
           to_fixed_string(pos.y) + ",energy:" + to_fixed_string(energy) + ")";
  }

  bool sees(const vec3& point) const {
    return dist(pos, point) < absl::GetFlag(FLAGS_view_radius);
  }

  bool eat(const Surroundings& surroundings) {
    for (Food* food : surroundings.foods) {
      if (dist(food->pos, pos) < absl::GetFlag(FLAGS_eat_radius)) {
        food->energy -= absl::GetFlag(FLAGS_food_energy_gain);
        energy += absl::GetFlag(FLAGS_food_energy_gain);
        return true;
      }
    }
    return false;
  }

  bool eatten(const Surroundings& surroundings) {
    for (Predator* predator : surroundings.predators) {
      if (dist(predator->pos, pos) < absl::GetFlag(FLAGS_eatten_radius)) {
        return true;
      }
    }
    return false;
  }

  template <class T>

  const T* findClosest(const std::vector<T*>& v) {
    float closest_dist2 = 1000;
    const T* closest = 0;
    for (const T* x : v) {
      if ((x->pos - pos).len2() < closest_dist2) {
        closest_dist2 = (x->pos - pos).len2();
        closest = x;
      }
    }
    return closest;
  }

  void move(const Surroundings& surroundings) {
    vec3 dir;

    const Predator* closest_predator = findClosest(surroundings.predators);
    if (closest_predator) {
      vec3 to_predator = (closest_predator->pos - pos);
      dir = to_predator * brain.predator_weight;
    } else {
      const Food* closest_food = findClosest(surroundings.foods);
      if (closest_food) {
        vec3 to_food = (closest_food->pos - pos);
        dir = to_food * brain.food_weight;
      } else {
        dir = vec3(rnd(-1, 1), rnd(-1, 1), 0);
      }
    }

    if (dir.len() > absl::GetFlag(FLAGS_velocity)) {
      dir.len(absl::GetFlag(FLAGS_velocity));
    }
    pos += dir;
    energy -= absl::GetFlag(FLAGS_move_cost) * dir.len();
  }

  Creature reproduce() {
    // energy -= absl::GetFlag(FLAGS_reproduction_energy);
    energy /= 2;
    Creature res = *this;
    pos -= vec3(0.01, 0.01, 0);
    res.pos += vec3(0.01, 0.01, 0);
    res.id = global_id++;
    res.brain.food_weight += rnd(-absl::GetFlag(FLAGS_mutation_amplitude),
                                 absl::GetFlag(FLAGS_mutation_amplitude));
    res.brain.predator_weight += rnd(-absl::GetFlag(FLAGS_mutation_amplitude),
                                     absl::GetFlag(FLAGS_mutation_amplitude));
    return res;
  }

  void update(const Surroundings& surroundings) {
    energy -= absl::GetFlag(FLAGS_living_cost);
    if (eatten(surroundings)) {
      energy = -1;
    } else if (!eat(surroundings)) {
      move(surroundings);
    }
  }
};
int Creature::global_id = 0;

template <class T>
void filter_dead(std::vector<T>& v) {
  std::vector<T> alive;
  alive.reserve(v.size());
  for (const T& x : v) {
    if (x.energy > 0) {
      alive.push_back(x);
    }
  }
  v = alive;
}

struct World {
  std::vector<Creature> creatures;
  std::vector<Predator> predators;
  std::vector<Food> foods;

  World() { init(); }

  void init() {
    creatures.reserve(absl::GetFlag(FLAGS_num_creatures));
    for (int i = 0; i < absl::GetFlag(FLAGS_num_creatures); ++i) {
      creatures.push_back(Creature());
    }

    foods.reserve(absl::GetFlag(FLAGS_num_foods));
    for (int i = 0; i < absl::GetFlag(FLAGS_num_foods); ++i) {
      foods.push_back(Food());
    }

    predators.reserve(absl::GetFlag(FLAGS_num_predators));
    for (int i = 0; i < absl::GetFlag(FLAGS_num_predators); ++i) {
      predators.push_back(Predator());
    }
  }

  Surroundings surroundings(const Creature& creature) {
    Surroundings res;
    for (auto& food : foods) {
      if (creature.sees(food.pos)) {
        res.foods.push_back(&food);
      }
    }

    for (auto& predator : predators) {
      if (creature.sees(predator.pos)) {
        res.predators.push_back(&predator);
      }
    }

    return res;
  }

  void update() {
    std::vector<Creature> alive;
    alive.reserve(creatures.size() + 10);
    for (auto& creature : creatures) {
      creature.update(surroundings(creature));
      if (creature.energy > 0) {
        alive.push_back(creature);
        if (creature.energy > absl::GetFlag(FLAGS_reproduction_energy)) {
          alive.push_back(creature.reproduce());
        }
      }
    }
    creatures = alive;

    filter_dead(foods);
    if (rnd() < absl::GetFlag(FLAGS_food_spawn_prob)) {
      foods.push_back(Food());
    }

    for (auto& predator : predators) {
      predator.update();
    }

    filter_dead(predators);
    if (rnd() < absl::GetFlag(FLAGS_predator_spawn_prob)) {
      predators.push_back(Predator());
    }
  }

  void render(int i) const {
    Image img(500, 500);
    for (auto& creature : creatures) {
      int x = interpolate(creature.pos.x, Range(0, 1), Range(0, img.width()));
      int y = interpolate(creature.pos.y, Range(0, 1), Range(0, img.height()));
      if (img.is_safe(x, y)) {
        img(x, y) += Color(0, 0.4, 0);
      }
    }
    for (auto& food : foods) {
      int x = interpolate(food.pos.x, Range(0, 1), Range(0, img.width()));
      int y = interpolate(food.pos.y, Range(0, 1), Range(0, img.height()));
      if (img.is_safe(x, y)) {
        img(x, y) += Color(0.3, 0.3, 0);
      }
    }
    img.save(std::string("/tmp/language_evolution_anim_") + std::to_string(i) +
             ".ppm");
  }

  void print() const {
    std::cout << creatures.size() << " creatures alive" << std::endl;
    float total_food_energy = 0;
    for (const Food& f : foods) {
      total_food_energy += f.energy;
    }
    std::cout << foods.size() << " foods alive with total energy of "
              << to_fixed_string(total_food_energy) << std::endl;

    float average_food_weight = 0;
    for (const Creature& creature : creatures) {
      average_food_weight += creature.brain.food_weight;
    }
    average_food_weight /= creatures.size();
    std::cout << "Average food weight: " << average_food_weight << std::endl;

    float average_predator_weight = 0;
    for (const Creature& creature : creatures) {
      average_predator_weight += creature.brain.predator_weight;
    }
    average_predator_weight /= creatures.size();
    std::cout << "Average predator weight: " << average_predator_weight
              << std::endl;

    std::cout << std::endl;
  }
};

template <class T>
void graph(Image& image, const std::vector<T>& v, const Color& color) {
  T m = *std::max_element(v.cbegin(), v.cend());
  const int padding = 100;
  for (int i = 0; i < v.size(); ++i) {
    int x = interpolate(i, Range(0, v.size()), Range(0, image.width()));
    int y = interpolate(v[i], Range(0, m),
                        Range(image.height() - padding, padding));
    image(x, y) = color;
  }
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  World world;

  std::vector<int> creatures_over_time;
  creatures_over_time.reserve(absl::GetFlag(FLAGS_iterations));
  std::vector<int> foods_over_time;
  foods_over_time.reserve(absl::GetFlag(FLAGS_iterations));

  for (int i = 0; i < absl::GetFlag(FLAGS_iterations); ++i) {
    creatures_over_time.push_back(world.creatures.size());
    foods_over_time.push_back(world.foods.size());
    // if (world.creatures.size() > 0) {
    //   std::cout << world.creatures[0].str() << std::endl;
    // }
    if (i % absl::GetFlag(FLAGS_print_every) == 0) {
      world.print();
    }

    std::cerr << world.creatures.size() << " " << world.foods.size() << " "
              << world.predators.size() << "*\r";

    world.update();
  }

  std::cerr << "Simulation complete, max creatures "
            << *std::max_element(creatures_over_time.cbegin(),
                                 creatures_over_time.cend())
            << ", max foods "
            << *std::max_element(foods_over_time.cbegin(),
                                 foods_over_time.cend())
            << std::endl;

  world.print();

  Image image(1000, 500);
  graph(image, creatures_over_time, Color(1, 0, 0));
  graph(image, foods_over_time, Color(0, 0, 1));
  image.show();

  return EXIT_SUCCESS;
}
