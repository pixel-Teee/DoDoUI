#pragma once

#include "ChildrenBase.h"//FChildren depends on it

#include "SlateCore/SlotBase.h"//TSlotBase depends on it

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

#include "BasicLayoutWidgetSlot.h"//TSingleWidgetChildrenWithBasicLayoutSlot depends on it

//#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"//TSingleWidgetChildrenWithBasicLayoutSlot depends on it

#include <functional>

namespace DoDo
{
	/* a FChildren that has only one child and can take a templated slot */
	template<typename SlotType>
	class TSingleWidgetChildrenWithSlot : public FChildren, TSlotBase<SlotType>
	{
	public:
		TSingleWidgetChildrenWithSlot(SWidget* in_owner)
			: FChildren(in_owner)
			, TSlotBase<SlotType>(static_cast<const FChildren&>(*this))//let slot to access FChildren
		{
			
		}

		TSingleWidgetChildrenWithSlot(SWidget* in_owner, DoDoUtf8String in_name)
			: FChildren(in_owner, in_name)
			, TSlotBase<SlotType>(static_cast<const FChildren&>(*this))
		{
			
		}

		TSingleWidgetChildrenWithSlot(std::nullptr_t) = delete;

	public:
		//return the number of FChildren, just one
		virtual int32_t num() const override { return 1;  }

		virtual std::shared_ptr<SWidget> get_child_at(int32_t index) override
		{
			assert(index == 0);
			return this->get_widget();//FSlotBase's function
		}

		virtual std::shared_ptr<const SWidget> get_child_at(int32_t index) const override
		{
			assert(index == 0);
			return this->get_widget();
		}

	public:
		//inherited from a TSlotBase<SlotType>::FSlotArguments
		struct FSlotArguments : protected TSlotBase<SlotType>::FSlotArguments
		{
			FSlotArguments()
				: TSlotBase<SlotType>::FSlotArguments(TSlotBase<SlotType>::ConstructSlotIsFChildren)
			{
				
			}

			friend TSingleWidgetChildrenWithSlot;

			//note:return this type, have real type
			typename SlotType::FSlotArguments& operator[](const std::shared_ptr<SWidget>& in_child_widget)
			{
				TSlotBase<SlotType>::FSlotArguments::attach_widget(in_child_widget);
				return static_cast<typename SlotType::FSlotArguments&>(*this);
			}
		};

		void Construct(FSlotArguments&& in_args)
		{
			//steal FSlotArguments's argument to TSlotBase
			TSlotBase<SlotType>::Construct(*this, std::move(in_args));
		}
	public:
		TSlotBase<SlotType>&& As_Slot() { return *this; }

		const TSlotBase<SlotType>& As_Slot() const { return *this; }

		using TSlotBase<SlotType>::get_owner_widget;
		using TSlotBase<SlotType>::attach_widget;
		using TSlotBase<SlotType>::detach_widget;
		using TSlotBase<SlotType>::get_widget;
		using TSlotBase<SlotType>::invalidate;

		SlotType& operator[](const std::shared_ptr<SWidget>& in_child_widget)
		{
			this->attach_widget(in_child_widget);
			return static_cast<SlotType&>(*this);
		}

		SlotType& Expose(SlotType*& out_var_to_init)
		{
			out_var_to_init = static_cast<SlotType*>(*this);
			return static_cast<SlotType&>(*this);
		}

	private:
		virtual const FSlotBase& get_slot_at(int32_t child_index) const override
		{
			assert(child_index == 0);
			return *this;
		}

		//todo:implement return FWidgetRef
	};

