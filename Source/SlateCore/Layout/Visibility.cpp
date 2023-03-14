#include <PreCompileHeader.h>

#include "Visibility.h"

namespace DoDo
{
	/*static initialization*/
	const EVisibility EVisibility::visible(EVisibility::VIS_Visible);
	const EVisibility EVisibility::Collapsed(EVisibility::VIS_Collapsed);
	const EVisibility EVisibility::Hidden(EVisibility::Hidden);
	const EVisibility EVisibility::HitTestInvisible(EVisibility::HitTestInvisible);
	const EVisibility EVisibility::SelfHitTestInvisible(EVisibility::SelfHitTestInvisible);
	const EVisibility EVisibility::All(EVisibility::VIS_All);
	/*static initialization*/
}
