#include "VDFbo.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDFbo::VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList)
		: mFilePathOrText("")
		, mFboName("fbo")
		, mWidth(1024)
		, mHeight(768)
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
		mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());

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
	}
	VDFbo::~VDFbo(void) {
	}

	XmlTree	VDFbo::toXml() const
	{
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("path", mFilePathOrText);
		xml.setAttribute("width", mWidth);
		xml.setAttribute("height", mHeight);
		xml.setAttribute("shadername", mShaderName);

		return xml;
	}

	void VDFbo::fromXml(const XmlTree &xml)
	{
		mId = xml.getAttributeValue<string>("id", "");
		string mGlslPath = xml.getAttributeValue<string>("shadername", "0.glsl");
		mWidth = xml.getAttributeValue<int>("width", 1024);
		mHeight = xml.getAttributeValue<int>("height", 768);
		CI_LOG_V("fbo id " + mId + "fbo shadername " + mGlslPath);
		mShaderName = mGlslPath;
		mFboTextureShader->setLabel(mShaderName);
	}
	void VDFbo::setFragmentShader(string aFragmentShaderString) {
		try {
			mFboTextureShader = gl::GlslProg::create(mPassthruVextexShaderString, aFragmentShaderString);
			mFboTextureFragmentShaderString = aFragmentShaderString; // set only if compiles successfully
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

	std::string VDFbo::getLabel() {
		mFbo->setLabel(mId + " " + mFboTextureShader->getLabel());
		return mFbo->getLabel();
	}

	void VDFbo::setPosition(int x, int y) {
		mPosX = ((float)x / (float)mWidth) - 0.5;
		mPosY = ((float)y / (float)mHeight) - 0.5;
	}
	void VDFbo::setZoom(float aZoom) {
		mZoom = aZoom;
	}
	int VDFbo::getTextureWidth() {
		return mWidth;
	};

	int VDFbo::getTextureHeight() {
		return mHeight;
	};

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

	/*ci::gl::Texture2dRef VDFbo::getInputTexture(unsigned int aIndex) {
		if (aIndex > mTextureList.size() - 1) aIndex = mTextureList.size() - 1;
		return mTextureList[aIndex]->getTexture();
		}
		}*/
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
		mFboTextureShader->uniform("iResolution", vec3(mWidth, mHeight, 1.0));
		mFboTextureShader->uniform("iChannelResolution[0]", iChannelResolution0);
		mFboTextureShader->uniform("iChannel0", 0);
		mFboTextureShader->uniform("iZoom", mZoom);
		gl::ScopedTextureBind tex(mTextureList[inputTextureIndex]->getTexture());
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight));
		return mFbo->getColorTexture();
	}
} // namespace VideoDromm

