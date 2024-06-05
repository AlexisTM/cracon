#ifndef CRACON_CRACON_HPP
#define CRACON_CRACON_HPP

#include <cassert>
#include <cracon/similarity_traits.hpp>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>

namespace cracon {
class File {
 public:
  /**
   * @brief Sets the configuration filenames and parses them if it exists.
   *
   * Create the files otherwise. Returns false if it failed to be created/read.
   *
   * @param filename_filename The json file to to read or create
   * @param default_filename The default configuration file to create
   * @return true File loaded or created
   * @return false File couldn't be read or created
   */
  bool init(std::string const &filename_filename,
            std::string const &default_filename);
  /**
   * @brief True if we the local representation differs from the files.
   */
  bool should_write() { return should_write_default_ || should_write_config_; }

  /**
   * @brief Set a parameter value
   *
   * If the value is dissimilar, it will crash (assert(false)) in debug mode.
   *
   * @tparam T Type of the parameter
   * @param accessor The json pointer in the form "/some_module/some_parameter"
   * @param new_value The value to write
   * @return The new value to be saved locally
   */
  template <typename T>
  [[nodiscard]] auto set(std::string const &accessor, T const &new_value) -> T {
    std::unique_lock lock(mutex_);

    nlohmann::json::json_pointer pointer(accessor);
    auto &val = config_[pointer];
    should_write_config_ = true;
    val = new_value;
    if (val.is_null()) {
      printf("[cracon] [WARN] The key didn't exist for %s\n", accessor.c_str());
    } else {
      if (!is_similar<T>(val)) {
        printf(
            "[cracon] [WARN] The new key is not a similar type to the "
            "precedent configuration: %s replaced by %s\n",
            accessor.c_str(), val.dump().c_str());
      }
    }
    return new_value;
  }

  /**
   * @brief Get the value of a parameter.
   *
   * Set the default value in the defaults.
   * If the value is null, create it, update the json.
   * If the value is not similar (different type), update it and print an error.
   * Else return the parsed value.
   *
   * @tparam T Type of the parameter
   * @param accessor The location of the data within the JSON
   * @param default_val A default value if the data at the pointer doesn't exist
   * @return T Either the data in the JSON or the default value
   */
  template <typename T>
  [[nodiscard]] auto get(std::string const &accessor, T const &default_val)
      -> T {
    std::unique_lock lock(mutex_);

    nlohmann::json::json_pointer pointer(accessor);
    default_[pointer] = default_val;
    should_write_default_ = true;
    try {
      auto &val = config_.at(pointer);
      if (val.is_null()) {
        printf(
            "[cracon] [INFO] The requested key doesn't exist for %s defaulted "
            "to \n",
            default_[pointer].dump().c_str());
        return default_val;
      }
      // This can happen if: The config file is the wrong type or the code is
      // using the wrong type; It is considered the code is right;
      if (!is_similar<T>(val)) {
        fprintf(stderr,
                "[cracon] [ERROR] The read value %s is not a similar type to "
                "%s at %s defaulted to %s\n",
                val.dump().c_str(), typeid(T).name(), accessor.c_str(),
                default_[pointer].dump().c_str());
        return default_val;
      }
      return val.get<T>();
    } catch (std::exception const &ex) {
      printf(
          "[cracon] [INFO] The requested key doesn't exist for %s defaulted "
          "to %s. Error: %s\n",
          accessor.c_str(), default_[pointer].dump().c_str(), ex.what());
      return default_val;
    }
  }

  bool write();

 private:
  // This doesn't lock the mutex as it is an internal function called by the
  // mutexed function write()
  bool write_to_file(std::string const &filename, nlohmann::json const &config);
  nlohmann::json config_ = nlohmann::json::object();
  nlohmann::json default_ = nlohmann::json::object();
  // If data has been changed and this file shall be updated on the next update
  // time.
  bool should_write_config_ = true;
  bool should_write_default_ = true;
  // Saved for later writing to the file as the file is closed after each usage.
  std::string filename_config_ = "";
  std::string filename_default_ = "";
  // Prevent multiple write/read to the JSON representation & file.
  std::mutex mutex_;
};

/**
 * @brief Wrapper around File to allow Param & Group helper classes.
 *
 * This instanciates File as a shared_ptr which is then shared across other
 * classes.
 *
 */
class SharedFile : public File {
 public:
  /**
   * @brief Sets the configuration filenames and parses them if it exists. See
   * `File::init`
   */
  bool init(std::string const &filename_filename,
            std::string const &default_filename);

