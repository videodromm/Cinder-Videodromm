#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"
// Message router
#include "VDRouter.h"
// Animation
#include "VDAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUIAnimation instance
	typedef std::shared_ptr<class VDUIAnimation> VDUIAnimationRef;

	class VDUIAnimation
	{
	public:
		VDUIAnimation(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation);
		static VDUIAnimationRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDUIAnimation>(new VDUIAnimation(aVDSettings, aVDMix, aVDRouter, aVDAnimation));
		}
		~VDUIAnimation();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Message router
		VDRouterRef					mVDRouter;
		// Animation
		VDAnimationRef				mVDAnimation;
		// imgui
		float						f = 0.0f;
		char						buf[64];
		// zoom
		float						minZoom;
		float						maxZoom;
	};
}