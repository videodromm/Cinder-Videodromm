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
//#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace VideoDromm
{
	// stores the pointer to the VDUIWarps instance
	typedef std::shared_ptr<class VDUIWarps> VDUIWarpsRef;

	class VDUIWarps
	{
	public:
		VDUIWarps(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter);
		static VDUIWarpsRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter)
		{
			return shared_ptr<VDUIWarps>(new VDUIWarps(aVDSettings, aVDMix, aVDRouter));
		}
		~VDUIWarps();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Router
		VDRouterRef					mVDRouter;
		// imgui
		char						buf[64];

	};
}