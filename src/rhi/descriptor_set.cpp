#include "descriptor_set.h"

namespace marlon::rhi
{
  auto get_object(Descriptor_set *p) noexcept -> Object *
  {
    return &p->base;
  }
}
