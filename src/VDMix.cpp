#include "VDMix.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {

	VDMix::VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		: mFlipV(false)
		, mFlipH(false)
	{
		CI_LOG_V("VDMix readSettings");
		// Settings
		mVDSettings = aVDSettings;
		// Animation
		mVDAnimation = aVDAnimation;

		fboFmt.setColorTextureFormat(fmt);
		// initialize the textures list with audio texture
		mTexturesFilepath = getAssetPath("") / mVDSettings->mAssetsPath / "textures.xml";
		initTextureList();

		// initialize the shaders list 
		initShaderList();
		mMixesFilepath = getAssetPath("") / "mixes.xml";
		/*if (fs::exists(mMixesFilepath)) {
			// load textures from file if one exists
			// TODO readSettings(mVDSettings, mVDAnimation, loadFile(mMixesFilepath));
			}*/
			// render fbo
		mRenderFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);
		// mix fbo to render
		warpMixToRender = 0;
		//mWarpSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
		mWarpJson = getAssetPath("") / mVDSettings->mAssetsPath / "warps.json";

		mCurrentBlend = 0;
		for (size_t i = 0; i < mVDAnimation->getBlendModesCount(); i++)
		{
			mBlendFbos[i] = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, fboFmt);
		}

		mGlslMix = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getMixFragmentShaderString());
		// 20161209 problem on Mac mGlslMix->setLabel("mixfbo");
		mGlslBlend = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getMixFragmentShaderString());
		// 20161209 problem on Mac mGlslBlend->setLabel("blend mixfbo");

		// shared output
		mSharedOutputActive = false;
		mSharedFboIndex = 0;
		mSpoutInitialized = false;
		strcpy(mSenderName, "Videodromm Sender"); // we have to set a sender name first
		mWarpAnimationActive = false;
		mWarpActiveIndex = 0;
		mSolo = -1;
		// 20180416 
		for (int i = 0; i < 5; i++)
		{
			if (i < mTextureList.size() - 1) {
				if (i < mFboList.size() - 1) {
					setFboInputTexture(i, i);
				}
			}
		}

	}

