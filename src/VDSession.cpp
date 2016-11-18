//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace VideoDromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;
	// Utils
	mVDUtils = VDUtils::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings);
	mVDAnimation->tapTempo();
	// Message router
	mVDRouter = VDRouter::create(mVDSettings, mVDAnimation);
	// mix Ffbo to render
	warpMixToRender = 0;
	// init fbo format
	//fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	//fmt.setBorderColor(Color::black());		
	// uncomment this to enable 4x antialiasing	
	//fboFmt.setSamples( 4 ); 
	fboFmt.setColorTextureFormat(fmt);
	// render fbo
	mRenderFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);

	// Mix
	mVDMix = VDMix::create(mVDSettings, mVDAnimation, mVDRouter);
	// warping
	gl::enableDepthRead();
	gl::enableDepthWrite();
	// initialize warps
	mWarpSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
	//if (fs::exists(mWarpSettings)) {
	// load warp settings from file if one exists
	//mWarps = Warp::readSettings(loadFile(mWarpSettings)); // TODO load from json file
	//}
	//else {
	// otherwise create a warp from scratch
	createWarp();

	//Warp::setSize(mWarps, ivec2(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight));// create small new warps too
	Warp::setSize(mWarps, ivec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight)); // create small new warps 
	Warp::handleResize(mWarps);

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
}

VDSessionRef VDSession::create(VDSettingsRef aVDSettings)
{
	return shared_ptr<VDSession>(new VDSession(aVDSettings));
}
void VDSession::resize() {
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarps);
}
void VDSession::update() {
	mVDAnimation->update();
	mVDRouter->update();
	mVDMix->update();
}
bool VDSession::save()
{
	// save warp settings
	Warp::writeSettings(mWarps, writeFile(mWarpSettings));
	// save in sessionPath
	JsonTree doc;

	JsonTree settings = JsonTree::makeArray("settings");
	settings.addChild(ci::JsonTree("bpm", mOriginalBpm));
	settings.addChild(ci::JsonTree("beatsperbar", iBeatsPerBar));
	settings.addChild(ci::JsonTree("fadeindelay", mFadeInDelay));
	settings.addChild(ci::JsonTree("fadeoutdelay", mFadeOutDelay));
	settings.addChild(ci::JsonTree("endframe", mEndFrame));
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
	if (mShaderLeft.length() > 0) assets.addChild(ci::JsonTree("ShaderLeft", mShaderLeft));
	if (mShaderRight.length() > 0) assets.addChild(ci::JsonTree("ShaderRight", mShaderRight));
	doc.pushBack(assets);

	doc.write(writeFile(sessionPath), JsonTree::WriteOptions());

	return true;
}

void VDSession::restore()
{
	// check to see if json file exists
	if (!fs::exists(sessionPath)) {
		return;
	}

	try {
		JsonTree doc(loadFile(sessionPath));

		JsonTree settings(doc.getChild("settings"));
		if (settings.hasChild("bpm")) mBpm = mOriginalBpm = settings.getValueForKey<float>("bpm");
		if (settings.hasChild("beatsperbar")) iBeatsPerBar = settings.getValueForKey<int>("beatsperbar");
		if (iBeatsPerBar < 1) iBeatsPerBar = 1;
		if (settings.hasChild("fadeindelay")) mFadeInDelay = settings.getValueForKey<int>("fadeindelay");
		if (settings.hasChild("fadeoutdelay")) mFadeOutDelay = settings.getValueForKey<int>("fadeoutdelay");
		if (settings.hasChild("endframe")) mEndFrame = settings.getValueForKey<int>("endframe");
		mTargetFps = mBpm / 60.0f * mFpb;

		JsonTree assets(doc.getChild("assets"));
		if (assets.hasChild("wavefile")) mWaveFileName = assets.getValueForKey<string>("wavefile");
		if (assets.hasChild("waveplaybackdelay")) mWavePlaybackDelay = assets.getValueForKey<int>("waveplaybackdelay");
		if (assets.hasChild("moviefile")) mMovieFileName = assets.getValueForKey<string>("moviefile");
		if (assets.hasChild("movieplaybackdelay")) mMoviePlaybackDelay = assets.getValueForKey<int>("movieplaybackdelay");
		if (assets.hasChild("imagesequencepath")) mImageSequencePath = assets.getValueForKey<string>("imagesequencepath");
		if (assets.hasChild("text")) mText = assets.getValueForKey<string>("text");
		if (assets.hasChild("textplaybackdelay")) mTextPlaybackDelay = assets.getValueForKey<int>("textplaybackdelay");
		if (assets.hasChild("textplaybackend")) mTextPlaybackEnd = assets.getValueForKey<int>("textplaybackend");
		if (assets.hasChild("ShaderLeft")) mShaderLeft = assets.getValueForKey<string>("ShaderLeft");
		if (assets.hasChild("ShaderRight")) mShaderRight = assets.getValueForKey<string>("ShaderRight");
	}
	catch (const JsonTree::ExcJsonParserError& exc)  {
		CI_LOG_W(exc.what());
	}
}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
	mFpb = 16;
	mBpm = mOriginalBpm;
	mTargetFps = mBpm / 60.0f * mFpb;
}

