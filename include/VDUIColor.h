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

namespace VideoDromm
{
	// stores the pointer to the VDUIColor instance
	typedef std::shared_ptr<class VDUIColor> VDUIColorRef;

	class VDUIColor
	{
	public:
		VDUIColor(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIColorRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIColor>(new VDUIColor(aVDSettings, aVDSession));
		}
		~VDUIColor();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		float							getValue(unsigned int aCtrl);
		void							setValue(unsigned int aCtrl, float aValue);
		void							toggleAuto(unsigned int aCtrl);
		void							resetAutoAnimation(unsigned int aCtrl);

		// imgui
		int							ctrl;
		map<int, float>				localValues;
		float						f = 0.0f;
		char						buf[64];
		float						color[4];
		float						backcolor[4];
	};
}