//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace VideoDromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	CI_LOG_V("VDSession ctor");
	mVDSettings = aVDSettings;
	// Utils
	mVDUtils = VDUtils::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings);
	mVDAnimation->tapTempo();
	// allow log to file
	mVDLog = VDLog::create();
	// init fbo format
	//fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	//fmt.setBorderColor(Color::black());		
	// uncomment this to enable 4x antialiasing	
	//fboFmt.setSamples( 4 ); 
	fboFmt.setColorTextureFormat(fmt);

	// initialize the textures list with audio texture
	mTexturesFilepath = getAssetPath("") / mVDSettings->mAssetsPath / "textures.xml";
	initTextureList();

	// initialize the shaders list 
	initShaderList();
	mMixesFilepath = getAssetPath("") / "mixes.xml";
	if (fs::exists(mMixesFilepath)) {
		// load textures from file if one exists
		readSettings(mVDSettings, mVDAnimation, loadFile(mMixesFilepath));
	}

	mPosX = mPosY = 0.0f;
	mZoom = 1.0f;

	// Mix
	mVDMix = VDMix::create(mVDSettings, mVDAnimation, mTextureList, mShaderList, mFboList);
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
	// init texture
	//mStreamedTexture = gl::Texture::create(mWidth, mHeight);

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
			string mGlslPath = fboChild->getAttributeValue<string>("shadername", ""); // no default in that case
			CI_LOG_V("fbo shadername " + mGlslPath);
			if (mGlslPath.length() > 0) {
				fs::path mFragFile = getAssetPath("") / mVDSettings->mAssetsPath / mGlslPath;
				if (fs::exists(mFragFile)) {
					VDShaderRef s(new VDShader(mVDSettings, mVDAnimation, mFragFile.string(), ""));
					if (s->isValid()) {
						mShaderList.push_back(s);
						// each shader element has a fbo
						VDFboRef f(new VDFbo(mVDSettings, mVDAnimation, mTextureList));
						// create fbo xml
						XmlTree			fboXml;
						fboXml.setTag(mGlslPath);
						fboXml.setAttribute("id", mShaderList.size() - 1);
						fboXml.setAttribute("width", "640");
						fboXml.setAttribute("height", "480");
						fboXml.setAttribute("shadername", mGlslPath);
						fboXml.setAttribute("inputtextureindex", fboChild->getAttributeValue<string>("inputtextureindex", "0"));
						f->fromXml(fboXml);
						f->setShaderIndex(mShaderList.size()-1);
						mFboList.push_back(f);
					}
				}
			}
		}
	}
}
// control values
float VDSession::getControlValue(unsigned int aCtrl) {
	return mVDAnimation->getFloatUniformValueByIndex(aCtrl);
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

/* void VDSession::writeSettings(const VDMixList &VDMixlist, const ci::DataTargetRef &target) {

// create config document and root <textures>
TODO XmlTree			doc;
doc.setTag("mixes");
doc.setAttribute("version", "1.0");

//
for (unsigned int i = 0; i < VDMixlist.size(); ++i) {
// create <texture>
XmlTree			mix;
mix.setTag("fbo");
mix.setAttribute("id", i + 1);
// details specific to texture type
mix.push_back(VDMixlist[i]->toXml());

// add fbo to doc
doc.push_back(mix);
}

// write file
doc.write(target);
}
XmlTree	VDMix::toXml() const {
XmlTree		xml;
xml.setTag("details");
// TODO rewrite xml.setAttribute("fbopath", mFbosPath);
xml.setAttribute("width", mWidth);
xml.setAttribute("height", mHeight);
return xml;
}*/


bool VDSession::initShaderList() {
	bool isFirstLaunch = false;

	if (mShaderList.size() == 0) {
		CI_LOG_V("VDSession::init mShaderList");
		// direct input texture channel 0
		fs::path mFragFile = getAssetPath("") / "texture0.frag";
		VDShaderRef t0(new VDShader(mVDSettings, mVDAnimation, mFragFile.string(), ""));
		if (t0->isValid()) {
			mShaderList.push_back(t0);
			// each shader element has a fbo
			VDFboRef f(new VDFbo(mVDSettings, mVDAnimation, mTextureList));
			// create fbo xml
			XmlTree			fboXml;
			fboXml.setTag("tex0 fbo");
			fboXml.setAttribute("id", "1");
			fboXml.setAttribute("width", "640");
			fboXml.setAttribute("height", "480");
			fboXml.setAttribute("shadername", "texture0.frag");
			fboXml.setAttribute("inputtextureindex", math<int>::max(0, mTextureList.size() / 2)); // whatever value...
			f->fromXml(fboXml);
			f->setShaderIndex(0);
			mFboList.push_back(f);
			isFirstLaunch = true;
		}
		else {
			CI_LOG_V("VDSession::init mShaderList texture0 failed");
		}
		// direct input texture channel 1
		mFragFile = getAssetPath("") / "texture1.frag";
		VDShaderRef t1(new VDShader(mVDSettings, mVDAnimation, mFragFile.string(), ""));
		if (t1->isValid()) {
			mShaderList.push_back(t1);
			// each shader element has a fbo
			VDFboRef f(new VDFbo(mVDSettings, mVDAnimation, mTextureList));
			// create fbo xml
			XmlTree			fboXml;
			fboXml.setTag("tex1 fbo");
			fboXml.setAttribute("id", "2");
			fboXml.setAttribute("width", "640");
			fboXml.setAttribute("height", "480");;
			fboXml.setAttribute("shadername", "texture1.frag");
			fboXml.setAttribute("inputtextureindex", math<int>::max(0, mTextureList.size() / 3)); // whatever value...
			f->fromXml(fboXml);
			f->setShaderIndex(1);
			mFboList.push_back(f);
			isFirstLaunch = true;
		}
		else {
			CI_LOG_V("VDSession::init mShaderList texture1 failed");
		}
	}
	return isFirstLaunch;
}
bool VDSession::initTextureList() {
	bool isFirstLaunch = false;
	if (mTextureList.size() == 0) {
		CI_LOG_V("VDSession::init mTextureList");
		isFirstLaunch = true;
		// add an audio texture as first texture
		TextureAudioRef t(new TextureAudio(mVDAnimation));

		// add texture xml
		XmlTree			textureXml;
		textureXml.setTag("texture");
		textureXml.setAttribute("id", "0");
		textureXml.setAttribute("texturetype", "audio");

		t->fromXml(textureXml);
		mTextureList.push_back(t);
		// then read textures.xml
		if (fs::exists(mTexturesFilepath)) {
			// load textures from file if one exists
			//mTextureList = VDTexture::readSettings(mVDAnimation, loadFile(mTexturesFilepath));
			XmlTree			doc;
			try { doc = XmlTree(loadFile(mTexturesFilepath)); }
			catch (...) { CI_LOG_V("could not load textures.xml"); }
			if (doc.hasChild("textures")) {
				XmlTree xml = doc.getChild("textures");
				for (XmlTree::ConstIter textureChild = xml.begin("texture"); textureChild != xml.end(); ++textureChild) {
					CI_LOG_V("texture ");

					string texturetype = textureChild->getAttributeValue<string>("texturetype", "unknown");
					CI_LOG_V("texturetype " + texturetype);
					XmlTree detailsXml = textureChild->getChild("details");
					// read or add the assets path
					string mFolder = detailsXml.getAttributeValue<string>("folder", "");
					if (mFolder.length() == 0) detailsXml.setAttribute("folder", mVDSettings->mAssetsPath);
					// create the texture
					if (texturetype == "image") {
						TextureImageRef t(TextureImage::create());
						t->fromXml(detailsXml);
						mTextureList.push_back(t);
				}
					else if (texturetype == "imagesequence") {
						TextureImageSequenceRef t(new TextureImageSequence(mVDAnimation));
						t->fromXml(detailsXml);
						mTextureList.push_back(t);
					}
					else if (texturetype == "movie") {
#if defined( CINDER_MSW )
						TextureMovieRef t(new TextureMovie());
						t->fromXml(detailsXml);
						mTextureList.push_back(t);
#endif
					}
					else if (texturetype == "camera") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
						TextureCameraRef t(new TextureCamera());
						t->fromXml(detailsXml);
						mTextureList.push_back(t);
#else
						// camera not supported on this platform
						CI_LOG_V("camera not supported on this platform");
						XmlTree		xml;
						xml.setTag("details");
						xml.setAttribute("path", "0.jpg");
						xml.setAttribute("width", 640);
						xml.setAttribute("height", 480);
						t->fromXml(xml);
						mTextureList.push_back(t);
#endif
					}
					else if (texturetype == "shared") {
#if defined( CINDER_MSW )
						TextureSharedRef t(new TextureShared());
						t->fromXml(detailsXml);
						mTextureList.push_back(t);
#endif
					}
					else if (texturetype == "audio") {
						// audio texture done in initTextures
					}
					else if (texturetype == "stream") {
						// stream texture done when websocket texture received
					}
					else {
						// unknown texture type
						CI_LOG_V("unknown texture type");
						TextureImageRef t(new TextureImage());
						XmlTree		xml;
						xml.setTag("details");
						xml.setAttribute("path", "0.jpg");
						xml.setAttribute("width", 640);
						xml.setAttribute("height", 480);
						t->fromXml(xml);
						mTextureList.push_back(t);
					}
			}
		}
	}
}
	return isFirstLaunch;
}

