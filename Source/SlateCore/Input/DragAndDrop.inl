#pragma once

class FDragDropEvent;

/*@return the content being dragged if it matched the 'OperationType'; invalid Ptr otherwise*/
template<typename OperationType>
std::shared_ptr<OperationType> FDragDropEvent::get_operation_as() const
{
	if (m_content && m_content->is_of_type<OperationType>())
	{
		return std::static_pointer_cast<OperationType>();
	}
	else
	{
		return std::shared_ptr<OperationType>();//nullptr
	}
}