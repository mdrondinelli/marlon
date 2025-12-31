#include "command_buffer.h"

namespace marlon::rhi
{
  auto cmd_bind_compute_pipeline(
    Command_buffer *cmd,
    Compute_pipeline *pipeline
  ) -> void
  {
    cmd->cmd_bind_compute_pipeline(std::move(cmd), std::move(pipeline));
  }

  auto cmd_bind_graphics_pipeline(
    Command_buffer *cmd,
    Graphics_pipeline *pipeline
  ) -> void
  {
    cmd->cmd_bind_graphics_pipeline(std::move(cmd), std::move(pipeline));
  }

  auto cmd_bind_descriptor_set(
    Command_buffer *cmd,
    std::uint32_t index,
    Descriptor_set *descriptor_set
  ) -> void
  {
    cmd->cmd_bind_descriptor_set(
      std::move(cmd),
      index,
      std::move(descriptor_set)
    );
  }

  auto cmd_dispatch(
    Command_buffer *cmd,
    Buffer *argument_buffer,
    std::uint64_t argument_offset
  ) -> void
  {
    cmd->cmd_dispatch(
      std::move(cmd),
      std::move(argument_buffer),
      argument_offset
    );
  }

  auto cmd_draw(
    Command_buffer *cmd,
    Buffer *argument_buffer,
    std::uint64_t argument_offset,
    std::uint32_t argument_stride,
    Buffer *count_buffer,
    std::uint64_t count_offset,
    std::uint32_t max_count
  ) -> void
  {
    cmd->cmd_draw(
      std::move(cmd),
      std::move(argument_buffer),
      argument_offset,
      argument_stride,
      std::move(count_buffer),
      count_offset,
      max_count
    );
  }

  auto cmd_barrier(
    Command_buffer *cmd,
    Barrier_scope const &src,
    Barrier_scope const &dst
  ) -> void
  {
    cmd->cmd_barrier(std::move(cmd), src, dst);
  }
} // namespace marlon::rhi
