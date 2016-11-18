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
	// stores the pointer to the VDUIBlend instance
	typedef std::shared_ptr<class VDUIBlend> VDUIBlendRef;

	class VDUIBlend
	{
	public:
		VDUIBlend(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIBlendRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIBlend>(new VDUIBlend(aVDSettings, aVDSession));
		}
		~VDUIBlend();
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