#pragma region blendmodes
	unsigned int VDMix::getFboBlendCount() {
		return mBlendFbos.size();
	}
	ci::gl::TextureRef VDMix::getFboThumb(unsigned int aBlendIndex) {
		//if (aBlendIndex > mBlendFbos.size() - 1) aBlendIndex = mBlendFbos.size() - 1;
		return mBlendFbos[aBlendIndex]->getColorTexture();
	}
	void VDMix::useBlendmode(unsigned int aBlendIndex) {
		//if (aBlendIndex > mBlendFbos.size() - 1) aBlendIndex = 0;
		mVDSettings->iBlendmode = aBlendIndex;
	}
	ci::gl::TextureRef VDMix::getMixTexture(unsigned int aMixFboIndex) {
		if (aMixFboIndex > mMixFbos.size() - 1) aMixFboIndex = 0;
		if (!mMixFbos[aMixFboIndex].texture) {
			// should never happen 
			mMixFbos[aMixFboIndex].texture = gl::Texture2d::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		}
		if (!mMixFbos[aMixFboIndex].fbo) {
			// should never happen
			mMixFbos[aMixFboIndex].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		}
		// texturing sharing
		if (mSharedOutputActive && mSharedFboIndex == aMixFboIndex) {
#if defined( CINDER_MSW )
			// spout
			if (mSpoutInitialized) {
				mSpoutSender.SendTexture(mMixFbos[mSharedFboIndex].texture->getId(), mMixFbos[mSharedFboIndex].texture->getTarget(), mVDSettings->mFboWidth, mVDSettings->mFboHeight);
			}
#endif
#if defined( CINDER_MAC )
			// syphon
			mSyphonServer.publishTexture(mMixFbos[mSharedFboIndex].texture);
#endif
		}
		return mMixFbos[aMixFboIndex].texture;
	}
	// spout output
	void VDMix::toggleSharedOutput(unsigned int aMixFboIndex) {
		if (aMixFboIndex < mMixFbos.size()) {
			mSharedFboIndex = aMixFboIndex;
		}
		mSharedOutputActive = !mSharedOutputActive;
#if defined( CINDER_MSW )
		if (mSharedOutputActive && !mSpoutInitialized) {
			// Initialize a sender
			mSpoutInitialized = mSpoutSender.CreateSender(mSenderName, mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		}
#endif
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
		if (mCurrentBlend > mBlendFbos.size() - 1) mCurrentBlend = 0;
		gl::ScopedFramebuffer scopedFbo(mBlendFbos[mCurrentBlend]);
		gl::clear(Color::black());
		// texture binding must be before ScopedGlslProg
		mFboList[mWarps[warpMixToRender]->getAFboIndex()]->getRenderedTexture()->bind(0);
		mFboList[mWarps[warpMixToRender]->getBFboIndex()]->getRenderedTexture()->bind(1);
		gl::ScopedGlslProg glslScope(mGlslBlend);
		gl::drawSolidRect(Rectf(0, 0, mBlendFbos[mCurrentBlend]->getWidth(), mBlendFbos[mCurrentBlend]->getHeight()));
	}
#pragma endregion blendmodes


#pragma region warps
	void VDMix::setCurrentEditIndex(unsigned int aIndex) {
		mCurrentEditIndex = aIndex;
	}
	unsigned int VDMix::getSoloOrActiveIndex() {
		unsigned int rtn = 0;
		if (mSolo > -1) {
			rtn = mSolo;
		}
		else {
			if (mCurrentEditIndex > mWarps.size() - 1) mCurrentEditIndex = 0;
			rtn = mCurrentEditIndex;
		}
		return rtn;
	}
	void VDMix::createWarp(string wName, unsigned int aFboIndex, unsigned int aShaderIndex, unsigned int bFboIndex, unsigned int bShaderIndex, float xFade) {
		int newIndex = mMixFbos.size();
		// ensure bounds are valid
		aFboIndex = math<int>::min(aFboIndex, getFboListSize() - 1);
		bFboIndex = math<int>::min(bFboIndex, getFboListSize() - 1);

		aShaderIndex = math<int>::min(aShaderIndex, getShadersCount() - 1);
		bShaderIndex = math<int>::min(bShaderIndex, getShadersCount() - 1);
		mMixFbos[newIndex].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		mMixFbos[newIndex].texture = gl::Texture2d::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		mMixFbos[newIndex].name = wName;

		mWarps.push_back(WarpBilinear::create());// 20170103 was WarpPerspectiveBilinear
		Warp::handleResize(mWarps);
		Warp::setSize(mWarps, ivec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight)); // create small new warps 
		Warp::handleResize(mWarps);

		//int i = mWarps.size() - 1; // must have at least 1 warp!
		//mWarps[newIndex]->ABCrossfade = xFade;
		mWarps[newIndex]->setAFboIndex(aFboIndex);
		mWarps[newIndex]->setAShaderIndex(aShaderIndex);
		mWarps[newIndex]->setAShaderFilename(mShaderList[aShaderIndex]->getName());
		mWarps[newIndex]->setBFboIndex(bFboIndex);
		mWarps[newIndex]->setBShaderIndex(bShaderIndex);
		mWarps[newIndex]->setBShaderFilename(mShaderList[bShaderIndex]->getName());
		mWarps[newIndex]->setMixFboIndex(newIndex);
		mWarps[newIndex]->setName(toString(mWarps[newIndex]->getMixFboIndex()) + wName);
		updateWarpName(newIndex);
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
			mWarps[aWarpIndex]->setAShaderFilename(mShaderList[aWarpShaderIndex]->getName());
			updateWarpName(aWarpShaderIndex);
		}
	}
	void VDMix::setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex) {
		if (aWarpIndex < mWarps.size() && aWarpShaderIndex < mShaderList.size()) {
			mWarps[aWarpIndex]->setBShaderIndex(aWarpShaderIndex);
			mWarps[aWarpIndex]->setBShaderFilename(mShaderList[aWarpShaderIndex]->getName());
			updateWarpName(aWarpShaderIndex);
		}
	}

	void VDMix::updateWarpName(unsigned int aWarpIndex) {
		if (aWarpIndex < mWarps.size()) {
			mWarps[aWarpIndex]->setName(toString(mWarps[aWarpIndex]->getMixFboIndex()) + mFboList[mWarps[aWarpIndex]->getAFboIndex()]->getName().substr(0, 5) + "/" + mFboList[mWarps[aWarpIndex]->getBFboIndex()]->getName().substr(0, 5));
		}
	}
	void VDMix::save()
	{
		CI_LOG_V("VDMix save: " + mWarpJson.string());

		// save warp settings
		Warp::save(mWarps, writeFile(mWarpJson));

	}
	void VDMix::load()
	{
		CI_LOG_V("VDMix load: " + mWarpJson.string());

		// load warps
		if (fs::exists(mWarpJson)) mWarps = Warp::load(loadFile(mWarpJson));
		// create corresponding mMixFbos
		unsigned int m = mMixFbos.size();
		while (mMixFbos.size() < mWarps.size())
		{
			CI_LOG_V("mMixFbos.size() < mWarps.size(), we create a new mixFbo");
			mMixFbos[m].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
			mMixFbos[m].texture = gl::Texture2d::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
			mMixFbos[m].name = "new";
			m++;
		}
		// ensure all indexes are valid
		for (auto &warp : mWarps) {
			// create the fbos and shaders
			// 20180416 warp->setAShaderIndex(createShaderFbo(warp->getAShaderFilename(), 0));
			// 20180416 warp->setBShaderIndex(createShaderFbo(warp->getBShaderFilename(), 1));
			// ensure all indexes are valid
			if (warp->getAFboIndex() > mFboList.size() - 1) warp->setAFboIndex(0);
			if (warp->getBFboIndex() > mFboList.size() - 1) warp->setBFboIndex(0);
			if (warp->getAShaderIndex() > mShaderList.size() - 1) warp->setAShaderIndex(0);
			if (warp->getBShaderIndex() > mShaderList.size() - 1) warp->setBShaderIndex(0);
			if (warp->getMixFboIndex() > mMixFbos.size() - 1) warp->setMixFboIndex(0);
		}
	}

	// Render the scene into the FBO
	ci::gl::Texture2dRef VDMix::getRenderTexture()
	{
		gl::ScopedFramebuffer fbScp(mRenderFbo);
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());

		// animate
		if (mWarpAnimationActive) {
			mWarpActiveIndex++;
			if (mWarpActiveIndex > mWarps.size() - 1) mWarpActiveIndex = 0;
			mSolo = mWarpActiveIndex;
		}
		// if solo then only render this solo warp
		if (mSolo > -1) {
			mWarps[mSolo]->draw(getMixTexture(mSolo), getMixTexture(mSolo)->getBounds());
		}
		else {
			// iterate over the warps and draw their content
			int i = 0;
			for (auto &warp : mWarps) {
				//warp->draw(mMixes[0]->getMixTexture(mWarpFboIndex), Area(0, 0, mMixes[0]->getFboTextureWidth(mWarpFboIndex), mMixes[0]->getFboTextureHeight(mWarpFboIndex)));
				// if (warp->isActive()) warp->draw(getMixTexture(i), Area(-30, 44, 640, 480));
				// before 20180326: if (warp->isActive()) warp->draw(getMixTexture(i), getMixTexture(i)->getBounds());
				// 20180326: 
				if (warp->isActive()) warp->draw(getMixTexture(i));
				i++;
			}
		}
		mRenderedTexture = mRenderFbo->getColorTexture();
		return mRenderedTexture;
	}
	void VDMix::toggleWarpAnimationActive() {
		// reset solo
		(mWarpAnimationActive) ? mSolo = -1 : mWarpActiveIndex = 0;
		mWarpAnimationActive = !mWarpAnimationActive;
	}