void VDSession::resize() {
	mVDMix->resize();
}
void VDSession::update() {
	// fps calculated in main app
	mVDSettings->sFps = toString(floor(getControlValue(30)));
	if (mVDWebsocket->hasReceivedStream() && (getElapsedFrames() % 100 == 0)) {
		updateStream();
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
			if (settings.hasChild("bpm")) mVDAnimation->mBpm = mOriginalBpm = settings.getValueForKey<float>("bpm");
			if (settings.hasChild("beatsperbar")) mVDAnimation->iBeatsPerBar = settings.getValueForKey<int>("beatsperbar");
			if (mVDAnimation->iBeatsPerBar < 1) mVDAnimation->iBeatsPerBar = 1;
			if (settings.hasChild("fadeindelay")) mFadeInDelay = settings.getValueForKey<int>("fadeindelay");
			if (settings.hasChild("fadeoutdelay")) mFadeOutDelay = settings.getValueForKey<int>("fadeoutdelay");
			if (settings.hasChild("endframe")) mVDAnimation->mEndFrame = settings.getValueForKey<int>("endframe");
			mTargetFps = mVDAnimation->mBpm / 60.0f * mFpb;
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
	mVDAnimation->mBpm = mOriginalBpm;
	mTargetFps = mVDAnimation->mBpm / 60.0f * mFpb;
}

void VDSession::reset()
{
	// parameters exposed in json file
	mFlipV = false;
	mFlipH = false;
	mVDAnimation->mBpm = mOriginalBpm = 166;
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
		// if we don't reuse fbo, create corresponding fbo	
		if (index == 0) {
			// find a removed shader
			bool notFound = true;
			for (unsigned int s = 0; s < mShaderList.size(); ++s) {
				if (!mShaderList[s]->isValid() && notFound) {
					notFound = false;
					loadFboFragmentShader(absolutePath, s);
				}
			}
			if (notFound) loadFragmentShader(absolutePath);
		}
		else {
			if (index > mFboList.size() - 1) {
				loadFragmentShader(absolutePath);
			}
			else {
				loadFboFragmentShader(absolutePath, index);
			}
		}
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
	// load success, reset zoom
	// useless mVDWebsocket->changeFloatValue(22, 1.0f);
	//return rtn;
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
				mVDWebsocket->changeFloatValue(22, mVDAnimation->getFloatUniformValueByIndex(22) - 0.05f);
				break;
			case KeyEvent::KEY_LEFT:
				//mVDTextures->rewindMovie();				
				if (mVDAnimation->getFloatUniformValueByIndex(21) > 0.1) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(21) - 0.1);
				break;
			case KeyEvent::KEY_RIGHT:
				//mVDTextures->fastforwardMovie();
				if (mVDAnimation->getFloatUniformValueByIndex(21) < 1.0) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(21) + 0.1);
				break;
			case KeyEvent::KEY_PAGEDOWN:
				// crossfade right
				if (mVDAnimation->getFloatUniformValueByIndex(18) < 1.0) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(18) + 0.1);
				break;
			case KeyEvent::KEY_PAGEUP:
				// crossfade left
				if (mVDAnimation->getFloatUniformValueByIndex(18) > 0.0) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(18) - 0.1);
				break;

			default:
				handled = false;
				break;
			}
		}
		event.setHandled(handled);
		return event.isHandled();
	}
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
				mVDWebsocket->changeFloatValue(22, 1.0f);
				break;
			default:
				handled = false;
				break;
			}
		}
		event.setHandled(handled);
		return event.isHandled();
	}
}
#pragma endregion events

