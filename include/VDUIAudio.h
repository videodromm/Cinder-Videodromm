#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUIAudio instance
	typedef std::shared_ptr<class VDUIAudio> VDUIAudioRef;

	class VDUIAudio
	{
	public:
		VDUIAudio(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDUIAudioRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDUIAudio>(new VDUIAudio(aVDSettings, aVDAnimation));
		}
		~VDUIAudio();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		// imgui
		float						f = 0.0f;
		char						buf[64];
	};
}