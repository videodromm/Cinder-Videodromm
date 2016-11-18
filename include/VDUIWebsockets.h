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

#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace VideoDromm
{
	// stores the pointer to the VDUIWebsockets instance
	typedef std::shared_ptr<class VDUIWebsockets> VDUIWebsocketsRef;

	class VDUIWebsockets
	{
	public:
		VDUIWebsockets(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIWebsocketsRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIWebsockets>(new VDUIWebsockets(aVDSettings, aVDSession));
		}
		~VDUIWebsockets();
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