#pragma endregion warps
	void VDMix::renderMix() {
		gl::ScopedFramebuffer scopedFbo(mMixFbos[warpMixToRender].fbo);
		gl::clear(Color::black());
		// render A and B fbos 
		mFboList[mWarps[warpMixToRender]->getAFboIndex()]->getFboTexture();
		mFboList[mWarps[warpMixToRender]->getBFboIndex()]->getFboTexture();
		// texture binding must be before ScopedGlslProg
		mFboList[mWarps[warpMixToRender]->getAFboIndex()]->getRenderedTexture()->bind(0);
		mFboList[mWarps[warpMixToRender]->getBFboIndex()]->getRenderedTexture()->bind(1);
		if (mTextureList.size() > 2) mTextureList[2]->getTexture()->bind(2);
		if (mTextureList.size() > 3) mTextureList[3]->getTexture()->bind(3);
		if (mTextureList.size() > 4) mTextureList[4]->getTexture()->bind(4);
		if (mTextureList.size() > 5) mTextureList[5]->getTexture()->bind(5);
		gl::ScopedGlslProg glslScope(mGlslMix);
		mGlslMix->uniform("iCrossfade", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE)); // mWarps[warpMixToRender]->ABCrossfade);

		gl::drawSolidRect(Rectf(0, 0, mMixFbos[mWarps[warpMixToRender]->getMixFboIndex()].fbo->getWidth(), mMixFbos[mWarps[warpMixToRender]->getMixFboIndex()].fbo->getHeight()));

		// save to a texture
		mMixFbos[warpMixToRender].texture = mMixFbos[warpMixToRender].fbo->getColorTexture();
		warpMixToRender++;
		if (warpMixToRender >= mWarps.size()) {
			warpMixToRender = 0;
		}
		if (mSolo > -1) {
			warpMixToRender = mSolo;
			CI_LOG_V(" solo " + toString(mSolo));
		}
	}

	string VDMix::getMixFboName(unsigned int aMixFboIndex) {
		if (aMixFboIndex > mMixFbos.size() - 1) aMixFboIndex = mMixFbos.size() - 1;
		mMixFbos[aMixFboIndex].name = mFboList[mWarps[aMixFboIndex]->getAFboIndex()]->getShaderName() + "/" + mFboList[mWarps[aMixFboIndex]->getAFboIndex()]->getShaderName();
		return mMixFbos[aMixFboIndex].name;
	}

	void VDMix::resize() {
		// tell the warps our window has been resized, so they properly scale up or down
		Warp::handleResize(mWarps);
		mRenderFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);
	}
	void VDMix::updateAudio() {
		mTextureList[0]->getTexture();
	}
	void VDMix::update() {
		// update audio texture
		if (mVDSettings->mUseAudio) updateAudio();

		// check if xFade changed
		/*if (mVDSettings->xFadeChanged) {
			mVDSettings->xFadeChanged = false;
			for (auto &warp : mWarps) {
				// create the fbos and shaders
				warp->ABCrossfade = mVDSettings->xFade;
			}
		}*/

		mGlslMix->uniform("iBlendmode", mVDSettings->iBlendmode);
		mGlslMix->uniform("iGlobalTime", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ITIME));
		// was vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0)):
		mGlslMix->uniform("iResolution", vec3(mVDAnimation->getFloatUniformValueByName("iResolutionX"), mVDAnimation->getFloatUniformValueByName("iResolutionY"), 1.0));
		//mGlslMix->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		// 20180318 mGlslMix->uniform("iMouse", mVDAnimation->getVec4UniformValueByName("iMouse"));
		mGlslMix->uniform("iWeight0", mVDAnimation->getFloatUniformValueByName("iWeight0")); // fbo mix
		mGlslMix->uniform("iWeight1", mVDAnimation->getFloatUniformValueByName("iWeight1")); // texture
		mGlslMix->uniform("iWeight2", mVDAnimation->getFloatUniformValueByName("iWeight2")); // texture
		mGlslMix->uniform("iWeight3", mVDAnimation->getFloatUniformValueByName("iWeight3")); // texture
		mGlslMix->uniform("iWeight4", mVDAnimation->getFloatUniformValueByName("iWeight4")); // texture
		mGlslMix->uniform("iMouse", vec3(mVDAnimation->getFloatUniformValueByIndex(35), mVDAnimation->getFloatUniformValueByIndex(36), mVDAnimation->getFloatUniformValueByIndex(37)));
		mGlslMix->uniform("iDate", mVDAnimation->getVec4UniformValueByName("iDate"));
		mGlslMix->uniform("iChannel0", 0); // fbo shader 
		mGlslMix->uniform("iChannel1", 1); // fbo shader
		mGlslMix->uniform("iChannel2", 2); // texture 1
		mGlslMix->uniform("iChannel3", 3); // texture 2
		mGlslMix->uniform("iChannel4", 4); // texture 3
		mGlslMix->uniform("iRatio", mVDAnimation->getFloatUniformValueByName("iRatio"));
		mGlslMix->uniform("iRenderXY", mVDSettings->mRenderXY);
		mGlslMix->uniform("iZoom", mVDAnimation->getFloatUniformValueByName("iZoom"));
		mGlslMix->uniform("iAlpha", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFA) * mVDSettings->iAlpha);
		mGlslMix->uniform("iChromatic", mVDAnimation->getFloatUniformValueByName("iChromatic"));
		mGlslMix->uniform("iRotationSpeed", mVDAnimation->getFloatUniformValueByIndex(9));
		mGlslMix->uniform("iCrossfade", mVDAnimation->getFloatUniformValueByName("iCrossfade")); //mWarps[warpMixToRender]->ABCrossfade);
		mGlslMix->uniform("iPixelate", mVDAnimation->getFloatUniformValueByName("iPixelate"));
		mGlslMix->uniform("iExposure", mVDAnimation->getFloatUniformValueByName("iExposure"));
		mGlslMix->uniform("iToggle", (int)mVDAnimation->getBoolUniformValueByName("iToggle"));
		mGlslMix->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		mGlslMix->uniform("iBackgroundColor", mVDAnimation->getVec3UniformValueByName("iBackgroundColor"));// vec3(mVDAnimation->getFloatUniformValueByIndex(5), mVDAnimation->getFloatUniformValueByIndex(6), mVDAnimation->getFloatUniformValueByIndex(7)));
		mGlslMix->uniform("iVignette", (int)mVDAnimation->getBoolUniformValueByName("iVignette"));
		mGlslMix->uniform("iVAmount", mVDAnimation->getFloatUniformValueByName("iVAmount"));
		mGlslMix->uniform("iVFallOff", mVDAnimation->getFloatUniformValueByName("iVFallOff"));
		mGlslMix->uniform("iInvert", (int)mVDAnimation->getBoolUniformValueByName("iInvert"));
		mGlslMix->uniform("iTempoTime", mVDAnimation->getFloatUniformValueByName("iTempoTime"));
		mGlslMix->uniform("iGlitch", (int)mVDAnimation->getBoolUniformValueByName("iGlitch"));
		mGlslMix->uniform("iTrixels", mVDAnimation->getFloatUniformValueByName("iTrixels"));
		mGlslMix->uniform("iRedMultiplier", mVDAnimation->getFloatUniformValueByName("iRedMultiplier"));
		mGlslMix->uniform("iGreenMultiplier", mVDAnimation->getFloatUniformValueByName("iGreenMultiplier"));
		mGlslMix->uniform("iBlueMultiplier", mVDAnimation->getFloatUniformValueByName("iBlueMultiplier"));
		mGlslMix->uniform("iFlipV", mVDAnimation->isFlipV());
		mGlslMix->uniform("iFlipH", mVDAnimation->isFlipH());
		mGlslMix->uniform("iParam1", mVDAnimation->getFloatUniformValueByName("iParam1"));
		mGlslMix->uniform("iParam2", mVDAnimation->getFloatUniformValueByName("iParam2"));
		mGlslMix->uniform("iXorY", mVDSettings->iXorY);
		mGlslMix->uniform("iBadTv", mVDAnimation->getFloatUniformValueByName("iBadTv"));
		mGlslMix->uniform("iFps", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFPS));
		mGlslMix->uniform("iContour", mVDAnimation->getFloatUniformValueByName("iContour"));
		mGlslMix->uniform("iSobel", mVDAnimation->getFloatUniformValueByName("iSobel"));
		renderMix();

		// blendmodes preview
		if (mVDAnimation->renderBlend()) {
			mCurrentBlend = getElapsedFrames() % mVDAnimation->getBlendModesCount();
			mGlslBlend->uniform("iBlendmode", mCurrentBlend);
			mGlslBlend->uniform("iGlobalTime", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ITIME));
			mGlslBlend->uniform("iResolution", vec3(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, 1.0));
			//mGlslBlend->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
			// 20180318 mGlslBlend->uniform("iMouse", mVDAnimation->getVec4UniformValueByName("iMouse"));
			mGlslBlend->uniform("iWeight0", mVDAnimation->getFloatUniformValueByName("iWeight0"));
			mGlslBlend->uniform("iWeight1", mVDAnimation->getFloatUniformValueByName("iWeight1"));
			mGlslBlend->uniform("iWeight2", mVDAnimation->getFloatUniformValueByName("iWeight2"));
			mGlslBlend->uniform("iWeight3", mVDAnimation->getFloatUniformValueByName("iWeight3"));
			mGlslBlend->uniform("iWeight4", mVDAnimation->getFloatUniformValueByName("iWeight4"));
			mGlslBlend->uniform("iMouse", vec3(mVDAnimation->getFloatUniformValueByIndex(35), mVDAnimation->getFloatUniformValueByIndex(36), mVDAnimation->getFloatUniformValueByIndex(37)));
			mGlslBlend->uniform("iDate", mVDAnimation->getVec4UniformValueByName("iDate"));
			mGlslBlend->uniform("iChannel0", 0); // fbo shader 
			mGlslBlend->uniform("iChannel1", 1); // fbo shader
			mGlslBlend->uniform("iChannel2", 2); // texture 1
			mGlslBlend->uniform("iChannel3", 3); // texture 2
			mGlslBlend->uniform("iChannel4", 4); // texture 3
			mGlslBlend->uniform("iAudio0", 0);
			mGlslBlend->uniform("iFreq0", mVDAnimation->getFloatUniformValueByName("iFreq0"));
			mGlslBlend->uniform("iFreq1", mVDAnimation->getFloatUniformValueByName("iFreq1"));
			mGlslBlend->uniform("iFreq2", mVDAnimation->getFloatUniformValueByName("iFreq2"));
			mGlslBlend->uniform("iFreq3", mVDAnimation->getFloatUniformValueByName("iFreq3"));
			mGlslBlend->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
			mGlslBlend->uniform("iColor", vec3(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFR), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFG), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFB)));
			mGlslBlend->uniform("iBackgroundColor", mVDAnimation->getVec3UniformValueByName("iBackgroundColor"));//vec3(mVDAnimation->getFloatUniformValueByIndex(5), mVDAnimation->getFloatUniformValueByIndex(6), mVDAnimation->getFloatUniformValueByIndex(7)));
			mGlslBlend->uniform("iSteps", (int)mVDAnimation->getFloatUniformValueByName("iSteps"));
			mGlslBlend->uniform("iRatio", mVDAnimation->getFloatUniformValueByName("iRatio"));
			mGlslBlend->uniform("width", 1);
			mGlslBlend->uniform("height", 1);
			mGlslBlend->uniform("iRenderXY", mVDSettings->mRenderXY);
			mGlslBlend->uniform("iZoom", mVDAnimation->getFloatUniformValueByIndex(12));
			mGlslBlend->uniform("iAlpha", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFA) * mVDSettings->iAlpha);
			mGlslBlend->uniform("iChromatic", mVDAnimation->getFloatUniformValueByName("iChromatic"));
			mGlslBlend->uniform("iRotationSpeed", mVDAnimation->getFloatUniformValueByIndex(9));
			mGlslBlend->uniform("iCrossfade", 0.5f);// blendmode only work if different than 0 or 1.0 mVDAnimation->getFloatUniformValueByIndex( mVDSettings->IXFADE]);
			mGlslBlend->uniform("iPixelate", mVDAnimation->getFloatUniformValueByName("iPixelate"));
			mGlslBlend->uniform("iExposure", mVDAnimation->getFloatUniformValueByName("iExposure"));
			mGlslBlend->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
			mGlslBlend->uniform("iFade", (int)mVDSettings->iFade);
			mGlslBlend->uniform("iToggle", (int)mVDAnimation->getBoolUniformValueByName("iToggle"));
			mGlslBlend->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
			mGlslBlend->uniform("iTransition", mVDSettings->iTransition);
			mGlslBlend->uniform("iAnim", mVDSettings->iAnim.value());
			mGlslBlend->uniform("iRepeat", (int)mVDSettings->iRepeat);
			mGlslBlend->uniform("iVignette", (int)mVDAnimation->getBoolUniformValueByName("iVignette"));
			mGlslBlend->uniform("iVAmount", mVDAnimation->getFloatUniformValueByName("iVAmount"));
			mGlslBlend->uniform("iVFallOff", mVDAnimation->getFloatUniformValueByName("iVFallOff"));
			mGlslBlend->uniform("iInvert", (int)mVDAnimation->getBoolUniformValueByName("iInvert"));
			mGlslBlend->uniform("iDebug", (int)mVDSettings->iDebug);
			mGlslBlend->uniform("iShowFps", (int)mVDSettings->iShowFps);
			mGlslBlend->uniform("iFps", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFPS));
			mGlslBlend->uniform("iTempoTime", mVDAnimation->getFloatUniformValueByName("iTempoTime"));
			mGlslBlend->uniform("iGlitch", (int)mVDAnimation->getBoolUniformValueByName("iGlitch"));
			mGlslBlend->uniform("iTrixels", mVDAnimation->getFloatUniformValueByName("iTrixels"));
			mGlslBlend->uniform("iBeat", mVDSettings->iBeat);
			mGlslBlend->uniform("iSeed", mVDSettings->iSeed);
			mGlslBlend->uniform("iRedMultiplier", mVDAnimation->getFloatUniformValueByName("iRedMultiplier"));
			mGlslBlend->uniform("iGreenMultiplier", mVDAnimation->getFloatUniformValueByName("iGreenMultiplier"));
			mGlslBlend->uniform("iBlueMultiplier", mVDAnimation->getFloatUniformValueByName("iBlueMultiplier"));
			mGlslBlend->uniform("iFlipH", 0);
			mGlslBlend->uniform("iFlipV", 0);
			mGlslBlend->uniform("iParam1", mVDAnimation->getFloatUniformValueByName("iParam1"));
			mGlslBlend->uniform("iParam2", mVDAnimation->getFloatUniformValueByName("iParam2"));
			mGlslBlend->uniform("iXorY", mVDSettings->iXorY);
			mGlslBlend->uniform("iBadTv", mVDAnimation->getFloatUniformValueByName("iBadTv"));
			mGlslBlend->uniform("iContour", mVDAnimation->getFloatUniformValueByName("iContour"));
			mGlslBlend->uniform("iSobel", mVDAnimation->getFloatUniformValueByName("iSobel"));
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
				fileWarpsName = "warps" + toString(getElapsedFrames()) + ".json";
				Warp::save(mWarps, writeFile(mWarpJson));
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
	void VDMix::setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		if (aInputTextureIndex > mTextureList.size() - 1) aInputTextureIndex = mTextureList.size() - 1;
		mFboList[aFboIndex]->setInputTexture(mTextureList, aInputTextureIndex);
	}
	unsigned int VDMix::getFboInputTextureIndex(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getInputTextureIndex();
	}
	void VDMix::initShaderList() {

		if (mShaderList.size() == 0) {
			CI_LOG_V("VDSession::init mShaderList");
			createShaderFboFromString("void main(void){vec2 uv = gl_FragCoord.xy / iResolution.xy;uv = abs(2.0*(uv - 0.5));vec4 t1 = texture2D(iChannel0, vec2(uv[0], 0.1));vec4 t2 = texture2D(iChannel0, vec2(uv[1], 0.1));float fft = t1[0] * t2[0];gl_FragColor = vec4(sin(fft*3.141*2.5), sin(fft*3.141*2.0), sin(fft*3.141*1.0), 1.0);}", "fftmatrix", "fftMatrixProduct.glsl");
			createShaderFboFromString("void main(void){vec2 uv = fragCoord.xy / iResolution.xy;vec4 tex = texture(iChannel0, uv);fragColor = vec4(vec3( tex.r, tex.g, tex.b ),1.0);}", "0", "0.glsl");
			createShaderFboFromString("void main(void){vec2 uv = fragCoord.xy / iResolution.xy;vec4 tex = texture(iChannel0, uv);fragColor = vec4(vec3( tex.r, tex.g, tex.b ),1.0);}", "1", "1.glsl");
			createShaderFboFromString("void main(void) {vec2 uv = 2 * (gl_FragCoord.xy / iResolution.xy - vec2(0.5));float radius = length(uv);float angle = atan(uv.y, uv.x);float col = .0;col += 1.5*sin(iGlobalTime + 13.0 * angle + uv.y * 20);col += cos(.9 * uv.x * angle * 60.0 + radius * 5.0 - iGlobalTime * 2.);fragColor = (1.2 - radius) * vec4(vec3(col), 1.0);}", "hexler330", "hexler330.glsl");

			createShaderFboFromString("void main(void){vec2 uv = 2 * (fragCoord.xy / iResolution.xy - vec2(0.5));float specx = texture2D( iChannel0, vec2(0.25,5.0/100.0) ).x;float specy = texture2D( iChannel0, vec2(0.5,5.0/100.0) ).x;float specz = 1.0*texture2D( iChannel0, vec2(0.7,5.0/100.0) ).x;float r = length(uv); float p = atan(uv.y/uv.x); uv = abs(uv);float col = 0.0;float amp = (specx+specy+specz)/3.0;uv.y += sin(uv.y*3.0*specx-iGlobalTime/5.0*specy+r*10.);uv.x += cos((iGlobalTime/5.0)+specx*30.0*uv.x);col += abs(1.0/uv.y/30.0) * (specx+specz)*15.0;col += abs(1.0/uv.x/60.0) * specx*8. ; fragColor=vec4(vec3( col ),1.0);}", "Hexler2", "Hexler2.glsl");
			createShaderFboFromString("void main(void){vec2 uv = 2 * (gl_FragCoord.xy / iResolution.xy - vec2(0.5));vec2 spec = 1.0*texture2D(iChannel0, vec2(0.25, 5.0 / 100.0)).xx;float col = 0.0;uv.x += sin(iGlobalTime * 6.0 + uv.y*1.5)*spec.y;col += abs(0.8 / uv.x) * spec.y;gl_FragColor = vec4(col, col, col, 1.0);}", "SoundVizVert", "SoundVizVert.glsl");
			createShaderFboFromString("void main(void){vec2 uv = 2 * (gl_FragCoord.xy / iResolution.xy - vec2(0.5));vec2 spec = 1.0*texture2D(iChannel0, vec2(0.25, 5.0 / 100.0)).yy;float col = 0.0;uv.y += sin(iGlobalTime * 6.0 + uv.x*1.5)*spec.x;col += abs(0.8/uv.y) * spec.x;gl_FragColor = vec4(col, col, col, 1.0);}", "SoundVizHoriz", "SoundVizHoriz.glsl");
			//createShaderFboFromString("#define f(a,b)sin(50.3*length(fragCoord.xy/iResolution.xy*4.-vec2(cos(a),sin(b))-3.)) \n void main(){float t=iGlobalTime;fragColor=vec4(f(t,t)*f(1.4*t,.7*t));}", "Hyper-lightweight2XOR", "Hyper-lightweight2XOR.glsl");
			createShaderFboFromString("void main(void) {float d = pow(dot(fragCoord.xy, iResolution.xy ), 0.52); d =  d * 0.5;float x = sin(6.0+0.1*d + iGlobalTime*-6.0) * 10.0;fragColor = vec4( x, x, x, 1 );}", "WallSide", "WallSide.glsl");

			createShaderFboFromString("void main(void){float d = distance(fragCoord.xy, iResolution.xy * vec2(0.5,0.5).xy);float x = sin(5.0+0.1*d + iGlobalTime*-4.0) * 5.0;x = clamp( x, 0.0, 1.0 );fragColor = vec4(x, x, x, 1.0);}", "Circular", "Circular.glsl");
			createShaderFboFromString("void main(void){vec4 p = vec4(fragCoord.xy,0.,1.)/iResolution.y - vec4(.9,.5,0,0), c=p-p;float t=iGlobalTime,r=length(p.xy+=sin(t+sin(t*.8))*.4),a=atan(p.y,p.x);for (float i = 0.;i<60.;i++) c = c*.98 + (sin(i+vec4(5,3,2,1))*.5+.5)*smoothstep(.99, 1., sin(log(r+i*.05)-t-i+sin(a +=t*.01)));fragColor = c*r;}", "2TweetsChallenge", "2TweetsChallenge.glsl");
			createShaderFboFromString("void main(void){vec2 p = -1.0+2.0*fragCoord.xy/iResolution.xy;float w = sin(iGlobalTime+6.5*sqrt(dot(p,p))*cos(p.x));float x = cos(int(iRatio*10.0)*atan(p.y,p.x) + 1.8*w);vec3 col = iColor*15.0;fragColor = vec4(col*x,1.0);}", "gstnSmoke", "gunstonSmoke.glsl");
			createShaderFboFromString("void main(void) {vec2  px = 4.0*(-iResolution.xy + 2.0*fragCoord.xy)/iResolution.y;float id = 0.5 + 0.5*cos(iGlobalTime + sin(dot(floor(px+0.5),vec2(113.1,17.81)))*43758.545);vec3 co = 0.5 + 0.5*cos(iGlobalTime + 3.5*id + vec3(0.0,1.57,3.14) );vec2 pa = smoothstep( 0.0, 0.2, id*(0.5 + 0.5*cos(6.2831*px)) );fragColor = vec4( co*pa.x*pa.y, 1.0 );}", "ColorGrid", "ColorGrid.glsl");
			/* createShaderFbo("2.glsl", 0);
			createShaderFbo("3.glsl", 0);
			createShaderFbo("4.glsl", 0);
			createShaderFbo("5.glsl", 0); */
		}
	}
	bool VDMix::initTextureList() {
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
							// TODO CHECK USELESS? #if defined( CINDER_MSW )
							/* disabled for SpoutIn/Out problem in same app
							TextureSharedRef t(new TextureShared());
							t->fromXml(detailsXml);
							mTextureList.push_back(t);*/
							// TODO texturetype = textureChild->setAttribute("texturetype", "image");
							TextureImageRef t(TextureImage::create());
							t->fromXml(detailsXml);
							mTextureList.push_back(t);
							//#endif
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
							xml.setAttribute("width", mVDSettings->mFboWidth);
							xml.setAttribute("height", mVDSettings->mFboHeight);
							t->fromXml(xml);
							mTextureList.push_back(t);
						}
					}
				}
			}
		}
		return isFirstLaunch;
	}
	void VDMix::fboFlipV(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
		mFboList[aFboIndex]->flipV();
	}
	bool VDMix::isFboFlipV(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
		return mFboList[aFboIndex]->isFlipV();
	}
	void VDMix::setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex) {
		CI_LOG_V("setFboFragmentShaderIndex, before, fboIndex: " + toString(aFboIndex) + " shaderIndex " + toString(aFboShaderIndex));
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		if (aFboShaderIndex > mShaderList.size() - 1) aFboShaderIndex = mShaderList.size() - 1;
		CI_LOG_V("setFboFragmentShaderIndex, after, fboIndex: " + toString(aFboIndex) + " shaderIndex " + toString(aFboShaderIndex));
		mFboList[aFboIndex]->setFragmentShader(aFboShaderIndex, mShaderList[aFboShaderIndex]->getFragmentString(), mShaderList[aFboShaderIndex]->getName());
		// route message
		// LOOP! mVDWebsocket->changeFragmentShader(mShaderList[aFboShaderIndex]->getFragmentString());
	}
	unsigned int VDMix::getFboFragmentShaderIndex(unsigned int aFboIndex) {
		unsigned int rtn = mFboList[aFboIndex]->getShaderIndex();
		//CI_LOG_V("getFboFragmentShaderIndex, fboIndex: " + toString(aFboIndex)+" shaderIndex: " + toString(rtn));
		if (rtn > mShaderList.size() - 1) rtn = mShaderList.size() - 1;
		return rtn;
	}
