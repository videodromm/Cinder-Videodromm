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
	// stores the pointer to the VDUIFbos instance
	typedef std::shared_ptr<class VDUIFbos> VDUIFbosRef;

	class VDUIFbos
	{
	public:
		VDUIFbos(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIFbosRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIFbos>(new VDUIFbos(aVDSettings, aVDSession));
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
		int							xStep = 1;
		int							yStep = 1;
		// sequence
		int							playheadPositions[12];
		float						speeds[12];
	};
}