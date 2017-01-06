//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace VideoDromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	CI_LOG_V("VDSession ctor");
	mVDSettings = aVDSettings;
	// allow log to file
	mVDLog = VDLog::create();	
	// Utils
	mVDUtils = VDUtils::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings);
	mVDAnimation->tapTempo();

	// init fbo format
	//fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	//fmt.setBorderColor(Color::black());		
	// uncomment this to enable 4x antialiasing	
	//fboFmt.setSamples( 4 ); 
	fboFmt.setColorTextureFormat(fmt);
	mPosX = mPosY = 0.0f;
	mZoom = 1.0f;

	// Mix
	mVDMix = VDMix::create(mVDSettings, mVDAnimation);
	// Websocket
	mVDWebsocket = VDWebsocket::create(mVDSettings, mVDAnimation, mVDMix);
	// Message router
	mVDRouter = VDRouter::create(mVDSettings, mVDAnimation, mVDWebsocket);
	// warping
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// reset no matter what, so we don't miss anything
	reset();

	// check to see if VDSession.xml file exists and restore if it does
	sessionPath = getAssetPath("") / mVDSettings->mAssetsPath / sessionFileName;
	if (fs::exists(sessionPath))
	{
		restore();
	}
	else
	{
		// Create json file if it doesn't already exist.
		std::ofstream oStream(sessionPath.string());
		oStream.close();
		save();
	}
	// check if something went wrong, if so, create a default warp
	if (mVDMix->getWarpCount() == 0) {
		// init for received shaders from websockets for warp 0
		mVDMix->createWarp("default", 1, 1, 2, 2, 1.0f);
	}
	// check if something went wrong, if so, create a default warp
	if (mVDMix->getTriangleCount() == 0) {
		// init for received shaders from websockets for warp 0
		mVDMix->createTriangle("default", 1, 1, 2, 2, 1.0f);
	}
}

VDSessionRef VDSession::create(VDSettingsRef aVDSettings)
{
	return shared_ptr<VDSession>(new VDSession(aVDSettings));
}

void VDSession::readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const DataSourceRef &source) {
	XmlTree			doc;

	CI_LOG_V("VDSession readSettings");
	// try to load the specified xml file
	try {
		doc = XmlTree(source);
		CI_LOG_V("VDSession xml doc ok");
	}
	catch (...) {
		CI_LOG_V("VDSession xml doc error");
	}

	// check if this is a valid file 
	bool isOK = doc.hasChild("fbos");
	if (!isOK) return;

	//
	if (isOK) {
		XmlTree mixXml = doc.getChild("fbos");
		fromXml(mixXml);
	}
}
void VDSession::fromXml(const XmlTree &xml) {

	// find fbo childs in xml
	if (xml.hasChild("fbo")) {
		CI_LOG_V("VDSession got fbo childs");
		for (XmlTree::ConstIter fboChild = xml.begin("fbo"); fboChild != xml.end(); ++fboChild) {
			CI_LOG_V("VDSession create fbo ");
			mVDMix->createShaderFbo(fboChild->getAttributeValue<string>("shadername", ""), 0);
		}
	}
}
// control values
float VDSession::getControlValue(unsigned int aCtrl) {
	return mVDAnimation->getFloatUniformValueByIndex(aCtrl);
}
float VDSession::getControlValueByName(string aCtrlName) {
	return mVDAnimation->getFloatUniformValueByName(aCtrlName);
}
void VDSession::setControlValue(unsigned int aCtrl, float aValue) {
	// done in router mVDAnimation->changeFloatValue(aCtrl, aValue);
	mVDWebsocket->changeFloatValue(aCtrl, aValue);
}
bool VDSession::getBoolValue(unsigned int aCtrl) {
	return mVDAnimation->getBoolUniformValueByIndex(aCtrl);
}
void VDSession::toggleValue(unsigned int aCtrl) {
	mVDWebsocket->toggleValue(aCtrl);
}
void VDSession::toggleAuto(unsigned int aCtrl) {
	mVDWebsocket->toggleAuto(aCtrl);
}
void VDSession::toggleTempo(unsigned int aCtrl) {
	mVDWebsocket->toggleTempo(aCtrl);
}
void VDSession::resetAutoAnimation(unsigned int aIndex) {
	mVDWebsocket->resetAutoAnimation(aIndex);
}
float VDSession::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDAnimation->getMinUniformValueByIndex(aIndex);
}
float VDSession::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDAnimation->getMaxUniformValueByIndex(aIndex);
}

