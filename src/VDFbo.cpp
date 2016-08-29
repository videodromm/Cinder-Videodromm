#include "VDFbo.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDFbo::VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList)//, mWidth(640), mHeight(480)
		: mFilePathOrText("")
		, mFboName("fbo")
	{
		CI_LOG_V("VDFbo constructor");
		mVDSettings = aVDSettings;
		mVDAnimation = aVDAnimation;
		mTextureList = aTextureList;
		mType = UNKNOWN;
		inputTextureIndex = 0;
		mPosX = mPosY = 0.0f;
		mZoom = 1.0f;

		// init the fbo whatever happens next
		gl::Fbo::Format format;
		mFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, format.depthTexture());
		mError = "";
		// init with passthru shader
		mShaderName = "fbotexture";
		try
		{
			fs::path vertexFile = getAssetPath("") / "passthru.vert";
			if (fs::exists(vertexFile)) {
				mPassthruVextexShaderString = loadString(loadAsset("passthru.vert"));
				CI_LOG_V("passthru.vert loaded");
			}
			else
			{
				CI_LOG_V("passthru.vert does not exist, should quit");
			}
		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = string(exc.what());
			CI_LOG_V("unable to load/compile passthru vertex shader:" + string(exc.what()));
		}
		catch (const std::exception &e)
		{
			mError = string(e.what());
			CI_LOG_V("unable to load passthru vertex shader:" + string(e.what()));
		}
		// load passthru fragment shader
		try {
			fs::path fragFile = getAssetPath("") / "fbotexture.frag";
			if (fs::exists(fragFile)) {
				mFboTextureFragmentShaderString = loadString(loadAsset("fbotexture.frag"));
			}
			else {
				mError = "fbotexture.frag does not exist, should quit";
				CI_LOG_V(mError);
			}
			mFboTextureShader = gl::GlslProg::create(mPassthruVextexShaderString, mFboTextureFragmentShaderString);
			mFboTextureShader->setLabel(mShaderName);
			CI_LOG_V("fbotexture.frag loaded and compiled");
		}
		catch (gl::GlslProgCompileExc &exc) {
			mError = string(exc.what());
			CI_LOG_V("unable to load/compile fbotexture fragment shader:" + string(exc.what()));
		}
		catch (const std::exception &e) {
			mError = string(e.what());
			CI_LOG_V("unable to load fbotexture fragment shader:" + string(e.what()));
		}
		if (mError.length() > 0) mVDSettings->mMsg = mError;
	}
	VDFbo::~VDFbo(void) {
	}

	XmlTree	VDFbo::toXml() const
	{
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("path", mFilePathOrText);
		xml.setAttribute("width", mVDSettings->mFboWidth);
		xml.setAttribute("height", mVDSettings->mFboHeight);
		xml.setAttribute("shadername", mShaderName);

		return xml;
	}

	void VDFbo::fromXml(const XmlTree &xml)
	{
		mId = xml.getAttributeValue<string>("id", "");
		string mGlslPath = xml.getAttributeValue<string>("shadername", "0.glsl");
		mWidth = xml.getAttributeValue<int>("width", mVDSettings->mFboWidth);
		mHeight = xml.getAttributeValue<int>("height", mVDSettings->mFboHeight);
		CI_LOG_V("fbo id " + mId + "fbo shadername " + mGlslPath);
		mShaderName = mGlslPath;
		mFboTextureShader->setLabel(mShaderName);
	}
	void VDFbo::setFragmentShader(string aFragmentShaderString, string aName) {
		try {
			mFboTextureShader = gl::GlslProg::create(mPassthruVextexShaderString, aFragmentShaderString);
			mFboTextureFragmentShaderString = aFragmentShaderString; // set only if compiles successfully
			mFboTextureShader->setLabel(aName);
		}
		catch (gl::GlslProgCompileExc &exc) {
			mError = string(exc.what());
			CI_LOG_V("unable to load/compile fragment shader:" + string(exc.what()));
		}
		catch (const std::exception &e) {
			mError = string(e.what());
			CI_LOG_V("unable to load fragment shader:" + string(e.what()));
		}
	}
	void VDFbo::setShaderIndex(unsigned int aShaderIndex) {
		mShaderIndex = aShaderIndex;
	}

	std::string VDFbo::getLabel() {
		mFbo->setLabel(mFboTextureShader->getLabel() + " " + mId); // add mId to make it unique for imgui
		return mFbo->getLabel();
	}

	void VDFbo::setPosition(int x, int y) {
		mPosX = ((float)x / (float)mVDSettings->mFboWidth) - 0.5;
		mPosY = ((float)y / (float)mVDSettings->mFboHeight) - 0.5;
	}
	void VDFbo::setZoom(float aZoom) {
		mZoom = aZoom;
	}

	ci::ivec2 VDFbo::getSize() {
		return mFbo->getSize();
	}

	ci::Area VDFbo::getBounds() {
		return mFbo->getBounds();
	}

	GLuint VDFbo::getId() {
		return mFbo->getId();
	}

	std::string VDFbo::getName(){
		return mFboName;
	}
	void VDFbo::setInputTexture(unsigned int aTextureIndex) {
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		inputTextureIndex = aTextureIndex;
	}

	ci::gl::Texture2dRef VDFbo::getTexture() {
		iChannelResolution0 = vec3(mPosX, mPosY, 0.5);
		gl::ScopedFramebuffer fbScp(mFbo);
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());
		//gl::ScopedGlslProg shaderScp(mFboTextureShader);
		//mFboTextureShader = mShaderList[mShaderIndex]->getShader();
		mFboTextureShader->bind();
		mFboTextureShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		mFboTextureShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		//mFboTextureShader->uniform("iChannelResolution[0]", iChannelResolution0);
		mFboTextureShader->uniform("iChannel0", 0);
		mFboTextureShader->uniform("iZoom", mZoom);
		gl::ScopedTextureBind tex(mTextureList[inputTextureIndex]->getTexture());
		// lopocompris
		//gl::drawSolidRect(Rectf(0, 0, 900, 700));
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight));
		return mFbo->getColorTexture();
	}
} // namespace VideoDromm