#pragma region warps

void VDSession::createWarpMix() {
	mVDMix->createWarp();
}
void VDSession::setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade) {
	mVDMix->setWarpCrossfade(aWarpIndex, aCrossfade);
}
float VDSession::getWarpCrossfade(unsigned int aWarpIndex) {
	return mVDMix->getWarpCrossfade(aWarpIndex);
}
unsigned int VDSession::getWarpAShaderIndex(unsigned int aWarpIndex) {
	return mVDMix->getWarpAShaderIndex(aWarpIndex);
}
unsigned int VDSession::getWarpBShaderIndex(unsigned int aWarpIndex) {
	return mVDMix->getWarpBShaderIndex(aWarpIndex);
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
string VDSession::getWarpName(unsigned int aWarpIndex) {
	return mVDMix->getWarpName(aWarpIndex);
}
unsigned int VDSession::getWarpAFboIndex(unsigned int aWarpIndex) {
	return mVDMix->getWarpAFboIndex(aWarpIndex);
}
unsigned int VDSession::getWarpBFboIndex(unsigned int aWarpIndex) {
	return mVDMix->getWarpBFboIndex(aWarpIndex);
}
unsigned int VDSession::getWarpCount() {
	return mVDMix->getWarpCount();
}

// Render the scene into the FBO
ci::gl::Texture2dRef VDSession::getRenderTexture()
{
	return mVDMix->getRenderTexture();
}
#pragma endregion warps

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
void VDSession::fboFlipV(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
	mFboList[aFboIndex]->flipV();
}
bool VDSession::isFboFlipV(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
	return mFboList[aFboIndex]->isFlipV();
}
ci::gl::TextureRef VDSession::getMixTexture(unsigned int aMixFboIndex) {
	return mVDMix->getMixTexture(aMixFboIndex);
}
int VDSession::loadFragmentShader(string aFilePath) {
	int rtn = -1;
	CI_LOG_V("loadFragmentShader " + aFilePath);
	VDShaderRef s(new VDShader(mVDSettings, mVDAnimation, aFilePath, ""));
	if (s->isValid()) {
		mShaderList.push_back(s);
		rtn = mShaderList.size() - 1;
		// create a new fbo
		VDFboRef f(new VDFbo(mVDSettings, mVDAnimation, mTextureList));
		// create fbo xml
		XmlTree			fboXml;
		fboXml.setTag(mShaderList[rtn]->getName());
		fboXml.setAttribute("id", toString(rtn));
		fboXml.setAttribute("width", "640");
		fboXml.setAttribute("height", "480");
		fboXml.setAttribute("shadername", mShaderList[rtn]->getName());
		fboXml.setAttribute("inputtextureindex", "0");
		f->fromXml(fboXml);
		f->setShaderIndex(rtn);
		mFboList.push_back(f);
	}
	return rtn;
}
int VDSession::loadFboFragmentShader(string aFilePath, unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
	int rtn = -1;
	CI_LOG_V("fbo" + toString(aFboIndex) + ": loadPixelFragmentShader " + aFilePath);
	rtn = loadFragmentShader(aFilePath);
	if (rtn > -1) {
		mFboList[aFboIndex]->setFragmentShader(rtn, mShaderList[rtn]->getFragmentString(), mShaderList[rtn]->getName());
	}
	//mVDSettings->mShaderToLoad = "";

	return rtn;
}
string VDSession::getFboFragmentShaderText(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
	unsigned int shaderIndex = mFboList[aFboIndex]->getShaderIndex();
	if (shaderIndex > mShaderList.size() - 1) shaderIndex = 0;
	return mShaderList[shaderIndex]->getFragmentString();
}
void VDSession::setPosition(int x, int y) {
	mPosX = ((float)x / (float)mWidth) - 0.5f;
	mPosY = ((float)y / (float)mHeight) - 0.5f;
	for (auto &fbo : mFboList)
	{
		fbo->setPosition(mPosX, mPosY);
	}
}
void VDSession::setZoom(float aZoom) {
	mZoom = aZoom;
	for (auto &fbo : mFboList)
	{
		fbo->setZoom(mZoom);
	}
}

string VDSession::getFboName(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	return mFboList[aFboIndex]->getName();
}
string VDSession::getMixFboName(unsigned int aMixFboIndex) {
	return mVDMix->getMixFboName(aMixFboIndex);
}
int VDSession::getFboTextureWidth(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	return mFboList[aFboIndex]->getTextureWidth();
}
int VDSession::getFboTextureHeight(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	return mFboList[aFboIndex]->getTextureHeight();
}
void VDSession::setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	if (aInputTextureIndex > mTextureList.size() - 1) aInputTextureIndex = mTextureList.size() - 1;
	mFboList[aFboIndex]->setInputTexture(aInputTextureIndex);
}
unsigned int VDSession::getFboInputTextureIndex(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	return mFboList[aFboIndex]->getInputTextureIndex();
}

