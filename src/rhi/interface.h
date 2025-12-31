#ifndef MARLON_RHI_INTERFACE_H
#define MARLON_RHI_INTERFACE_H

#include <marlon/rhi/rhi.h>

#include "object.h"

namespace marlon::rhi
{
  struct Interface
  {
    Object base;

    Descriptor_set_layout *(*new_descriptor_set_layout)(
      Interface *,
      Descriptor_set_layout_create_info const &
    );

    Descriptor_set
      *(*new_descriptor_set)(Interface *, Descriptor_set_create_info const &);

    Pipeline_layout
      *(*new_pipeline_layout)(Interface *, Pipeline_layout_create_info const &);

    Compute_pipeline *(*new_compute_pipeline)(
      Interface *,
      Compute_pipeline_create_info const &
    );

    Graphics_pipeline *(*new_graphics_pipeline)(
      Interface *,
      Graphics_pipeline_create_info const &
    );

    Buffer *(*new_buffer)(Interface *, Buffer_create_info const &);

    Image *(*new_image)(Interface *, Image_create_info const &);

    Swapchain *(*new_swapchain)(Interface *, Swapchain_create_info const &);

    Command_buffer
      *(*new_command_buffer)(Interface *, Command_buffer_create_info const &);
  };
} // namespace marlon::rhi

#endif
