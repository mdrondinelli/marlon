#include <marlon/rhi/vulkan.h>

int main()
{
  namespace rhi = marlon::rhi;
  auto const interface = rhi::vulkan::new_interface({
    .bits = rhi::vulkan::Interface_create_bit::debug,
    .wsi_extensions = {},
  });
  auto const image = interface->new_image({
    .type = rhi::Image_type::_2d,
    .format = rhi::Image_format::b8g8r8a8_srgb,
    .extent = {1920, 1080, 1},
    .mip_level_count = 1,
    .array_layer_count = 1,
    .usage_bits = rhi::Image_usage_bit::transfer_src |
                  rhi::Image_usage_bit::color_attachment,
  });
  return 0;
}
