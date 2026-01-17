#include <marlon/rhi/rhi.h>

namespace marlon::rhi
{
  Object::Object(Deallocator *deallocator, std::size_t size)
      : _deallocator{deallocator}, _size{size}
  {
  }

  auto Object::acquire() noexcept -> void
  {
    ++_reference_count;
  }

  auto Object::release() noexcept -> void
  {
    if (--_reference_count == 0)
    {
      auto const deallocator = _deallocator;
      this->~Object();
      deallocator->free(this, _size);
    }
  }

  Interface::Interface(Deallocator *deallocator, std::size_t size) noexcept
      : Object{deallocator, size}
  {
  }

  Descriptor_set_layout::Descriptor_set_layout(
    Deallocator *deallocator,
    std::size_t size
  ) noexcept
      : Object{deallocator, size}
  {
  }

  Descriptor_set::Descriptor_set(
    Deallocator *deallocator,
    std::size_t size
  ) noexcept
      : Object{deallocator, size}
  {
  }

  Pipeline_layout::Pipeline_layout(
    Deallocator *deallocator,
    std::size_t size
  ) noexcept
      : Object{deallocator, size}
  {
  }

  Compute_pipeline::Compute_pipeline(
    Deallocator *deallocator,
    std::size_t size
  ) noexcept
      : Object{deallocator, size}
  {
  }

  Graphics_pipeline::Graphics_pipeline(
    Deallocator *deallocator,
    std::size_t size
  ) noexcept
      : Object{deallocator, size}
  {
  }

  Buffer::Buffer(Deallocator *deallocator, std::size_t size) noexcept
      : Object{deallocator, size}
  {
  }

  Image::Image(Deallocator *deallocator, std::size_t size) noexcept
      : Object{deallocator, size}
  {
  }

  Surface::Surface(Deallocator *deallocator, std::size_t size) noexcept
      : Object{deallocator, size}
  {
  }

  Swapchain::Swapchain(Deallocator *deallocator, std::size_t size) noexcept
      : Object{deallocator, size}
  {
  }

  Command_buffer::Command_buffer(
    Deallocator *deallocator,
    std::size_t size
  ) noexcept
      : Object{deallocator, size}
  {
  }
} // namespace marlon::rhi
