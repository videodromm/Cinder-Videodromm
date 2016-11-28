#include "VDMix.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {

	VDMix::VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList, VDShaderList aShaderList, VDFboList aFboList)
		: mFlipV(false)
		, mFlipH(false)
	{
		CI_LOG_V("VDMix readSettings");
		// Settings
		mVDSettings = aVDSettings;
		// Animation
		mVDAnimation = aVDAnimation;
		// Router
		mTextureList = aTextureList;
		mShaderList = aShaderList;
		mFboList = aFboList;
		// init fbo format
		//fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		//fmt.setBorderColor(Color::black());		
		// uncomment this to enable 4x antialiasing	
		//fboFmt.setSamples( 4 ); 
		fboFmt.setColorTextureFormat(fmt);
		// render fbo
		mRenderFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);

		// mix fbo to render
		warpMixToRender = 0;
		mWarpSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
		mWarpJson = getAssetPath("") / mVDSettings->mAssetsPath / "warps.json";
		//if (fs::exists(mWarpSettings)) {
		// load warp settings from file if one exists
		//mWarps = Warp::readSettings(loadFile(mWarpSettings)); // TODO load from json file
		//}
		//else {
		// otherwise create a warp from scratch

		mCurrentBlend = 0;
		for (size_t i = 0; i < mVDAnimation->getBlendModesCount(); i++)
		{
			mBlendFbos.push_back(gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, fboFmt));
		}

		mGlslMix = gl::GlslProg::create(loadAsset("passthru.vert"), loadAsset("mixfbo.frag"));
		mGlslMix->setLabel("mixfbo");
		mGlslBlend = gl::GlslProg::create(loadAsset("passthru.vert"), loadAsset("mixfbo.frag"));
		mGlslBlend->setLabel("blend mixfbo");
	}

#pragma region blendmodes
	unsigned int VDMix::getFboBlendCount() {
		return mBlendFbos.size();
	}
	ci::gl::TextureRef VDMix::getFboThumb(unsigned int aBlendIndex) {
		if (aBlendIndex > mBlendFbos.size() - 1) aBlendIndex = mBlendFbos.size() - 1;
		/*mCurrentBlend = aBlendIndex;
		renderBlend();*/
		return mBlendFbos[aBlendIndex]->getColorTexture();
	}
	void VDMix::useBlendmode(unsigned int aBlendIndex) {
		if (aBlendIndex > mBlendFbos.size() - 1) aBlendIndex = 0;
		mVDSettings->iBlendmode = aBlendIndex;
	}
	ci::gl::TextureRef VDMix::getMixTexture(unsigned int aMixFboIndex) {
		if (aMixFboIndex > mMixFbos.size() - 1) aMixFboIndex = 0;
		return mMixFbos[aMixFboIndex]->getColorTexture();
	}
	ci::gl::TextureRef VDMix::getFboTexture(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
		return mFboList[aFboIndex]->getFboTexture();
	}
	ci::gl::TextureRef VDMix::getFboRenderedTexture(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
		return mFboList[aFboIndex]->getRenderedTexture();
	}

	void VDMix::renderBlend()
	{
		//mCurrentBlend++;
		if (mCurrentBlend > mBlendFbos.size() - 1) mCurrentBlend = 0;
		gl::ScopedFramebuffer scopedFbo(mBlendFbos[mCurrentBlend]);
		gl::clear(Color::black());

		//mFboList[mWarpMix[warpMixToRender].AFboIndex]->getRenderedTexture()->bind(0);
		//mFboList[mWarpMix[warpMixToRender].BFboIndex]->getRenderedTexture()->bind(1);
		mFboList[mWarps[warpMixToRender]->getAFboIndex()]->getRenderedTexture()->bind(0);
		mFboList[mWarps[warpMixToRender]->getBFboIndex()]->getRenderedTexture()->bind(1);
		gl::ScopedGlslProg glslScope(mGlslBlend);
		gl::drawSolidRect(Rectf(0, 0, mBlendFbos[mCurrentBlend]->getWidth(), mBlendFbos[mCurrentBlend]->getHeight()));
	}
#pragma endregion blendmodes

