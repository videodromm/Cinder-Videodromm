#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Session
#include "VDSession.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUIChannels instance
	typedef std::shared_ptr<class VDUIChannels> VDUIChannelsRef;

	class VDUIChannels
	{
	public:
		VDUIChannels(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIChannelsRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIChannels>(new VDUIChannels(aVDSettings, aVDSession));
		}
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		float						f = 0.0f;
		char						buf[64];
	};
}