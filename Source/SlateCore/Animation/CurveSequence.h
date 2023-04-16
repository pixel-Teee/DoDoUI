#pragma once

#include "CurveHandle.h"

namespace DoDo {
	/*
	* a sequence of curves that can be used to drive animations for slate widgets
	* active timer registration is handled for the widget being animated when calling play
	* 
	* each curve within the sequence has a time offset and a duration
	* this makes FCurveSequence convenient for crating staggered animations
	* 
	* e.g.
	*	//we want to zoom in a widget, and then fade in its contents
	*	FCurveHandle zoomCurve = Sequence.AddCurve(0, 0.15f)
	*	FCurveHandle FadeCurve = Sequence.AddCurve(0.15f, 0.1f)
	*	Sequence.Play(this->AsShared());
	*/

	struct FCurveSequence : public std::enable_shared_from_this<FCurveSequence>
	{
	public:
		/*a curve has a time offset and duration*/
		struct FSlateCurve
		{
			/*length of this animation in seconds*/
			float m_duration_seconds;

			/*start time for this animation*/
			float m_start_time;

			/*
			* type of easing function to use for this curve
			* could be passed it at call site
			*/
			ECurveEaseFunction m_ease_function;
		};

		/*default constructor*/
		FCurveSequence();

		/*make sure the active timer is unregistered*/
		~FCurveSequence();

	private:
		/*all the curves in this sequence*/
		std::vector<FSlateCurve> m_curves;

		/*when the curve started playing*/
		double m_start_time;

		/*when the curve was paused*/
		double m_pause_time;

		/*how long the entire sequence lasts*/
		double m_total_duration;

		/*are we playing the animation in reverse*/
		uint8_t m_b_in_reverse : 1;

		/*is the sequence playing on a loop?*/
		uint8_t m_b_is_looping : 1;

		/*is the sequence currently paused?*/
		uint8_t m_b_is_paused : 1;
	};
}