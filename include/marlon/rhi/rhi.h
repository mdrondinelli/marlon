#ifndef MARLON_RHI_RHI_H
#define MARLON_RHI_RHI_H

#include <cstdint>

#include <atomic>
#include <span>

#include <marlon/concepts.h>
#include <marlon/enum_bitset.h>

namespace marlon::rhi
{
  struct Object;
  struct Interface;
  struct Descriptor_set_layout;
  struct Descriptor_set;
  struct Pipeline_layout;
  struct Compute_pipeline;
  struct Graphics_pipeline;
  struct Buffer;
  struct Image;
  struct Surface;
  struct Swapchain;
  struct Command_buffer;

  enum class Shader_stage_bit
  {
    // VERTEX_BIT
    vertex = 0x00000001,
    // FRAGMENT_BIT
    fragment = 0x00000010,
    // COMPUTE_BIT
    compute = 0x00000020,
  };

  using Shader_stage_bitset = Enum_bitset<Shader_stage_bit>;

  constexpr auto operator&(Shader_stage_bit lhs, Shader_stage_bit rhs) noexcept
  {
    return Shader_stage_bitset{lhs} & rhs;
  }

  constexpr auto operator|(Shader_stage_bit lhs, Shader_stage_bit rhs) noexcept
  {
    return Shader_stage_bitset{lhs} | rhs;
  }

  constexpr auto operator^(Shader_stage_bit lhs, Shader_stage_bit rhs) noexcept
  {
    return Shader_stage_bitset{lhs} ^ rhs;
  }

  enum class Pipeline_stage_bit : std::uint64_t
  {
    // NONE
    none = 0ull,
    // DRAW_INDIRECT_BIT
    argument_input = 0x00000002ull,
    // VERTEX_SHADER_BIT
    vertex_shader = 0x00000008ull,
    // FRAGMENT_SHADER_BIT
    fragment_shader = 0x00000080ull,
    // EARLY_FRAGMENT_TESTS_BIT
    early_fragment_tests = 0x00000100ull,
    // LATE_FRAGMENT_TESTS_BIT
    late_fragment_tests = 0x00000200ull,
    // COLOR_ATTACHMENT_OUTPUT_BIT
    color_attachment_output = 0x00000400ull,
    // COMPUTE_SHADER_BIT
    compute_shader = 0x00000800ull,
    // COPY_BIT
    copy = 0x100000000ull,
    // RESOLVE_BIT
    resolve = 0x200000000ull,
    // BLIT_BIT
    blit = 0x400000000ull,
    // INDEX_INPUT_BIT
    index_input = 0x1000000000ull,
  };

  using Pipeline_stage_bitset = Enum_bitset<Pipeline_stage_bit>;

  constexpr auto
  operator&(Pipeline_stage_bit lhs, Pipeline_stage_bit rhs) noexcept
  {
    return Pipeline_stage_bitset{lhs} & rhs;
  }

  constexpr auto
  operator|(Pipeline_stage_bit lhs, Pipeline_stage_bit rhs) noexcept
  {
    return Pipeline_stage_bitset{lhs} | rhs;
  }

  constexpr auto
  operator^(Pipeline_stage_bit lhs, Pipeline_stage_bit rhs) noexcept
  {
    return Pipeline_stage_bitset{lhs} ^ rhs;
  }

  enum class Barrier_access_bit : std::uint64_t
  {
    // NONE
    none = 0ull,
    // INDIRECT_COMMAND_READ_BIT
    argument_read = 0x00000001ull,
    // INDEX_READ_BIT
    index_read = 0x00000002ull,
    // UNIFORM_READ_BIT
    uniform_read = 0x00000008ull,
    // COLOR_ATTACHMENT_READ
    color_attachment_read = 0x00000080ull,
    // COLOR_ATTACHMENT_WRITE
    color_attachment_write = 0x00000100ull,
    // DEPTH_STENCIL_ATTACHMENT_READ
    depth_stencil_attachment_read = 0x00000080ull,
    // DEPTH_STENCIL_ATTACHMENT_WRITE
    depth_stencil_attachment_write = 0x00000100ull,
    // TRANSFER_READ
    transfer_read = 0x00000800ull,
    // TRANSFER_WRITE
    transfer_write = 0x00001000ull,
    // SHADER_SAMPLED_READ
    shader_sampled_read = 0x100000000ull,
    // SHADER_STORAGE_READ
    shader_storage_read = 0x200000000ull,
    // SHADER_STORAGE_WRITE
    shader_storage_write = 0x400000000ull,
  };

