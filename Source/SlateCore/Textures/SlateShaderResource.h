#pragma once

#include "Core/Math/IntPoint.h"

#include <glm/glm.hpp>

#include <memory>

namespace DoDo {
	namespace ESlateShaderResource
	{
		enum Type
		{
			NativeTexture,

			TextureObject,//UTexture Object Resource

			Invalid
		};
	}
	
	/*
	* base class for all platform independent texture types
	*/
	class FSlateShaderResource
	{
	public:
		/*
		* gets the width of the resource
		* 
		* @return resource width (in pixels)
		*/
		virtual uint32_t get_width() const = 0;

		/*
		* gets the height of the resource
		* 
		* @return resource height (in pixels)
		*/
		virtual uint32_t get_height() const = 0;

		/*
		* gets the type of the resource
		* 
		* @return resource type
		*/
		virtual ESlateShaderResource::Type get_type() const = 0;

		virtual ~FSlateShaderResource() { }
	};
	class FSlateShaderResourceProxy;
	/*
	* data used to lookup a resource from a handle and to determine validity of the handle
	* this is shared between the handle and the resource
	*/
	class FSlateSharedHandleData
	{
	public:
		FSlateSharedHandleData(FSlateShaderResourceProxy* in_proxy = nullptr)
			: m_proxy(in_proxy)
		{}

	public:
		/*rendering proxy used to directly access the resource quickly*/
		FSlateShaderResourceProxy* m_proxy;
	};
	/*
	* a proxy resource
	* 
	* may point to a full resource or point or to a texture resource in an atlas
	* note:this class does not free any resources, resources should be owned and freed elsewhere
	*/
	class FSlateShaderResourceProxy
	{
	public:
		/*the start uv of the texture, if atlased this is some subUV of the atlas, 0,0 otherwise*/
		glm::vec2 m_start_uv;

		/*the size of the texture in uv space, if atlas this some sub uv of the atlas, 1,1 otherwise*/
		glm::vec2 m_size_uv;

		/*the size of the texture, regardless of atlasing this is the size of the actual texture*/
		FIntPoint m_actual_size;

		/*the resource to be used for rendering*/
		FSlateShaderResource* m_resource;
		
		/*shared data between resources and handles to this resource, is created the first time to handle is needed*/
		std::shared_ptr<FSlateSharedHandleData> m_handle_data;
		/*
		* default constructor
		*/
		FSlateShaderResourceProxy()
			: m_start_uv(0.0f, 0.0f)
			, m_size_uv(1.0f, 1.0f)
			, m_resource(nullptr)
			, m_actual_size(0.0f, 0.0f)
		{}

		~FSlateShaderResourceProxy()
		{
			if (m_handle_data)
			{
				m_handle_data->m_proxy = nullptr;
			}
		}
	};

	/*
	* abstract base class for platform independent texture resource accessible by the shader
	*/
	template<typename ResourceType>
	class TSlateTexture : public FSlateShaderResource
	{
	public:
		TSlateTexture() {}

		TSlateTexture(ResourceType& in_shader_resource)
			: m_shader_resource(in_shader_resource)
		{}

		virtual ~TSlateTexture() {}

		ResourceType& get_typed_resource()
		{
			return m_shader_resource;
		}

		virtual ESlateShaderResource::Type get_type() const override
		{
			return ESlateShaderResource::NativeTexture;
		}
	protected:

		//holds the resource
		ResourceType m_shader_resource;
	};

	/*
	* a slate resource handle is used as fast path for looking up a rendering resource for a given brush when adding slate draw elements
	* this can be cached and stored safely in code, it will become invalid when a resource is destroyed
	*/
	class FSlateResourceHandle
	{
		friend class FSlateShaderResourceManager;//texture manager
	public:
		FSlateResourceHandle() {}

		/*
		* return true if the handle still points to a valid rendering resource
		*/
		bool is_valid() const { return m_data.get() && m_data->m_proxy; }

		/*
		* return the resource proxy used to render
		*/
		const FSlateShaderResourceProxy* get_resource_proxy() const
		{
			return m_data.get() ? m_data->m_proxy : nullptr;
		}
	private:
		/*internal data to pair the handle to the resource*/
		std::shared_ptr<FSlateSharedHandleData> m_data;
	};
}