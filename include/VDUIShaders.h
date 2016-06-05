#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"
// Router
#include "VDRouter.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUIShaders instance
	typedef std::shared_ptr<class VDUIShaders> VDUIShadersRef;

	class VDUIShaders
	{
	public:
		VDUIShaders(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter);
		static VDUIShadersRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter)
		{
			return shared_ptr<VDUIShaders>(new VDUIShaders(aVDSettings, aVDMix, aVDRouter));
		}
		~VDUIWebsockets();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Router
		VDRouterRef					mVDRouter;
		// imgui
		float						f = 0.0f;
		char						buf[64];
	};
}