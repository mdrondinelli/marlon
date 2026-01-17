#include <marlon/rhi/vulkan.h>

#include <cassert>
#include <cstring>

#include <algorithm>
#include <bit>
#include <mutex>
#include <vector>

#include <marlon/malloc.h>

namespace marlon::rhi::vulkan
{
  namespace
  {
    template <typename T>
    struct Vk_parent;

    template <>
    struct Vk_parent<VkInstance>
    {
      using type = std::nullptr_t;
    };

    template <>
    struct Vk_parent<VkDevice>
    {
      using type = VkInstance;
    };

    template <>
    struct Vk_parent<VkDescriptorSetLayout>
    {
      using type = VkDevice;
    };

    template <>
    struct Vk_parent<VkPipelineLayout>
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

    template <>
    struct Vk_deleter<VkPipelineLayout>
    {
      auto operator()(
        VkDevice device,
        VkPipelineLayout pipeline_layout
      ) const noexcept
      {
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
      }
    };

    template <typename T>
    struct Vk_guard
    {
      constexpr Vk_guard() noexcept = default;

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

      constexpr auto get() const noexcept -> T
      {
        return _value;
      }

      constexpr auto release() noexcept -> T
      {
        auto temp = _value;
        _value = nullptr;
        return temp;
      }

    private:
      Vk_parent_t<T> _parent{};
      T _value{};
    };

    struct Malloc_deallocator: rhi::Deallocator
    {
      auto free(Object *object, std::size_t) noexcept -> void override
      {
        std::free(object);
      }
    };

    auto malloc_deallocator = Malloc_deallocator{};

    struct Interface_impl: Interface
    {
      Vk_guard<VkInstance> vk_instance{};
      VkPhysicalDevice vk_physical_device{};
      std::uint32_t vk_queue_family_index{};
      Vk_guard<VkDevice> vk_device{};
      VkQueue vk_queue{};

      Interface_impl(
        rhi::Deallocator *deallocator,
        Interface_create_info const &create_info
      );

      auto new_surface(Surface_create_info const &create_info)
        -> rhi::Surface * final;

      auto select_device(rhi::Surface *surface) -> void final;

      auto new_descriptor_set_layout(
        Descriptor_set_layout_create_info const &create_info
      ) -> rhi::Descriptor_set_layout * final;

      auto new_descriptor_set(Descriptor_set_create_info const &create_info)
        -> rhi::Descriptor_set * final;

      auto new_pipeline_layout(Pipeline_layout_create_info const &create_info)
        -> rhi::Pipeline_layout * final;

      auto new_compute_pipeline(Compute_pipeline_create_info const &create_info)
        -> rhi::Compute_pipeline * final;

      auto new_graphics_pipeline(
        Graphics_pipeline_create_info const &create_info
      ) -> rhi::Graphics_pipeline * final;

      auto new_buffer(Buffer_create_info const &create_info)
        -> rhi::Buffer * final;

      auto new_image(Image_create_info const &create_info)
        -> rhi::Image * final;

      auto new_swapchain(Swapchain_create_info const &create_info)
        -> rhi::Swapchain * final;

      auto new_command_buffer(Command_buffer_create_info const &create_info)
        -> rhi::Command_buffer * final;
    };

    struct Surface_impl: rhi::Surface
    {
      VkSurfaceKHR vk_handle;
    };

    struct Descriptor_set_layout_impl: rhi::Descriptor_set_layout
    {
      VkDescriptorSetLayout vk_handle;
    };

    struct Pipeline_layout_impl: rhi::Pipeline_layout
    {
      VkPipelineLayout vk_handle;
      Descriptor_set_layout **descriptor_set_layouts;
      std::uint32_t descriptor_set_layout_count;
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
      auto result_storage =
        marlon::guarded_malloc(sizeof(Descriptor_set_layout));
      auto const result = new (result_storage.get()) Descriptor_set_layout{
        .base =
          rhi::Descriptor_set_layout{
            .base =
              rhi::Object{
                .parent =
                  rhi::acquire_object(rhi::upcast<rhi::Object *>(_interface)),
                .deleter = delete_descriptor_set_layout,
                .reference_count = 1,
              },
          },
        .vk_handle = vk_handle.get(),
      };
      vk_handle.release();
      result_storage.release();
      return rhi::upcast<rhi::Descriptor_set_layout *>(result);
    }

    auto new_descriptor_set(
      rhi::Interface *,
      rhi::Descriptor_set_create_info const &
    ) -> rhi::Descriptor_set *
    {
      return nullptr;
    }

