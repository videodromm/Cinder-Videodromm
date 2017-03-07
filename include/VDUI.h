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
// Triangles
#include "VDUITriangles.h"
// Render
#include "VDUIRender.h"

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
		// UITextures
		VDUITexturesRef				mUITextures;
		bool						showUITextures;
		// UIFbos
		VDUIFbosRef					mUIFbos;
		bool						showUIFbos;
		// UIAnimation
		VDUIAnimationRef			mUIAnimation;
		bool						showUIAnimation;
		// UIMidi
		VDUIMidiRef					mUIMidi;
		bool						showUIMidi;
		// UIAudio
		VDUIAudioRef				mUIAudio;
		bool						showUIAudio;
		// UIColor
		VDUIColorRef				mUIColor;
		bool						showUIColor;
		// UITempo
		VDUITempoRef				mUITempo;
		bool						showUITempo;
		// UIBlend
		VDUIBlendRef				mUIBlend;
		bool						showUIBlend;
		// UIMouse
		VDUIMouseRef				mUIMouse;
		bool						showUIMouse;
		// UIOsc
		VDUIOscRef					mUIOsc;
		bool						showUIOsc;
		// UIWebsockets
		VDUIWebsocketsRef			mUIWebsockets;
		bool						showUIWebsockets;
		// UIShaders
		VDUIShadersRef				mUIShaders;
		bool						showUIShaders;
		// UIWarps
		VDUIWarpsRef				mUIWarps;
		bool						showUIWarps;
		// UITriangles
		VDUITrianglesRef			mUITriangles;
		bool						showUITriangles;
		// UIRender
		VDUIRenderRef				mUIRender;
		bool						showUIRender;
		
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