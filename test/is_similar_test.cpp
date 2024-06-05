#include "nlohmann/json.hpp"
#include <array>

#include <gtest/gtest.h>

#include <flacon/flacon.hpp>
#include <vector>

using namespace flacon;

template <typename T>
void test_all_items(nlohmann::json const &json_data, std::string const &name)
{
  for (auto &data : json_data.items())
  {
    std::cout << "handling " << data.key() << std::endl;
    if (data.key().find(name) != std::string::npos)
    {
      EXPECT_TRUE(is_similar<T>(data.value())) << data.key() << " should be a " << name;
    }
    else
    {
      EXPECT_FALSE(is_similar<T>(data.value())) << data.key() << " should not be a " << name;
    }
  }
}

// The fixture for testing class Foo.
class IsSimilarTest : public ::testing::Test
{
protected:
  // You can remove any or all of the following functions if their bodies would

  IsSimilarTest()
  {
    // Integers
    json_data_["int8_unsigned8_one"] = 1;
    json_data_["int8_unsigned8_zero"] = 0;
    json_data_["int8_neg_one"] = -1;
    json_data_["int8_unsigned8"] = 127;
    json_data_["int8_min"] = -128;
    json_data_["double_neg_large"] = -10.0e42;
    json_data_["double_large"] = 10.0e42;
    json_data_["int_unsigned_large"] = 5000000000;
    json_data_["int_large"] = -5000000000;
    json_data_["int_unsigned8_max"] = 255;

    // strings
    json_data_["string_empty"] = "";
    json_data_["string_number"] = "1";
    json_data_["string_longish"] = "The universe is large, I mean really large, you know? ";

    // float & double
    json_data_["float_double_one"] = 1.0;
    json_data_["float_double_neg_one"] = -1.0;

    // bools
    json_data_["bool_true"] = true;
    json_data_["bool_false"] = false;

    // array
    json_data_["array_str"] = nlohmann::json::array({"hey", "oh"});
    json_data_["array_i"] = nlohmann::json::array({1, 2, 3});

    // Using setters from C++, the values are not identical to the JSON just read equivalent.
    json_data_ = nlohmann::json::parse(json_data_.dump());
  }

  ~IsSimilarTest() override
  {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override
  {
  }

  void TearDown() override
  {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
public:
  nlohmann::json json_data_;
};

TEST_F(IsSimilarTest, test_int64)
{
  test_all_items<int64_t>(json_data_, "int");
}

TEST_F(IsSimilarTest, test_int8)
{
  test_all_items<int8_t>(json_data_, "int8");
}

TEST_F(IsSimilarTest, test_uint64)
{
  test_all_items<uint64_t>(json_data_, "unsigned");
}

TEST_F(IsSimilarTest, test_uint8)
{
  test_all_items<uint8_t>(json_data_, "unsigned8");
}

TEST_F(IsSimilarTest, test_string)
{
  test_all_items<std::string>(json_data_, "string");
}

TEST_F(IsSimilarTest, test_float)
{
  test_all_items<float>(json_data_, "float");
}

TEST_F(IsSimilarTest, test_double)
{
  test_all_items<double>(json_data_, "double");
}

TEST_F(IsSimilarTest, test_boolean)
{
  test_all_items<bool>(json_data_, "bool");
}

TEST_F(IsSimilarTest, test_vector_str)
{
  test_all_items<std::vector<std::string>>(json_data_, "array_str");
}

TEST_F(IsSimilarTest, test_vector_int)
{
  test_all_items<std::vector<int>>(json_data_, "array_i");
}

TEST_F(IsSimilarTest, test_array_str_2)
{
  test_all_items<std::array<std::string, 2>>(json_data_, "array_str");
}

TEST_F(IsSimilarTest, test_array_str_wrong_number)
{
  test_all_items<std::array<std::string, 10>>(json_data_, "matchnone!");
}

TEST_F(IsSimilarTest, test_array_int_3)
{
  test_all_items<std::array<int, 3>>(json_data_, "array_i");
}

TEST_F(IsSimilarTest, test_array_int_wrong_number)
{
  test_all_items<std::array<int, 10>>(json_data_, "matchnone!");
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