void VDSession::resize() {
	mVDMix->resize();
}
void VDSession::update() {
	// fps calculated in main app
	mVDSettings->sFps = toString(floor(getControlValue(mVDSettings->IFPS)));
	if (mVDWebsocket->hasReceivedStream() && (getElapsedFrames() % 100 == 0)) {
		updateStream(mVDWebsocket->getBase64Image());
	}
	if (mVDWebsocket->hasReceivedShader()) {
		if (mVDMix->getWarpCrossfade(0) < 0.5) {
			setFragmentShaderString(2, mVDWebsocket->getReceivedShader());
			mVDMix->crossfadeWarp(0, 1.0f);
		}
		else {
			setFragmentShaderString(1, mVDWebsocket->getReceivedShader());
			mVDMix->crossfadeWarp(0, 0.0f);
		}
	}
	if (mVDSettings->iGreyScale)
	{
		mVDWebsocket->changeFloatValue(1, mVDAnimation->getFloatUniformValueByIndex(3));
		mVDWebsocket->changeFloatValue(2, mVDAnimation->getFloatUniformValueByIndex(3));
		mVDWebsocket->changeFloatValue(5, mVDAnimation->getFloatUniformValueByIndex(7));
		mVDWebsocket->changeFloatValue(6, mVDAnimation->getFloatUniformValueByIndex(7));
	}
	mVDAnimation->update();
	mVDRouter->update();
	mVDWebsocket->update();
	mVDMix->update();
}
bool VDSession::save()
{
	// save warp settings
	mVDMix->save();
	// save uniforms settings
	mVDAnimation->save();
	// save in sessionPath
	JsonTree doc;

	JsonTree settings = JsonTree::makeArray("settings");
	settings.addChild(ci::JsonTree("bpm", mOriginalBpm));
	settings.addChild(ci::JsonTree("beatsperbar", mVDAnimation->iBeatsPerBar));
	settings.addChild(ci::JsonTree("fadeindelay", mFadeInDelay));
	settings.addChild(ci::JsonTree("fadeoutdelay", mFadeOutDelay));
	settings.addChild(ci::JsonTree("endframe", mVDAnimation->mEndFrame));
	doc.pushBack(settings);

	JsonTree assets = JsonTree::makeArray("assets");
	if (mWaveFileName.length() > 0) assets.addChild(ci::JsonTree("wavefile", mWaveFileName));
	assets.addChild(ci::JsonTree("waveplaybackdelay", mWavePlaybackDelay));
	if (mMovieFileName.length() > 0) assets.addChild(ci::JsonTree("moviefile", mMovieFileName));
	assets.addChild(ci::JsonTree("movieplaybackdelay", mMoviePlaybackDelay));
	if (mImageSequencePath.length() > 0) assets.addChild(ci::JsonTree("imagesequencepath", mImageSequencePath));
	if (mText.length() > 0) {
		assets.addChild(ci::JsonTree("text", mText));
		assets.addChild(ci::JsonTree("textplaybackdelay", mTextPlaybackDelay));
		assets.addChild(ci::JsonTree("textplaybackend", mTextPlaybackEnd));
	}
	doc.pushBack(assets);
	// warps
	JsonTree warps = JsonTree::makeArray("warps");
	warps.addChild(ci::JsonTree("size", mVDMix->getWarpCount()));
	doc.pushBack(warps);
	for (unsigned int w = 0; w < mVDMix->getWarpCount(); w++) {
		JsonTree jsonWarp = JsonTree::makeArray("warp" + toString(w));
		jsonWarp.addChild(ci::JsonTree("name", mVDMix->getWarpName(w)));
		jsonWarp.addChild(ci::JsonTree("afboindex", mVDMix->getWarpAFboIndex(w)));
		jsonWarp.addChild(ci::JsonTree("bfboindex", mVDMix->getWarpBFboIndex(w)));
		jsonWarp.addChild(ci::JsonTree("ashaderindex", mVDMix->getWarpAShaderIndex(w)));
		jsonWarp.addChild(ci::JsonTree("bshaderindex", mVDMix->getWarpBShaderIndex(w)));
		jsonWarp.addChild(ci::JsonTree("xfade", mVDMix->getWarpCrossfade(w)));
		doc.pushBack(jsonWarp);
	}

	doc.write(writeFile(sessionPath), JsonTree::WriteOptions());

	return true;
}

