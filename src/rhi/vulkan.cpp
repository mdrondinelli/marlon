#include <marlon/rhi/vulkan.h>

#include <cassert>
#include <cstring>

#include <algorithm>
#include <bit>
#include <mutex>
#include <vector>

#include <volk.h>

#include <marlon/malloc.h>

#include "descriptor_set_layout.h"
#include "interface.h"
#include "surface.h"

namespace marlon::rhi::vulkan
{
  struct Interface
  {
    rhi::Interface base;
    VkInstance vk_instance;
    VkPhysicalDevice vk_physical_device;
    std::uint32_t vk_queue_family_index;
    VkDevice vk_device;
    VkQueue vk_queue;
  };

  struct Surface
  {
    rhi::Surface base;
    VkSurfaceKHR vk_handle;
  };

  namespace
  {
    struct Descriptor_set_layout
    {
      rhi::Descriptor_set_layout base;
      VkDescriptorSetLayout vk_handle;
    };

    auto get_base(Descriptor_set_layout *p) noexcept
    {
      return &p->base;
    }

    template <typename T>
    struct Vk_parent;

    template <>
    struct Vk_parent<VkInstance>
    {
      using type = std::nullptr_t;
    };

    template <>
    struct Vk_parent<VkDescriptorSetLayout>
    {
      using type = VkDevice;
    };

    template <typename T>
    using Vk_parent_t = Vk_parent<T>::type;

    template <typename T>
    struct Vk_deleter;

    template <>
    struct Vk_deleter<VkInstance>
    {
      auto operator()(std::nullptr_t, VkInstance instance) const noexcept
        -> void
      {
        vkDestroyInstance(instance, nullptr);
      }
    };

    template <>
    struct Vk_deleter<VkDescriptorSetLayout>
    {
      auto operator()(
        VkDevice device,
        VkDescriptorSetLayout descriptor_set_layout
      ) const noexcept
      {
        vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
      }
    };

    template <typename T>
    struct Vk_guard
    {
      constexpr explicit Vk_guard(Vk_parent_t<T> parent, T value) noexcept
          : _parent{parent}, _value{value}
      {
      }

      ~Vk_guard()
      {
        if (_value)
        {
          Vk_deleter<T>{}(_parent, _value);
        }
      }

      constexpr auto release() noexcept -> T
      {
        auto temp = _value;
        _value = nullptr;
        return temp;
      }

    private:
      Vk_parent_t<T> _parent;
      T _value;
    };

    auto ensure_volk_initialized() -> void
    {
      static auto initialized = false;
      static auto mutex = std::mutex{};
      auto const lock = std::scoped_lock{mutex};
      if (!initialized)
      {
        if (auto const result = volkInitialize(); result != VK_SUCCESS)
        {
          throw Error{result};
        }
        initialized = true;
      }
    }

    auto create_vk_instance(
      std::span<char const * const> layers,
      std::span<char const * const> extensions
    ) -> Vk_guard<VkInstance>
    {
      auto const vk_instance_create_info = VkInstanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .pApplicationInfo = {},
        .enabledLayerCount = static_cast<std::uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
      };
      auto vk_instance = VkInstance{};
      if (auto const vk_result =
            vkCreateInstance(&vk_instance_create_info, nullptr, &vk_instance);
          vk_result != VK_SUCCESS)
      {
        throw Error{vk_result};
      }
      return Vk_guard{nullptr, vk_instance};
    }

    auto delete_interface(rhi::Object *self) noexcept -> void
    {
      auto const _self = rhi::downcast<Interface *>(self);
      assert(_self->vk_device);
      vkDestroyDevice(_self->vk_device, nullptr);
      assert(_self->vk_instance);
      vkDestroyInstance(_self->vk_instance, nullptr);
      marlon::free(self);
    }

    auto delete_descriptor_set_layout(rhi::Object *self) noexcept -> void
    {
      auto const _self = rhi::downcast<Descriptor_set_layout *>(self);
      auto const owner = rhi::downcast<Interface *>(self->parent);
      assert(owner->vk_device);
      assert(_self->vk_handle);
      vkDestroyDescriptorSetLayout(owner->vk_device, _self->vk_handle, nullptr);
      rhi::release_object(rhi::upcast<rhi::Object *>(owner));
      marlon::free(self);
    }