  /**
   * Live parameter directly writing to/from the file.
   *
   * Writes to Default on creation. Writes to config on change.
   */
  template <typename Type>
  class Param {
   public:
    /*
     * @brief Create a new Param
     *
     * The aim of this structure is to reduce the risk of typos in param_name
     * between get and set functions.
     */
    Param(std::shared_ptr<File> config, std::string const &param_name,
          Type default_value)
        : config_(config), accessor_(param_name), default_(default_value) {
      data_ = config_->get<Type>(accessor_, default_value);
    };

    /**
     * @brief Allow to store this structure on the stack to avoid double
     * indirection.
     */
    Param() {
      data_ = Type();
      default_ = Type();
    };

    /**
     * @brief Fetches a copy of the current data. This doesn't touch the file.
     *
     * @return Type data
     */
    Type get() {
      assert(config_ != nullptr);
      return data_;
    }

    /**
     * @brief Get the reference to the current data.
     *
     * If you modified the data, call `update()`
     *
     * @return Type data
     */
    Type &get_ref() {
      assert(config_ != nullptr);
      return data_;
    }

    /**
     * @brief Get the pointer to the current data.
     *
     * If you modified the data, call `update()`
     *
     * @return Type data
     */
    Type *get_ptr() {
      assert(config_ != nullptr);
      return &data_;
    }

    /**
     * @brief Changes the data here and in the configuration
     */
    void set(Type data) {
      assert(config_ != nullptr);
      data_ = config_->set<Type>(accessor_, data);
    }

    /**
     * @brief Force updating the configuration as internal data changed from the
     * reference or the pointer.
     */
    void update() {
      assert(config_ != nullptr);
      (void) config_->set<Type>(accessor_, data_);
    }

    /**
     * @brief Sets the default value
     *
     */
    void reset() { set(default_); }

   private:
    Type data_;
    Type default_;
    std::shared_ptr<File> config_ = nullptr;
    std::string accessor_;
  };

  /**
   * Group used to simplify access to parameters
   */
  class Group {
   public:
    Group(std::shared_ptr<File> config, std::string const &config_name)
        : config_(config), namespace_(config_name){};

    [[nodiscard]] Group get_group(std::string const &config_name) {
      return Group(config_, namespace_ + "/" + config_name);
    }
    template <typename T>
    [[nodiscard]] auto set(std::string const &accessor, T const &new_value)
        -> T {
      std::string pointer = "/" + namespace_ + "/" + accessor;
      return config_->set(pointer, new_value);
    }
    template <typename T>
    [[nodiscard]] auto get(std::string const &accessor, T const &default_val)
        -> T {
      std::string pointer = "/" + namespace_ + "/" + accessor;
      return config_->get(pointer, default_val);
    }
    template <typename Type>
    [[nodiscard]] auto get_param(std::string const &param_name,
                                 Type const &default_val) -> Param<Type> {
      std::string pointer = "/" + namespace_ + "/" + param_name;
      return Param<Type>(config_, pointer, default_val);
    }

   private:
    std::shared_ptr<File> config_;
    std::string namespace_;
  };

  /**
   * @brief Allows to group parameters to reduce naming redundancy (thus typos).
   *
   * @tparam T Type of the output
   * @param accessor The location of the data within the JSON
   * @param default_val A default value if the data at the pointer doesn't exist
   * @return T Either the data in the JSON or the default value
   */
  [[nodiscard]] Group get_group(std::string const &config_name) {
    return Group(file_, config_name);
  }

  /**
   * @brief Returns a standalone parameter which doesn't need the name to work.
   *
   * This prevents typos.
   *
   * @tparam Type
   * @param param_name
   * @param default_val
   * @return Param<Type>
   */
  template <typename Type>
  Param<Type> get_param(std::string const &param_name,
                        Type const &default_val) {
    return Param<Type>(file_, param_name, default_val);
  }

  // Same as File::get()
  template <typename T>
  [[nodiscard]] auto get(std::string const &accessor, T const &default_val)
      -> T {
    return file_->get<T>(accessor, default_val);
  }

  // Same as File::set()
  template <typename T>
  [[nodiscard]] auto set(std::string const &accessor, T const &new_value) -> T {
    return file_->set(accessor, new_value);
  }

  // Same as File::should_write()
  bool should_write();
  // Same as File::write()
  bool write();

 private:
  std::shared_ptr<File> file_ = std::make_shared<File>();
};

std::string get_package_share_directory(const std::string &package_name);
}  // namespace cracon

#endif  // CRACON_CRACON_HPP
