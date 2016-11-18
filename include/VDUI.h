#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
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
// Warps
#include "VDUIWarps.h"

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
		VDUI(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUI>(new VDUI(aVDSettings, aVDSession));
		}
		~VDUI();
		void    Run(const char* title, unsigned int fps);
		void	resize();
		bool	isReady() { return !mIsResizing; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// Console
		VDConsoleRef				mUIConsole;
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
		// UIWarps
		VDUIWarpsRef				mUIWarps;
		bool						showUIWarps;
		void						showVDUIWarps();
		
		char						buf[64];
		bool						mIsResizing;
		// imgui
		float						color[4];
		float						backcolor[4];

		// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15
		int							w;
		int							h;
		int							displayHeight;
		int							xPos;
		int							yPos;
		int							yPosRow1;
		int							yPosRow2;
		int							yPosRow3;
		int							largeW;
		int							largeH;
		int							largePreviewW;
		int							largePreviewH;
		int							margin;
		int							inBetween;

		bool						mouseGlobal;
	};
}