	/* a FChildren that has only one child and support alignment and padding */
	template<EInvalidateWidgetReason In_Padding_Invalidation_Reason = EInvalidateWidgetReason::Layout>
	class TSingleWidgetChildrenWithBasicLayoutSlot : public TSingleWidgetChildrenWithSlot<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>
		, public TPaddingSingleWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>, In_Padding_Invalidation_Reason>
		, public TAlignmentSingleWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>
	{
	public:
		using ParentType = TSingleWidgetChildrenWithSlot<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>;//parent type
		using PaddingMixinType = TPaddingSingleWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>, In_Padding_Invalidation_Reason>;
		using AlignmentMixinType = TAlignmentSingleWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>;

	public:
		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TSingleWidgetChildrenWithBasicLayoutSlot(WidgetType* in_owner)
			: ParentType(in_owner)
			, PaddingMixinType(*in_owner)
			, AlignmentMixinType(*in_owner, HAlign_Fill, VAlign_Fill)
		{
			
		}

		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TSingleWidgetChildrenWithBasicLayoutSlot(WidgetType* in_owner, const EHorizontalAlignment in_halign, const EVerticalAlignment in_valign)
			: ParentType(in_owner)
			, PaddingMixinType(*in_owner)
			, AlignmentMixinType(*in_owner, in_halign, in_valign)
		{
			
		}

		TSingleWidgetChildrenWithBasicLayoutSlot(std::nullptr_t) = delete;
		TSingleWidgetChildrenWithBasicLayoutSlot(std::nullptr_t, const EHorizontalAlignment in_halign, const EVerticalAlignment in_valign) = delete;

		//todo:implement SLATE_SLOT_BEGIN_ARGS_TwonMixins macro
		//this macro lets TSingleWidgetChildrenWithBasicLayoutSlot inherited from ParentType
		SLATE_SLOT_BEGIN_ARGS_TwoMixins(TSingleWidgetChildrenWithBasicLayoutSlot, ParentType, PaddingMixinType, AlignmentMixinType)
		SLATE_SLOT_END_ARGS()

		void Construct(FSlotArguments&& in_args)
		{
			ParentType::Construct(std::move(in_args));
			PaddingMixinType::Construct_Mixin(std::move(in_args));
			AlignmentMixinType::Construct_Mixin(std::move(in_args));
		}
	};

	/*
	 * a generic FChildren that stores children along with layout-related information
	 * the type containing widget* and layout info is specified by child type
	 * child type must have a public member SWidget* widget
	 */
	template<typename SlotType>
	class TPanelChildren : public FChildren//todo:implement some FChildren virtual function, to traverse slots
	{
	private:
		std::vector<std::unique_ptr<SlotType>> m_children;
		//todo:implement type check
		//static constexpr  bool b_support_slot_with_slate_attribute = std::is_base_of<>
	public:
		using FChildren::FChildren;//FChildren constructor

		int32_t num() const override
		{
			return m_children.size();
		}

		std::shared_ptr<SWidget> get_child_at(int32_t index) override
		{
			return m_children[index]->get_widget();//from slot to get widget
		}

		std::shared_ptr<const SWidget> get_child_at(int32_t index) const override
		{
			return m_children[index]->get_widget();
		}

		//int32_t num_slot() const override;

		const FSlotBase& get_slot_at(int32_t child_index) const override//note:get slot
		{
			return *m_children[child_index];
		}

		bool Support_Slot_With_Slate_Attribute() const override
		{
			return true;//todo:implement TWidgetSlotWithAttributeSupport
		}

		int32_t add_slot(typename SlotType::FSlotArguments&& slot_argument)
		{
			//interms slot argument to construct a slot
			std::unique_ptr<SlotType> new_slot = slot_argument.steal_slot();//steal slot's widget
			//todo:check new_slot is nullptr?
			m_children.push_back(std::move(new_slot));//add a new_slot

			int32_t result = m_children.size() - 1;

			//call FSlot's construct function
			m_children[result]->Construct(*this, std::move(slot_argument));

			return result;
		}

		void add_slots(std::vector<typename SlotType::FSlotArguments> slot_arguments)
		{
			m_children.reserve(m_children.size() + slot_arguments.size());

			for(typename SlotType::FSlotArguments& arg : slot_arguments)
			{
				add_slot(std::move(arg));
			}
		}

		void insert_slot(typename SlotType::FSlotArguments&& slot_arguments, int32_t index)
		{
			std::unique_ptr<SlotType> new_slot = slot_arguments.steal_slot();
			
			//insert at index
			m_children.insert(m_children.begin() + index, std::move(new_slot));//todo:may need to fix

			m_children[index]->Construct(*this, std::move(slot_arguments));
		}

		void reserve(int32_t num_to_reserve)
		{
			m_children.reserve(num_to_reserve);
		}

		bool is_valid_index(int32_t index) const
		{
			return index >= 0 && index < m_children.size();
		}

