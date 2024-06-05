#include <gtest/gtest.h>

#include <flacon/flacon.hpp>
#include <string>
#include <vector>

std::string current_folder = "";

#define ADD_TEST_INT(TYPE, VALUE, DEFAULT_VALUE)                           \
  TEST(FileTest, getter_##TYPE) {                                              \
    flacon::File file;                                                     \
    bool success =                                                         \
        file.init(current_folder + "/static_test_data.json",               \
                  current_folder + "/static_test_data_default.json");      \
    ASSERT_TRUE(success) << "The config file should be R/W";               \
    TYPE val = file.get("/number_0", DEFAULT_VALUE);                       \
    EXPECT_EQ(val, 0);                                                     \
    val = file.get("/number_100", DEFAULT_VALUE);                          \
    EXPECT_EQ(val, 100);                                                   \
    val = file.get("/nonexisting", VALUE);                                 \
    EXPECT_EQ(val, VALUE) << "Fails to find the value, thus shall return " \
                             "default & add value to the json";            \
    EXPECT_TRUE(file.should_write());                                      \
    val = file.get("/nonexisting", DEFAULT_VALUE);                         \
    EXPECT_EQ(val, DEFAULT_VALUE)                                          \
        << "The precedent default value should have been added";           \
    val = file.set("/nonexisting", VALUE);                                 \
    val = file.get("/nonexisting", DEFAULT_VALUE);                         \
    EXPECT_EQ(val, VALUE) << "The precedent default value is overwritten"; \
  }

ADD_TEST_INT(int, 42, 69)
ADD_TEST_INT(int8_t, 42, 69)
ADD_TEST_INT(int16_t, 42, 69)
ADD_TEST_INT(int32_t, 42, 69)
ADD_TEST_INT(int64_t, 42, 69)
ADD_TEST_INT(long, 42, 69)
ADD_TEST_INT(uint8_t, 42, 69)
ADD_TEST_INT(uint16_t, 42, 69)
ADD_TEST_INT(uint32_t, 42, 69)
ADD_TEST_INT(uint64_t, 42, 69)

