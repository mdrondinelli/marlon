#include "compute_pipeline.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Compute_pipeline *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Compute_pipeline *>(void *vp) noexcept
    -> Compute_pipeline *
  {
    return std::launder(static_cast<Compute_pipeline *>(vp));
  }
}