void VDSession::sendFragmentShader(unsigned int aShaderIndex) {
	mVDWebsocket->changeFragmentShader(mShaderList[aShaderIndex]->getFragmentString());
}

void VDSession::setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex) {
	CI_LOG_V("setFboFragmentShaderIndex, before, fboIndex: " + toString(aFboIndex) + " shaderIndex " + toString(aFboShaderIndex));
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	if (aFboShaderIndex > mShaderList.size() - 1) aFboShaderIndex = mShaderList.size() - 1;
	CI_LOG_V("setFboFragmentShaderIndex, after, fboIndex: " + toString(aFboIndex) + " shaderIndex " + toString(aFboShaderIndex));
	mFboList[aFboIndex]->setFragmentShader(aFboShaderIndex, mShaderList[aFboShaderIndex]->getFragmentString(), mShaderList[aFboShaderIndex]->getName());
	// route message
	// LOOP! mVDWebsocket->changeFragmentShader(mShaderList[aFboShaderIndex]->getFragmentString());
}
unsigned int VDSession::getFboFragmentShaderIndex(unsigned int aFboIndex) {
	unsigned int rtn = mFboList[aFboIndex]->getShaderIndex();
	//CI_LOG_V("getFboFragmentShaderIndex, fboIndex: " + toString(aFboIndex)+" shaderIndex: " + toString(rtn));
	if (rtn > mShaderList.size() - 1) rtn = mShaderList.size() - 1;
	return rtn;
}
unsigned int VDSession::getMixFbosCount() {
	return mVDMix->getMixFbosCount();
};

