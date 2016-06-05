#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"
// Message router
#include "VDRouter.h"
// Animation
#include "VDAnimation.h"
// Session
#include "VDSession.h"

// Console
#include "VDConsole.h"
// UITextures
#include "VDUITextures.h"
// UIFbos
#include "VDUIFbos.h"
// Animation
#include "VDUIAnimation.h"
// Midi
#include "VDUIMidi.h"
// Audio
#include "VDUIAudio.h"
// Color
#include "VDUIColor.h"
// Tempo
#include "VDUITempo.h"
// Blend
#include "VDUIBlend.h"
// Websockets
#include "VDUIWebsockets.h"
// Osc
#include "VDUIOsc.h"
// Mouse
#include "VDUIMouse.h"
// Shaders
#include "VDUIShaders.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDConsole instance
	typedef std::shared_ptr<class VDUI> VDUIRef;

	class VDUI
	{
	public:
		VDUI(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation, VDSessionRef aVDSession);
		static VDUIRef	create(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUI>(new VDUI(aVDSettings, aVDMix, aVDRouter, aVDAnimation, aVDSession));
		}
		~VDUI();
		void    Run(const char* title, unsigned int fps);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Mix
		VDMixRef					mVDMix;
		// Message router
		VDRouterRef					mVDRouter;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Session
		VDSessionRef				mVDSession;
		// Console
		VDConsoleRef				mVDConsole;
		bool						showConsole;
		void						showVDConsole(bool* opened);
		// UITextures
		VDUITexturesRef				mUITextures;
		bool						showUITextures;
		void						showVDUITextures();
		// UIFbos
		VDUIFbosRef					mUIFbos;
		bool						showUIFbos;
		void						showVDUIFbos();
		// UIAnimation
		VDUIAnimationRef			mUIAnimation;
		bool						showUIAnimation;
		void						showVDUIAnimation();
		// UIMidi
		VDUIMidiRef					mUIMidi;
		bool						showUIMidi;
		void						showVDUIMidi();
		// UIAudio
		VDUIAudioRef				mUIAudio;
		bool						showUIAudio;
		void						showVDUIAudio();
		// UIColor
		VDUIColorRef				mUIColor;
		bool						showUIColor;
		void						showVDUIColor();
		// UITempo
		VDUITempoRef				mUITempo;
		bool						showUITempo;
		void						showVDUITempo();
		// UIBlend
		VDUIBlendRef				mUIBlend;
		bool						showUIBlend;
		void						showVDUIBlend();
		// UIMouse
		VDUIMouseRef				mUIMouse;
		bool						showUIMouse;
		void						showVDUIMouse();
		// UIOsc
		VDUIOscRef					mUIOsc;
		bool						showUIOsc;
		void						showVDUIOsc();
		// UIWebsockets
		VDUIWebsocketsRef			mUIWebsockets;
		bool						showUIWebsockets;
		void						showVDUIWebsockets();
		// UIShaders
		VDUIShadersRef				mUIShaders;
		bool						showUIShaders;
		void						showVDUIShaders();


		char						buf[64];
	};
}