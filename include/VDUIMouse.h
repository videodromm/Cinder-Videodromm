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
	// stores the pointer to the VDUIMouse instance
	typedef std::shared_ptr<class VDUIMouse> VDUIMouseRef;

	class VDUIMouse
	{
	public:
		VDUIMouse(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation);
		static VDUIMouseRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDUIMouse>(new VDUIMouse(aVDSettings, aVDMix, aVDAnimation));
		}
		~VDUIMouse();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Animation
		VDAnimationRef				mVDAnimation;
		// imgui
		float						f = 0.0f;
		char						buf[64];
		bool						mouseGlobal;

	};
}