#ifndef MARLON_RHI_RHI_H
#define MARLON_RHI_RHI_H

#include <cstdint>

#include <atomic>
#include <span>

#include <marlon/enum_bitset.h>

#include "enums.h"
#include "ptr.h"

namespace marlon::rhi
{
  struct Deallocator
  {
    virtual ~Deallocator() = default;

    virtual void free(Object *object, std::size_t size) noexcept = 0;
  };

  struct Object
  {
    explicit Object(Deallocator *deallocator, std::size_t size);

    virtual ~Object() = default;

    auto acquire() noexcept -> void;

    auto release() noexcept -> void;

  private:
    Deallocator *_deallocator;
    std::size_t _size;
    std::atomic<std::uint32_t> _reference_count{1};
  };

  struct Descriptor_set_layout: Object
  {
    explicit Descriptor_set_layout(
      Deallocator *deallocator,
      std::size_t size
    ) noexcept;
  };

  struct Descriptor_set: Object
  {
    explicit Descriptor_set(
      Deallocator *deallocator,
      std::size_t size
    ) noexcept;
  };

  struct Pipeline_layout: Object
  {
    explicit Pipeline_layout(
      Deallocator *deallocator,
      std::size_t size
    ) noexcept;
  };

  struct Compute_pipeline: Object
  {
    explicit Compute_pipeline(
      Deallocator *deallocator,
      std::size_t size
    ) noexcept;
  };

  struct Graphics_pipeline: Object
  {
    explicit Graphics_pipeline(
      Deallocator *deallocator,
      std::size_t size
    ) noexcept;
  };

  struct Buffer: Object
  {
    explicit Buffer(Deallocator *deallocator, std::size_t size) noexcept;
  };

  struct Image: Object
  {
    explicit Image(Deallocator *deallocator, std::size_t size) noexcept;
  };

  struct Surface: Object
  {
    explicit Surface(Deallocator *deallocator, std::size_t size) noexcept;
  };

  struct Swapchain: Object
  {
    explicit Swapchain(Deallocator *deallocator, std::size_t size) noexcept;
  };

  struct Barrier_scope;

  struct Command_buffer: Object
  {
    explicit Command_buffer(
      Deallocator *deallocator,
      std::size_t size
    ) noexcept;

    virtual auto
    cmd_bind_compute_pipeline(Command_buffer *cmd, Compute_pipeline *pipeline)
      -> void = 0;

    virtual auto
    cmd_bind_graphics_pipeline(Command_buffer *cmd, Graphics_pipeline *pipeline)
      -> void = 0;

    virtual auto cmd_bind_descriptor_set(
      Command_buffer *cmd,
      std::uint32_t index,
      Descriptor_set *descriptor_set
    ) -> void = 0;

    virtual auto cmd_dispatch(
      Command_buffer *cmd,
      Buffer *argument_buffer,
      std::uint64_t argument_offset
    ) -> void = 0;

    virtual auto cmd_draw(
      Command_buffer *cmd,
      Buffer *argument_buffer,
      std::uint64_t argument_offset,
      std::uint32_t argument_stride,
      Buffer *count_buffer,
      std::uint64_t count_offset,
      std::uint32_t max_count
    ) -> void = 0;

    virtual auto cmd_barrier(
      Command_buffer *cmd,
      Barrier_scope const &src,
      Barrier_scope const &dst
    ) -> void = 0;
  };

  struct Descriptor_set_layout_create_info;
  struct Descriptor_set_create_info;
  struct Pipeline_layout_create_info;
  struct Compute_pipeline_create_info;
  struct Graphics_pipeline_create_info;
  struct Buffer_create_info;
  struct Image_create_info;
  struct Swapchain_create_info;
  struct Command_buffer_create_info;

  struct Interface: Object
  {
    explicit Interface(Deallocator *deallocator, std::size_t size) noexcept;

    virtual auto new_descriptor_set_layout(
      Descriptor_set_layout_create_info const &create_info
    ) -> Ptr<Descriptor_set_layout> = 0;

    virtual auto
    new_descriptor_set(Descriptor_set_create_info const &create_info)
      -> Ptr<Descriptor_set> = 0;

    virtual auto
    new_pipeline_layout(Pipeline_layout_create_info const &create_info)
      -> Ptr<Pipeline_layout> = 0;

    virtual auto
    new_compute_pipeline(Compute_pipeline_create_info const &create_info)
      -> Ptr<Compute_pipeline> = 0;

    virtual auto
    new_graphics_pipeline(Graphics_pipeline_create_info const &create_info)
      -> Ptr<Graphics_pipeline> = 0;

    virtual auto new_buffer(Buffer_create_info const &create_info)
      -> Ptr<Buffer> = 0;

    virtual auto new_image(Image_create_info const &create_info)
      -> Ptr<Image> = 0;

    virtual auto new_swapchain(Swapchain_create_info const &create_info)
      -> Ptr<Swapchain> = 0;

    virtual auto
    new_command_buffer(Command_buffer_create_info const &create_info)
      -> Ptr<Command_buffer> = 0;
  };

  struct Descriptor_set_layout_binding
  {
    /// The index of the binding within the descriptor set.
    std::uint32_t index;

    /// The type of descriptor attached to the binding.
    Descriptor_type descriptor_type;

    /// The number of descriptors attached to the binding.
    std::uint32_t descriptor_count;

    /// The shader stages which can access the binding.
    Shader_stage_bitset stage_bits;
  };

  struct Descriptor_set_layout_create_info
  {
    /// The bindings of the descriptor set.
    std::span<Descriptor_set_layout_binding const> bindings;
  };

  struct Descriptor_set_create_info
  {
    // The layout of the descriptor set.
    Ptr<Descriptor_set_layout> layout;
  };

  struct Pipeline_layout_create_info
  {
    // The descriptor set layouts of the pipeline layout.
    std::span<Ptr<Descriptor_set_layout> const> descriptor_set_layouts;
  };

  struct Compute_pipeline_create_info
  {
    Ptr<Pipeline_layout> layout;
  };

  struct Graphics_pipeline_create_info
  {
    Ptr<Pipeline_layout> layout;
  };

  struct Buffer_create_info
  {
  };

  struct Extent3
  {
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t depth;
  };

  struct Image_create_info
  {
    Image_type type;
    Image_format format;
    Extent3 extent;
    std::uint32_t mip_level_count;
    std::uint32_t array_layer_count;
    Image_usage_bitset usage_bits;  
  };

  struct Swapchain_create_info
  {
    Ptr<Surface> surface;
  };

  struct Command_buffer_create_info
  {
  };

  struct Barrier_scope
  {
    Pipeline_stage_bitset stage_bits;
    Barrier_access_bitset access_bits;
  };
} // namespace marlon::rhi

#endif
