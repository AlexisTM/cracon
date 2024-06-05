#include <cracon/cracon.hpp>

int main(int argc, char** argv) {
  cracon::File config;
  bool success = config.init("config.json", "defaults.json");
  if (!success) {
    fprintf(stderr, "We failed to open, read or write the configuration files.");
    exit(EXIT_FAILURE);
  }

  int int_value = config.get("/int", 1);
  auto some_string = config.get<std::string>("/oh/hi", "mark");
  auto a_vector = config.get<std::vector<float>>("/vector", {1., 2., 3.});

  int_value = config.set("/int", 42);
  if(config.should_write()) { // Only write it if there is a change
    config.write();
  }
  exit(EXIT_SUCCESS);
}
