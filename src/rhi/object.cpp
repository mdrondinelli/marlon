#include "object.h"

namespace marlon::rhi
{
  auto acquire_object(Object *p) noexcept -> Object *
  {
    ++p->reference_count;
    return p;
  }

  auto release_object(Object *p) noexcept -> void
  {
    if (!--p->reference_count)
    {
      p->deleter(p);
    }
  }
} // namespace marlon::rhi
