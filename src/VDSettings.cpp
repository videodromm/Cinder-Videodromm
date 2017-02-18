//
//  VDSettings.cpp
//

#include "VDSettings.h"

using namespace ci;
using namespace std;
using namespace VideoDromm;

VDSettings::VDSettings()
{
	// reset no matter what, so we don't miss anything
	reset();

	// check to see if Settings.xml file exists and restore if it does
	fs::path params = getAppPath() / settingsFileName;
	if (fs::exists(params))
	{
		restore();
	}
	else
	{
		save();
	}
}

VDSettingsRef VDSettings::create()
{
	return shared_ptr<VDSettings>(new VDSettings());
}

bool VDSettings::save()
{
	fs::path directory = getAppPath();
	/*if (!fs::exists(directory)) {
		if (!createDirectories(directory / settingsFileName)) {
		return false;
		}
		}*/

	fs::path path = directory / settingsFileName;

	XmlTree settings("settings", "");

	XmlTree AutoLayout("AutoLayout", "");
	AutoLayout.setAttribute("value", toString(mAutoLayout));
	settings.push_back(AutoLayout);

	XmlTree Standalone("Standalone", "");
	Standalone.setAttribute("value", toString(mStandalone));
	settings.push_back(Standalone);

	XmlTree MainWindowWidth("MainWindowWidth", "");
	MainWindowWidth.setAttribute("value", toString(mMainWindowWidth));
	settings.push_back(MainWindowWidth);

	XmlTree MainWindowHeight("MainWindowHeight", "");
	MainWindowHeight.setAttribute("value", toString(mMainWindowHeight));
	settings.push_back(MainWindowHeight);

	XmlTree MainWindowX("MainWindowX", "");
	MainWindowX.setAttribute("value", toString(mMainWindowX));
	settings.push_back(MainWindowX);

	XmlTree MainWindowY("MainWindowY", "");
	MainWindowY.setAttribute("value", toString(mMainWindowY));
	settings.push_back(MainWindowY);

	XmlTree RenderWidth("RenderWidth", "");
	RenderWidth.setAttribute("value", toString(mRenderWidth));
	settings.push_back(RenderWidth);

	XmlTree RenderHeight("RenderHeight", "");
	RenderHeight.setAttribute("value", toString(mRenderHeight));
	settings.push_back(RenderHeight);

	XmlTree RenderX("RenderX", "");
	RenderX.setAttribute("value", toString(mRenderX));
	settings.push_back(RenderX);

	XmlTree RenderY("RenderY", "");
	RenderY.setAttribute("value", toString(mRenderY));
	settings.push_back(RenderY);

	XmlTree AspectRatio("AspectRatio", "");
	AspectRatio.setAttribute("value", toString(mAspectRatio));
	settings.push_back(AspectRatio);

	XmlTree FboWidth("FboWidth", "");
	FboWidth.setAttribute("value", toString(mFboWidth));
	settings.push_back(FboWidth);

	XmlTree FboHeight("FboHeight", "");
	FboHeight.setAttribute("value", toString(mFboHeight));
	settings.push_back(FboHeight);

	XmlTree MIDIEnabled("MIDIEnabled", "");
	MIDIEnabled.setAttribute("value", toString(mMIDIEnabled));
	settings.push_back(MIDIEnabled);

	XmlTree OSCEnabled("OSCEnabled", "");
	OSCEnabled.setAttribute("value", toString(mOSCEnabled));
	settings.push_back(OSCEnabled);

	XmlTree OSCReceiverPort("OSCReceiverPort", "");
	OSCReceiverPort.setAttribute("value", toString(mOSCReceiverPort));
	settings.push_back(OSCReceiverPort);

	XmlTree OSCDestinationPort("OSCDestinationPort", "");
	OSCDestinationPort.setAttribute("value", toString(mOSCDestinationPort));
	settings.push_back(OSCDestinationPort);

	XmlTree OSCDestinationHost("OSCDestinationHost", "");
	OSCDestinationHost.setAttribute("value", toString(mOSCDestinationHost));
	settings.push_back(OSCDestinationHost);

	XmlTree OSCDestinationPort2("OSCDestinationPort2", "");
	OSCDestinationPort2.setAttribute("value", toString(mOSCDestinationPort2));
	settings.push_back(OSCDestinationPort2);

	XmlTree OSCDestinationHost2("OSCDestinationHost2", "");
	OSCDestinationHost2.setAttribute("value", toString(mOSCDestinationHost2));
	settings.push_back(OSCDestinationHost2);

	XmlTree MIDIOpenAllInputPorts("MIDIOpenAllInputPorts", "");
	MIDIOpenAllInputPorts.setAttribute("value", toString(mMIDIOpenAllInputPorts));
	settings.push_back(MIDIOpenAllInputPorts);

	XmlTree ShowUI("ShowUI", "");
	ShowUI.setAttribute("value", toString(mShowUI));
	settings.push_back(ShowUI);

	XmlTree CursorVisible("CursorVisible", "");
	CursorVisible.setAttribute("value", toString(mCursorVisible));
	settings.push_back(CursorVisible);

	XmlTree IsOSCSender("IsOSCSender", "");
	IsOSCSender.setAttribute("value", toString(mIsOSCSender));
	settings.push_back(IsOSCSender);

	XmlTree AreWebSocketsEnabledAtStartup("AreWebSocketsEnabledAtStartup", "");
	AreWebSocketsEnabledAtStartup.setAttribute("value", toString(mAreWebSocketsEnabledAtStartup));
	settings.push_back(AreWebSocketsEnabledAtStartup);

	XmlTree IsRouter("IsRouter", "");
	IsRouter.setAttribute("value", toString(mIsRouter));
	settings.push_back(IsRouter);

	XmlTree IsWebSocketsServer("IsWebSocketsServer", "");
	IsWebSocketsServer.setAttribute("value", toString(mIsWebSocketsServer));
	settings.push_back(IsWebSocketsServer);

	XmlTree WebSocketsProtocol("WebSocketsProtocol", "");
	WebSocketsProtocol.setAttribute("value", mWebSocketsProtocol);
	settings.push_back(WebSocketsProtocol);

	XmlTree WebSocketsHost("WebSocketsHost", "");
	WebSocketsHost.setAttribute("value", mWebSocketsHost);
	settings.push_back(WebSocketsHost);

	XmlTree WebSocketsPort("WebSocketsPort", "");
	WebSocketsPort.setAttribute("value", toString(mWebSocketsPort));
	settings.push_back(WebSocketsPort);

	XmlTree FftSize("FftSize", "");
	FftSize.setAttribute("value", toString(mFftSize));
	settings.push_back(FftSize);

	XmlTree WindowSize("WindowSize", "");
	WindowSize.setAttribute("value", toString(mWindowSize));
	settings.push_back(WindowSize);

	XmlTree Info("Info", "");
	Info.setAttribute("value", toString(mInfo));
	settings.push_back(Info);

	XmlTree AssetsPath("AssetsPath", "");
	AssetsPath.setAttribute("value", toString(mAssetsPath));
	settings.push_back(AssetsPath);

	XmlTree UseLineIn("UseLineIn", "");
	UseLineIn.setAttribute("value", mUseLineIn);
	settings.push_back(UseLineIn);

	XmlTree SplitWarpH("SplitWarpH", "");
	SplitWarpH.setAttribute("value", toString(mSplitWarpH));
	settings.push_back(SplitWarpH);

	XmlTree SplitWarpV("SplitWarpV", "");
	SplitWarpV.setAttribute("value", toString(mSplitWarpV));
	settings.push_back(SplitWarpV);

	// write XML file
	settings.write(writeFile(path));

	return true;
}

