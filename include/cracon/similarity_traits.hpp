#ifndef CRACON_SIMILARITY_TRAIT_HPP
#define CRACON_SIMILARITY_TRAIT_HPP

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <type_traits>

namespace cracon {

template <typename T>
struct is_vector : public std::false_type {};
template <typename T>
struct is_vector<std::vector<T>> : public std::true_type {};

template <typename T>
struct is_array : public std::false_type {};
template <typename T, std::size_t N>
struct is_array<std::array<T, N>> : public std::true_type {};

/**
 * @brief Is similar checks if the JSON value provided is similar to the type T.
 *
 * Similar means:
 *  - Signed values cannot be converted to an unsigned.
 *  - Int values cannot be converted to floats
 *  - Float values cannot be converted to ints
 *  - Strings are std::string
 *  - Vector and Array are similar but the array checks on the size. The vector
 * has to be N+1
 *  - In Vector<T>, T has to be the same type as all the inbuild elements
 *  - The input value has to be representable (0 < uint8 < 255) within the
 * receiving type
 *
 * @tparam T Input type
 * @param value The JSON value to check
 * @return true Similar type (can be safely converted)
 * @return false Dissimilar type (can't be parsed)
 */
template <typename T>
bool is_similar(nlohmann::json const &value) {
  if constexpr (std::is_same_v<T, bool>) {
    return value.is_boolean();
  } else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
    bool result = value.is_number_integer();
    if (result)  // Limit checks
    {
      int64_t parsed_value = value.get<int64_t>();
      if (parsed_value < std::numeric_limits<T>::lowest()) {
        fprintf(stderr, "[cracon] [WARN] Out of bounds (min)\n");

        return false;
      } else if (parsed_value > std::numeric_limits<T>::max()) {
        fprintf(stderr, "[cracon] [WARN] Out of bounds (max)\n");
        return false;
      }
    }
    return result;
  } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
    bool result = value.is_number_integer();
    if (result)  // Limit checks
    {
      int64_t parsed_value = value.get<int64_t>();
      if (parsed_value < 0) {
        fprintf(stderr, "[cracon] [WARN] Out of bounds (min) \n");
        return false;
      } else if (parsed_value > std::numeric_limits<T>::max()) {
        fprintf(stderr, "[cracon] [WARN] Out of bounds (max)\n");
        return false;
      }
    }
    return result;
  } else if constexpr (std::is_floating_point_v<T>) {
    bool result = value.is_number_float();
    if (result)  // Limit checks
    {
      double high_limit = value.get<double>();
      if (high_limit < std::numeric_limits<T>::lowest()) {
        fprintf(stderr, "[cracon] [WARN] Out of bounds (min) \n");
        return false;
      } else if (high_limit > std::numeric_limits<T>::max()) {
        fprintf(stderr, "[cracon] [WARN] Out of bounds (max)\n");
        return false;
      }
    }
    return result;
  } else if constexpr (std::is_same_v<T, std::string>) {
    return value.is_string();
  } else if constexpr (is_array<T>::value) {
    if (value.is_array() && value.size() == std::tuple_size<T>::value) {
      // Check if all elements are correct
      for (auto &element : value) {
        if (!is_similar<typename T::value_type>(element)) {
          return false;
        }
      }
      return true;
    }
    return false;
  } else if constexpr (is_vector<T>::value) {
    if (value.is_array()) {
      // Ensure the types are valid
      for (auto &element : value) {
        if (!is_similar<typename T::value_type>(element)) {
          return false;
        }
      }
      return true;
    }
    return false;
  } else if constexpr (std::is_enum_v<T>) {
    if (value.is_number_integer()) {
      int high_limit = value.get<int>();
      if (high_limit < 0) {
        fprintf(stderr, "[cracon] [WARN] Enums have to be >0\n");
        return false;
      } else if (high_limit > std::pow(2, sizeof(T) * 8 - 1)) {
        // The value has to be representable in the enum
        fprintf(stderr, "[cracon] [WARN] Out of bounds (max)\n");
        return false;
      }
      return true;
    }
    return false;
  }

  fprintf(stderr, "[cracon] [ERROR] type is not supported\n");
  return false;
}
}  // namespace cracon

#endif  // CRACON_SIMILARITY_TRAIT_HPP
