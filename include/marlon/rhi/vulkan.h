#ifndef MARLON_RHI_VULKAN_H
#define MARLON_RHI_VULKAN_H

#include <exception>
#include <span>

#include <marlon/enum_bitset.h>
#include <marlon/rhi/rhi.h>

namespace marlon::rhi::vulkan
{
  using Dispatchable_handle = void *;
  using Nondispatchable_handle = std::uint64_t;

  struct Interface;

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
    // The underlying VkSurfaceKHR.
    // `Interface::create_surface` takes ownership of this handle.
    Nondispatchable_handle vk_handle;
  };

  struct Error: std::exception
  {
    constexpr explicit Error(int code) noexcept
        : code{code}
    {
    }

    int code;
  };

  struct Interface: rhi::Interface
  {
    explicit Interface(
      rhi::Deallocator *deallocator,
      std::size_t size
    ) noexcept;

    virtual auto get_vk_instance() const noexcept -> Dispatchable_handle = 0;

    virtual auto new_surface(Surface_create_info const &create_info)
      -> rhi::Ptr<rhi::Surface> = 0;

    virtual auto select_device(rhi::Ptr<rhi::Surface> surface) -> void = 0;
  };

  auto new_interface(Interface_create_info const &create_info)
    -> Ptr<Interface>;
} // namespace marlon::rhi::vulkan

#endif
