#include <gtest/gtest.h>

#include <cracon/cracon.hpp>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest-death-test.h"

std::string current_folder = "";

TEST(GroupTest, setter_and_writer) {
  std::string filename = current_folder + "/group_output_test.json";
  std::remove(filename.c_str());  // Remove the file if it exists
  cracon::SharedFile file;
  bool success =
      file.init(filename, current_folder + "/group_output_test_default.json");
  ASSERT_TRUE(success) << "The config file should be R/W";
  auto group1 = file.get_group("nonexisting");
  auto group2 = group1.get_group("but");
  auto group3 = group2.get_group("now");
  auto group4 = group3.get_group("it/does");
  auto set_value = group4.set("hurray", 500);
  EXPECT_TRUE(file.should_write());
  EXPECT_TRUE(file.write());

  success =
      file.init(filename, current_folder + "/group_output_test_default.json");
  ASSERT_TRUE(success) << "The file should have been created";
  int val = file.get("/nonexisting/but/now/it/does/hurray", 1000);
  EXPECT_EQ(val, set_value) << "The precedent set should have been set.";

  /*
  Content of build/cracon/group_output_test.json:
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

TEST(GroupTest, parameters) {
  std::string filename = current_folder + "/group_param_test.json";
  std::remove(filename.c_str());  // Remove the file if it exists
  cracon::SharedFile file;
  bool success =
      file.init(filename, current_folder + "/group_output_test_default.json");
  auto param = file.get_param("/nonexisting/but/now/it/does/hurray", 1000);
  EXPECT_EQ(param.get(), 1000);
  int val = file.get("/nonexisting/but/now/it/does/hurray", 1000);
  EXPECT_EQ(val, 1000);
  param.set(2000);
  EXPECT_EQ(param.get(), 2000);
  val = file.get("/nonexisting/but/now/it/does/hurray", 2000);
  EXPECT_EQ(val, 2000);
}


TEST(GroupTest, param_change_from_ref) {
  std::string filename = current_folder + "/group_param_test.json";
  std::remove(filename.c_str());  // Remove the file if it exists
  cracon::SharedFile file;
  bool success =
      file.init(filename, current_folder + "/group_output_test_default.json");
  auto large_param = file.get_param<std::array<int32_t, 25>>("/large_element", {});
  auto& ref_to_data = large_param.get_ref();
  for(int i = 0; i < ref_to_data.size(); i++) {
    ref_to_data[i] = i;
  }
  large_param.update();

  auto val = file.get<std::array<int32_t, 25>>("/large_element", {});
  for(int i = 0; i < val.size(); i++) {
    EXPECT_EQ(val[i], i);
  }
  file.write();
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
