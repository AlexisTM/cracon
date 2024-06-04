#include <cstdio>
#include <flacon/flacon.hpp>

class Car {
 public:
  Car(flacon::SharedFile::Group config) {
    speed = config.get_param<int64_t>("speed", 9000);
    horsepower = config.get_param<int64_t>("horsepower", 120);
    motor_curve = config.get_param<std::array<int, 24>>("motor_curve", {});
  }

  flacon::SharedFile::Param<int64_t> speed;
  flacon::SharedFile::Param<int64_t> horsepower;
  flacon::SharedFile::Param<std::array<int, 24>> motor_curve;
};

int main() {
  // The SharedFile allows to use the Group and Param features
  flacon::SharedFile config;
  bool success = config.init("config.json", "defaults.json");
  if (!success) {
    fprintf(stderr, "We failed to open, read or write the configuration files.");
    exit(EXIT_FAILURE);
  }
  Car car = Car(config.get_group("car"));
  car.speed.set(1000);

  printf("speed: %lld, horsepower %lld\n", car.speed.get(), car.horsepower.get());

  // Avoid copies of large data. Note the `auto&`
  printf("curve: ");
  auto& motor_curve = car.motor_curve.get_ref();
  for(int i = 0; i < motor_curve.size(); i++) {
    printf("%lld ", motor_curve[i]);
  }
  printf("\n");

  config.write();
}
