# Cracon C++ Library

Cracon stands for Craft Config.

**Cracon** is a lightweight C++ library for handling _JSON_ configuration files. It provides a simple and intuitive API for reading, writing configurations. It outputs two files, one `default` with all defaults parameters set in the code, and one for changed parameters.

It supports (tested for):
- booleans
- enums
- (u)int(00)_t
- float/double
- std::vector
- std::string
- std::array

Most of the heavy lifting is done by [Niels Lohmann's awesome JSON library](https://json.nlohmann.me).

## Features

- **Easy** to use
- Somewhat **Type-safe** (it is still JSON)
- **JSON** format to be human readable
- Defaults are defined **in the code**
- Defaults are **reported** to allow review of the defaults in CI/CD
- Defaults are **not mangled** to the actual configuration to allow to modify defaults who were not explicitly modified
- Only variations are in the json file

## Build & test

```bash
# For Linux
git clone https://github.com/AlexisTM/cracon
mkdir cracon/build && cd cracon/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build .
# For Windows
cmake --build . --config Release

ctest
```

## Integration in your project

This library uses [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) for dependency management. This permits many other usage, see [examples/cmake...](examples/) for different integrations.

```cmake
include(cmake/CPM.cmake)
CPMAddPackage(
  NAME cracon
  GITHUB_REPOSITORY alexistm/cracon
  GIT_TAG main
  VERSION 0.0.1
  OPTIONS "BUILD_EXAMPLES OFF" "BUILD_TESTING OFF"
)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} cracon)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

```

## API

### Basic usage

```cpp
cracon::File config;
bool success = config.init("config.json", "defaults.json");
if (!success) {
  // The file couldn't be opened/read/written/created
}

int int_value = config.get("/int", 1);
auto some_string = config.get<std::string>("/oh/hi", "mark");
auto a_vector = config.get<std::vector<float>>("/vector", {1., 2., 3.});

int_value = config.set("/int", 42);
if(config.should_write()) { // Only write it if there is a change
  config.write();
}
```

Will result in:

```json
// "defaults.json"
{
  "int": 1,
  "oh": { "hi": "mark" },
  "data": [1.0, 2.0, 3.0]
}

// config.json
{
  "int": 42,
}
```

### Parameter groups

Avoid typos by using a parameter group.

```c++
cracon::SharedFile config;
bool success = config.init("config.json", "defaults.json");
if (!success) {
  fprintf(stderr, "We failed to open, read or write the configuration files.");
  exit(EXIT_FAILURE);
}
auto car = config.get_group("car");
auto motor = car.get_group("motor");
auto speed = motor.get("speed", 9000);

auto the_same_motor = config.get_group("car/motor");
int horsepower = the_same_motor.get("horsepower", 120);

printf("Speed: %d, horsepower %d\n", speed, horsepower);
config.write();

// {
//   "car": {
//     "motor": {
//       "speed": 9000,
//       "horsepower": 120
//     }
//   }
// }
```

### Param helper

Live parameters is a wrapper around a value. It wraps a value to use a single line to read/write a parameter and avoid typos in the configuration path.

```c++
cracon::SharedFile config;
bool success = config.init("config.json", "defaults.json");
auto some_int = config.get_param("int", 42); // cracon::File::Param<int>
int value = some_int.get(); // 42
int other_value = some_int.set(50); // Write down 50 & updates internals
```

## Considerations

- If `File::get` is called multiple times, only the last call defines the default. Call `File::get` once for consistency or use `Param::get` which won't reparse the data each time. This check was not added as it increases the overhead significantly if it is called often/big configuration files.
- `set` does not write to the defaults

## Contributing

Contributions to Cracon are welcome! Feel free to open issues, submit pull requests, or provide feedback. Especially if you are a CMake Guru and knows how to ensure this is as easy to integrate as possible.

## License

Cracon is released under the MIT License. See the [LICENSE](LICENSE) file for details.
