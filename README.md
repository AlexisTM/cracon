# Cracon, Craft your configuration!

![cracon uwu logo](doc/img/cracon.jpg)

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

## API

### Basic usage

```cpp
auto config = cracon::File("config.json", "defaults.json");
// Or config.init("config.json", "defaults.json")

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

### Parameters and groups

Groups avoids typos when repeating the same namespace multiple times.
Parameters wraps a value to use a single line to read/write a parameter and avoids repeating the accessor/key.

Both groups and parameters keep a shared_ptr to the File, which is initially created by the SharedFile.

```c++
class Car {
 public:
  Car(cracon::SharedFile::Group config) {
    speed = config.get_param<int64_t>("speed", 9000);
    horsepower = config.get_param<int64_t>("horsepower", 120);
    motor_curve = config.get_param<std::array<int, 24>>("motor_curve", {});
  }

  cracon::SharedFile::Param<int64_t> speed;
  cracon::SharedFile::Param<int64_t> horsepower;
  cracon::SharedFile::Param<std::array<int, 24>> motor_curve;
};

int main() {
  auto config = cracon::SharedFile("config.json", "defaults.json");
  Car car = Car(config.get_group("car"));
  car.speed.set(1000);
  config.write();
}
```

## Debugging

Build this project with `-DCRACON_ENABLE_LOG=ON` to enable logging.

```
$ ./cracon_basic_usage.exe
[cracon] [INFO] The requested key doesn't exist for /oh/hi defaulted to "mark". Error: [json.exception.out_of_range.403] key 'oh' not found
[cracon] [INFO] The requested key doesn't exist for /vector defaulted to [1.0,2.0,3.0]. Error: [json.exception.out_of_range.403] key 'vector' not found
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

## Considerations

- If `File::get` is called multiple times, only the last call defines the default. Call `File::get` once for consistency or use `Param::get` which won't reparse the data each time. This check was not added as it increases the overhead significantly if it is called often/big configuration files.
- `set` does not write to the defaults

## Contributing

Contributions to Cracon are welcome! Feel free to open issues, submit pull requests, or provide feedback. Especially if you are a CMake Guru and knows how to ensure this is as easy to integrate as possible.

## License

Cracon is released under the MIT License. See the [LICENSE](LICENSE) file for details.