#pragma endregion warps
	void VDMix::createWarp(string wName, unsigned int aFboIndex, unsigned int aShaderIndex, unsigned int bFboIndex, unsigned int bShaderIndex, float xFade) {

		mMixFbos.push_back(gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt));

		mWarps.push_back(WarpPerspectiveBilinear::create());
		Warp::handleResize(mWarps);
		Warp::setSize(mWarps, ivec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight)); // create small new warps 
		Warp::handleResize(mWarps);

		int i = mWarps.size() - 1; // must have at least 1 warp!
		mWarps[i]->ABCrossfade = xFade;
		mWarps[i]->setAFboIndex(aFboIndex);
		mWarps[i]->setAShaderIndex(aShaderIndex);
		mWarps[i]->setBFboIndex(bFboIndex);
		mWarps[i]->setBShaderIndex(bShaderIndex);
		mWarps[i]->setMixFboIndex(i);
		mWarps[i]->setName(toString(mWarps[i]->getMixFboIndex()) + wName);
	}
	void VDMix::setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade) {
		if (aWarpIndex < mWarps.size()) {
			mWarps[aWarpIndex]->ABCrossfade = aCrossfade;
		}
	}
	float VDMix::getWarpCrossfade(unsigned int aWarpIndex) {
		if (aWarpIndex > mWarps.size() - 1) aWarpIndex = 0;
		return mWarps[aWarpIndex]->ABCrossfade;
	}
	void VDMix::setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
		if (aWarpIndex < mWarps.size() && aWarpFboIndex < mFboList.size()) {
			mWarps[aWarpIndex]->setAFboIndex(aWarpFboIndex);
			updateWarpName(aWarpIndex);
		}
	}
	void VDMix::setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
		if (aWarpIndex < mWarps.size() && aWarpFboIndex < mFboList.size()) {
			mWarps[aWarpIndex]->setBFboIndex(aWarpFboIndex);
			updateWarpName(aWarpIndex);
		}
	}
	unsigned int VDMix::getWarpAShaderIndex(unsigned int aWarpIndex) {
		if (aWarpIndex > mWarps.size() - 1) aWarpIndex = 0;
		return mWarps[aWarpIndex]->getAShaderIndex();
	}
	unsigned int VDMix::getWarpBShaderIndex(unsigned int aWarpIndex) {
		if (aWarpIndex > mWarps.size() - 1) aWarpIndex = 0;
		return mWarps[aWarpIndex]->getBShaderIndex();
	}
	void VDMix::setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex) {
		if (aWarpIndex < mWarps.size() && aWarpShaderIndex < mShaderList.size()) {
			mWarps[aWarpIndex]->setAShaderIndex(aWarpShaderIndex);
			updateWarpName(aWarpShaderIndex);
		}
	}
	void VDMix::setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex) {
		if (aWarpIndex < mWarps.size() && aWarpShaderIndex < mShaderList.size()) {
			mWarps[aWarpIndex]->setBShaderIndex(aWarpShaderIndex);
			updateWarpName(aWarpShaderIndex);
		}
	}

	void VDMix::updateWarpName(unsigned int aWarpIndex) {
		if (aWarpIndex < mWarps.size()) {
			mWarps[aWarpIndex]->setName(toString(mWarps[aWarpIndex]->getMixFboIndex()) + mFboList[mWarps[aWarpIndex]->getAFboIndex()]->getName().substr(0, 5) + "/" + mFboList[mWarps[aWarpIndex]->getBFboIndex()]->getName().substr(0, 5));
		}
	}
	string VDMix::getWarpName(unsigned int aWarpIndex) {
		return mWarps[aWarpIndex]->getName();
	}
	unsigned int VDMix::getWarpAFboIndex(unsigned int aWarpIndex) {
		return mWarps[aWarpIndex]->getAFboIndex();
	}
	unsigned int VDMix::getWarpBFboIndex(unsigned int aWarpIndex) {
		return mWarps[aWarpIndex]->getBFboIndex();
	}
	unsigned int VDMix::getWarpCount() {
		return mWarps.size();
	}
	void VDMix::crossfadeWarp(unsigned int aWarpIndex, float aValue) {
		timeline().apply(&mWarps[aWarpIndex]->ABCrossfade, aValue, 2.0f);
	}
	void VDMix::save()
	{
		CI_LOG_V("VDMix save: " + mWarpJson.string());

		// save warp settings
		Warp::writeSettings(mWarps, writeFile(mWarpSettings));
		Warp::save(mWarps, writeFile(mWarpJson));
	}
	void VDMix::load()
	{
		CI_LOG_V("VDMix load: " + mWarpJson.string());

		// load warp settings
		if (fs::exists(mWarpJson)) mWarps = Warp::load(loadFile(mWarpJson));
		// create corresponding mMixFbos
		while (mMixFbos.size() < mWarps.size())
		{
			mMixFbos.push_back(gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt));
		}
	}
	// Render the scene into the FBO
	ci::gl::Texture2dRef VDMix::getRenderTexture()
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
		mRenderedTexture = mRenderFbo->getColorTexture();

		return mRenderFbo->getColorTexture();
	}


