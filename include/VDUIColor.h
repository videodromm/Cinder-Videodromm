#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"
// Animation
#include "VDAnimation.h"
// Message router
#include "VDRouter.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUIColor instance
	typedef std::shared_ptr<class VDUIColor> VDUIColorRef;

	class VDUIColor
	{
	public:
		VDUIColor(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation);
		static VDUIColorRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDUIColor>(new VDUIColor(aVDSettings, aVDMix, aVDRouter, aVDAnimation));
		}
		~VDUIColor();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Message router
		VDRouterRef					mVDRouter;
		// imgui
		float						f = 0.0f;
		char						buf[64];
		float						color[4];
		float						backcolor[4];
	};
}