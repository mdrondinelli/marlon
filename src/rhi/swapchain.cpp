#include "swapchain.h"

#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  auto get_base(Swapchain *p) noexcept -> Object *
  {
    return &p->base;
  }

  template <>
  auto laundering_cast<Swapchain *>(void *vp) noexcept -> Swapchain *
  {
    return std::launder(static_cast<Swapchain *>(vp));
  }
}
