#include "descriptor_set_layout.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Descriptor_set_layout *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Descriptor_set_layout *>(void *vp) noexcept
    -> Descriptor_set_layout *
  {
    return std::launder(static_cast<Descriptor_set_layout *>(vp));
  }
}