#pragma endregion warps
	string VDMix::getMixFboName(unsigned int aMixFboIndex) {
		if (aMixFboIndex > mMixFbos.size() - 1) aMixFboIndex = mMixFbos.size() - 1;
		mMixFbos[aMixFboIndex]->setLabel(mFboList[mWarps[aMixFboIndex]->getAFboIndex()]->getShaderName() + "/" + mFboList[mWarps[aMixFboIndex]->getAFboIndex()]->getShaderName());
		return mMixFbos[aMixFboIndex]->getLabel();
	}
	void VDMix::renderMix() {

		gl::ScopedFramebuffer scopedFbo(mMixFbos[mWarps[warpMixToRender]->getMixFboIndex()]);
		gl::clear(Color::black());

		// texture binding must be before ScopedGlslProg
		mFboList[mWarps[warpMixToRender]->getAFboIndex()]->getFboTexture();
		mFboList[mWarps[warpMixToRender]->getBFboIndex()]->getFboTexture();

		mFboList[mWarps[warpMixToRender]->getAFboIndex()]->getRenderedTexture()->bind(0);
		mFboList[mWarps[warpMixToRender]->getBFboIndex()]->getRenderedTexture()->bind(1);
		gl::ScopedGlslProg glslScope(mGlslMix);
		mGlslMix->uniform("iCrossfade", mWarps[warpMixToRender]->ABCrossfade);

		gl::drawSolidRect(Rectf(0, 0, mMixFbos[mWarps[warpMixToRender]->getMixFboIndex()]->getWidth(), mMixFbos[mWarps[warpMixToRender]->getMixFboIndex()]->getHeight()));
		warpMixToRender++;
		if (warpMixToRender >= mWarps.size()) {
			warpMixToRender = 0;
		}
		/*if (warpMixToRender == 1) {
			mRenderedTexture = mMixFbos[mWarpMix[warpMixToRender].MixFboIndex]->getColorTexture();
		}*/
	}
	ci::gl::Texture2dRef VDMix::getRenderedTexture() {
		getRenderTexture();
		return mRenderedTexture;
	}
	void VDMix::resize() {
		// tell the warps our window has been resized, so they properly scale up or down
		Warp::handleResize(mWarps);
	}
	void VDMix::update() {
		// update audio texture
		mTextureList[0]->getTexture();
		mGlslMix->uniform("iBlendmode", mVDSettings->iBlendmode);
		mGlslMix->uniform("iGlobalTime", mVDAnimation->getFloatUniformValueByIndex(49));
		mGlslMix->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		//mGlslMix->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		mGlslMix->uniform("iMouse", mVDAnimation->getVec4UniformValue("iMouse"));
		mGlslMix->uniform("iDate", mVDAnimation->getVec4UniformValue("iDate"));
		mGlslMix->uniform("iChannel0", 0);
		mGlslMix->uniform("iChannel1", 1);
		mGlslMix->uniform("iRatio", mVDAnimation->getFloatUniformValueByIndex(11));//check if needed: +1;//mVDSettings->iRatio);
		mGlslMix->uniform("iRenderXY", mVDSettings->mRenderXY);
		mGlslMix->uniform("iZoom", mVDAnimation->getFloatUniformValueByIndex(22));
		mGlslMix->uniform("iAlpha", mVDAnimation->getFloatUniformValueByIndex(4) * mVDSettings->iAlpha);
		mGlslMix->uniform("iChromatic", mVDAnimation->getFloatUniformValueByIndex(10));
		mGlslMix->uniform("iRotationSpeed", mVDAnimation->getFloatUniformValueByIndex(19));
		//mGlslMix->uniform("iCrossfade", mVDAnimation->getFloatUniformValueByIndex(18));
		mGlslMix->uniform("iCrossfade", mWarps[warpMixToRender]->ABCrossfade);
		mGlslMix->uniform("iPixelate", mVDAnimation->getFloatUniformValueByIndex(15));
		mGlslMix->uniform("iExposure", mVDAnimation->getFloatUniformValueByIndex(14));
		mGlslMix->uniform("iToggle", (int)mVDAnimation->getBoolUniformValueByIndex(46));
		mGlslMix->uniform("iLight", (int)mVDSettings->iLight);
		mGlslMix->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
		mGlslMix->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		mGlslMix->uniform("iBackgroundColor", vec3(mVDAnimation->getFloatUniformValueByIndex(5), mVDAnimation->getFloatUniformValueByIndex(6), mVDAnimation->getFloatUniformValueByIndex(7)));
		mGlslMix->uniform("iVignette", (int)mVDAnimation->getBoolUniformValueByIndex(47));
		mGlslMix->uniform("iInvert", (int)mVDAnimation->getBoolUniformValueByIndex(48));
		mGlslMix->uniform("iTempoTime", mVDAnimation->iTempoTime);
		mGlslMix->uniform("iGlitch", (int)mVDAnimation->getBoolUniformValueByIndex(45));
		mGlslMix->uniform("iTrixels", mVDAnimation->getFloatUniformValueByIndex(16));
		mGlslMix->uniform("iRedMultiplier", mVDSettings->iRedMultiplier);
		mGlslMix->uniform("iGreenMultiplier", mVDSettings->iGreenMultiplier);
		mGlslMix->uniform("iBlueMultiplier", mVDSettings->iBlueMultiplier);
		mGlslMix->uniform("iFlipV", mVDAnimation->isFlipV());
		mGlslMix->uniform("iFlipH", mVDAnimation->isFlipH());
		mGlslMix->uniform("iParam1", mVDSettings->iParam1);
		mGlslMix->uniform("iParam2", mVDSettings->iParam2);
		mGlslMix->uniform("iXorY", mVDSettings->iXorY);
		mGlslMix->uniform("iBadTv", mVDSettings->iBadTv);
		mGlslMix->uniform("iFps", mVDAnimation->getFloatUniformValueByIndex(30));

		renderMix();
		// blendmodes preview
		if (mVDAnimation->renderBlend()) {
			mCurrentBlend = getElapsedFrames() % mVDAnimation->getBlendModesCount();
			mGlslBlend->uniform("iBlendmode", mCurrentBlend);
			mGlslBlend->uniform("iGlobalTime", mVDAnimation->getFloatUniformValueByIndex(49));
			mGlslBlend->uniform("iResolution", vec3(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, 1.0));
			//mGlslBlend->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
			mGlslBlend->uniform("iMouse", mVDAnimation->getVec4UniformValue("iMouse"));
			mGlslBlend->uniform("iDate", mVDAnimation->getVec4UniformValue("iDate"));
			mGlslBlend->uniform("iChannel0", 0);
			mGlslBlend->uniform("iChannel1", 1);
			mGlslBlend->uniform("iAudio0", 0);
			mGlslBlend->uniform("iFreq0", mVDAnimation->iFreqs[0]);
			mGlslBlend->uniform("iFreq1", mVDAnimation->iFreqs[1]);
			mGlslBlend->uniform("iFreq2", mVDAnimation->iFreqs[2]);
			mGlslBlend->uniform("iFreq3", mVDAnimation->iFreqs[3]);
			mGlslBlend->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
			mGlslBlend->uniform("iColor", vec3(mVDAnimation->getFloatUniformValueByIndex(1), mVDAnimation->getFloatUniformValueByIndex(2), mVDAnimation->getFloatUniformValueByIndex(3)));
			mGlslBlend->uniform("iBackgroundColor", vec3(mVDAnimation->getFloatUniformValueByIndex(5), mVDAnimation->getFloatUniformValueByIndex(6), mVDAnimation->getFloatUniformValueByIndex(7)));
			mGlslBlend->uniform("iSteps", (int)mVDAnimation->getFloatUniformValueByIndex(20));
			mGlslBlend->uniform("iRatio", mVDAnimation->getFloatUniformValueByIndex(11));
			mGlslBlend->uniform("width", 1);
			mGlslBlend->uniform("height", 1);
			mGlslBlend->uniform("iRenderXY", mVDSettings->mRenderXY);
			mGlslBlend->uniform("iZoom", mVDAnimation->getFloatUniformValueByIndex(22));
			mGlslBlend->uniform("iAlpha", mVDAnimation->getFloatUniformValueByIndex(4) * mVDSettings->iAlpha);
			mGlslBlend->uniform("iChromatic", mVDAnimation->getFloatUniformValueByIndex(10));
			mGlslBlend->uniform("iRotationSpeed", mVDAnimation->getFloatUniformValueByIndex(19));
			mGlslBlend->uniform("iCrossfade", 0.5f);// blendmode only work if different than 0 or 1.0 mVDAnimation->getFloatUniformValueByIndex(18]);
			mGlslBlend->uniform("iPixelate", mVDAnimation->getFloatUniformValueByIndex(15));
			mGlslBlend->uniform("iExposure", mVDAnimation->getFloatUniformValueByIndex(14));
			mGlslBlend->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
			mGlslBlend->uniform("iFade", (int)mVDSettings->iFade);
			mGlslBlend->uniform("iToggle", (int)mVDAnimation->getBoolUniformValueByIndex(46));
			mGlslBlend->uniform("iLight", (int)mVDSettings->iLight);
			mGlslBlend->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
			mGlslBlend->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
			mGlslBlend->uniform("iTransition", mVDSettings->iTransition);
			mGlslBlend->uniform("iAnim", mVDSettings->iAnim.value());
			mGlslBlend->uniform("iRepeat", (int)mVDSettings->iRepeat);
			mGlslBlend->uniform("iVignette", (int)mVDAnimation->getBoolUniformValueByIndex(47));
			mGlslBlend->uniform("iInvert", (int)mVDAnimation->getBoolUniformValueByIndex(48));
			mGlslBlend->uniform("iDebug", (int)mVDSettings->iDebug);
			mGlslBlend->uniform("iShowFps", (int)mVDSettings->iShowFps);
			mGlslBlend->uniform("iFps", mVDAnimation->getFloatUniformValueByIndex(30));
			mGlslBlend->uniform("iTempoTime", mVDAnimation->iTempoTime);
			mGlslBlend->uniform("iGlitch", (int)mVDAnimation->getBoolUniformValueByIndex(45));
			mGlslBlend->uniform("iTrixels", mVDAnimation->getFloatUniformValueByIndex(16));
			mGlslBlend->uniform("iBeat", mVDSettings->iBeat);
			mGlslBlend->uniform("iSeed", mVDSettings->iSeed);
			mGlslBlend->uniform("iRedMultiplier", mVDSettings->iRedMultiplier);
			mGlslBlend->uniform("iGreenMultiplier", mVDSettings->iGreenMultiplier);
			mGlslBlend->uniform("iBlueMultiplier", mVDSettings->iBlueMultiplier);
			mGlslBlend->uniform("iFlipH", 0);
			mGlslBlend->uniform("iFlipV", 0);
			mGlslBlend->uniform("iParam1", mVDSettings->iParam1);
			mGlslBlend->uniform("iParam2", mVDSettings->iParam2);
			mGlslBlend->uniform("iXorY", mVDSettings->iXorY);
			mGlslBlend->uniform("iBadTv", mVDSettings->iBadTv);
			renderBlend();
		}
	}
