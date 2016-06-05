#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Message router
#include "VDRouter.h"

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
		VDUIMidi(VDSettingsRef aVDSettings, VDRouterRef aVDRouter);
		static VDUIMidiRef	create(VDSettingsRef aVDSettings, VDRouterRef aVDRouter)
		{
			return shared_ptr<VDUIMidi>(new VDUIMidi(aVDSettings, aVDRouter));
		}
		~VDUIMidi();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Message router
		VDRouterRef					mVDRouter;
		// imgui
		float						f = 0.0f;
		char						buf[64];
	};
}