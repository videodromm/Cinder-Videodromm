#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Timeline.h"
//#include "cinder/Json.h"
#include "cinder/Xml.h"


// fonts
//#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm {

	typedef std::shared_ptr<class VDSettings> VDSettingsRef;
	//! struct for textures 
	/* obsolete
	struct Texta
	{
		char						name[32];
		int							sequenceIndex;
		bool						isSequence;
	};
	//! struct for thumbs fbos only
	struct FrameBuffa
	{
		string						name;
		gl::FboRef					fbo;
		bool						isFlipV;
		bool						isFlipH;
	};
	struct ShadaFbo
	{
	ci::gl::Fbo					fbo;
	int							shadaIndex;
	bool						active;
	};*/
	//! struct to keep track of the texture names for spout senders and shader fbo-rendered textures 
	struct Sender
	{
		char						SenderName[256];
		unsigned int				width, height;
		ci::gl::TextureRef			texture;
		bool						active;
	};
	struct Shada
	{
		gl::GlslProgRef				shader;
		string						name;
		string						text;
		bool						active;
		int							microseconds;
	};
	/*struct WarpInput
	{
	int							leftIndex;
	int							leftMode;		// 0 for input texture, 1 for shader
	int							rightIndex;
	int							rightMode;		// 0 for input texture, 1 for shader
	float						controlValues[18];		// from 0 left to 1 right
	bool						hasTexture;		// has already a texture? if not the first one is put on left and right
	bool						active;
	};
	struct WarpFbo
	{
		ci::gl::FboRef				fbo;
		int							textureIndex;
		int							textureMode;	// 0 for input texture, 1 for shader
		bool						active;
	};*/

	class VDSettings
	{
	public:
		VDSettings();
		static VDSettingsRef create();

		//! maximum number of fbos, shaders, textures
		static const int			MAX = 14;
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

		std::string mParamString;

		// params
		int							mMode, mPreviousMode, mNewMode;
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
		// log to console
		std::string					mMsg;
		bool						newMsg;
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
		bool						mLockFR, mLockFG, mLockFB, mLockFA, mLockBR, mLockBG, mLockBB, mLockBA;
		string						mImageFile;

		// shader uniforms	
		float						iGlobalTime;        // shader playback time (in seconds)
		float						iSpeedMultiplier;        // speed multiplier
		float						iChannelTime[4];
		vec3						iResolution;        // viewport resolution (in pixels)
		vec3						iChannelResolution[MAX];	// channel resolution (in pixels)
		vec4						iMouse;             // mouse pixel coords. xy: current (if MLB down), zw: click
		bool						iFade;
		bool						iRepeat;
		bool						iLight;
		bool						iLightAuto;
		int							iBlendMode;
		float						iRedMultiplier;
		float						iGreenMultiplier;
		float						iBlueMultiplier;
		float						iParam1;
		float						iParam2;
		bool						iXorY;
		ci::Anim<float> 			iBadTv;
		ci::Anim<float> 			iAlpha;

		string						mAssetsPath;
		bool						iShowFps;
		bool						iDebug;
		float						iFps;
		string						sFps;
		bool						iGreyScale;
		// transition
		int							iTransition;
		Anim<float>					iAnim;
		double						mTransitionDuration;
		//! Time in seconds at which the transition to the next shader starts.
		double						mTransitionTime;

		int							multFactor;
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
		static const int			MODE_MIX = 0;
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
		static const int			MIDI_IN = 6;

		int							mWindowToCreate;
		ColorA						FPSColor;
		ColorA						ColorGreen;
		ColorA						ColorRed;
		ColorA						ColorYellow;
		ColorA						ColorPurple;
		// audio
		float						*mData;
		float						maxVolume;
		bool						mUseLineIn;
		bool						mIsPlaying;
		//float						mAudioMultFactor; mVDSettings->controlValues[13]
		float						iFreqs[4];
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
		// colors
		bool						tFR, tFG, tFB, tFA, tBR, tBG, tBB, tBA;

		ci::Anim<float>				mStateTransition;

		//ivec2						mRenderResolution;        // render resolution (replaces iResolution which increments magically)
		// 
		bool						mSendToOutput;
		bool						autoInvert;
		// shader
		string						mShaderToLoad;

		// spout
		int							mFboResolution;
		int							mOutputVideoResolution;
		vec2						mOutputResolution;
		// OSC/MIDI/JSON controlled UI and params
		map<int, float>				controlValues;
		// indexes for textures
		map<int, int>				iChannels;
		int							selectedChannel;
		// fbo indexes for warping
		//map<int, int>				iWarpFboChannels;
		//! warp fbos
		//! warpInputs: vector of warp input textures/shader fbo texture
		//vector<WarpInput>			warpInputs;
		//void						setCrossfade(int value) { warpInputs[mVDSettings->selectedWarp].controlValues[18] = value; };

		//vector<WarpFbo>				mWarpFbos;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI; 
		bool						mSplitWarpH;
		bool						mSplitWarpV;
		int							mUIZoom;
		int							mCurrentPreviewFboIndex;
		int							mSphereFboIndex, mMeshFboIndex, mLiveFboIndex, mMixFboIndex, mAudioFboIndex;
		int							mLeftFboIndex, mRightFboIndex, mVertexSphereFboIndex, mWarp1FboIndex, mWarp2FboIndex, mABPFboIndex;
		int							mLeftFragIndex, mRightFragIndex, mPreviewFragIndex, mPreviousFragIndex, mWarp1FragIndex, mWarp2FragIndex, mLiveFragIndex;
		float						iZoomLeft, iZoomRight;
		int							iTrack;
		// meshes
		int							mMeshIndex;
		// VertexSphere
		int							mVertexSphereTextureIndex;
		// camera
		CameraPersp					mCamera;
		// Cam						mCam;
		vec2						mCamEyePointXY;
		float						mCamEyePointZ;
		// web sockets
		bool						mIsRouter;
		bool						mAreWebSocketsEnabledAtStartup;
		bool						mIsWebSocketsServer;
		uint16_t					mWebSocketsPort;
		string						mWebSocketsHost;
		//abp
		float						mBend;
		float						liveMeter;
		// info to backup in XML
		string						mInfo;
		// ableton liveOSC
		string						mTrackName;
	private:
		const string settingsFileName = "VDSettings.xml";
	};

}
