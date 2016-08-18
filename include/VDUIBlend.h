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

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUIBlend instance
	typedef std::shared_ptr<class VDUIBlend> VDUIBlendRef;

	class VDUIBlend
	{
	public:
		VDUIBlend(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation);
		static VDUIBlendRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDUIBlend>(new VDUIBlend(aVDSettings, aVDMix, aVDAnimation));
		}
		~VDUIBlend();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Animation
		VDAnimationRef				mVDAnimation;
		// imgui
		//float						f = 0.0f;
		char						buf[64];
		int							xPos, yPos;
	};
}