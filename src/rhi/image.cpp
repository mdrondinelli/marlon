#include "image.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Image *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Image *>(void *vp) noexcept -> Image *
  {
    return std::launder(static_cast<Image *>(vp));
  }
}
