#include <marlon/rhi/vulkan.h>

#include <cstdlib>

#include <volk.h>

#include "interface.h"

namespace marlon::rhi::vulkan
{
  struct Interface
  {
    rhi::Interface base;
    VkInstance instance;
    VkPhysicalDevice physical_device;
    std::uint32_t queue_family_index;
    VkDevice device;
    VkQueue queue;
  };

  namespace
  {
    auto delete_interface(rhi::Object *self) -> void
    {
      std::free(self);
    }

    struct Descriptor_set_layout
    {
      rhi::Descriptor_set_layout base;
      VkDescriptorSetLayout vk_handle;
    };

    auto new_descriptor_set_layout(
      rhi::Interface *interface,
      rhi::Descriptor_set_layout_create_info const &create_info
    ) -> rhi::Descriptor_set_layout *
    {
      auto const _interface = reinterpret_cast<Interface *>(interface);
      auto const vk_bindings = [&]()
      {
        auto result = std::vector<VkDescriptorSetLayoutBinding>{};
        result.reserve(create_info.bindings.size());
        for (auto const &binding : create_info.bindings)
        {
          result.push_back({
            .binding = binding.index,
            .descriptor_type =
              static_cast<VkDescriptorType>(binding.descriptor_type),
            .descriptor_count = binding.descriptor_count,
            .stage_flags =
              static_cast<VkStageFlags>(create_info.stage_bits.value()),
            .pImmutableSamplers = nullptr,
          });
        }
        return result;
      }();
      auto const vk_create_info = VkDescriptorSetLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
        .bindingCount = static_cast<std::uint32_t>(vk_bindings.size()),
        .pBindings = vk_bindings.data(),
      };
      auto const vk_handle = [&]()
      {
        auto result = VkDescriptorSetLayout{};
        auto const vk_result = vkCreateDescriptorSetLayout(
          _interface->device,
          &vk_create_info,
          &result
        );
        if (vk_result != VK_SUCCESS)
        {
          throw Error{vk_result};
        }
        return result;
      }();
      return new (std::malloc(sizeof(Descriptor_set_layout)))
        Descriptor_set_layout{
          .base =
            rhi::Descriptor_set_layout{
              .base =
                rhi::Object{
                  .parent = _interface->base,
                  .deleter = delete_descriptor_set_layout,
                  .reference_count = 1,
                },
            },
          .vk_handle = vk_handle,
        };
    }

    auto new_descriptor_set(
      rhi::Interface *,
      rhi::Descriptor_set_create_info const &
    ) -> rhi::Descriptor_set *
    {
      return nullptr;
    }

    auto new_pipeline_layout(
      rhi::Interface *,
      rhi::Pipeline_layout_create_info const &
    ) -> rhi::Pipeline_layout *
    {
      return nullptr;
    }

    auto new_compute_pipeline(
      rhi::Interface *,
      rhi::Compute_pipeline_create_info const &
    ) -> rhi::Compute_pipeline *
    {
      return nullptr;
    }

    auto new_graphics_pipeline(
      rhi::Interface *,
      rhi::Graphics_pipeline_create_info const &
    ) -> rhi::Graphics_pipeline *
    {
      return nullptr;
    }

    auto new_buffer(rhi::Interface *, rhi::Buffer_create_info const &)
      -> rhi::Buffer *
    {
      return nullptr;
    }

    auto new_image(rhi::Interface *, rhi::Image_create_info const &)
      -> rhi::Image *
    {
      return nullptr;
    }

    auto new_swapchain(rhi::Interface *, rhi::Swapchain_create_info const &)
      -> rhi::Swapchain *
    {
      return nullptr;
    }

    auto new_command_buffer(
      rhi::Interface *,
      rhi::Command_buffer_create_info const &
    ) -> rhi::Command_buffer *
    {
      return nullptr;
    }
  } // namespace

  auto new_interface(Interface_create_info const &) -> Interface *
  {
    return new (std::malloc(sizeof(Interface))) Interface{
      .base =
        rhi::Interface{
          .base =
            rhi::Object{
              .parent = nullptr,
              .deleter = delete_interface,
              .reference_count = 1,
            },
          .new_descriptor_set_layout = new_descriptor_set_layout,
          .new_descriptor_set = new_descriptor_set,
          .new_pipeline_layout = new_pipeline_layout,
          .new_compute_pipeline = new_compute_pipeline,
          .new_graphics_pipeline = new_graphics_pipeline,
          .new_buffer = new_buffer,
          .new_image = new_image,
          .new_swapchain = new_swapchain,
          .new_command_buffer = new_command_buffer,
        },
      .instance = {},
      .physical_device = {},
      .queue_family_index = {},
      .device = {},
      .queue = {},
    };
  }
} // namespace marlon::rhi::vulkan
