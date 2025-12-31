#include <marlon/rhi/ptr.h>

#include "buffer.h"
#include "command_buffer.h"
#include "compute_pipeline.h"
#include "descriptor_set.h"
#include "descriptor_set_layout.h"
#include "graphics_pipeline.h"
#include "image.h"
#include "interface.h"
#include "pipeline_layout.h"
#include "surface.h"
#include "swapchain.h"

namespace marlon::rhi::detail
{
  auto get_object(Interface *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Descriptor_set_layout *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Descriptor_set *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Pipeline_layout *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Compute_pipeline *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Graphics_pipeline *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Buffer *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Image *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Surface *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Swapchain *p) noexcept -> Object *
  {
    return &p->base;
  }

  auto get_object(Command_buffer *p) noexcept -> Object *
  {
    return &p->base;
  }
} // namespace marlon::rhi::detail
