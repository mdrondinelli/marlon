#include "graphics_pipeline.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Graphics_pipeline *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Graphics_pipeline *>(void *vp) noexcept
    -> Graphics_pipeline *
  {
    return std::launder(static_cast<Graphics_pipeline *>(vp));
  }
}