void VDSession::reset()
{
	// parameters exposed in json file
	mFlipV = false;
	mFlipH = false;
	mBpm = mOriginalBpm = 166;
	iBeatsPerBar = 1;
	mWaveFileName = "";
	mWavePlaybackDelay = 10;
	mMovieFileName = "";
	mImageSequencePath = "";
	mMoviePlaybackDelay = 10;
	mFadeInDelay = 1;
	mFadeOutDelay = 1;
	mEndFrame = 20000000;
	mText = "";
	mTextPlaybackDelay = 10;
	mTextPlaybackEnd = 2020000;
	mShaderLeft = "";
	mShaderRight = "";

	resetSomeParams();
}
int VDSession::getWindowsResolution() {
	return mVDUtils->getWindowsResolution();
}
#pragma region events
bool VDSession::handleMouseMove(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarps, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDown(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarps, event)) {
		// let your application perform its mouseDown handling here
		mVDAnimation->controlValues[21] = event.getX() / getWindowWidth();
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDrag(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarps, event)) {
		// let your application perform its mouseDrag handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseUp(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarps, event)) {
		// let your application perform its mouseUp handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleKeyDown(KeyEvent &event)
{
	bool handled = true;

#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif

	if (isModDown) {
		switch (event.getCode()) {
		case KeyEvent::KEY_s:
			fileWarpsName = "warps" + toString(getElapsedFrames()) + ".xml";
			mWarpSettings = getAssetPath("") / mVDSettings->mAssetsPath / fileWarpsName;
			Warp::writeSettings(mWarps, writeFile(mWarpSettings));
			mWarpSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
			break;
		default:
			handled = false;
			break;
		}
	}
	else {
		// pass this key event to the warp editor first
		if (!Warp::handleKeyDown(mWarps, event)) {
			// pass this event to Mix handler
			if (!mVDAnimation->handleKeyDown(event)) {
				switch (event.getCode()) {
				case KeyEvent::KEY_w:
					// toggle warp edit mode
					Warp::enableEditMode(!Warp::isEditModeEnabled());
					break;
				case KeyEvent::KEY_n:
					createWarp();
					// TODO? Warp::handleResize(mWarps);
					break;
				case KeyEvent::KEY_LEFT:
					//mVDTextures->rewindMovie();				
					break;
				case KeyEvent::KEY_RIGHT:
					//mVDTextures->fastforwardMovie();				
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

				default:
					handled = false;
					break;
				}
			}
		}
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleKeyUp(KeyEvent &event)
{
	bool handled = true;

	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarps, event)) {
		if (!mVDAnimation->handleKeyUp(event)) {
			// Animation did not handle the key, so handle it here
			handled = false;
		}
	}
	event.setHandled(handled);
	return event.isHandled();
}
#pragma endregion events

#pragma region warps

void VDSession::createWarp() {

	mMixFbos.push_back(gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt));

	mWarps.push_back(WarpPerspectiveBilinear::create());
	Warp::handleResize(mWarps);
	int i = mWarps.size() - 1; // must have at least 1 warp!
	mWarpMix[i].ABCrossfade = 0.5;
	mWarpMix[i].AFboIndex = 1;
	mWarpMix[i].AShaderIndex = 4;
	mWarpMix[i].AMode = 0;
	mWarpMix[i].BFboIndex = 2;
	mWarpMix[i].BShaderIndex = 5;
	mWarpMix[i].BMode = 0;
	mWarpMix[i].MixFboIndex = mWarps.size() - 1;
	mWarpMix[i].Name = mWarpMix[i].MixFboIndex;
}
void VDSession::setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade) {
	if (aWarpIndex < mWarpMix.size()) {
		mWarpMix[aWarpIndex].ABCrossfade = aCrossfade;
	}
}
float VDSession::getWarpCrossfade(unsigned int aWarpIndex) {
	if (aWarpIndex > mWarpMix.size() - 1) aWarpIndex = 0;
	return mWarpMix[aWarpIndex].ABCrossfade;
}
void VDSession::setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
	if (aWarpIndex < mWarpMix.size() && aWarpFboIndex < getFboListSize()) {
		mWarpMix[aWarpIndex].AFboIndex = aWarpFboIndex;
		updateWarpName(aWarpIndex);
		mVDRouter->changeControlValue(200 + aWarpIndex, aWarpFboIndex);
	}
}
void VDSession::setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
	if (aWarpIndex < mWarpMix.size() && aWarpFboIndex < getFboListSize()) {
		mWarpMix[aWarpIndex].BFboIndex = aWarpFboIndex;
		updateWarpName(aWarpIndex);
		mVDRouter->changeControlValue(300 + aWarpIndex, aWarpFboIndex);
	}
}
void VDSession::updateWarpName(unsigned int aWarpIndex) {
	if (aWarpIndex < mWarpMix.size()) {
		mWarpMix[aWarpIndex].Name = toString(mWarpMix[aWarpIndex].MixFboIndex) + getFboName(mWarpMix[aWarpIndex].AFboIndex).substr(0, 5) + "/" + getFboName(mWarpMix[aWarpIndex].BFboIndex).substr(0, 5);
	}
}