    auto new_descriptor_set_layout(
      rhi::Interface *interface,
      rhi::Descriptor_set_layout_create_info const &create_info
    ) -> rhi::Descriptor_set_layout *
    {
      auto const _interface = rhi::downcast<Interface *>(interface);
      auto const vk_bindings = [&]()
      {
        auto result = std::vector<VkDescriptorSetLayoutBinding>{};
        result.reserve(create_info.bindings.size());
        for (auto const &binding : create_info.bindings)
        {
          result.push_back({
            .binding = binding.index,
            .descriptorType =
              static_cast<VkDescriptorType>(binding.descriptor_type),
            .descriptorCount = binding.descriptor_count,
            .stageFlags =
              static_cast<VkShaderStageFlags>(binding.stage_bits.value()),
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
      auto vk_handle = [&]()
      {
        auto result = VkDescriptorSetLayout{};
        auto const vk_result = vkCreateDescriptorSetLayout(
          _interface->vk_device,
          &vk_create_info,
          nullptr,
          &result
        );
        if (vk_result != VK_SUCCESS)
        {
          throw Error{vk_result};
        }
        return Vk_guard{_interface->vk_device, result};
      }();
      auto const result = new (marlon::malloc(sizeof(Descriptor_set_layout)))
        Descriptor_set_layout{
          .base =
            rhi::Descriptor_set_layout{
              .base =
                rhi::Object{
                  .parent = rhi::acquire_object(
                    rhi::upcast<rhi::Object *>(_interface)
                  ),
                  .deleter = delete_descriptor_set_layout,
                  .reference_count = 1,
                },
            },
          .vk_handle = vk_handle.release(),
        };
      return rhi::upcast<rhi::Descriptor_set_layout *>(result);
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

    auto delete_surface(rhi::Object *self) noexcept -> void
    {
      auto const _self = rhi::downcast<Surface *>(self);
      auto const owner = rhi::downcast<Interface *>(self->parent);
      vkDestroySurfaceKHR(owner->vk_instance, _self->vk_handle, nullptr);
      rhi::release_object(rhi::upcast<rhi::Object *>(owner));
      marlon::free(self);
    }
  } // namespace

  auto new_interface(Interface_create_info const &create_info) -> Interface *
  {
    ensure_volk_initialized();
    auto const vk_validation_layer = "VK_LAYER_KHRONOS_validation";
    auto const vk_instance_layers =
      create_info.bits & Interface_create_bit::debug
        ? std::span{&vk_validation_layer, 1}
        : std::span<char const * const>{};
    auto const vk_instance_extensions = [&]()
    {
      auto result = std::vector<char const *>{};
      if (create_info.bits & Interface_create_bit::debug)
      {
        result.emplace_back("VK_EXT_debug_utils");
      }
      if (create_info.bits & Interface_create_bit::wsi)
      {
        result.emplace_back("VK_KHR_surface");
      }
      for (auto const &wsi_extension : create_info.wsi_extensions)
      {
        auto const is_cstring_in_range =
          [](char const *cstring_to_find, auto const &range)
        {
          auto const it = std::ranges::find_if(
            range,
            [&](char const *cstring_in_range)
            {
              return std::strcmp(cstring_in_range, cstring_to_find) == 0;
            }
          );
          return it != range.end();
        };
        if (!is_cstring_in_range(wsi_extension, result))
        {
          result.emplace_back(wsi_extension);
        }
      }
      return result;
    }();
    auto vk_instance =
      create_vk_instance(vk_instance_layers, vk_instance_extensions);
    return new (marlon::malloc(sizeof(Interface))) Interface{
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
      .vk_instance = vk_instance.release(),
      .vk_physical_device = {},
      .vk_queue_family_index = {},
      .vk_device = {},
      .vk_queue = {},
    };
  }

  auto new_surface(Interface *interface, Surface_create_info const &create_info)
    -> Surface *
  {
    return new (marlon::malloc(sizeof(Surface))) Surface{
      .base =
        rhi::Surface{
          .base =
            rhi::Object{
              .parent =
                acquire_object(rhi::upcast<rhi::Object *>(interface)),
              .deleter = delete_surface,
              .reference_count = 1,
            },
        },
      .vk_handle = std::bit_cast<VkSurfaceKHR>(create_info.vk_handle),
    };
  }
} // namespace marlon::rhi::vulkan

namespace marlon::rhi
{
  template <>
  auto laundering_cast<vulkan::Interface *>(void *vp) noexcept
    -> vulkan::Interface *
  {
    return std::launder(static_cast<vulkan::Interface *>(vp));
  }

  template <>
  auto laundering_cast<vulkan::Descriptor_set_layout *>(void *vp) noexcept
    -> vulkan::Descriptor_set_layout *
  {
    return std::launder(static_cast<vulkan::Descriptor_set_layout *>(vp));
  }

  template <>
  auto laundering_cast<vulkan::Surface *>(void *vp) noexcept
    -> vulkan::Surface *
  {
    return std::launder(static_cast<vulkan::Surface *>(vp));
  }
} // namespace marlon::rhi
