#include <catch2/catch_test_macros.hpp>
#include <marlon/enum_bitset.h>

enum class Test_bit
{
  zero = 1 << 0,
  one = 1 << 1,
  two = 1 << 2,
};

using Test_bitset = marlon::Enum_bitset<Test_bit>;

constexpr auto operator&(Test_bit lhs, Test_bit rhs) -> Test_bitset
{
  return Test_bitset{lhs} & rhs;
}

constexpr auto operator|(Test_bit lhs, Test_bit rhs) -> Test_bitset
{
  return Test_bitset{lhs} | rhs;
}

constexpr auto operator^(Test_bit lhs, Test_bit rhs) -> Test_bitset
{
  return Test_bitset{lhs} ^ rhs;
}

TEST_CASE("Enum_bitset works")
{
  REQUIRE((Test_bit::zero & Test_bit::zero).value() == 1);
  REQUIRE((Test_bit::zero & Test_bit::zero) == Test_bit::zero);
  REQUIRE(!(Test_bit::zero & Test_bit::one));
  REQUIRE(!(Test_bit::zero & Test_bit::two));
  REQUIRE((Test_bit::zero | Test_bit::zero).value() == 1);
  REQUIRE((Test_bit::zero | Test_bit::zero) == Test_bit::zero);
  REQUIRE((Test_bit::zero | Test_bit::one).value() == 3);
  REQUIRE((Test_bit::zero | Test_bit::one) & Test_bit::zero);
  REQUIRE((Test_bit::zero | Test_bit::one) & Test_bit::one);
  REQUIRE((Test_bit::zero | Test_bit::two) & Test_bit::zero);
  REQUIRE((Test_bit::zero | Test_bit::two) & Test_bit::two);
  REQUIRE((Test_bit::zero ^ Test_bit::zero).value() == 0);
  REQUIRE((Test_bit::zero ^ Test_bit::one).value() == 3);
  REQUIRE((Test_bit::zero ^ Test_bit::one ^ Test_bit::zero).value() == 2);
}
