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
	// stores the pointer to the VDUIOsc instance
	typedef std::shared_ptr<class VDUIOsc> VDUIOscRef;
#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

	class VDUIOsc
	{
	public:
		VDUIOsc(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter);
		static VDUIOscRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter)
		{
			return shared_ptr<VDUIOsc>(new VDUIOsc(aVDSettings, aVDMix, aVDRouter));
		}
		~VDUIOsc();
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