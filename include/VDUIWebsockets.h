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
	// stores the pointer to the VDUIWebsockets instance
	typedef std::shared_ptr<class VDUIWebsockets> VDUIWebsocketsRef;

	class VDUIWebsockets
	{
	public:
		VDUIWebsockets(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter);
		static VDUIWebsocketsRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter)
		{
			return shared_ptr<VDUIWebsockets>(new VDUIWebsockets(aVDSettings, aVDMix, aVDRouter));
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