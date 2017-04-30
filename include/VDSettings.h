#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Timeline.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm {

	typedef std::shared_ptr<class VDSettings> VDSettingsRef;

	class VDSettings
	{
	public:
		VDSettings();
		static VDSettingsRef create();

		//! maximum number of fbos, shaders, textures
		static const int			MAX = 14;
		static const int			IFPS = 25;
		// texture modes
		static const int			TEXTUREMODEMIX = 0;				// mix two shaders
		static const int			TEXTUREMODEAUDIO = 1;			// audio spectrum
		static const int			TEXTUREMODELEFT = 2;			// left
		static const int			TEXTUREMODERIGHT = 3;			// right
		static const int			TEXTUREMODEINPUT = 4;			// spout
		static const int			TEXTUREMODESHADER = 5;			// shader
		static const int			TEXTUREMODEIMAGE = 6;			// image
		static const int			TEXTUREMODEIMAGESEQUENCE = 7;	// image sequence
		static const int			TEXTUREMODETHUMB = 8;			// thumb
		static const int			TEXTUREMODEFBO = 9;				// fbo
		static const int			TEXTUREMODETEXT = 10;			// text
		static const int			TEXTUREMODEMOVIE = 11;			// movie
		bool						save();
		bool						restore();
		void						reset();
		void						resetSomeParams();

		// params
		//int							mMode, mPreviousMode, mNewMode;
		int							mPreviewWidth, mPreviewHeight, mPreviewFboWidth, mPreviewFboHeight;
		float						mAspectRatio;
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight, mFboWidth, mFboHeight;
		float						mCodeEditorWidth, mCodeEditorHeight;
		// MIDI
		bool						mMIDIEnabled;
		// OSC
		bool						mOSCEnabled;
		std::string					mOSCDestinationHost;
		uint16_t					mOSCDestinationPort;
		std::string					mOSCDestinationHost2;
		uint16_t					mOSCDestinationPort2;
		uint16_t					mOSCReceiverPort;
		std::string					mOSCMsg;
		bool						mOSCNewMsg;		// log to console
		std::string					mMsg;
		bool						mNewMsg;
		std::string					InfoMsg;
		bool						mIsOSCSender;
		// render windows
		int							mRenderWidth;
		int							mRenderHeight;
		vec2						mRenderXY, mLeftRenderXY, mRightRenderXY, mPreviewRenderXY, mWarp1RenderXY, mWarp2RenderXY;
		vec2						mRenderPosXY;
		vec2						mPreviewFragXY;
		vec2						mCamPosXY;
		//vec2						mRenderResoXY;
		bool						mAutoLayout;
		bool						mStandalone;

		// code editor
		vec2						mRenderCodeEditorXY;
		bool						mLiveCode;
		bool						mShowUI;
		bool						mCursorVisible;
		bool						isUIDirty;
		bool						mMIDIOpenAllInputPorts;
		int							mCount;
		string						mImageFile;

		// shader uniforms	
		float						iSpeedMultiplier;        // speed multiplier
		float						iChannelTime[4];
		bool						iFade;
		bool						iRepeat;
		int							iBlendmode;
		float						iParam1;
		float						iParam2;
		bool						iXorY;
		//ci::Anim<float> 			iBadTv;
		ci::Anim<float> 			iAlpha;

		string						mAssetsPath;
		bool						iShowFps;
		bool						iDebug;
		string						sFps;
		bool						iGreyScale;
		unsigned int				shaderEditIndex;
		// transition
		int							iTransition;
		Anim<float>					iAnim;
		double						mTransitionDuration;
		//! Time in seconds at which the transition to the next shader starts.
		double						mTransitionTime;

		//int							multFactor;
		// windows and params
		int							mRenderX;
		int							mRenderY;
		int							mDisplayCount;
		bool						mPreviewEnabled;
		string						mCurrentFilePath;
		// Textures
		bool						mRenderThumbs;
		int							currentSelectedIndex;

		// modes, should be the same in App
		/*static const int			MODE_MIX = 0;
		static const int			MODE_WARP = 1;
		static const int			MODE_AUDIO = 2;
		static const int			MODE_SPHERE = 3;
		static const int			MODE_MESH = 4;
		static const int			MODE_LIVE = 5;
		static const int			MODE_ABP = 6;
		static const int			MODE_KINECT = 8;
		// windows to create, should be the same in App and UI
		static const int			NONE = 0;
		static const int			RENDER_1 = 1;
		static const int			RENDER_DELETE = 5;
		static const int			MIDI_IN = 6;*/

		int							mWindowToCreate;
		ColorA						FPSColor;
		ColorA						ColorGreen;
		ColorA						ColorRed;
		ColorA						ColorYellow;
		ColorA						ColorPurple;
		// audio
		bool						mIsPlaying;
		bool						mUseLineIn;
		int							iBeat;
		int							mFftSize;
		int							mWindowSize;
		float						iSeed;

		// z EyePointZ
		float						defaultEyePointZ;
		float						minEyePointZ;
		float						maxEyePointZ;
		bool						tEyePointZ;
		bool						autoEyePointZ;

		ci::Anim<float>				mStateTransition;

		// 
		bool						mSendToOutput;
		bool						autoInvert;

		// spout
		int							mFboResolution;
		//int							mOutputVideoResolution;
		//vec2						mOutputResolution;
		// indexes for textures
		map<int, int>				iChannels;
		int							selectedChannel;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI;
		bool						mSplitWarpH;
		bool						mSplitWarpV;
		int							mUIZoom;
		int							mCurrentPreviewFboIndex;
		/*int							mSphereFboIndex, mMeshFboIndex, mLiveFboIndex, mMixFboIndex, mAudioFboIndex;
		int							mLeftFboIndex, mRightFboIndex, mVertexSphereFboIndex, mWarp1FboIndex, mWarp2FboIndex, mABPFboIndex;
		int							mLeftFragIndex, mRightFragIndex, mPreviewFragIndex, mPreviousFragIndex, mWarp1FragIndex, mWarp2FragIndex, mLiveFragIndex;
		float						iZoomLeft, iZoomRight;*/
		int							iTrack;
		// meshes
		int							mMeshIndex;
		// VertexSphere
		int							mVertexSphereTextureIndex;
		// camera
		CameraPersp					mCamera;
		vec2						mCamEyePointXY;
		float						mCamEyePointZ;
		// web sockets
		bool						mIsRouter;
		bool						mAreWebSocketsEnabledAtStartup;
		bool						mIsWebSocketsServer;
		string						mWebSocketsProtocol;
		string						mWebSocketsHost;
		uint16_t					mWebSocketsPort;
		std::string					mWebSocketsMsg;
		bool						mWebSocketsNewMsg;
		// midi
		std::string					mMidiMsg;
		float						xFade;
		bool						xFadeChanged;

		// abp
		float						mBend;
		float						liveMeter;
		// info to backup in XML
		string						mInfo;
		// ableton liveOSC
		string						mTrackName;
		// imgui positions and sizes
		int							uiMargin;
		// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15
		int							uiElementWidth;
		int							uiElementHeight;

		int							uiXPos;
		int							uiXPosCol1;
		int							uiXPosCol2;
		int							uiXPosCol3;
		int							uiYPos;
		int							uiYPosRow1;
		int							uiYPosRow2;
		int							uiYPosRow3;
		int							uiLargeW;
		int							uiLargeH;
		int							uiLargePreviewW;
		int							uiLargePreviewH;
		int							uiPreviewH;
		std::string					getDefaultVextexShaderString() { return mDefaultVextexShaderString; };
		std::string					getDefaultFragmentShaderString() { return mDefaultFragmentShaderString; };
		std::string					getMixFragmentShaderString() { return mMixFragmentShaderString; };
	private:
		const string settingsFileName = "VDSettings.xml";
		//! default vertex shader
		std::string						mDefaultVextexShaderString;
		std::string						mDefaultFragmentShaderString;
		std::string						mMixFragmentShaderString;

	};

}
