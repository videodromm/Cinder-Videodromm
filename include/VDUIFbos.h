#pragma once

#include "cinder/app/App.h"

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
	// stores the pointer to the VDUIFbos instance
	typedef std::shared_ptr<class VDUIFbos> VDUIFbosRef;

	class VDUIFbos
	{
	public:
		VDUIFbos(VDSettingsRef aVDSettings, VDMixRef aVDMix);
		static VDUIFbosRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix)
		{
			return shared_ptr<VDUIFbos>(new VDUIFbos(aVDSettings, aVDMix));
		}
		~VDUIFbos();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// imgui
		float						f = 0.0f;
		char						buf[64];
	};
}