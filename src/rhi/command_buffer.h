#ifndef MARLON_RHI_COMMAND_BUFFER_H
#define MARLON_RHI_COMMAND_BUFFER_H

#include <marlon/rhi/rhi.h>

#include "object.h"

namespace marlon::rhi
{
  struct Command_buffer
  {
    Object base;

    void (*cmd_bind_compute_pipeline)(
      Command_buffer *cmd,
      Compute_pipeline *pipeline
    );

    void (*cmd_bind_graphics_pipeline)(
      Command_buffer *cmd,
      Graphics_pipeline *pipeline
    );

    void (*cmd_bind_descriptor_set)(
      Command_buffer *cmd,
      std::uint32_t index,
      Descriptor_set *descriptor_set
    );

    void (*cmd_dispatch)(
      Command_buffer *cmd,
      Buffer *argument_buffer,
      std::uint64_t argument_offset
    );

    void (*cmd_draw)(
      Command_buffer *cmd,
      Buffer *argument_buffer,
      std::uint64_t argument_offset,
      std::uint32_t argument_stride,
      Buffer *count_buffer,
      std::uint64_t count_offset,
      std::uint32_t max_count
    );

    void (*cmd_barrier)(
      Command_buffer *cmd,
      Barrier_scope const &src,
      Barrier_scope const &dst
    );
  };
} // namespace marlon::rhi

#endif