TEST(FileTest, getter_float) {
  flacon::File file;
  bool success = file.init(current_folder + "/static_test_data.json",
                           current_folder + "/static_test_data_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  float val = file.get<float>("/float_0", 500.0);
  EXPECT_EQ(val, 0.0);
  val = file.get<float>("/float_100", 500.0);
  EXPECT_EQ(val, 100.0);
  val = file.get<float>("/nonexisting", 500.0);
  EXPECT_EQ(val, 500.0) << "Fails to find the value, thus shall return default "
                           "& add value to the json";
  EXPECT_TRUE(file.should_write());
  val = file.get<float>("/nonexisting", 1000.0);
  EXPECT_EQ(val, 1000.0) << "The precedent default value is overwritten";
}

TEST(FileTest, getter_string) {
  flacon::File file;
  bool success = file.init(current_folder + "/static_test_data.json",
                           current_folder + "/static_test_data_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  std::string val = file.get<std::string>("/string_1", "FAIL");
  EXPECT_EQ(val, "");
  val = file.get<std::string>("/string_2", "FAIL");
  EXPECT_EQ(
      val,
      "Pretty long string, so small string optimization doesn't fit in, nice");
  val = file.get<std::string>("/nonexisting", "New value");
  EXPECT_EQ(val, "New value") << "Fails to find the value, thus shall return "
                                 "default & add value to the json";
  EXPECT_TRUE(file.should_write());
  val = file.get<std::string>("/nonexisting", "Shouldn't be set");
  EXPECT_EQ(val, "Shouldn't be set")
      << "The precedent default value should have been added";
}

TEST(FileTest, getter_vectors) {
  flacon::File file;
  bool success = file.init(current_folder + "/static_test_data.json",
                           current_folder + "/static_test_data_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  std::vector<int> val_int =
      file.get<std::vector<int>>("/vector_int", {4, 4, 4});
  EXPECT_EQ(val_int.size(), 3UL);
  for (size_t i = 0; i < val_int.size(); i++) {
    EXPECT_EQ(val_int[i], i + 1);
  }

  std::vector<float> val_float =
      file.get<std::vector<float>>("/vector_float", {4., 4., 4.});
  EXPECT_EQ(val_float.size(), 3UL);
  for (size_t i = 0; i < val_float.size(); i++) {
    EXPECT_EQ(val_float[i], (float)(i + 1));
  }

  std::vector<std::string> default_str{"Oh", "Hi", "Mark"};
  std::vector<std::string> val_str = file.get("/vector_str", default_str);
  EXPECT_EQ(val_str.size(), 3UL);
  for (size_t i = 0; i < val_str.size(); i++) {
    EXPECT_EQ(val_str[i], std::to_string(i + 1));
  }

  val_str = file.get("/vector_invalid", default_str);
  EXPECT_EQ(val_str.size(), 3UL);
  EXPECT_EQ(val_str[0], "Oh");
  EXPECT_EQ(val_str[1], "Hi");
  EXPECT_EQ(val_str[2], "Mark");

  val_str = file.get("/incorrect_key", default_str);
  EXPECT_EQ(val_str.size(), 3UL);
  EXPECT_EQ(val_str[0], "Oh");
  EXPECT_EQ(val_str[1], "Hi");
  EXPECT_EQ(val_str[2], "Mark");
}

TEST(FileTest, getter_array) {
  flacon::File file;
  bool success = file.init(current_folder + "/static_test_data.json",
                           current_folder + "/static_test_data_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  std::array<int, 3> val_int =
      file.get<std::array<int, 3>>("/vector_int", {4, 4, 4});
  EXPECT_EQ(val_int.size(), 3UL);
  for (size_t i = 0; i < val_int.size(); i++) {
    EXPECT_EQ(val_int[i], i + 1);
  }

  std::array<float, 3> val_float =
      file.get<std::array<float, 3>>("/vector_float", {4., 4., 4.});
  EXPECT_EQ(val_float.size(), 3UL);
  for (size_t i = 0; i < val_float.size(); i++) {
    EXPECT_EQ(val_float[i], (float)(i + 1));
  }

  std::array<std::string, 3> default_str{"Oh", "Hi", "Mark"};
  std::array<std::string, 3> val_str = file.get("/vector_str", default_str);
  EXPECT_EQ(val_str.size(), 3UL);
  for (size_t i = 0; i < val_str.size(); i++) {
    EXPECT_EQ(val_str[i], std::to_string(i + 1));
  }

  val_str = file.get("/vector_invalid", default_str);
  EXPECT_EQ(val_str.size(), 3UL);
  EXPECT_EQ(val_str[0], "Oh");
  EXPECT_EQ(val_str[1], "Hi");
  EXPECT_EQ(val_str[2], "Mark");

  val_str = file.get("/incorrect_key", default_str);
  EXPECT_EQ(val_str.size(), 3UL);
  EXPECT_EQ(val_str[0], "Oh");
  EXPECT_EQ(val_str[1], "Hi");
  EXPECT_EQ(val_str[2], "Mark");
}

TEST(FileTest, getter_deep_parameter) {
  flacon::File file;
  bool success = file.init(current_folder + "/static_test_data.json",
                           current_folder + "/static_test_data_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  int val = file.get("/this/is/pretty/deep", 500);
  EXPECT_EQ(val, 42);
  val = file.get("/nonexisting/but/now/it/does/hurray", 500);
  EXPECT_TRUE(file.should_write());
  EXPECT_EQ(val, 500);
  val = file.get("/nonexisting/but/now/it/does/hurray", 1000);
  EXPECT_EQ(val, 1000) << "The precedent default value is overwritten";
}

TEST(FileTest, setter_and_writer) {
  std::string filename = current_folder + "/output_setter_and_writer.json";
  std::remove(filename.c_str());  // Remove the file if it exists
  flacon::File file;
  bool success = file.init(
      filename, current_folder + "/output_setter_and_writer_default.json");

  auto set_value = file.set("/nonexisting/but/now/it/does/hurray", 500);
  EXPECT_TRUE(file.should_write());
  EXPECT_TRUE(file.write());

  success = file.init(
      filename, current_folder + "/output_setter_and_writer_default.json");

  int val = file.get("/nonexisting/but/now/it/does/hurray", 1000);
  EXPECT_EQ(val, set_value) << "The precedent set should have been set.";

  /*
  Content of build/output_setter_and_writer.json:
  {
    "nonexisting": {
        "but": {
            "now": {
                "it": {
                    "does": {
                        "hurray": 500
                    }
                }
            }
        }
    }
  }
*/
}

TEST(FileTest, getters_changes_defaults) {
  std::string filename = current_folder + "/output_setter_and_writer.json";
  std::remove(filename.c_str());  // Remove the file if it exists
  flacon::File file;
  bool success = file.init(
      filename, current_folder + "/getters_changes_defaults_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  auto get_value = file.get("/nonexisting/but/now/it/does/hurray", 500);
  EXPECT_TRUE(file.should_write());
  EXPECT_TRUE(file.write());

  // We check the default file has the correct data
  success = file.init(current_folder + "/getters_changes_defaults_default.json",
                      current_folder + "other_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  int val = file.get("/nonexisting/but/now/it/does/hurray", 1000);
  EXPECT_EQ(val, get_value) << "The precedent set should have been set.";

  /*
  Content of build/getters_changes_defaults_default.json:
  {
    "nonexisting": {
        "but": {
            "now": {
                "it": {
                    "does": {
                        "hurray": 500
                    }
                }
            }
        }
    }
  }
*/
}

int main(int argc, char **argv) {
  std::string current_file(argv[0]);
  size_t pos = current_file.rfind('/');
  if (pos == std::string::npos) {
    pos = current_file.rfind('\\');  // Because Windows
  }
  if (pos != std::string::npos) {
    current_folder = current_file.substr(0, pos + 1);
  }

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
