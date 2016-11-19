#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"

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
		VDUIAudio(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIAudioRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIAudio>(new VDUIAudio(aVDSettings, aVDSession));
		}
		~VDUIAudio();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		float						f = 0.0f;
		char						buf[64];
		float						multx;
	};
}