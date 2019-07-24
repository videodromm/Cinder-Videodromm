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

namespace videodromm
{
	// stores the pointer to the VDUIOsc instance
	typedef std::shared_ptr<class VDUIOsc> VDUIOscRef;
#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

	class VDUIOsc
	{
	public:
		VDUIOsc(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIOscRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIOsc>(new VDUIOsc(aVDSettings, aVDSession));
		}
		~VDUIOsc();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		//float						f = 0.0f;
		char						buf[64];
	};
}