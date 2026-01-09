#include "pipeline_layout.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Pipeline_layout *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Pipeline_layout *>(void *vp) noexcept
    -> Pipeline_layout *
  {
    return std::launder(static_cast<Pipeline_layout *>(vp));
  }
}
