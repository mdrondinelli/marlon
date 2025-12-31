#ifndef MARLON_RHI_OBJECT_H
#define MARLON_RHI_OBJECT_H

#include <atomic>
#include <cstdint>

namespace marlon::rhi
{
  struct Object
  {
    Object *parent;
    void (*deleter)(Object *);
    std::atomic<std::uint64_t> reference_count;
  };
} // namespace marlon::rhi

#endif