bool VDSettings::restore()
{
	// check to see if Settings.xml file exists
	fs::path params = getAppPath() / settingsFileName;
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");

			if (settings.hasChild("Standalone")) {
				XmlTree Standalone = settings.getChild("Standalone");
				mStandalone = Standalone.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("AutoLayout")) {
				XmlTree AutoLayout = settings.getChild("AutoLayout");
				mAutoLayout = AutoLayout.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("AspectRatio")) {
				XmlTree AspectRatio = settings.getChild("AspectRatio");
				mAspectRatio = AspectRatio.getAttributeValue<float>("value");
			}
			if (settings.hasChild("FboWidth")) {
				XmlTree FboWidth = settings.getChild("FboWidth");
				mFboWidth = FboWidth.getAttributeValue<int>("value");
			}
			if (settings.hasChild("FboHeight")) {
				XmlTree FboHeight = settings.getChild("FboHeight");
				mFboHeight = FboHeight.getAttributeValue<int>("value");
			}
			if (settings.hasChild("MIDIEnabled")) {
				XmlTree MIDIEnabled = settings.getChild("MIDIEnabled");
				mMIDIEnabled = MIDIEnabled.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCEnabled")) {
				XmlTree OSCEnabled = settings.getChild("OSCEnabled");
				mOSCEnabled = OSCEnabled.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("IsOSCSender")) {
				XmlTree IsOSCSender = settings.getChild("IsOSCSender");
				mIsOSCSender = IsOSCSender.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCDestinationPort")) {
				XmlTree OSCDestinationPort = settings.getChild("OSCDestinationPort");
				mOSCDestinationPort = OSCDestinationPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost")) {
				XmlTree OSCDestinationHost = settings.getChild("OSCDestinationHost");
				mOSCDestinationHost = OSCDestinationHost.getAttributeValue<string>("value");
			}
			if (settings.hasChild("OSCDestinationPort2")) {
				XmlTree OSCDestinationPort2 = settings.getChild("OSCDestinationPort2");
				mOSCDestinationPort2 = OSCDestinationPort2.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost2")) {
				XmlTree OSCDestinationHost2 = settings.getChild("OSCDestinationHost2");
				mOSCDestinationHost2 = OSCDestinationHost2.getAttributeValue<string>("value");
			}
			if (settings.hasChild("MIDIOpenAllInputPorts")) {
				XmlTree MIDIOpenAllInputPorts = settings.getChild("MIDIOpenAllInputPorts");
				mMIDIOpenAllInputPorts = MIDIOpenAllInputPorts.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("ShowUI")) {
				XmlTree ShowUI = settings.getChild("ShowUI");
				mShowUI = ShowUI.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("CursorVisible")) {
				XmlTree CursorVisible = settings.getChild("CursorVisible");
				mCursorVisible = CursorVisible.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("AreWebSocketsEnabledAtStartup")) {
				XmlTree AreWebSocketsEnabledAtStartup = settings.getChild("AreWebSocketsEnabledAtStartup");
				mAreWebSocketsEnabledAtStartup = AreWebSocketsEnabledAtStartup.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsRouter")) {
				XmlTree IsRouter = settings.getChild("IsRouter");
				mIsRouter = IsRouter.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsWebSocketsServer")) {
				XmlTree IsWebSocketsServer = settings.getChild("IsWebSocketsServer");
				mIsWebSocketsServer = IsWebSocketsServer.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("WebSocketsProtocol")) {
				XmlTree WebSocketsProtocol = settings.getChild("WebSocketsProtocol");
				mWebSocketsProtocol = WebSocketsProtocol.getAttributeValue<string>("value");
			}
			if (settings.hasChild("WebSocketsHost")) {
				XmlTree WebSocketsHost = settings.getChild("WebSocketsHost");
				mWebSocketsHost = WebSocketsHost.getAttributeValue<string>("value");
			}
			if (settings.hasChild("WebSocketsPort")) {
				XmlTree WebSocketsPort = settings.getChild("WebSocketsPort");
				mWebSocketsPort = WebSocketsPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("FftSize")) {
				XmlTree FftSize = settings.getChild("FftSize");
				mFftSize = FftSize.getAttributeValue<int>("value");
			}
			if (settings.hasChild("WindowSize")) {
				XmlTree WindowSize = settings.getChild("WindowSize");
				mWindowSize = WindowSize.getAttributeValue<int>("value");
			}
			if (settings.hasChild("Info")) {
				XmlTree Info = settings.getChild("Info");
				mInfo = Info.getAttributeValue<string>("value");
			}
			if (settings.hasChild("AssetsPath")) {
				XmlTree AssetsPath = settings.getChild("AssetsPath");
				mAssetsPath = AssetsPath.getAttributeValue<string>("value");
				fs::path mPath = getAssetPath("") / mAssetsPath;
				if (!fs::exists(mPath)) {
					// reset path
					mAssetsPath = "";
				}
			}
			if (settings.hasChild("UseLineIn")) {
				XmlTree UseLineIn = settings.getChild("UseLineIn");
				mUseLineIn = UseLineIn.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("SplitWarpH")) {
				XmlTree SplitWarpH = settings.getChild("SplitWarpH");
				mSplitWarpH = SplitWarpH.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("SplitWarpV")) {
				XmlTree SplitWarpV = settings.getChild("SplitWarpV");
				mSplitWarpV = SplitWarpV.getAttributeValue<bool>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (mAutoLayout)
			{
				// init, overriden by GetWindowsResolution
				mMainWindowWidth = 1024;
				mMainWindowHeight = 768;
				mRenderWidth = 1024;
				mRenderHeight = 768;
				mRenderX = 1024;
				mRenderY = 0;				
			}
			else
			{
				if (settings.hasChild("MainWindowWidth")) {
					XmlTree MainWindowWidth = settings.getChild("MainWindowWidth");
					mMainWindowWidth = MainWindowWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowHeight")) {
					XmlTree MainWindowHeight = settings.getChild("MainWindowHeight");
					mMainWindowHeight = MainWindowHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowX")) {
					XmlTree MainWindowX = settings.getChild("MainWindowX");
					mMainWindowX = MainWindowX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowY")) {
					XmlTree MainWindowY = settings.getChild("MainWindowY");
					mMainWindowY = MainWindowY.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderWidth")) {
					XmlTree RenderWidth = settings.getChild("RenderWidth");
					mRenderWidth = RenderWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderHeight")) {
					XmlTree RenderHeight = settings.getChild("RenderHeight");
					mRenderHeight = RenderHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderX")) {
					XmlTree RenderX = settings.getChild("RenderX");
					mRenderX = RenderX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderY")) {
					XmlTree RenderY = settings.getChild("RenderY");
					mRenderY = RenderY.getAttributeValue<int>("value");
				}

			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void VDSettings::resetSomeParams() {
	mCamPosXY = vec2(0.0f);
	mCount = 1;

	// EyePointZ
	defaultEyePointZ = -400.f;
	minEyePointZ = -800.0;
	maxEyePointZ = 0.0;
	tEyePointZ = autoEyePointZ = false;
	// unicorns
	//iBadTv = 0.0f;
	iAlpha = 1.0f;
	iParam1 = iParam2 = 1.0f;
	iSpeedMultiplier = 1.0f;

	iGreyScale = false;
	iFade = mSendToOutput = iLight = iLightAuto = iRepeat = iXorY = false;

	// transition
	iTransition = 0;
	iAnim = 0.0;
	mTransitionDuration = 2.0f;
	mTransitionTime = 1.0f;

	iZoomLeft = iZoomRight = 1.0f;
	autoInvert = false;
	// imgui
	uiXPos = uiMargin = 3;
	uiElementWidth = mPreviewFboWidth + uiMargin;
	uiElementHeight = mPreviewFboHeight * 2.3;
	// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15 mPreviewWidth = 160;mPreviewHeight = 120;
	uiPreviewH = mPreviewHeight + uiMargin;
	uiLargeW = (mPreviewFboWidth + uiMargin) * 3;
	uiLargeH = (mPreviewFboHeight + uiMargin) * 7;
	uiLargePreviewW = mPreviewFboWidth * 1.6;
	uiLargePreviewH = mPreviewHeight * 2.2;
	uiXPosCol1 = uiLargeW + uiMargin * 2;
	uiXPosCol2 = uiXPosCol1 + uiMargin + uiElementWidth; //largeW + mVDSettings->uiMargin * 2;
	uiXPosCol3 = uiXPosCol2 + uiMargin;
	//uiYPos;
	uiYPosRow1 = 18;
	uiYPosRow2 = 160;
	uiYPosRow3 = uiYPosRow2 + uiLargeH + uiMargin * 1.5;
	shaderEditIndex = 0;
}

void VDSettings::reset()
{
	// parameters exposed in XML
	mMIDIOpenAllInputPorts = mAutoLayout = mShowUI = mCursorVisible = true;
	mRenderThumbs = mStandalone = mSplitWarpH = mSplitWarpV = false;
	mOutputVideoResolution = 1024;
	mInfo = "";
	mTrackName = "";
	iTrack = 0;
	mMainWindowX = mRenderX = 0;
	mMainWindowY = mRenderY = 0;
	mMainWindowWidth = 1280;
	mMainWindowHeight = 720;
	mRenderWidth = 1024;
	mRenderHeight = 768;
	// parameters not exposed in XML
	mRenderXY = mLeftRenderXY = mRightRenderXY = mPreviewRenderXY = mWarp1RenderXY = mWarp2RenderXY = vec2(0.0f);
	mRenderPosXY = vec2(0.0, 320.0);
	//mRenderResoXY = vec2(mRenderWidth, mRenderHeight);
	//mRenderResolution = ivec2(mRenderWidth, mRenderHeight);
	mPreviewFragXY = vec2(0.0, 0.0);
	mAspectRatio = 0.75; // ratio 4:3 (0.75) 16:9 (0.5625)
	mFboWidth = 640;
	mFboHeight = mFboWidth * mAspectRatio;
	mPreviewFboWidth = 70;
	mPreviewFboHeight = mPreviewFboWidth* mAspectRatio;
	mPreviewWidth = 120;
	mPreviewHeight = mPreviewWidth * mAspectRatio;
	mRenderCodeEditorXY.x = 0;
	mRenderCodeEditorXY.y = 0;
	mCodeEditorWidth = 800;
	mCodeEditorHeight = 600;

	mWindowToCreate = NONE;

	mMode = mPreviousMode = mNewMode = 0; // Mix mode by default
	mCurrentFilePath = "currentMix.frag";
	mAssetsPath = "";
	mMarginSmall = 2;
	mPreviewEnabled = true;
	//audio
	mIsPlaying = false;
	iBeat = 0;
	iSeed = 0.1;
	mFftSize = 512;
	mWindowSize = 1024;
	liveMeter = 0.0f;

	// shader uniforms
	for (int i = 0; i < 4; i++)
	{
		iChannelTime[i] = i;
	}
#ifdef _DEBUG
	iDebug = true;
#else
	iDebug = false;
#endif  // _DEBUG
	sFps = "60";
	iShowFps = true;

	multFactor = 126.0;
	currentSelectedIndex = 0;
	selectedWarp = 0;

	for (int a = 0; a < MAX; a++)
	{
		iChannels[a] = a;
	}
	iChannels[4] = 10;//spout
	selectedChannel = 0;
	// fbo indexes for warp (should be constants)
	mFboResolution = 2048;
	mMixFboIndex = 0;
	mLeftFboIndex = 1;
	mRightFboIndex = 2;
	mWarp1FboIndex = 3;
	mWarp2FboIndex = 4;
	mCurrentPreviewFboIndex = 5;
	mABPFboIndex = 6;
	mLiveFboIndex = 7;
	mSphereFboIndex = 8;
	mMeshFboIndex = 9;
	mAudioFboIndex = 10;
	mVertexSphereFboIndex = 11;

	mPreviewFragIndex = 0;
	mPreviousFragIndex = 1;
	mLeftFragIndex = 0;
	mRightFragIndex = 1;
	mWarp1FragIndex = 2;
	mWarp2FragIndex = 3;
	mLiveFragIndex = 7;
	mWarpCount = 3;
	FPSColor = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorGreen = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorRed = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	ColorYellow = ColorA(1.0f, 1.0f, 0.0f, 1.0f);
	ColorPurple = ColorA(0.5f, 0.0f, 1.0f, 1.0f);
	isUIDirty = true;
	mLiveCode = false;

	mStateTransition = 1.0;

	mOptimizeUI = false;
	// spout
	mOutputResolution = vec2(640, 480);
	// meshes
	mMeshIndex = 0;
	// vertex sphere
	mVertexSphereTextureIndex = 1;

	// initialize our camera
	mCamEyePointXY = vec2(0.f, 0.f);
	mCamEyePointZ = -400.f;
	mCamera.setEyePoint(vec3(mCamEyePointXY.x, mCamEyePointXY.y, mCamEyePointZ));
	//mCamera.setCenterOfInterestPoint(vec3(0.f, 0.f, 0.f));

	mUIZoom = 1;

	// midi
	mMIDIEnabled = true;

	// OSC
	mOSCEnabled = false;
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 7001;
	mOSCDestinationHost2 = "127.0.0.1";
	mOSCDestinationPort2 = 7002;
	mOSCReceiverPort = 7000;
	mOSCMsg = "";
	mOSCNewMsg = false;
	mMsg = "";
	mWebSocketsMsg = "";
	mNewMsg = false;
	mWebSocketsNewMsg = false;
	mMidiMsg = "";
	InfoMsg = "";
	mIsOSCSender = false;
	xFade = 1.0f;
	xFadeChanged = false;
	// web sockets
	mIsRouter = false;
	mAreWebSocketsEnabledAtStartup = true;
	mIsWebSocketsServer = false;
	mWebSocketsProtocol = "ws://";
	mWebSocketsHost = "localhost";
	mWebSocketsPort = 8088;
	// Blendmode 
	iBlendmode = 0;
	// abp
	mBend = 1.0f;

	resetSomeParams();
}