#pragma endregion fbos
#pragma region textures

int VDSession::getTextureWidth() {
	return mWidth;
};

int VDSession::getTextureHeight() {
	return mHeight;
};
ci::gl::TextureRef VDSession::getInputTexture(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getTexture();
}
string VDSession::getInputTextureName(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getName();
}
unsigned int VDSession::getInputTexturesCount() {
	return mTextureList.size();
}
unsigned int VDSession::getInputTextureOriginalWidth(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getOriginalWidth();
}
unsigned int VDSession::getInputTextureOriginalHeight(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getOriginalHeight();
}
bool VDSession::loadImageSequence(string aFolder, unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	CI_LOG_V("loadImageSequence " + aFolder + " at textureIndex " + toString(aTextureIndex));
	// add texture xml
	XmlTree			textureXml;
	textureXml.setTag("texture");
	textureXml.setAttribute("id", "0");
	textureXml.setAttribute("texturetype", "sequence");
	textureXml.setAttribute("path", aFolder);
	TextureImageSequenceRef t(new TextureImageSequence(mVDAnimation));
	if (t->fromXml(textureXml)) {
		mTextureList.push_back(t);
		return true;
	}
	else {
		return false;
	}
}
void VDSession::loadMovie(string aFile, unsigned int aTextureIndex) {
#if defined( CINDER_MSW )
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	CI_LOG_V("loadMovie " + aFile + " at textureIndex " + toString(aTextureIndex));
	// add texture xml
	XmlTree			textureXml;
	textureXml.setTag("texture");
	textureXml.setAttribute("id", "0");
	textureXml.setAttribute("texturetype", "movie");
	TextureMovieRef t(new TextureMovie());
	t->fromXml(textureXml);
	mTextureList.push_back(t);
#endif
}
void VDSession::loadImageFile(string aFile, unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	CI_LOG_V("loadImageFile " + aFile + " at textureIndex " + toString(aTextureIndex));
	mTextureList[aTextureIndex]->loadFromFullPath(aFile);
}
void VDSession::loadAudioFile(string aFile) {
	mTextureList[0]->loadFromFullPath(aFile);
}
void VDSession::updateStream() {
	int found = -1;
	for (int i = 0; i < mTextureList.size(); i++)
	{
		if (mTextureList[i]->getType() == mTextureList[i]->STREAM) found = i;
	}
	if (found < 0) {
		// create stream texture
		TextureStreamRef t(new TextureStream(mVDAnimation));
		// add texture xml
		XmlTree			textureXml;
		textureXml.setTag("texture");
		textureXml.setAttribute("id", "9");
		textureXml.setAttribute("texturetype", "stream");
		t->fromXml(textureXml);
		mTextureList.push_back(t);
		found = mTextureList.size() - 1;
	}
	//string stream = mVDWebsocket->getBase64Image();
	//mTextureList[found]->loadFromFullPath(stream);
	mTextureList[found]->loadFromFullPath(*mVDWebsocket->getBase64Image());
}
int VDSession::getInputTextureXLeft(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getXLeft();
}
void VDSession::setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft) {
	mTextureList[aTextureIndex]->setXLeft(aXLeft);
}
int VDSession::getInputTextureYTop(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getYTop();
}
void VDSession::setInputTextureYTop(unsigned int aTextureIndex, int aYTop) {
	mTextureList[aTextureIndex]->setYTop(aYTop);
}
int VDSession::getInputTextureXRight(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getXRight();
}
void VDSession::setInputTextureXRight(unsigned int aTextureIndex, int aXRight) {
	mTextureList[aTextureIndex]->setXRight(aXRight);
}
int VDSession::getInputTextureYBottom(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getYBottom();
}
void VDSession::setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->setYBottom(aYBottom);
}
bool VDSession::isFlipVInputTexture(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->isFlipV();
}
void VDSession::inputTextureFlipV(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->flipV();
}
bool VDSession::isFlipHInputTexture(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->isFlipH();
}
void VDSession::inputTextureFlipH(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->flipH();
}

