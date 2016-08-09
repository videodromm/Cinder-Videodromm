#include "VDMix.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDMix::VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		: mFbosPath("fbos.xml")
		, mName("")
		, mFlipV(false)
		, mFlipH(false)
		, mWidth(640)
		, mHeight(480)
	{
		// Settings
		mVDSettings = aVDSettings;
		// Animation
		mVDAnimation = aVDAnimation;
		mLeftFboIndex = mRightFboIndex = 0;

		// initialize the shaders list 
		initShaderList();

		// initialize the textures list with audio texture
		mTexturesFilepath = getAssetPath("") / mVDSettings->mAssetsPath / "textures.xml";
		initTextureList();

		// initialize the fbo list
		initFboList();
		if (mName.length() == 0) {
			mName = mFbosPath;
		}
		mPosX = mPosY = 0.0f;
		mZoom = 1.0f;
		// init the fbo whatever happens next
		gl::Fbo::Format format;
		//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
		mMixFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
		mLeftFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
		mRightFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
		// useless for now:
		//mUseLeftFbo = mUseRightFbo = true;
		// use fbo texture for live coding
		mUseFbo = false;
		// mix shader index
		mMixShaderIndex = 0;
		// load shadertoy uniform variables declarations
		//shaderInclude = loadString(loadAsset("shadertoy.inc"));
		// create blendmodes preview fbos
		gl::Texture::Format fmt;
		fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		fmt.setBorderColor(Color::black());

		gl::Fbo::Format fboFmt;
		fboFmt.setColorTextureFormat(fmt);

		for (size_t i = 0; i < 27; i++)
		{
			mFboBlend.push_back(gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt));
		}
		mCurrentBlend = 0;
	}
	bool VDMix::initFboList() {
		bool isFirstLaunch = false;
		if (mFboList.size() == 0) {
			CI_LOG_V("VDMix::init mFboList");
			isFirstLaunch = true;
			VDFboRef f(new VDFbo(mVDSettings, mVDAnimation, mTextureList));
			// create fbo xml
			XmlTree			fboXml;
			fboXml.setTag("audio fbo 0");
			fboXml.setAttribute("id", "0");
			fboXml.setAttribute("width", "640");
			fboXml.setAttribute("height", "480");;
			f->fromXml(fboXml);
			mFboList.push_back(f);
		}
		return isFirstLaunch;
	}
	bool VDMix::initShaderList() {
		bool isFirstLaunch = false;
		if (mShaderList.size() == 0) {
			CI_LOG_V("VDMix::init mShaderList");
			isFirstLaunch = true;
			VDShaderRef s(new VDShader(mVDSettings, "", ""));
			// create shader xml
			XmlTree			shaderXml;
			shaderXml.setTag("mix");
			shaderXml.setAttribute("id", "0");
			shaderXml.setAttribute("vertfile", "passthru.vert");
			shaderXml.setAttribute("fragfile", "mixfbo.frag");
			s->fromXml(shaderXml);
			mShaderList.push_back(s);
		}
		return isFirstLaunch;
	}
	bool VDMix::initTextureList() {
		bool isFirstLaunch = false;
		if (mTextureList.size() == 0) {
			CI_LOG_V("VDMix::init mTextureList");
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
							TextureImageSequenceRef t(new TextureImageSequence());
							t->fromXml(detailsXml);
							mTextureList.push_back(t);
						}
						else if (texturetype == "movie") {
							TextureMovieRef t(new TextureMovie());
							t->fromXml(detailsXml);
							mTextureList.push_back(t);
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
							TextureSharedRef t(new TextureShared());
							t->fromXml(detailsXml);
							mTextureList.push_back(t);
						}
						else if (texturetype == "audio") {
							/* audio texture done in initTextures
							TextureAudioRef t(new TextureAudio(mVDAnimation));
							t->fromXml(detailsXml);
							mTextureList.push_back(t);
							*/
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
	VDMix::~VDMix(void) {

	}
	VDMixList VDMix::readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const DataSourceRef &source) {
		XmlTree			doc;
		VDMixList	VDMixlist;

		CI_LOG_V("VDMix readSettings");
		// try to load the specified xml file
		try {
			doc = XmlTree(source);
			CI_LOG_V("VDMix xml doc ok");
		}
		catch (...) { return VDMixlist; }

		// check if this is a valid file 
		bool isOK = doc.hasChild("mixes");
		if (!isOK) return VDMixlist;

		//
		if (isOK) {

			XmlTree mixXml = doc.getChild("mixes");

			// iterate fbos
			for (XmlTree::ConstIter child = mixXml.begin("mix"); child != mixXml.end(); ++child) {
				// add the mix to the list
				CI_LOG_V("Add Mix " + child->getValue());
				VDMixRef t(new VDMix(aVDSettings, aVDAnimation));
				t->fromXml(*child);
				VDMixlist.push_back(t);
			}
		}

		return VDMixlist;
	}

	void VDMix::writeSettings(const VDMixList &VDMixlist, const ci::DataTargetRef &target) {

		// create config document and root <textures>
		/* TODO XmlTree			doc;
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
		doc.write(target);*/
	}
	XmlTree	VDMix::toXml() const {
		XmlTree		xml;
		xml.setTag("details");
		// TODO rewrite xml.setAttribute("fbopath", mFbosPath);
		xml.setAttribute("width", mWidth);
		xml.setAttribute("height", mHeight);
		return xml;
	}

	void VDMix::fromXml(const XmlTree &xml) {
		// initialize the shaders list 
		initShaderList();
		// initialize the textures list with audio texture
		initTextureList();
		// initialize the fbo list
		bool isFirstLaunch = initFboList();
		// find fbo childs in xml
		if (xml.hasChild("fbo")) {
			CI_LOG_V("VDMix got fbo child ");
			for (XmlTree::ConstIter fboChild = xml.begin("fbo"); fboChild != xml.end(); ++fboChild) {
				CI_LOG_V("VDMix create fbo ");
				VDFboRef f(new VDFbo(mVDSettings, mVDAnimation, mTextureList));
				f->fromXml(*fboChild);
				mFboList.push_back(f);
				int fboIndex = mFboList.size() - 1;
				
				string mGlslPath = fboChild->getAttributeValue<string>("shadername", "0.glsl");
				CI_LOG_V("fbo shadername " + mGlslPath);
				f->setLabel(mGlslPath);
				if (mGlslPath.length() > 0) {
					fs::path fr = getAssetPath("") / mVDSettings->mAssetsPath / mGlslPath;
					if (fs::exists(fr)) {
						loadFboFragmentShader(fr.string(), fboIndex);
						CI_LOG_V("successfully loaded " + mGlslPath);
					}
					else {
						CI_LOG_V("try upper level because file does not exist: " + mGlslPath);
						fr = getAssetPath("") / mGlslPath;
						if (fs::exists(fr)) {
							loadFboFragmentShader(fr.string(), fboIndex);
							CI_LOG_V("successfully loaded " + mGlslPath);
						}
					}
				}
			}
			if (mFboList.size() > 2) mLeftFboIndex = mFboList.size() - 2;
			if (mFboList.size() > 1) mRightFboIndex = mFboList.size() - 1;
		}
	}
	int VDMix::loadFboFragmentShader(string aFilePath, unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
		int rtn = -1;
		CI_LOG_V("fbo" + toString(aFboIndex) + ": loadPixelFragmentShader " + aFilePath);
		VDShaderRef s(new VDShader(mVDSettings, aFilePath, ""));
		mShaderList.push_back(s);

		rtn = mShaderList.size() - 1;
		mFboList[aFboIndex]->setShaderIndex(rtn);
		mFboList[aFboIndex]->setFragmentShader(s->getFragmentString());
		mVDSettings->mShaderToLoad = "";

		return rtn;
	}
	string VDMix::getFboFragmentShaderText(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
		unsigned int shaderIndex = mFboList[aFboIndex]->getShaderIndex();
		if (shaderIndex > mShaderList.size() - 1) shaderIndex = 0;
		return mShaderList[shaderIndex]->getFragmentString();
	}
	void VDMix::setPosition(int x, int y) {
		mPosX = ((float)x / (float)mWidth) - 0.5;
		mPosY = ((float)y / (float)mHeight) - 0.5;
		for (auto &fbo : mFboList)
		{
			fbo->setPosition(mPosX, mPosY);
		}
	}
	void VDMix::setZoom(float aZoom) {
		mZoom = aZoom;
		for (auto &fbo : mFboList)
		{
			fbo->setZoom(mZoom);
		}
	}
	int VDMix::getTextureWidth() {
		return mWidth;
	};

	int VDMix::getTextureHeight() {
		return mHeight;
	};
	ci::gl::Texture2dRef VDMix::getInputTexture(unsigned int aTextureIndex) {
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

	string VDMix::getFboName(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getName();
	}
	string VDMix::getFboLabel(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getLabel();
	}
	int VDMix::getFboTextureWidth(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getTextureWidth();
	}
	int VDMix::getFboTextureHeight(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getTextureHeight();
	}
	unsigned int VDMix::getFboInputTextureIndex(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getInputTextureIndex();
	}
	ci::ivec2 VDMix::getSize() {
		return mMixFbo->getSize();
	}
	ci::Area VDMix::getBounds() {
		return mMixFbo->getBounds();
	}
	GLuint VDMix::getId() {
		return mMixFbo->getId();
	}
	std::string VDMix::getName(){
		return mName;
	}

	ci::gl::TextureRef VDMix::getRightFboTexture() {
		return mRightFbo->getColorTexture();
	}
	ci::gl::TextureRef VDMix::getLeftFboTexture() {
		return mLeftFbo->getColorTexture();
	}
	void VDMix::setLeftFboIndex(unsigned int aFboIndex) {
		if (aFboIndex < mFboList.size()) mLeftFboIndex = aFboIndex;
	}
	void VDMix::setRightFboIndex(unsigned int aFboIndex) {
		if (aFboIndex < mFboList.size()) mRightFboIndex = aFboIndex;
	}
	int VDMix::loadFileFromAbsolutePath(string aAbsolutePath, int aIndex) {
		int rtn = -1;
		string ext = "";
		// use the last of the dropped files
		int dotIndex = aAbsolutePath.find_last_of(".");
		int slashIndex = aAbsolutePath.find_last_of("\\");

		if (dotIndex != std::string::npos && dotIndex > slashIndex) ext = aAbsolutePath.substr(aAbsolutePath.find_last_of(".") + 1);

		if (ext == "wav" || ext == "mp3") {
			loadAudioFile(aAbsolutePath);
		}
		else if (ext == "png" || ext == "jpg") {
			if (aIndex < 1) aIndex = 1;
			if (aIndex > 3) aIndex = 3;
			loadImageFile(aAbsolutePath, aIndex, true);
		}
		else if (ext == "glsl") {
			if (aIndex > getFboCount() - 1) aIndex = getFboCount() - 1;
			rtn = loadFboFragmentShader(aAbsolutePath, aIndex);		
		}
		else if (ext == "xml") {
		}
		else if (ext == "mov") {
			loadMovie(aAbsolutePath, aIndex);
		}
		else if (ext == "txt") {
		}
		else if (ext == "") {
			// try loading image sequence from dir
			//loadImageSequence(index, mFile);
		}
		return rtn;
	}

	void VDMix::loadMovie(string aFile, unsigned int aTextureIndex) {
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
	}

	void VDMix::loadImageFile(string aFile, unsigned int aTextureIndex, bool right) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		CI_LOG_V("loadImageFile " + aFile + " at textureIndex " + toString(aTextureIndex));
		mTextureList[aTextureIndex]->loadFromFullPath(aFile);
	}
	void VDMix::loadAudioFile(string aFile) {
		mTextureList[0]->loadFromFullPath(aFile);
	}
	ci::gl::Texture2dRef VDMix::getFboTexture(unsigned int aFboIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		return mFboList[aFboIndex]->getTexture();
	}
	void VDMix::setFboInputTexture(unsigned int aFboIndex, unsigned int aFboInputTextureIndex) {
		if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
		if (aFboInputTextureIndex > mTextureList.size() - 1) aFboInputTextureIndex = mTextureList.size() - 1;
		mFboList[aFboIndex]->setInputTexture(aFboInputTextureIndex);
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
	bool VDMix::getInputTextureTopDown(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->isTopDown();
	}
	void VDMix::toggleInputTextureTopDown(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->toggleTopDown();
	}
	bool VDMix::getInputTextureLockBounds(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		return mTextureList[aTextureIndex]->getLockBounds();
	}
	void VDMix::toggleInputTextureLockBounds(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mTextureList[aTextureIndex]->toggleLockBounds();
	}

	void VDMix::setCrossfade(float aCrossfade) {
		mVDAnimation->controlValues[21] = aCrossfade;
	}
	// shaders
	unsigned int VDMix::getShadersCount() {
		return mShaderList.size();
	}
	string VDMix::getShaderName(unsigned int aShaderIndex) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		return mShaderList[aShaderIndex]->getName();
	}
	ci::gl::Texture2dRef VDMix::getShaderThumb(unsigned int aShaderIndex) {
		return mShaderList[aShaderIndex]->getThumb();
	}
	void VDMix::editShader(unsigned int aShaderIndex) {
		mVDSettings->shaderEditIndex = aShaderIndex;
	}
	void VDMix::createShaderThumb(unsigned int aShaderIndex) {
		if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
		return mShaderList[aShaderIndex]->createThumb();
	}
	ci::gl::TextureRef VDMix::getTexture() {
		iChannelResolution0 = vec3(mPosX, mPosY, 0.5);
		gl::ScopedFramebuffer fbScp(mMixFbo);
		gl::clear(Color::gray(0.2f));
		gl::ScopedViewport scpVp(ivec2(0), mMixFbo->getSize());
		//gl::ScopedGlslProg shaderScp(mShaderList[mMixShaderIndex]->getShader());
		mMixShader = mShaderList[mMixShaderIndex]->getShader();

		gl::ScopedTextureBind tex1(getFboTexture(mLeftFboIndex));
		gl::ScopedTextureBind tex2(getFboTexture(mRightFboIndex));
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight));

		vec3 spectrum = vec3(mVDAnimation->iFreqs[0], mVDAnimation->iFreqs[1], mVDAnimation->iFreqs[2]);
		mMixShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		mMixShader->uniform("iResolution", vec3(mWidth, mHeight, 1.0));
		mMixShader->uniform("iChannelResolution[0]", iChannelResolution0);
		mMixShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		mMixShader->uniform("iZoom", mZoom);
		mMixShader->uniform("iChannel0", 0);
		mMixShader->uniform("iChannel1", 1);
		mMixShader->uniform("iChannel2", 2);
		mMixShader->uniform("iAudio0", 0);
		mMixShader->uniform("iFreq0", mVDAnimation->iFreqs[0]);
		mMixShader->uniform("iFreq1", mVDAnimation->iFreqs[1]);
		mMixShader->uniform("iFreq2", mVDAnimation->iFreqs[2]);
		mMixShader->uniform("iFreq3", mVDAnimation->iFreqs[3]);
		mMixShader->uniform("spectrum", spectrum);
		mMixShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
		mMixShader->uniform("iColor", vec3(mVDAnimation->controlValues[1], mVDAnimation->controlValues[2], mVDAnimation->controlValues[3]));// mVDSettings->iColor);
		mMixShader->uniform("iBackgroundColor", vec3(mVDAnimation->controlValues[5], mVDAnimation->controlValues[6], mVDAnimation->controlValues[7]));// mVDSettings->iBackgroundColor);
		mMixShader->uniform("iSteps", (int)mVDAnimation->controlValues[20]);
		mMixShader->uniform("iRatio", mVDAnimation->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio); 
		mMixShader->uniform("width", 1);
		mMixShader->uniform("height", 1);
		mMixShader->uniform("iRenderXY", vec2(0.0, 0.0));
		mMixShader->uniform("iAlpha", mVDAnimation->controlValues[4]);
		mMixShader->uniform("iBlendmode", mVDSettings->iBlendMode);
		mMixShader->uniform("iRotationSpeed", mVDAnimation->controlValues[19]);
		mMixShader->uniform("iCrossfade", mVDAnimation->controlValues[21]);
		mMixShader->uniform("iPixelate", mVDAnimation->controlValues[15]);
		mMixShader->uniform("iExposure", mVDAnimation->controlValues[14]);
		mMixShader->uniform("iFade", (int)mVDSettings->iFade);
		mMixShader->uniform("iToggle", (int)mVDAnimation->controlValues[46]);
		mMixShader->uniform("iLight", (int)mVDSettings->iLight);
		mMixShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
		mMixShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		mMixShader->uniform("iTransition", mVDSettings->iTransition);
		mMixShader->uniform("iAnim", mVDSettings->iAnim.value());
		mMixShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
		mMixShader->uniform("iVignette", (int)mVDAnimation->controlValues[47]);
		mMixShader->uniform("iInvert", (int)mVDAnimation->controlValues[48]);
		mMixShader->uniform("iDebug", (int)mVDSettings->iDebug);
		mMixShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
		mMixShader->uniform("iFps", mVDSettings->iFps);
		// TODO mMixShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
		// TODO mMixShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
		mMixShader->uniform("iTempoTime", 1.0f);
		mMixShader->uniform("iGlitch", (int)mVDAnimation->controlValues[45]);
		mMixShader->uniform("iBeat", mVDSettings->iBeat);
		mMixShader->uniform("iSeed", mVDSettings->iSeed);
		mMixShader->uniform("iFlipH", mFlipH);
		mMixShader->uniform("iFlipV", mFlipV);
		mMixShader->uniform("iTrixels", mVDAnimation->controlValues[16]);
		mMixShader->uniform("iGridSize", mVDAnimation->controlValues[17]);
		mMixShader->uniform("iRedMultiplier", mVDSettings->iRedMultiplier);
		mMixShader->uniform("iGreenMultiplier", mVDSettings->iGreenMultiplier);
		mMixShader->uniform("iBlueMultiplier", mVDSettings->iBlueMultiplier);
		mMixShader->uniform("iParam1", mVDSettings->iParam1);
		mMixShader->uniform("iParam2", mVDSettings->iParam2);
		mMixShader->uniform("iXorY", mVDSettings->iXorY);
		mMixShader->uniform("iBadTv", mVDSettings->iBadTv);

		gl::drawSolidRect(Rectf(0, 0, mMixFbo->getWidth(), mMixFbo->getHeight()));
		return mMixFbo->getColorTexture();
	}
} // namespace VideoDromm
