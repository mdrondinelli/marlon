#include "buffer.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Buffer *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Buffer *>(void *vp) noexcept -> Buffer *
  {
    return std::launder(static_cast<Buffer *>(vp));
  }
}
