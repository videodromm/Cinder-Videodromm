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
	// stores the pointer to the VDUIMidi instance
	typedef std::shared_ptr<class VDUIMidi> VDUIMidiRef;

	class VDUIMidi
	{
	public:
		VDUIMidi(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIMidiRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIMidi>(new VDUIMidi(aVDSettings, aVDSession));
		}
		~VDUIMidi();
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