void VDSession::restore()
{
	// save load settings
	mVDMix->load();

	// check to see if json file exists
	if (!fs::exists(sessionPath)) {
		return;
	}

	try {
		JsonTree doc(loadFile(sessionPath));
		if (doc.hasChild("settings")) {
			JsonTree settings(doc.getChild("settings"));
			if (settings.hasChild("bpm")) { mOriginalBpm = settings.getValueForKey<float>("bpm", 166.0f); mVDAnimation->setBpm(mOriginalBpm); };
			if (settings.hasChild("beatsperbar")) mVDAnimation->iBeatsPerBar = settings.getValueForKey<int>("beatsperbar");
			if (mVDAnimation->iBeatsPerBar < 1) mVDAnimation->iBeatsPerBar = 1;
			if (settings.hasChild("fadeindelay")) mFadeInDelay = settings.getValueForKey<int>("fadeindelay");
			if (settings.hasChild("fadeoutdelay")) mFadeOutDelay = settings.getValueForKey<int>("fadeoutdelay");
			if (settings.hasChild("endframe")) mVDAnimation->mEndFrame = settings.getValueForKey<int>("endframe");			
			mTargetFps = mVDAnimation->getBpm() / 60.0f * mFpb;
		}

		if (doc.hasChild("assets")) {
			JsonTree assets(doc.getChild("assets"));
			if (assets.hasChild("wavefile")) mWaveFileName = assets.getValueForKey<string>("wavefile");
			if (assets.hasChild("waveplaybackdelay")) mWavePlaybackDelay = assets.getValueForKey<int>("waveplaybackdelay");
			if (assets.hasChild("moviefile")) mMovieFileName = assets.getValueForKey<string>("moviefile");
			if (assets.hasChild("movieplaybackdelay")) mMoviePlaybackDelay = assets.getValueForKey<int>("movieplaybackdelay");
			if (assets.hasChild("imagesequencepath")) mImageSequencePath = assets.getValueForKey<string>("imagesequencepath");
			if (assets.hasChild("text")) mText = assets.getValueForKey<string>("text");
			if (assets.hasChild("textplaybackdelay")) mTextPlaybackDelay = assets.getValueForKey<int>("textplaybackdelay");
			if (assets.hasChild("textplaybackend")) mTextPlaybackEnd = assets.getValueForKey<int>("textplaybackend");
		}

		// warps TODO in warp.cpp
		/*	if (doc.hasChild("warps")) {
				unsigned int warpsize = 0;
				JsonTree warps(doc.getChild("warps"));
				if (warps.hasChild("size")) warpsize = warps.getValueForKey<int>("size");
				for (unsigned int w = 0; w < warpsize; w++) {
				JsonTree jsonWarp(doc.getChild("warp" + toString(w)));
				string wName = (jsonWarp.hasChild("name")) ? jsonWarp.getValueForKey<string>("name") : "no name";
				unsigned int afboindex = (jsonWarp.hasChild("afboindex")) ? jsonWarp.getValueForKey<int>("afboindex") : 1;
				unsigned int bfboindex = (jsonWarp.hasChild("bfboindex")) ? jsonWarp.getValueForKey<int>("bfboindex") : 1;
				unsigned int aShaderIndex = (jsonWarp.hasChild("ashaderindex")) ? jsonWarp.getValueForKey<int>("ashaderindex") : 1;
				unsigned int bShaderIndex = (jsonWarp.hasChild("bshaderindex")) ? jsonWarp.getValueForKey<int>("bshaderindex") : 1;
				float xfade = (jsonWarp.hasChild("xfade")) ? jsonWarp.getValueForKey<float>("xfade") : 1.0f;
				mVDMix->createWarp(wName, afboindex, aShaderIndex, bfboindex, bShaderIndex, xfade);
				}

				}*/
	}
	catch (const JsonTree::ExcJsonParserError& exc) {
		CI_LOG_W(exc.what());
	}
}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
	mFpb = 16;
	mVDAnimation->setBpm(mOriginalBpm);
	mTargetFps = mOriginalBpm / 60.0f * mFpb;
}