bool VDSession::getInputTextureLockBounds(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getLockBounds();
}
void VDSession::toggleInputTextureLockBounds(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->toggleLockBounds();
}
void VDSession::togglePlayPause(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->togglePlayPause();
}
// movie
bool VDSession::isMovie(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return (mTextureList[aTextureIndex]->getType() == mTextureList[aTextureIndex]->MOVIE);
}
// sequence
bool VDSession::isSequence(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return (mTextureList[aTextureIndex]->getType() == mTextureList[aTextureIndex]->SEQUENCE);
}
bool VDSession::isLoadingFromDisk(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return (mTextureList[aTextureIndex]->isLoadingFromDisk());
}
void VDSession::toggleLoadingFromDisk(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->toggleLoadingFromDisk();
}
void VDSession::syncToBeat(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->syncToBeat();
}
void VDSession::reverse(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->reverse();
}
float VDSession::getSpeed(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getSpeed();
}
void VDSession::setSpeed(unsigned int aTextureIndex, float aSpeed) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->setSpeed(aSpeed);
}
int VDSession::getPlayheadPosition(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getPlayheadPosition();
}
void VDSession::setPlayheadPosition(unsigned int aTextureIndex, int aPosition) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	mTextureList[aTextureIndex]->setPlayheadPosition(aPosition);
}
int VDSession::getMaxFrame(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	return mTextureList[aTextureIndex]->getMaxFrame();
}
#pragma endregion textures

