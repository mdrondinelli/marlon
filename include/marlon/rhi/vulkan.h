#ifndef MARLON_RHI_VULKAN_H
#define MARLON_RHI_VULKAN_H

#include <exception>
#include <span>

#include <volk.h>

#include <marlon/enum_bitset.h>
#include <marlon/rhi/rhi.h>

namespace marlon::rhi::vulkan
{
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
    VkSurfaceKHR vk_handle;
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

    virtual auto new_surface(Surface_create_info const &create_info)
      -> rhi::Surface * = 0;

    virtual auto select_device(rhi::Surface *surface) -> void = 0;

    auto new_descriptor_set_layout(
      Descriptor_set_layout_create_info const &create_info
    ) -> rhi::Descriptor_set_layout * override;

    auto new_descriptor_set(Descriptor_set_create_info const &create_info)
      -> rhi::Descriptor_set * override;

    auto new_pipeline_layout(Pipeline_layout_create_info const &create_info)
      -> rhi::Pipeline_layout * override;

    auto new_compute_pipeline(Compute_pipeline_create_info const &create_info)
      -> rhi::Compute_pipeline * override;

    auto new_graphics_pipeline(Graphics_pipeline_create_info const &create_info)
      -> rhi::Graphics_pipeline * override;

    auto new_buffer(Buffer_create_info const &create_info)
      -> rhi::Buffer * override;

    auto new_image(Image_create_info const &create_info)
      -> rhi::Image * override;

    auto new_swapchain(Swapchain_create_info const &create_info)
      -> rhi::Swapchain * override;

    auto new_command_buffer(Command_buffer_create_info const &create_info)
      -> rhi::Command_buffer * override;
  };

  auto new_interface(Interface_create_info const &create_info) -> Interface *;
} // namespace marlon::rhi::vulkan

#endif
