#include "interface.h"

namespace marlon::rhi
{
  auto new_descriptor_set_layout(
    Interface *interface,
    Descriptor_set_layout_create_info const &create_info
  ) -> Descriptor_set_layout *
  {
    return interface->new_descriptor_set_layout(
      std::move(interface),
      create_info
    );
  }

  auto new_descriptor_set(
    Interface *interface,
    Descriptor_set_create_info const &create_info
  ) -> Descriptor_set *
  {
    return interface->new_descriptor_set(std::move(interface), create_info);
  }

  auto new_pipeline_layout(
    Interface *interface,
    Pipeline_layout_create_info const &create_info
  ) -> Pipeline_layout *
  {
    return interface->new_pipeline_layout(std::move(interface), create_info);
  }

  auto new_compute_pipeline(
    Interface *interface,
    Compute_pipeline_create_info const &create_info
  ) -> Compute_pipeline *
  {
    return interface->new_compute_pipeline(std::move(interface), create_info);
  }

  auto new_graphics_pipeline(
    Interface *interface,
    Graphics_pipeline_create_info const &create_info
  ) -> Graphics_pipeline *
  {
    return interface->new_graphics_pipeline(std::move(interface), create_info);
  }

  auto new_buffer(Interface *interface, Buffer_create_info const &create_info)
    -> Buffer *
  {
    return interface->new_buffer(std::move(interface), create_info);
  }

  auto new_image(Interface *interface, Image_create_info const &create_info)
    -> Image *
  {
    return interface->new_image(std::move(interface), create_info);
  }

  auto new_swapchain(
    Interface *interface,
    Swapchain_create_info const &create_info
  ) -> Swapchain *
  {
    return interface->new_swapchain(std::move(interface), create_info);
  }

  auto new_command_buffer(
    Interface *interface,
    Command_buffer_create_info const &create_info
  ) -> Command_buffer *
  {
    return interface->new_command_buffer(std::move(interface), create_info);
  }
} // namespace marlon::rhi