    auto delete_pipeline_layout(rhi::Object *self) noexcept -> void
    {
      auto const _self = rhi::downcast<Pipeline_layout *>(self);
      auto const owner = rhi::downcast<Interface *>(self->parent);
      assert(owner->vk_device);
      assert(_self->vk_handle);
      vkDestroyPipelineLayout(owner->vk_device, _self->vk_handle, nullptr);
      for (auto descriptor_set_index = std::uint32_t{};
           descriptor_set_index != _self->descriptor_set_layout_count;
           ++descriptor_set_index)
      {
        rhi::release_object(
          rhi::upcast<rhi::Object *>(
            _self->descriptor_set_layouts[descriptor_set_index]
          )
        );
      }
      rhi::release_object(rhi::upcast<rhi::Object *>(owner));
      rhi::release_object(rhi::upcast<rhi::Object *>(owner));
      marlon::free(self);
    }

    auto new_pipeline_layout(
      rhi::Interface *self,
      rhi::Pipeline_layout_create_info const &create_info
    ) -> rhi::Pipeline_layout *
    {
      auto const _self = rhi::downcast<Interface *>(self);
      auto const vk_set_layouts = [&]()
      {
        auto result = std::vector<VkDescriptorSetLayout>{};
        result.reserve(create_info.descriptor_set_layouts.size());
        for (auto const &descriptor_set_layout :
             create_info.descriptor_set_layouts)
        {
          result.emplace_back(
            rhi::downcast<Descriptor_set_layout *>(descriptor_set_layout)
              ->vk_handle
          );
        }
        return result;
      }();
      auto const vk_create_info = VkPipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .setLayoutCount = static_cast<std::uint32_t>(vk_set_layouts.size()),
        .pSetLayouts = vk_set_layouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
      };
      auto vk_handle = [&]()
      {
        auto result = VkPipelineLayout{};
        auto const vk_result = vkCreatePipelineLayout(
          _self->vk_device,
          &vk_create_info,
          nullptr,
          &result
        );
        if (vk_result != VK_SUCCESS)
        {
          throw Error{vk_result};
        }
        return Vk_guard{_self->vk_device, result};
      }();
      auto pipeline_layout_storage =
        marlon::guarded_malloc(sizeof(Pipeline_layout));
      auto descriptor_set_layouts_storage = marlon::guarded_malloc(
        sizeof(Descriptor_set_layout *) *
        create_info.descriptor_set_layouts.size()
      );
      auto const descriptor_set_layouts =
        new (descriptor_set_layouts_storage.get())
          Descriptor_set_layout *[create_info.descriptor_set_layouts.size()];
      for (auto descriptor_set_index = std::size_t{};
           descriptor_set_index != create_info.descriptor_set_layouts.size();
           ++descriptor_set_index)
      {
        descriptor_set_layouts[descriptor_set_index] =
          rhi::downcast<Descriptor_set_layout *>(
            create_info.descriptor_set_layouts[descriptor_set_index]
          );
      }
      auto const pipeline_layout = new (
        pipeline_layout_storage.get()
      ) Pipeline_layout{
        .base =
          rhi::Pipeline_layout{
            .base =
              rhi::Object{
                .parent = rhi::upcast<rhi::Object *>(self),
                .deleter = delete_pipeline_layout,
                .reference_count = 1,
              },
          },
        .vk_handle = vk_handle.get(),
        .descriptor_set_layouts = descriptor_set_layouts,
        .descriptor_set_layout_count =
          static_cast<std::uint32_t>(create_info.descriptor_set_layouts.size()),
      };
      for (auto const &descriptor_set_layout :
           create_info.descriptor_set_layouts)
      {
        rhi::acquire_object(rhi::upcast<rhi::Object *>(descriptor_set_layout));
      }
      vk_handle.release();
      pipeline_layout_storage.release();
      descriptor_set_layouts_storage.release();
      return rhi::upcast<rhi::Pipeline_layout *>(pipeline_layout);
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

  Interface::Interface(rhi::Deallocator *deallocator, std::size_t size) noexcept
      : rhi::Interface{deallocator, size}
  {
  }

  namespace
  {
    Interface_impl::Interface_impl(
      rhi::Deallocator *deallocator,
      Interface_create_info const &create_info
    )
        : Interface{deallocator, sizeof(Inteface_impl)}
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
      vk_instance =
        create_vk_instance(vk_instance_layers, vk_instance_extensions);
    }
  } // namespace

  auto new_interface(Interface_create_info const &create_info) -> Interface *
  {
    auto const storage = std::malloc(sizeof(Interface_impl));
    return new (storage) Interface_impl{
      &malloc_deallocator,
      create_info,
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
              .parent = acquire_object(rhi::upcast<rhi::Object *>(interface)),
              .deleter = delete_surface,
              .reference_count = 1,
            },
        },
      .vk_handle = create_info.vk_handle,
    };
  }
} // namespace marlon::rhi::vulkan
