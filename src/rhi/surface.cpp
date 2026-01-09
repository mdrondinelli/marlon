#include "surface.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Surface *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Surface *>(void *vp) noexcept -> Surface *
  {
    return std::launder(static_cast<Surface *>(vp));
  }
}
