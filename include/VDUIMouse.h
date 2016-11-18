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
	// stores the pointer to the VDUIMouse instance
	typedef std::shared_ptr<class VDUIMouse> VDUIMouseRef;

	class VDUIMouse
	{
	public:
		VDUIMouse(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIMouseRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIMouse>(new VDUIMouse(aVDSettings, aVDSession));
		}
		~VDUIMouse();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		float						f = 0.0f;
		char						buf[64];
		bool						mouseGlobal;

	};
}