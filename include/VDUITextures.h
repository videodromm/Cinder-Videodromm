#pragma once

#include "cinder/app/App.h"
#include "cinder/Rand.h"

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
	// stores the pointer to the VDConsole instance
	typedef std::shared_ptr<class VDUITextures> VDUITexturesRef;

	class VDUITextures
	{
	public:
		VDUITextures(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUITexturesRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUITextures>(new VDUITextures(aVDSettings, aVDSession));
		}
		~VDUITextures();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		char						buf[64];
		int							xStep = 1;
		int							yStep = 1;
		// sequence
		int							playheadPositions[12];
		float						speeds[12];
	};
}