		const SlotType& operator[](int32_t index) const
		{
			return *m_children[index];
		}

		SlotType& operator[](int32_t index)
		{
			return *m_children[index];
		}
	public:
		/*at the end of the scope a slot will be constructed and added to the FChildren*/
		struct FScopedWidgetSlotArguments : public SlotType::FSlotArguments
		{
		public:
			FScopedWidgetSlotArguments(std::unique_ptr<SlotType> in_slot, TPanelChildren<SlotType>& in_children, int32_t in_index)
			: SlotType::FSlotArguments(std::move(in_slot))
			, m_children(in_children)
			, m_index(in_index)
			{}

			FScopedWidgetSlotArguments(std::unique_ptr<SlotType> in_slot, TPanelChildren<SlotType>& in_children, int32_t in_index, std::function<void(const SlotType*, int32_t)> on_added)
			: SlotType::FSlotArguments(std::move(in_slot))
			, m_children(in_children)
			, m_index(in_index)
			, m_added(on_added)
			{}

			FScopedWidgetSlotArguments(const FScopedWidgetSlotArguments&) = delete;
			FScopedWidgetSlotArguments& operator=(const FScopedWidgetSlotArguments&) = delete;
			FScopedWidgetSlotArguments(FScopedWidgetSlotArguments&&) = delete;
			FScopedWidgetSlotArguments& operator=(FScopedWidgetSlotArguments&&) = delete;

			virtual ~FScopedWidgetSlotArguments()
			{
				if (const SlotType* slot_ptr = this->get_slot()) 
				{
					if (m_index == -1)
					{
						m_index = m_children.add_slot(std::move(*this)); //when destroy this, to add slot to TPanelChildren
					}
					else
					{
						m_children.insert_slot(std::move(*this), m_index);
					}
					if (m_added)
					{
						m_added(slot_ptr, m_index);
					}
				}
			}

		private:
			TPanelChildren<SlotType>& m_children;
			int32_t m_index;

			std::function<void(const SlotType*, int32_t)> m_added;//added function
		};
	};

	template<typename SlotType>
	class TPanelChildrenConstIterator
	{
	public:
		TPanelChildrenConstIterator(const TPanelChildren<SlotType>& in_container, EFlowDirection in_layout_flow)
			: container(in_container)
			, layout_flow(in_layout_flow)
		{
			reset();
		}

		TPanelChildrenConstIterator(const TPanelChildren<SlotType>& in_container, EOrientation in_orientation, EFlowDirection in_layout_flow)
			: container(in_container)
			, layout_flow(in_orientation == Orient_Vertical ? EFlowDirection::LeftToRight : in_layout_flow)
		{
			reset();
		}

		/*advances iterator to the next element in the container*/
		TPanelChildrenConstIterator<SlotType>& operator++()
		{
			switch(layout_flow)
			{
			default:
			case EFlowDirection::LeftToRight:
				++index;
				break;
			case EFlowDirection::RightToLeft:
				--index;
				break;
			}

			return *this;
		}

		TPanelChildrenConstIterator<SlotType>& operator--()
		{
			switch (layout_flow)
			{
			default:
			case EFlowDirection::LeftToRight:
				--index;
				break;
			case EFlowDirection::RightToLeft:
				++index;
				break;
			}

			return *this;
		}

		const SlotType& operator * () const
		{
			return container[index];
		}

		const SlotType& operator->() const
		{
			return &container[index];
		}

		//note:this is useful for loop stop
		explicit operator bool() const
		{
			return container.is_valid_index(index);
		}

		int32_t get_index()
		{
			return index;
		}

		void reset()
		{
			switch(layout_flow)
			{
			default:
			case EFlowDirection::LeftToRight:
				index = 0;
				break;
			case EFlowDirection::RightToLeft:
				index = container.num() - 1;
				break;
			}
		}

		void set_to_end()
		{
			switch (layout_flow)
			{
			default:
			case EFlowDirection::LeftToRight:
				index = container.num() - 1;
				break;
			case EFlowDirection::RightToLeft:
				index = 0;
				break;
			}
		}

	private:
		const TPanelChildren<SlotType>& container;
		int32_t index;
		EFlowDirection layout_flow;
	};
}
