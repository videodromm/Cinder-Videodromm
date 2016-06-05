#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"
// Session
#include "VDSession.h"
// Animation
#include "VDAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUITempo instance
	typedef std::shared_ptr<class VDUITempo> VDUITempoRef;

	class VDUITempo
	{
	public:
		VDUITempo(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation, VDSessionRef aVDSession);
		static VDUITempoRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUITempo>(new VDUITempo(aVDSettings, aVDMix, aVDAnimation, aVDSession));
		}
		~VDUITempo();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Mix
		VDMixRef					mVDMix;
		// imgui
		float						f = 0.0f;
		char						buf[64];
	};
}