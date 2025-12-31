#ifndef MARLON_ENUM_BITSET_H
#define MARLON_ENUM_BITSET_H

#include <type_traits>

namespace marlon
{
  template <typename T>
    requires std::is_enum_v<T>
  struct Enum_bitset
  {
    using Value_type = std::make_unsigned_t<std::underlying_type_t<T>>;

    constexpr Enum_bitset(T value)
        : _value{static_cast<Value_type>(value)}
    {
    }

    constexpr explicit Enum_bitset(Value_type value)
        : _value{value}
    {
    }

    constexpr auto value() const noexcept -> Value_type
    {
      return _value;
    }

    constexpr operator bool() const noexcept
    {
      return _value;
    }

    friend constexpr auto operator==(Enum_bitset lhs, Enum_bitset rhs) noexcept
      -> bool = default;

    friend constexpr auto operator&(Enum_bitset lhs, Enum_bitset rhs) noexcept
      -> Enum_bitset
    {
      return Enum_bitset{lhs.value() & rhs.value()};
    }

    friend constexpr auto operator|(Enum_bitset lhs, Enum_bitset rhs) noexcept
      -> Enum_bitset
    {
      return Enum_bitset{lhs.value() | rhs.value()};
    }

    friend constexpr auto operator^(Enum_bitset lhs, Enum_bitset rhs) noexcept
      -> Enum_bitset
    {
      return Enum_bitset{lhs.value() ^ rhs.value()};
    }

    constexpr auto operator&=(Enum_bitset rhs) noexcept -> Enum_bitset &
    {
      return *this = (*this & rhs);
    }

    constexpr auto operator|=(Enum_bitset rhs) noexcept -> Enum_bitset &
    {
      return *this = (*this | rhs);
    }

    constexpr auto operator^=(Enum_bitset rhs) noexcept -> Enum_bitset &
    {
      return *this = (*this ^ rhs);
    }

  private:
    Value_type _value;
  };
} // namespace marlon

#endif
