#include <marlon/rhi/vulkan.h>

#include <cassert>
#include <cstring>

#include <algorithm>
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
    struct Vk_parent<VkSurfaceKHR>
    {
      using type = VkInstance;
    };

    template <>
    struct Vk_parent<VkDevice>
    {
      using type = std::nullptr_t;
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
    struct Vk_deleter<VkSurfaceKHR>
    {
      auto operator()(VkInstance instance, VkSurfaceKHR surface) const noexcept
        -> void
      {
        vkDestroySurfaceKHR(instance, surface, nullptr);
      }
    };

    template <>
    struct Vk_deleter<VkDevice>
    {
      auto operator()(std::nullptr_t, VkDevice device) const noexcept -> void
      {
        vkDestroyDevice(device, nullptr);
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

      constexpr Vk_guard(Vk_guard &&other) noexcept
          : _parent{std::exchange(other._parent, VK_NULL_HANDLE)},
            _value{std::exchange(other._value, VK_NULL_HANDLE)}
      {
      }

      auto operator=(Vk_guard &&other) noexcept -> Vk_guard &
      {
        auto temp = std::move(other);
        swap(temp);
        return *this;
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
      constexpr auto swap(Vk_guard &other) noexcept -> void
      {
        std::swap(_parent, other._parent);
        std::swap(_value, other._value);
      }

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

    struct Interface_impl final: Interface
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

      auto get_vk_instance() const noexcept -> VkInstance final;

      auto new_surface(Surface_create_info const &create_info)
        -> rhi::Ptr<rhi::Surface> final;

      auto select_device(Ptr<rhi::Surface> surface) -> void final;

      auto new_descriptor_set_layout(
        Descriptor_set_layout_create_info const &create_info
      ) -> rhi::Ptr<rhi::Descriptor_set_layout> final;

      auto new_descriptor_set(Descriptor_set_create_info const &create_info)
        -> rhi::Ptr<rhi::Descriptor_set> final;

      auto new_pipeline_layout(Pipeline_layout_create_info const &create_info)
        -> rhi::Ptr<rhi::Pipeline_layout> final;

      auto new_compute_pipeline(Compute_pipeline_create_info const &create_info)
        -> rhi::Ptr<rhi::Compute_pipeline> final;

      auto new_graphics_pipeline(
        Graphics_pipeline_create_info const &create_info
      ) -> rhi::Ptr<rhi::Graphics_pipeline> final;

      auto new_buffer(Buffer_create_info const &create_info)
        -> rhi::Ptr<rhi::Buffer> final;

      auto new_image(Image_create_info const &create_info)
        -> rhi::Ptr<rhi::Image> final;

      auto new_swapchain(Swapchain_create_info const &create_info)
        -> rhi::Ptr<rhi::Swapchain> final;

      auto new_command_buffer(Command_buffer_create_info const &create_info)
        -> rhi::Ptr<rhi::Command_buffer> final;
    };

    struct Surface_impl final: rhi::Surface
    {
      rhi::Ptr<Interface_impl> parent;
      Vk_guard<VkSurfaceKHR> vk_handle;

      explicit Surface_impl(
        rhi::Deallocator *deallocator,
        rhi::Ptr<Interface_impl> parent,
        Surface_create_info const &create_info
      ) noexcept;
    };

    struct Descriptor_set_layout_impl final: rhi::Descriptor_set_layout
    {
      rhi::Ptr<Interface_impl> parent;
      Vk_guard<VkDescriptorSetLayout> vk_handle;

      explicit Descriptor_set_layout_impl(
        rhi::Deallocator *deallocator,
        rhi::Ptr<Interface_impl> parent,
        Descriptor_set_layout_create_info const &create_info
      );
    };

    struct Pipeline_layout_impl final: rhi::Pipeline_layout
    {
      rhi::Ptr<Interface_impl> parent;
      std::vector<rhi::Ptr<Descriptor_set_layout_impl>> descriptor_set_layouts;
      Vk_guard<VkPipelineLayout> vk_handle;

      explicit Pipeline_layout_impl(
        rhi::Deallocator *deallocator,
        rhi::Ptr<Interface_impl> parent,
        Pipeline_layout_create_info const &create_info
      );
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
        : Interface{deallocator, sizeof(Interface_impl)},
          vk_instance{
            [&]()
            {
              ensure_volk_initialized();
              auto const validation_layer = "VK_LAYER_KHRONOS_validation";
              auto const layers = create_info.bits & Interface_create_bit::debug
                                    ? std::span{&validation_layer, 1}
                                    : std::span<char const * const>{};
              auto const extensions = [&]()
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
                        return std::strcmp(cstring_in_range, cstring_to_find) ==
                               0;
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
              auto const create_info = VkInstanceCreateInfo{
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = {},
                .pApplicationInfo = {},
                .enabledLayerCount = static_cast<std::uint32_t>(layers.size()),
                .ppEnabledLayerNames = layers.data(),
                .enabledExtensionCount =
                  static_cast<std::uint32_t>(extensions.size()),
                .ppEnabledExtensionNames = extensions.data(),
              };
              auto handle = VkInstance{};
              if (auto const result =
                    vkCreateInstance(&create_info, nullptr, &handle);
                  result != VK_SUCCESS)
              {
                throw Error{result};
              }
              return Vk_guard{nullptr, handle};
            }()
          }
    {
    }

    auto Interface_impl::get_vk_instance() const noexcept -> VkInstance
    {
      return vk_instance.get();
    }

    auto Interface_impl::new_surface(Surface_create_info const &create_info)
      -> rhi::Ptr<rhi::Surface>
    {
      auto storage = marlon::guarded_malloc(sizeof(Surface_impl));
      auto const object = new (storage.get()) Surface_impl{
        &malloc_deallocator,
        rhi::copy_ptr(this),
        create_info,
      };
      storage.release();
      return rhi::move_ptr(object);
    }

    auto Interface_impl::select_device(rhi::Ptr<rhi::Surface>) -> void
    {
    }

    auto Interface_impl::new_descriptor_set_layout(
      Descriptor_set_layout_create_info const &create_info
    ) -> rhi::Ptr<rhi::Descriptor_set_layout>
    {
      auto storage = marlon::guarded_malloc(sizeof(Descriptor_set_layout_impl));
      auto const object = new (storage.get()) Descriptor_set_layout_impl{
        &malloc_deallocator,
        rhi::copy_ptr(this),
        create_info,
      };
      storage.release();
      return rhi::move_ptr(object);
    }

    auto Interface_impl::new_descriptor_set(Descriptor_set_create_info const &)
      -> Ptr<rhi::Descriptor_set>
    {
      return nullptr;
    }

    auto Interface_impl::new_pipeline_layout(
      Pipeline_layout_create_info const &create_info
    ) -> Ptr<rhi::Pipeline_layout>
    {
      auto storage = marlon::guarded_malloc(sizeof(Pipeline_layout_impl));
      auto const object = new (storage.get()) Pipeline_layout_impl{
        &malloc_deallocator,
        rhi::copy_ptr(this),
        create_info,
      };
      storage.release();
      return rhi::move_ptr(object);
    }

    auto Interface_impl::new_compute_pipeline(
      Compute_pipeline_create_info const &
    ) -> rhi::Ptr<rhi::Compute_pipeline>
    {
      return nullptr;
    }

    auto Interface_impl::new_graphics_pipeline(
      Graphics_pipeline_create_info const &
    ) -> rhi::Ptr<rhi::Graphics_pipeline>
    {
      return nullptr;
    }

    auto Interface_impl::new_buffer(Buffer_create_info const &)
      -> rhi::Ptr<rhi::Buffer>
    {
      return nullptr;
    }

    auto Interface_impl::new_image(Image_create_info const &)
      -> rhi::Ptr<rhi::Image>
    {
      return nullptr;
    }

    auto Interface_impl::new_swapchain(Swapchain_create_info const &)
      -> rhi::Ptr<rhi::Swapchain>
    {
      return nullptr;
    }

    auto Interface_impl::new_command_buffer(Command_buffer_create_info const &)
      -> rhi::Ptr<rhi::Command_buffer>
    {
      return nullptr;
    }

    Surface_impl::Surface_impl(
      rhi::Deallocator *deallocator,
      rhi::Ptr<Interface_impl> parent,
      Surface_create_info const &create_info
    ) noexcept
        : rhi::Surface{deallocator, sizeof(Surface_impl)},
          parent{std::move(parent)},
          vk_handle{parent->vk_instance.get(), create_info.vk_handle}
    {
    }

    Descriptor_set_layout_impl::Descriptor_set_layout_impl(
      rhi::Deallocator *deallocator,
      rhi::Ptr<Interface_impl> parent,
      Descriptor_set_layout_create_info const &create_info
    )
        : rhi::
            Descriptor_set_layout{deallocator, sizeof(Descriptor_set_layout_impl)},
          parent{std::move(parent)},
          vk_handle{
            [&]()
            {
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
                    .stageFlags = static_cast<VkShaderStageFlags>(
                      binding.stage_bits.value()
                    ),
                    .pImmutableSamplers = nullptr,
                  });
                }
                return result;
              }();
              auto const vk_create_info = VkDescriptorSetLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags =
                  VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
                .bindingCount = static_cast<std::uint32_t>(vk_bindings.size()),
                .pBindings = vk_bindings.data(),
              };
              auto vk_handle = VkDescriptorSetLayout{};
              auto const vk_result = vkCreateDescriptorSetLayout(
                this->parent->vk_device.get(),
                &vk_create_info,
                nullptr,
                &vk_handle
              );
              if (vk_result != VK_SUCCESS)
              {
                throw Error{vk_result};
              }
              return Vk_guard{parent->vk_device.get(), vk_handle};
            }()
          }
    {
    }

    Pipeline_layout_impl::Pipeline_layout_impl(
      rhi::Deallocator *deallocator,
      rhi::Ptr<Interface_impl> parent,
      Pipeline_layout_create_info const &create_info
    )
        : rhi::Pipeline_layout{deallocator, sizeof(Pipeline_layout_impl)},
          parent{std::move(parent)},
          descriptor_set_layouts{
            [&]()
            {
              auto result = std::vector<rhi::Ptr<Descriptor_set_layout_impl>>{};
              result.reserve(create_info.descriptor_set_layouts.size());
              for (auto const &descriptor_set_layout :
                   create_info.descriptor_set_layouts)
              {
                result.emplace_back(
                  static_cast<rhi::Ptr<Descriptor_set_layout_impl>>(
                    descriptor_set_layout
                  )
                );
              }
              return result;
            }()
          },
          vk_handle{
            [&]()
            {
              auto const vk_set_layouts = [&]()
              {
                auto result = std::vector<VkDescriptorSetLayout>{};
                result.reserve(descriptor_set_layouts.size());
                for (auto const &descriptor_set_layout : descriptor_set_layouts)
                {
                  result.emplace_back(descriptor_set_layout->vk_handle.get());
                }
                return result;
              }();
              auto const vk_create_info = VkPipelineLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = {},
                .setLayoutCount =
                  static_cast<std::uint32_t>(vk_set_layouts.size()),
                .pSetLayouts = vk_set_layouts.data(),
                .pushConstantRangeCount = 0,
                .pPushConstantRanges = nullptr,
              };
              auto vk_handle = VkPipelineLayout{};
              auto const vk_result = vkCreatePipelineLayout(
                this->parent->vk_device.get(),
                &vk_create_info,
                nullptr,
                &vk_handle
              );
              if (vk_result != VK_SUCCESS)
              {
                throw Error{vk_result};
              }
              return Vk_guard{this->parent->vk_device.get(), vk_handle};
            }()
          }
    {
    }
  } // namespace

  auto new_interface(Interface_create_info const &create_info)
    -> rhi::Ptr<Interface>
  {
    auto storage = marlon::guarded_malloc(sizeof(Interface_impl));
    auto const object =
      new (storage.get()) Interface_impl{&malloc_deallocator, create_info};
    storage.release();
    return rhi::move_ptr(object);
  }
} // namespace marlon::rhi::vulkan
