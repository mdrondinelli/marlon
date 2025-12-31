#ifndef MARLON_RHI_VULKAN_H
#define MARLON_RHI_VULKAN_H

#include <exception>

#include <marlon/rhi/rhi.h>

namespace marlon::rhi::vulkan
{
  struct Interface;
  struct Surface;

  struct Interface_create_info
  {
  };

  struct Surface_create_info
  {
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

  auto get_interface_base(Intarface *interface) noexcpet -> rhi::Interface *;

  auto select_device(Interface *interface, Surface *surface) -> void;

  auto new_surface(Interface *interface, Surface_create_info const &create_info)
    -> Surface *;

  auto get_surface_base(Surface *surface) noexcept -> rhi::Surface *;
} // namespace marlon::rhi::vulkan

#endif
