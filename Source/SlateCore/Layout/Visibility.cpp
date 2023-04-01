#include <PreCompileHeader.h>

#include "Visibility.h"

namespace DoDo
{
	/*static initialization*/
	const EVisibility EVisibility::visible(EVisibility::VIS_Visible);
	const EVisibility EVisibility::Collapsed(EVisibility::VIS_Collapsed);
	const EVisibility EVisibility::Hidden(EVisibility::VIS_Hidden);
	const EVisibility EVisibility::HitTestInvisible(EVisibility::VIS_HitTestInvisible);
	const EVisibility EVisibility::SelfHitTestInvisible(EVisibility::VIS_SelfHitTestInvisible);
	const EVisibility EVisibility::All(EVisibility::VIS_All);
	/*static initialization*/
}