#pragma region textures
	ci::gl::TextureRef VDMix::getInputTexture(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getTexture();
	}
	string VDMix::getInputTextureName(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getName();
	}
	unsigned int VDMix::getInputTexturesCount() {
		return mTextureList.size();
	}
	unsigned int VDMix::getInputTextureOriginalWidth(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getOriginalWidth();
	}
	unsigned int VDMix::getInputTextureOriginalHeight(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getOriginalHeight();
	}
	int VDMix::getInputTextureXLeft(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getXLeft();
	}
	void VDMix::setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft) {
		mTextureList[aTextureIndex]->setXLeft(aXLeft);
	}
	int VDMix::getInputTextureYTop(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getYTop();
	}
	void VDMix::setInputTextureYTop(unsigned int aTextureIndex, int aYTop) {
		mTextureList[aTextureIndex]->setYTop(aYTop);
	}
	int VDMix::getInputTextureXRight(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getXRight();
	}
	void VDMix::setInputTextureXRight(unsigned int aTextureIndex, int aXRight) {
		mTextureList[aTextureIndex]->setXRight(aXRight);
	}
	int VDMix::getInputTextureYBottom(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getYBottom();
	}
	void VDMix::setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->setYBottom(aYBottom);
	}
	bool VDMix::isFlipVInputTexture(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->isFlipV();
	}
	void VDMix::inputTextureFlipV(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->flipV();
	}
	bool VDMix::isFlipHInputTexture(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->isFlipH();
	}
	void VDMix::inputTextureFlipH(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->flipH();
	}

	bool VDMix::getInputTextureLockBounds(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getLockBounds();
	}
	void VDMix::toggleInputTextureLockBounds(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->toggleLockBounds();
	}
	void VDMix::togglePlayPause(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->togglePlayPause();
	}
	bool VDMix::loadImageSequence(string aFolder, unsigned int aTextureIndex) {
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
	void VDMix::loadMovie(string aFile, unsigned int aTextureIndex) {
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
	void VDMix::loadImageFile(string aFile, unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		CI_LOG_V("loadImageFile " + aFile + " at textureIndex " + toString(aTextureIndex));
		mTextureList[aTextureIndex]->loadFromFullPath(aFile);
	}
	void VDMix::loadAudioFile(string aFile) {
		mTextureList[0]->loadFromFullPath(aFile);
	}
	bool VDMix::isMovie(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return (mTextureList[aTextureIndex]->getType() == mTextureList[aTextureIndex]->MOVIE);
	}

	// sequence
	bool VDMix::isSequence(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return (mTextureList[aTextureIndex]->getType() == mTextureList[aTextureIndex]->SEQUENCE);
	}
	bool VDMix::isLoadingFromDisk(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return (mTextureList[aTextureIndex]->isLoadingFromDisk());
	}
	void VDMix::toggleLoadingFromDisk(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->toggleLoadingFromDisk();
	}
	void VDMix::syncToBeat(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->syncToBeat();
	}
	void VDMix::reverse(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->reverse();
	}
	float VDMix::getSpeed(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getSpeed();
	}
	void VDMix::setSpeed(unsigned int aTextureIndex, float aSpeed) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->setSpeed(aSpeed);
	}
	int VDMix::getPosition(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getPosition();
	}
	void VDMix::setPlayheadPosition(unsigned int aTextureIndex, int aPosition) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->setPlayheadPosition(aPosition);
	}
	int VDMix::getMaxFrame(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getMaxFrame();
	}
