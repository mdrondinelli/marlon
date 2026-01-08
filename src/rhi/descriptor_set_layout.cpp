#include "descriptor_set_layout.h"

namespace marlon::rhi
{
  auto get_object(Descriptor_set_layout *p) noexcept -> Object *
  {
    return &p->base;
  }
}
