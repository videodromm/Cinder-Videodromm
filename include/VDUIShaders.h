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
#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace VideoDromm
{
	// stores the pointer to the VDUIShaders instance
	typedef std::shared_ptr<class VDUIShaders> VDUIShadersRef;

	class VDUIShaders
	{
	public:
		VDUIShaders(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIShadersRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIShaders>(new VDUIShaders(aVDSettings, aVDSession));
		}
		~VDUIShaders();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// imgui
		char						buf[64];
		//! default fragment shader
		std::string					mFboTextureFragmentShaderString;
		string						mError;
		gl::GlslProgRef				aShader;
		std::string					mPassthruVextexShaderString;
		int							xPos, yPos;
	};
}