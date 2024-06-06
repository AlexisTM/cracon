#include <cracon/cracon.hpp>

int main() {
  cracon::File config = cracon::File("config.json", "defaults.json");

  int int_value = config.get("/int", 1);
  auto some_string = config.get<std::string>("/oh/hi", "mark");
  auto a_vector = config.get<std::vector<float>>("/vector", {1., 2., 3.});

  int_value = config.set("/int", 42);
  if (config.should_write()) {  // Only write it if there is a change
    config.write();
  }
}
