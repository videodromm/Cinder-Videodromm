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
		mInputTextureIndex = 0;
		mPosX = mPosY = 0.0f;
		mZoom = 1.0f;

		// init the fbo whatever happens next
		fboFmt.setColorTextureFormat(fmt);
		mFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
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

	XmlTree	VDFbo::toXml() const {
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("path", mFilePathOrText);
		xml.setAttribute("width", mVDSettings->mFboWidth);
		xml.setAttribute("height", mVDSettings->mFboHeight);
		xml.setAttribute("shadername", mShaderName);
		xml.setAttribute("inputtextureindex", mInputTextureIndex);
		return xml;
	}

	bool VDFbo::fromXml(const XmlTree &xml) {
		mId = xml.getAttributeValue<string>("id", "");
		string mGlslPath = xml.getAttributeValue<string>("shadername", "0.glsl");
		mWidth = xml.getAttributeValue<int>("width", mVDSettings->mFboWidth);
		mHeight = xml.getAttributeValue<int>("height", mVDSettings->mFboHeight);
		mInputTextureIndex = xml.getAttributeValue<int>("inputtextureindex", 0);
		CI_LOG_V("fbo id " + mId + "fbo shadername " + mGlslPath);
		mShaderName = mGlslPath;
		mFboTextureShader->setLabel(mShaderName);
		return true;
	}
	void VDFbo::setFragmentShader(unsigned int aShaderIndex, string aFragmentShaderString, string aName) {
		try {
			mFboTextureShader = gl::GlslProg::create(mPassthruVextexShaderString, aFragmentShaderString);
			mFboTextureFragmentShaderString = aFragmentShaderString; // set only if compiles successfully
			mFboTextureShader->setLabel(aName);
			mFboName = aName;
			mShaderIndex = aShaderIndex;
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
		mInputTextureIndex = aTextureIndex;
	}
	gl::GlslProgRef VDFbo::getShader() { 
		auto &uniforms = mFboTextureShader->getActiveUniforms();
		for (const auto &uniform : uniforms) {
			//CI_LOG_V(mFboTextureShader->getLabel() + ", uniform name:" + uniform.getName());
			if (mVDAnimation->isExistingUniform(uniform.getName())) {
				int uniformType = mVDAnimation->getUniformType(uniform.getName());
				switch (uniformType)
				{
				case 0:
					// float
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getFloatUniformValueByName(uniform.getName()));
					break;
				case 1:
					// sampler2D
					mFboTextureShader->uniform(uniform.getName(), mInputTextureIndex);
					break;
				case 2:
					// vec2
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec2UniformValue(uniform.getName()));
					break;
				case 3:
					// vec3
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec3UniformValue(uniform.getName()));
					break;
				case 4:
					// vec4
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec4UniformValue(uniform.getName()));
					break;
				case 5:
					// int
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getIntUniformValue(uniform.getName()));
					break;
				case 6:
					// bool
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getBoolUniformValue(uniform.getName()));
					break;
				default:
					break;
				}
			}
			else {
				if (uniform.getName() != "ciModelViewProjection") {
					mVDSettings->mMsg = mFboTextureShader->getLabel() + ", uniform not found:" + uniform.getName();
					CI_LOG_V(mVDSettings->mMsg);
				}
			}
		}
		return mFboTextureShader; 
	}

	ci::gl::Texture2dRef VDFbo::getFboTexture() {
		iChannelResolution0 = vec3(mPosX, mPosY, 0.5);
		gl::ScopedFramebuffer fbScp(mFbo);
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());
		//gl::ScopedGlslProg shaderScp(mFboTextureShader);

		mFboTextureShader->bind();
		auto &uniforms = mFboTextureShader->getActiveUniforms();
		for (const auto &uniform : uniforms) {
			//CI_LOG_V(mFboTextureShader->getLabel() + ", uniform name:" + uniform.getName());
			if (mVDAnimation->isExistingUniform(uniform.getName())) {
				int uniformType = mVDAnimation->getUniformType(uniform.getName());
				switch (uniformType)
				{
				case 0:
					// float
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getFloatUniformValueByName(uniform.getName()));
					break;
				case 1:
					// sampler2D
					mFboTextureShader->uniform(uniform.getName(), mInputTextureIndex);
					break;
				case 2:
					// vec2
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec2UniformValue(uniform.getName()));
					break;
				case 3:
					// vec3
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec3UniformValue(uniform.getName()));
					break;
				case 4:
					// vec4
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec4UniformValue(uniform.getName()));
					break;
				case 5:
					// int
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getIntUniformValue(uniform.getName()));
					break;
				case 6:
					// bool
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getBoolUniformValue(uniform.getName()));
					break;
				default:
					break;
				}
			}
			else {
				if (uniform.getName() != "ciModelViewProjection") {
					mVDSettings->mMsg = mFboTextureShader->getLabel() + ", uniform not found:" + uniform.getName();
					CI_LOG_V(mVDSettings->mMsg);
				}
			}
		}
		gl::ScopedTextureBind tex(mTextureList[mInputTextureIndex]->getTexture());
		// lopocompris
		//gl::drawSolidRect(Rectf(0, 0, 900, 700));
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight));
		return mFbo->getColorTexture();
	}
} // namespace VideoDromm