#pragma endregion textures
	// shaders
	void VDMix::updateShaderThumbFile(unsigned int aShaderIndex) {
		for (int i = 0; i < mFboList.size(); i++)
		{
			if (mFboList[i]->getShaderIndex() == aShaderIndex) mFboList[i]->updateThumbFile();
		}
	}
	void VDMix::removeShader(unsigned int aShaderIndex) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		mShaderList[aShaderIndex]->removeShader();
	}
	void VDMix::setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		mShaderList[aShaderIndex]->setFragmentString(aFragmentShaderString, aName);
		// if live coding shader compiles and is used by a fbo reload it
		for (int i = 0; i < mFboList.size(); i++)
		{
			if (mFboList[i]->getShaderIndex() == aShaderIndex) setFboFragmentShaderIndex(i, aShaderIndex);
		}
	}
	unsigned int VDMix::createShaderFboFromString(string aFragmentShaderString, string aShaderFilename, string aName) {
		unsigned int rtn = 0;
		unsigned int shaderId = 0;
		// create new shader
		VDShaderRef s(new VDShader(mVDSettings, mVDAnimation, aShaderFilename, aFragmentShaderString, aName));
		if (s->isValid()) {
			mShaderList.push_back(s);
			shaderId = mShaderList.size() - 1;
			// each shader element has a fbo
			VDFboRef f(new VDFbo(mVDSettings, mVDAnimation));
			// create fbo xml
			XmlTree			fboXml;
			fboXml.setTag(aShaderFilename);
			fboXml.setAttribute("id", shaderId);
			fboXml.setAttribute("width", mVDSettings->mFboWidth);
			fboXml.setAttribute("height", mVDSettings->mFboHeight);
			fboXml.setAttribute("shadername", aName);
			// 20180328 fboXml.setAttribute("inputtextureindex", math<int>::min(rtn, mTextureList.size() - 1));
			fboXml.setAttribute("inputtextureindex", 0);

			f->fromXml(fboXml);
			f->setShaderIndex(shaderId);
			f->setFragmentShader(shaderId, mShaderList[shaderId]->getFragmentString(), aName);
			mFboList.push_back(f);
			rtn = mFboList.size() - 1;
			// 20180322 setFboInputTexture(mFboList.size() - 1, math<int>::min(rtn, mTextureList.size() - 1));
			setFboInputTexture(rtn, 0);
		}
		return rtn;
	}
	string VDMix::getFragmentShaderString(unsigned int aShaderIndex) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		return mShaderList[aShaderIndex]->getFragmentString();
	}
	/*string VDMix::getVertexShaderString(unsigned int aShaderIndex) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		return mShaderList[aShaderIndex]->getVertexString();
	}*/
	string VDMix::getShaderName(unsigned int aShaderIndex) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		return mShaderList[aShaderIndex]->getName();
	}
	ci::gl::TextureRef VDMix::getShaderThumb(unsigned int aShaderIndex) {
		unsigned int found = 0;
		for (int i = 0; i < mFboList.size(); i++)
		{
			if (mFboList[i]->getShaderIndex() == aShaderIndex) found = i;
		}
		return getFboRenderedTexture(found);
	}
	void VDMix::updateStream(string * aStringPtr) {
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
		mTextureList[found]->loadFromFullPath(*aStringPtr);
	}


	unsigned int VDMix::createShaderFbo(string aShaderFilename, unsigned int aInputTextureIndex, unsigned int aIndex) {
		// initialize rtn to 0 to force creation
		unsigned int rtn = 0;
		string fName = aShaderFilename;
		if (aShaderFilename.length() > 0) {
			fs::path mFragFile = getAssetPath("") / mVDSettings->mAssetsPath / aShaderFilename;
			if (!fs::exists(mFragFile)) {
				// if file does not exist it may be a full path
				mFragFile = aShaderFilename;
			}
			if (fs::exists(mFragFile)) {
				if (aIndex < mFboList.size()) {
					if (mShaderList[aIndex]->loadFragmentStringFromFile(mFragFile.string())) {
						mFboList[aIndex]->setFragmentShader(aIndex, mShaderList[aIndex]->getFragmentString(), mShaderList[aIndex]->getName());
						mFboList[aIndex]->setInputTextureIndex(aInputTextureIndex);
						rtn = aIndex;
					}
				}
				// check if mShaderList contains a shader
				/* if (mShaderList.size() > 0) {
					fName = mFragFile.filename().string();
					// find a removed shader
					for (int i = mShaderList.size() - 1; i > 0; i--)
					{
						if (!mShaderList[i]->isValid() || fName == mShaderList[i]->getName()) { rtn = i; }
					}
					// find a not used shader if no removed shader
					if (rtn == 0) {
						// first reset all shaders (excluding the first 8 ones)
						for (int i = mShaderList.size() - 1; i > 8; i--)
						{
							mShaderList[i]->setActive(false);
						}
						// set active shaders according to warps
						for (auto &warp : mWarps) {
							// mShaderList.size() should be > 0 for this part
							if (warp->getAShaderIndex() < mShaderList.size() - 1) mShaderList[warp->getAShaderIndex()]->setActive(true);
							if (warp->getBShaderIndex() < mShaderList.size() - 1) mShaderList[warp->getBShaderIndex()]->setActive(true);
						}
						// find inactive shader index
						for (int i = mShaderList.size() - 1; i > 8; i--)
						{
							if (!mShaderList[i]->isActive()) rtn = i;
						}
					}
				}
				// if we found an available slot
				if (rtn > 0) {
					if (rtn < mFboList.size()) {
						if (mShaderList[rtn]->loadFragmentStringFromFile(aShaderFilename)) {
							mFboList[rtn]->setFragmentShader(rtn, mShaderList[rtn]->getFragmentString(), mShaderList[rtn]->getName());
							mFboList[rtn]->setInputTextureIndex(aInputTextureIndex);
						}
					}
				}
				else {
					// no slot available, create new shader
					// problem to investigate :
					rtn = createShaderFboFromString(loadString(loadFile(mFragFile)), aShaderFilename, fName);
				} */
				//mFboList[rtn]->updateThumbFile();
			}
		}
		return rtn;
	}
} // namespace VideoDromm