// Render the scene into the FBO
ci::gl::TextureRef VDSession::getRenderTexture()
{
	// this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	gl::ScopedFramebuffer fbScp(mRenderFbo);
	gl::clear(Color::black());
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());
	// iterate over the warps and draw their content
	int i = 0;
	for (auto &warp : mWarps) {
		//warp->draw(mMixes[0]->getMixTexture(mWarpFboIndex), Area(0, 0, mMixes[0]->getFboTextureWidth(mWarpFboIndex), mMixes[0]->getFboTextureHeight(mWarpFboIndex)));
		//warp->draw(mMixes[0]->getFboTexture(mWarpMix), mMixes[0]->getFboTexture(mWarpMix)->getBounds());
		warp->draw(getMixTexture(i), getMixTexture(i)->getBounds());
		i++;
	}
	return mRenderFbo->getColorTexture();
}
#pragma endregion warps

#pragma region fbos
void VDSession::setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex) {
	mVDMix->setFboInputTexture(aFboIndex, aInputTextureIndex);
}
unsigned int VDSession::getFboInputTextureIndex(unsigned int aFboIndex) {
	return mVDMix->getFboInputTextureIndex(aFboIndex);
}
string VDSession::getFboLabel(unsigned int aFboIndex) {
	return mVDMix->getFboLabel(aFboIndex);
}
ci::gl::TextureRef VDSession::getFboTexture(unsigned int aFboIndex) {
	return mVDMix->getFboTexture(aFboIndex);
}
string VDSession::getFboName(unsigned int aFboIndex) {
	return mVDMix->getFboName(aFboIndex);
}
#pragma endregion fbos
#pragma region textures
ci::gl::Texture2dRef VDSession::getInputTexture(unsigned int aTextureIndex) {
	return mVDMix->getInputTexture(aTextureIndex);
}
string VDSession::getInputTextureName(unsigned int aTextureIndex) {
	return mVDMix->getInputTextureName(aTextureIndex);
}
unsigned int VDSession::getInputTexturesCount() {
	return mVDMix->getInputTexturesCount();
}
#pragma endregion textures

// websockets
#pragma region websockets
#if defined( CINDER_MSW )
void VDSession::wsConnect() {
	mVDRouter->wsConnect();
}
void VDSession::wsPing() {
	mVDRouter->wsPing();
}
#endif
#pragma endregion websockets