#pragma region events

	bool VDMix::handleMouseMove(MouseEvent &event)
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

	bool VDMix::handleMouseDown(MouseEvent &event)
	{
		bool handled = true;
		// pass this mouse event to the warp editor first
		if (!Warp::handleMouseDown(mWarps, event)) {
			// let your application perform its mouseDown handling here
			handled = false;
		}
		event.setHandled(handled);
		return event.isHandled();
	}
	bool VDMix::handleMouseDrag(MouseEvent &event)
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
	bool VDMix::handleMouseUp(MouseEvent &event)
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

	bool VDMix::handleKeyDown(KeyEvent &event)
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
			case KeyEvent::KEY_n:
				createWarp();
				// TODO? Warp::handleResize(mWarps);
				break;
			default:
				handled = false;
				break;
			}
		}
		else {
			// pass this key event to the warp editor first
			if (!Warp::handleKeyDown(mWarps, event)) {
				handled = false;
			}
		}
		event.setHandled(handled);
		return event.isHandled();
	}
	bool VDMix::handleKeyUp(KeyEvent &event)
	{
		bool handled = true;
		// pass this key event to the warp editor first
		if (!Warp::handleKeyUp(mWarps, event)) {
			handled = false;
		}
		event.setHandled(handled);
		return event.isHandled();
	}
#pragma endregion events

	ci::ivec2 VDMix::getSize() {
		return mMixFbos[0]->getSize();
	}
	ci::Area VDMix::getBounds() {
		return mMixFbos[0]->getBounds();
	}
	GLuint VDMix::getId() {
		return mMixFbos[0]->getId();
	}
} // namespace VideoDromm
