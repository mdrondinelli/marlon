#ifndef MARLON_RHI_VULKAN_H
#define MARLON_RHI_VULKAN_H

#include <exception>
#include <span>

#include <marlon/enum_bitset.h>
#include <marlon/rhi/rhi.h>

namespace marlon::rhi::vulkan
{
  struct Interface;
  struct Surface;

  enum class Interface_create_bit
  {
    debug = 1 << 0,
    wsi = 1 << 1,
  };

  using Interface_create_bitset = Enum_bitset<Interface_create_bit>;

  constexpr auto
  operator&(Interface_create_bit lhs, Interface_create_bit rhs) noexcept
  {
    return Interface_create_bitset{lhs} & rhs;
  }

  constexpr auto
  operator|(Interface_create_bit lhs, Interface_create_bit rhs) noexcept
  {
    return Interface_create_bitset{lhs} | rhs;
  }

  constexpr auto
  operator^(Interface_create_bit lhs, Interface_create_bit rhs) noexcept
  {
    return Interface_create_bitset{lhs} ^ rhs;
  }

  struct Interface_create_info
  {
    Interface_create_bitset bits;
    std::span<char const * const> wsi_extensions;
  };

  struct Surface_create_info
  {
    std::uint64_t vk_handle;
  };

  struct Error: std::exception
  {
    constexpr explicit Error(int code) noexcept
        : code{code}
    {
    }

    int code;
  };

  auto new_interface(Interface_create_info const &create_info) -> Interface *;

  auto get_interface_base(Interface *interface) noexcept -> rhi::Interface *;

  auto select_device(Interface *interface, Surface *surface) -> void;

  auto new_surface(Interface *interface, Surface_create_info const &create_info)
    -> Surface *;

  auto get_surface_base(Surface *surface) noexcept -> rhi::Surface *;
} // namespace marlon::rhi::vulkan

#endif