void VDSession::reset()
{
	// parameters exposed in json file
	mFlipV = false;
	mFlipH = false;
	mOriginalBpm = 166;
	mVDAnimation->iBeatsPerBar = 1;
	mWaveFileName = "";
	mWavePlaybackDelay = 10;
	mMovieFileName = "";
	mImageSequencePath = "";
	mMoviePlaybackDelay = 10;
	mFadeInDelay = 5;
	mFadeOutDelay = 1;
	mVDAnimation->mEndFrame = 20000000;
	mText = "";
	mTextPlaybackDelay = 10;
	mTextPlaybackEnd = 2020000;

	resetSomeParams();
}
int VDSession::getWindowsResolution() {
	return mVDUtils->getWindowsResolution();
}
/*void VDSession::setCrossfade(float aCrossfade) {
	mVDRouter->changeFloatValue(21, aCrossfade);
	}*/
void VDSession::blendRenderEnable(bool render) {
	mVDAnimation->blendRenderEnable(render);
}

void VDSession::fileDrop(FileDropEvent event) {
	string ext = "";

	unsigned int index = (int)(event.getX() / (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin));
	int y = (int)(event.getY());
	if (index < 2 || y < mVDSettings->uiYPosRow3 || y > mVDSettings->uiYPosRow3 + mVDSettings->uiPreviewH) index = 0;
	ci::fs::path mPath = event.getFile(event.getNumFiles() - 1);
	string absolutePath = mPath.string();
	// use the last of the dropped files
	int dotIndex = absolutePath.find_last_of(".");
	int slashIndex = absolutePath.find_last_of("\\");

	if (dotIndex != std::string::npos && dotIndex > slashIndex) ext = absolutePath.substr(absolutePath.find_last_of(".") + 1);

	if (ext == "wav" || ext == "mp3") {
		loadAudioFile(absolutePath);
	}
	else if (ext == "png" || ext == "jpg") {
		if (index < 1) index = 1;
		if (index > 3) index = 3;
		loadImageFile(absolutePath, index);
	}
	else if (ext == "glsl" || ext == "frag") {
		loadFragmentShader(absolutePath);
	}
	else if (ext == "xml") {
	}
	else if (ext == "mov") {
		loadMovie(absolutePath, index);
	}
	else if (ext == "txt") {
	}
	else if (ext == "") {
		// try loading image sequence from dir
		if (!loadImageSequence(absolutePath, index)) {
			// try to load a folder of shaders
			loadShaderFolder(absolutePath);
		}
	}
}
#pragma region events
bool VDSession::handleMouseMove(MouseEvent &event)
{
	bool handled = true;
	mVDAnimation->changeVec4Value(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	// pass this mouse event to the warp editor first
	if (!mVDMix->handleMouseMove(event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDown(MouseEvent &event)
{
	bool handled = true;
	mVDAnimation->changeVec4Value(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	// pass this mouse event to the warp editor first
	if (!mVDMix->handleMouseDown(event)) {
		// let your application perform its mouseDown handling here
		mVDWebsocket->changeFloatValue(21, event.getX() / getWindowWidth());
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDrag(MouseEvent &event)
{
	bool handled = true;
	mVDAnimation->changeVec4Value(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	// pass this mouse event to the warp editor first
	if (!mVDMix->handleMouseDrag(event)) {
		// let your application perform its mouseDrag handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseUp(MouseEvent &event)
{
	bool handled = true;
	mVDAnimation->changeVec4Value(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	// pass this mouse event to the warp editor first
	if (!mVDMix->handleMouseUp(event)) {
		// let your application perform its mouseUp handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleKeyDown(KeyEvent &event)
{
	bool handled = true;
	float newValue;

	// pass this key event to the warp editor first
	if (!mVDMix->handleKeyDown(event)) {
		// pass this event to Mix handler
		if (!mVDAnimation->handleKeyDown(event)) {
			switch (event.getCode()) {
			case KeyEvent::KEY_w:
				// toggle warp edit mode
				Warp::enableEditMode(!Warp::isEditModeEnabled());
				break;
			case KeyEvent::KEY_n:
				createWarpMix();
				// TODO? Warp::handleResize(mWarps);
				break;
			case KeyEvent::KEY_SPACE:
				//mVDTextures->playMovie();
				//mVDAnimation->currentScene++;
				//if (mMovie) { if (mMovie->isPlaying()) mMovie->stop(); else mMovie->play(); }
				break;
			case KeyEvent::KEY_0:
				break;
			case KeyEvent::KEY_l:
				mVDAnimation->load();
				//mLoopVideo = !mLoopVideo;
				//if (mMovie) mMovie->setLoop(mLoopVideo);
				break;
			case KeyEvent::KEY_x:
				// trixels
				mVDWebsocket->changeFloatValue(16, mVDAnimation->getFloatUniformValueByIndex(16) + 0.05f);
				break;
			case KeyEvent::KEY_r:
				newValue = mVDAnimation->getFloatUniformValueByIndex(1) + 0.1f;
				if (newValue > 1.0f) newValue = 0.0f;
				mVDWebsocket->changeFloatValue(1, newValue);
				break;
			case KeyEvent::KEY_g:
				newValue = mVDAnimation->getFloatUniformValueByIndex(2) + 0.1f;
				if (newValue > 1.0f) newValue = 0.0f;
				mVDWebsocket->changeFloatValue(2, newValue);
				break;
			case KeyEvent::KEY_b:
				newValue = mVDAnimation->getFloatUniformValueByIndex(3) + 0.1f;
				if (newValue > 1.0f) newValue = 0.0f;
				mVDWebsocket->changeFloatValue(3, newValue);
				break;
			case KeyEvent::KEY_e:
				newValue = mVDAnimation->getFloatUniformValueByIndex(1) - 0.1f;
				if (newValue < 0.0f) newValue = 1.0;
				mVDWebsocket->changeFloatValue(1, newValue);
				break;
			case KeyEvent::KEY_f:
				newValue = mVDAnimation->getFloatUniformValueByIndex(2) - 0.1f;
				if (newValue < 0.0f) newValue = 1.0;
				mVDWebsocket->changeFloatValue(2, newValue);
				break;
			case KeyEvent::KEY_v:
				newValue = mVDAnimation->getFloatUniformValueByIndex(3) - 0.1f;
				if (newValue < 0.0f) newValue = 1.0;
				mVDWebsocket->changeFloatValue(3, newValue);
				break;
			case KeyEvent::KEY_c:
				// chromatic
				mVDWebsocket->changeFloatValue(10, mVDAnimation->getFloatUniformValueByIndex(10) + 0.05f);
				break;
			case KeyEvent::KEY_p:
				// pixelate
				mVDWebsocket->changeFloatValue(15, mVDAnimation->getFloatUniformValueByIndex(15) + 0.05f);
				break;
			case KeyEvent::KEY_t:
				// glitch
				mVDWebsocket->changeBoolValue(45, true);
				break;
			case KeyEvent::KEY_i:
				// invert
				mVDWebsocket->changeBoolValue(48, true);
				break;
			case KeyEvent::KEY_o:
				// toggle
				mVDWebsocket->toggleValue(46);
				break;
			case KeyEvent::KEY_z:
				// zoom
				mVDWebsocket->changeFloatValue(12, mVDAnimation->getFloatUniformValueByIndex(12) - 0.05f);
				break;
			case KeyEvent::KEY_LEFT:
				//mVDTextures->rewindMovie();				
				if (mVDAnimation->getFloatUniformValueByIndex(21) > 0.1f) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(21) - 0.1f);
				break;
			case KeyEvent::KEY_RIGHT:
				//mVDTextures->fastforwardMovie();
				if (mVDAnimation->getFloatUniformValueByIndex(21) < 1.0f) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(21) + 0.1f);
				break;
			case KeyEvent::KEY_PAGEDOWN:
				// crossfade right
				if (mVDAnimation->getFloatUniformValueByIndex(18) < 1.0f) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(18) + 0.1f);
				break;
			case KeyEvent::KEY_PAGEUP:
				// crossfade left
				if (mVDAnimation->getFloatUniformValueByIndex(18) > 0.0f) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(18) - 0.1f);
				break;

			default:
				handled = false;
				break;
			}
		}
	}
    event.setHandled(handled);
    return event.isHandled();
}
bool VDSession::handleKeyUp(KeyEvent &event) {
	bool handled = true;

	// pass this key event to the warp editor first
	if (!mVDMix->handleKeyUp(event)) {
		if (!mVDAnimation->handleKeyUp(event)) {
			// Animation did not handle the key, so handle it here
			switch (event.getCode()) {
			case KeyEvent::KEY_g:
				// glitch
				mVDWebsocket->changeBoolValue(45, false);
				break;
			case KeyEvent::KEY_t:
				// trixels
				mVDWebsocket->changeFloatValue(16, 0.0f);
				break;
			case KeyEvent::KEY_i:
				// invert
				mVDWebsocket->changeBoolValue(48, false);
				break;
			case KeyEvent::KEY_c:
				// chromatic
				mVDWebsocket->changeFloatValue(10, 0.0f);
				break;
			case KeyEvent::KEY_p:
				// pixelate
				mVDWebsocket->changeFloatValue(15, 1.0f);
				break;
			case KeyEvent::KEY_o:
				// toggle
				mVDWebsocket->changeBoolValue(46, false);
				break;
			case KeyEvent::KEY_z:
				// zoom
				mVDWebsocket->changeFloatValue(12, 1.0f);
				break;
			default:
				handled = false;
				break;
			}
		}
	}
    event.setHandled(handled);
    return event.isHandled();
}
#pragma endregion events

#pragma region warps

void VDSession::createWarpMix() {
	mVDMix->createWarp();
}
unsigned int VDSession::getWarpAShaderIndex(unsigned int aWarpIndex) {
	return mVDMix->getWarpAShaderIndex(aWarpIndex);
}
unsigned int VDSession::getWarpBShaderIndex(unsigned int aWarpIndex) {
	return mVDMix->getWarpBShaderIndex(aWarpIndex);
}

bool VDSession::isWarpTriangle() {
	return mVDMix->isWarpTriangle();
}
void VDSession::toggleWarpTriangle() {
	mVDMix->toggleWarpTriangle();
}
void VDSession::setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex) {
	mVDMix->setWarpAShaderIndex(aWarpIndex, aWarpShaderIndex);
	mVDWebsocket->changeShaderIndex(aWarpIndex, aWarpShaderIndex, 0);
}
void VDSession::setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex) {
	mVDMix->setWarpBShaderIndex(aWarpIndex, aWarpShaderIndex);
	mVDWebsocket->changeShaderIndex(aWarpIndex, aWarpShaderIndex, 1);
}
void VDSession::setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
	mVDMix->setWarpAFboIndex(aWarpIndex, aWarpFboIndex);
	mVDWebsocket->changeWarpFboIndex(aWarpIndex, aWarpFboIndex, 0);
}
void VDSession::setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
	mVDMix->setWarpBFboIndex(aWarpIndex, aWarpFboIndex);
	mVDWebsocket->changeWarpFboIndex(aWarpIndex, aWarpFboIndex, 1);
}
void VDSession::updateWarpName(unsigned int aWarpIndex) {
	mVDMix->updateWarpName(aWarpIndex);
}
#pragma endregion warps
// triangles
#pragma region triangles
void VDSession::setTriangleAFboIndex(unsigned int aTriangleIndex, unsigned int aTriangleFboIndex) {
	mVDMix->setTriangleAFboIndex(aTriangleIndex, aTriangleFboIndex);
	// TODO mVDWebsocket->changeTriangleFboIndex(aTriangleIndex, aTriangleFboIndex, 0);
}
void VDSession::setTriangleBFboIndex(unsigned int aTriangleIndex, unsigned int aTriangleFboIndex) {
	mVDMix->setTriangleBFboIndex(aTriangleIndex, aTriangleFboIndex);
	// TODO mVDWebsocket->changeTriangleFboIndex(aTriangleIndex, aTriangleFboIndex, 1);
}

#pragma endregion triangles
// fbos
#pragma region fbos
bool VDSession::isFlipH() {
	return mVDAnimation->isFlipH();
}
bool VDSession::isFlipV() {
	return mVDAnimation->isFlipV();
}
void VDSession::flipH() {
	mVDAnimation->flipH();
}
void VDSession::flipV() {
	mVDAnimation->flipV();
}

ci::gl::TextureRef VDSession::getMixTexture(unsigned int aMixFboIndex) {
	return mVDMix->getMixTexture(aMixFboIndex);
}
int VDSession::loadFragmentShader(string aFilePath) {
	int rtn = -1;
	CI_LOG_V("loadFragmentShader " + aFilePath);
	mVDMix->createShaderFbo(aFilePath); 
	
	return rtn;
}

string VDSession::getMixFboName(unsigned int aMixFboIndex) {
	return mVDMix->getMixFboName(aMixFboIndex);
}

void VDSession::sendFragmentShader(unsigned int aShaderIndex) {
	mVDWebsocket->changeFragmentShader(mVDMix->getFragmentString(aShaderIndex));
}

unsigned int VDSession::getMixFbosCount() {
	return mVDMix->getMixFbosCount();
};

#pragma endregion fbos
// shaders
#pragma region shaders
bool VDSession::loadShaderFolder(string aFolder) {
	string ext = "";
	fs::path p(aFolder);
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName = it->path().filename().string();
			int dotIndex = fileName.find_last_of(".");

			if (dotIndex != std::string::npos)
			{
				ext = fileName.substr(dotIndex + 1);
				if (ext == "glsl")
				{
					loadFragmentShader(aFolder + "/" + fileName);
				}
			}
		}
	}
	return true;
}

#pragma endregion shaders

// websockets
#pragma region websockets

void VDSession::wsConnect() {
	mVDWebsocket->wsConnect();
}
void VDSession::wsPing() {
	mVDWebsocket->wsPing();
}
void VDSession::wsWrite(string msg)
{
	mVDWebsocket->wsWrite(msg);
}
#pragma endregion websockets