  using Barrier_access_bitset = Enum_bitset<Barrier_access_bit>;

  constexpr auto
  operator&(Barrier_access_bit lhs, Barrier_access_bit rhs) noexcept
  {
    return Barrier_access_bitset{lhs} & rhs;
  }

  constexpr auto
  operator|(Barrier_access_bit lhs, Barrier_access_bit rhs) noexcept
  {
    return Barrier_access_bitset{lhs} | rhs;
  }

  constexpr auto
  operator^(Barrier_access_bit lhs, Barrier_access_bit rhs) noexcept
  {
    return Barrier_access_bitset{lhs} ^ rhs;
  }

  enum class Descriptor_type
  {
    // SAMPLER
    sampler = 0,
    // COMBINED_IMAGE_SAMPLER
    combined_image_sampler = 1,
    // SAMPLED_IMAGE
    sampled_image = 2,
    // STORAGE_IMAGE
    storage_image = 3,
    // UNIFORM_BUFFER
    uniform_buffer = 6,
    // STORAGE_BUFFER
    storage_buffer = 7,
  };

  struct Descriptor_set_layout_binding
  {
    std::uint32_t index;
    Descriptor_type descriptor_type;
    std::uint32_t descriptor_count;
    Shader_stage_bitset stage_bits;
  };

  struct Descriptor_set_layout_create_info
  {
    std::span<Descriptor_set_layout_binding const> bindings;
  };

  struct Descriptor_set_create_info
  {
    Descriptor_set_layout *layout;
  };

  struct Pipeline_layout_create_info
  {
    std::span<Descriptor_set_layout * const> descriptor_set_layouts;
  };

  struct Compute_pipeline_create_info
  {
    Pipeline_layout *layout;
  };

  struct Graphics_pipeline_create_info
  {
    Pipeline_layout *layout;
  };

  struct Buffer_create_info
  {
  };

  struct Image_create_info
  {
  };

  struct Swapchain_create_info
  {
    Surface *surface;
  };

  struct Command_buffer_create_info
  {
  };

  struct Barrier_scope
  {
    Pipeline_stage_bitset stage_bits;
    Barrier_access_bitset access_bits;
  };

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

  struct Interface: Object
  {
    explicit Interface(Deallocator *deallocator, std::size_t size) noexcept;

    virtual auto new_descriptor_set_layout(
      Descriptor_set_layout_create_info const &create_info
    ) -> Descriptor_set_layout * = 0;

    virtual auto
    new_descriptor_set(Descriptor_set_create_info const &create_info)
      -> Descriptor_set * = 0;

    virtual auto
    new_pipeline_layout(Pipeline_layout_create_info const &create_info)
      -> Pipeline_layout * = 0;

    virtual auto
    new_compute_pipeline(Compute_pipeline_create_info const &create_info)
      -> Compute_pipeline * = 0;

    virtual auto
    new_graphics_pipeline(Graphics_pipeline_create_info const &create_info)
      -> Graphics_pipeline * = 0;

    virtual auto new_buffer(Buffer_create_info const &create_info)
      -> Buffer * = 0;

    virtual auto new_image(Image_create_info const &create_info) -> Image * = 0;

    virtual auto new_swapchain(Swapchain_create_info const &create_info)
      -> Swapchain * = 0;

    virtual auto
    new_command_buffer(Command_buffer_create_info const &create_info)
      -> Command_buffer * = 0;
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
} // namespace marlon::rhi

#endif
