#ifndef MARLON_RHI_ENUMS_H
#define MARLON_RHI_ENUMS_H

#include <compare>
#include <cstdint>

#include <marlon/enum_bitset.h>

namespace marlon::rhi
{
  enum class Log_level
  {
    info,
    warning,
  };

  constexpr auto operator<=>(Log_level lhs, Log_level rhs) noexcept
  {
    return static_cast<int>(lhs) <=> static_cast<int>(rhs);
  }

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

  enum class Image_type
  {
    _1d = 0,
    _2d = 1,
    _3d = 2,
  };

  enum class Image_format
  {
    b8g8r8a8_srgb = 50,
  };

  enum class Image_usage_bit
  {
    // TRANSFER_SRC
    transfer_src = 0x00000001,
    // TRANSFER_DST
    transfer_dst = 0x00000002,
    // SAMPLED
    sampled = 0x00000004,
    // STORAGE
    storage = 0x00000008,
    // COLOR_ATTACHMENT
    color_attachment = 0x00000010,
    // DEPTH_STENCIL_ATTACHMENT
    depth_stencil_attachment = 0x00000020,
  };

  using Image_usage_bitset = Enum_bitset<Image_usage_bit>;

  constexpr auto operator&(Image_usage_bit lhs, Image_usage_bit rhs) noexcept
  {
    return Image_usage_bitset{lhs} & rhs;
  }

  constexpr auto operator|(Image_usage_bit lhs, Image_usage_bit rhs) noexcept
  {
    return Image_usage_bitset{lhs} | rhs;
  }

  constexpr auto operator^(Image_usage_bit lhs, Image_usage_bit rhs) noexcept
  {
    return Image_usage_bitset{lhs} ^ rhs;
  }
} // namespace marlon::rhi

#endif
