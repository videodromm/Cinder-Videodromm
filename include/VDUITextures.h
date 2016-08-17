#pragma once

#include "cinder/app/App.h"
#include "cinder/Rand.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"

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
		VDUITextures(VDSettingsRef aVDSettings, VDMixRef aVDMix);
		static VDUITexturesRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix)
		{
			return shared_ptr<VDUITextures>(new VDUITextures(aVDSettings, aVDMix));
		}
		~VDUITextures();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// imgui
		char						buf[64];
		int							xStep = 1;
		int							yStep = 1;
	};
}