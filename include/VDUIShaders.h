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
#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

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
		~VDUIShaders();
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
		//! default fragment shader
		std::string					mFboTextureFragmentShaderString;
		string						mError;
		gl::GlslProgRef				aShader;
		std::string					mPassthruVextexShaderString;

	};
}