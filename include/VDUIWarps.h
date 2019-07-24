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
//#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace videodromm
{
	// stores the pointer to the VDUIWarps instance
	typedef std::shared_ptr<class VDUIWarps> VDUIWarpsRef;

	class VDUIWarps
	{
	public:
		VDUIWarps(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIWarpsRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIWarps>(new VDUIWarps(aVDSettings, aVDSession));
		}
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		char						buf[64];
		int							xPos, yPos;
	};
}