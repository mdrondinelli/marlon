#include "descriptor_set.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Descriptor_set *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Descriptor_set *>(void *vp) noexcept -> Descriptor_set *
  {
    return std::launder(static_cast<Descriptor_set *>(vp));
  }
}