// shaders
#pragma region shaders
bool VDSession::loadShaderFolder(string aFolder) {
	int i = 0;
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
unsigned int VDSession::getShadersCount() {
	return mShaderList.size();
}

string VDSession::getShaderName(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	return mShaderList[aShaderIndex]->getName();
}
ci::gl::TextureRef VDSession::getShaderThumb(unsigned int aShaderIndex) {
	unsigned int found = 0;
	for (int i = 0; i < mFboList.size(); i++)
	{
		if (mFboList[i]->getShaderIndex() == aShaderIndex) found = i;
	}
	return mVDMix->getFboRenderedTexture(found);
}
void VDSession::setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	mShaderList[aShaderIndex]->setFragmentString(aFragmentShaderString, aName);
	// if live coding shader compiles and is used by a fbo reload it
	for (int i = 0; i < mFboList.size(); i++)
	{
		if (mFboList[i]->getShaderIndex() == aShaderIndex) setFboFragmentShaderIndex(i, aShaderIndex);
	}
}
string VDSession::getFragmentShaderString(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	return mShaderList[aShaderIndex]->getFragmentString();
}
string VDSession::getVertexShaderString(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	return mShaderList[aShaderIndex]->getVertexString();
}
/*void VDSession::renderShaderThumb(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	mShaderList[aShaderIndex]->renderThumb();
	}*/
void VDSession::updateShaderThumbFile(unsigned int aShaderIndex) {
	for (int i = 0; i < mFboList.size(); i++)
	{
		if (mFboList[i]->getShaderIndex() == aShaderIndex) mFboList[i]->updateThumbFile();
	}
}
void VDSession::removeShader(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	return mShaderList[aShaderIndex]->removeShader();
}
#pragma endregion shaders

// websockets
#pragma region websockets
#if defined( CINDER_MSW )
void VDSession::wsConnect() {
	mVDWebsocket->wsConnect();
}
void VDSession::wsWrite(string msg)
{
	mVDWebsocket->wsWrite(msg);
}
void VDSession::wsPing() {
	mVDWebsocket->wsPing();
}
#endif
#pragma endregion websockets