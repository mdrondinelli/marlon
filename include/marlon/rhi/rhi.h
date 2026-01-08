#ifndef MARLON_RHI_RHI_H
#define MARLON_RHI_RHI_H

#include <cstdint>
#include <span>

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

  auto acquire_object(Object *p) noexcept -> Object *;

  auto release_object(Object *p) noexcept -> void;

  constexpr auto get_object(Object *p) noexcept -> Object *
  {
    return p;
  }

  auto get_object(Interface *p) noexcept -> Object *;

  auto get_object(Descriptor_set_layout *p) noexcept -> Object *;

  auto get_object(Descriptor_set *p) noexcept -> Object *;

  auto get_object(Pipeline_layout *p) noexcept -> Object *;

  auto get_object(Compute_pipeline *p) noexcept -> Object *;

  auto get_object(Graphics_pipeline *p) noexcept -> Object *;

  auto get_object(Buffer *p) noexcept -> Object *;

  auto get_object(Image *p) noexcept -> Object *;

  auto get_object(Surface *p) noexcept -> Object *;

  auto get_object(Swapchain *p) noexcept -> Object *;

  auto get_object(Command_buffer *p) noexcept -> Object *;

  auto new_descriptor_set_layout(
    Interface *interface,
    Descriptor_set_layout_create_info const &create_info
  ) -> Descriptor_set_layout *;

  auto new_descriptor_set(
    Interface *interface,
    Descriptor_set_create_info const &create_info
  ) -> Descriptor_set *;

  auto new_pipeline_layout(
    Interface *interface,
    Pipeline_layout_create_info const &create_info
  ) -> Pipeline_layout *;

  auto new_compute_pipeline(
    Interface *interface,
    Compute_pipeline_create_info const &create_info
  ) -> Compute_pipeline *;

  auto new_graphics_pipeline(
    Interface *interface,
    Graphics_pipeline_create_info const &create_info
  ) -> Graphics_pipeline *;

  auto new_buffer(Interface *interface, Buffer_create_info const &create_info)
    -> Buffer *;

  auto new_image(Interface *interface, Image_create_info const &create_info)
    -> Image *;

  auto new_swapchain(
    Interface *interface,
    Swapchain_create_info const &create_info
  ) -> Swapchain *;

  auto new_command_buffer(
    Interface *interface,
    Command_buffer_create_info const &create_info
  ) -> Command_buffer *;

  auto cmd_bind_compute_pipeline(
    Command_buffer *cmd,
    Compute_pipeline *pipeline
  ) -> void;

  auto cmd_bind_graphics_pipeline(
    Command_buffer *cmd,
    Graphics_pipeline *pipeline
  ) -> void;

  auto cmd_bind_descriptor_set(
    Command_buffer *cmd,
    std::uint32_t index,
    Descriptor_set *descriptor_set
  ) -> void;

  auto cmd_dispatch(
    Command_buffer *cmd,
    Buffer *argument_buffer,
    std::uint64_t argument_offset
  ) -> void;

  auto cmd_draw(
    Command_buffer *cmd,
    Buffer *argument_buffer,
    std::uint64_t argument_offset,
    std::uint32_t argument_stride,
    Buffer *count_buffer,
    std::uint64_t count_offset,
    std::uint32_t max_count
  ) -> void;

  auto cmd_barrier(
    Command_buffer *cmd,
    Barrier_scope const &src,
    Barrier_scope const &dst
  ) -> void;
} // namespace marlon::rhi

#endif
