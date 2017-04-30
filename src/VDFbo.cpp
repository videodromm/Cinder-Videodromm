#include "VDFbo.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDFbo::VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		: mFilePathOrText("")
		, mFboName("fbo")
	{
		CI_LOG_V("VDFbo constructor");
		mVDSettings = aVDSettings;
		mVDAnimation = aVDAnimation;
		mType = UNKNOWN;

		mInputTextureIndex = 0; 


		mPosX = mPosY = 0.0f;
		mZoom = 1.0f;
		isReady = false;
		mFlipV = mFlipH = false;

		// init the fbo whatever happens next
		fboFmt.setColorTextureFormat(fmt);
		mFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		mError = "";
		// init with passthru shader
		mShaderName = "0";

		// load feedback fragment shader
		try {
			mFboTextureShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getDefaultFragmentShaderString());
			CI_LOG_V("fbo default vtx-frag compiled");
		}
		catch (gl::GlslProgCompileExc &exc) {
			mError = string(exc.what());
			CI_LOG_V("fbo unable to load/compile vtx-frag shader:" + string(exc.what()));
		}
		catch (const std::exception &e) {
			mError = string(e.what());
			CI_LOG_V("fbo unable to load vtx-frag shader:" + string(e.what()));
		}
		mFboName = "default";
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
		string mGlslPath = xml.getAttributeValue<string>("shadername", "0.frag");
		mWidth = xml.getAttributeValue<int>("width", mVDSettings->mFboWidth);
		mHeight = xml.getAttributeValue<int>("height", mVDSettings->mFboHeight);
		mInputTextureIndex = xml.getAttributeValue<int>("inputtextureindex", 0);
		CI_LOG_V("fbo id " + mId + "fbo shadername " + mGlslPath);
		mShaderName = mGlslPath;
		// 20161209 problem on Mac mFboTextureShader->setLabel(mShaderName);
		return true;
	}
	void VDFbo::setFragmentShader(unsigned int aShaderIndex, string aFragmentShaderString, string aName) {
		try {
			mFboTextureShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);
			mFboTextureFragmentShaderString = aFragmentShaderString; // set only if compiles successfully
			// 20161209 problem on Mac mFboTextureShader->setLabel(aName);
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

	std::string VDFbo::getName() {
		return mShaderName + " fb:" + mId;
		//return mShaderName + " " + mId;
	}
	std::string VDFbo::getShaderName() {
		return mShaderName;
	}
	void VDFbo::setInputTexture(VDTextureList aTextureList, unsigned int aTextureIndex) {
		mTextureList = aTextureList;
		if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
		mInputTextureIndex = aTextureIndex;
	}
	gl::GlslProgRef VDFbo::getShader() {
		auto &uniforms = mFboTextureShader->getActiveUniforms();
		for (const auto &uniform : uniforms) {
			//CI_LOG_V(mFboTextureShader->getLabel() + ", getShader uniform name:" + uniform.getName());
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
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec2UniformValueByName(uniform.getName()));
					break;
				case 3:
					// vec3
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec3UniformValueByName(uniform.getName()));
					break;
				case 4:
					// vec4
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getVec4UniformValueByName(uniform.getName()));
					break;
				case 5:
					// int
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getIntUniformValueByName(uniform.getName()));
					break;
				case 6:
					// bool
					mFboTextureShader->uniform(uniform.getName(), mVDAnimation->getBoolUniformValueByName(uniform.getName()));
					break;
				default:
					break;
				}
			}
			else {
				if (uniform.getName() != "ciModelViewProjection") {
					mVDSettings->mMsg = mShaderName + ", uniform not found:" + uniform.getName();
					CI_LOG_V(mVDSettings->mMsg);
				}
			}
		}
		// feedback
		/*auto &fbuniforms = mFeedbackShader->getActiveUniforms();
		for (const auto &uniform : fbuniforms) {
			//CI_LOG_V(mFboTextureShader->getLabel() + ", getShader uniform name:" + uniform.getName());
			if (mVDAnimation->isExistingUniform(uniform.getName())) {
				int uniformType = mVDAnimation->getUniformType(uniform.getName());
				switch (uniformType)
				{
				case 0:
					// float
					mFeedbackShader->uniform(uniform.getName(), mVDAnimation->getFloatUniformValueByName(uniform.getName()));
					break;
				case 1:
					// sampler2D
					mFeedbackShader->uniform(uniform.getName(), mCurrentFeedbackIndex);
					break;
				case 2:
					// vec2
					mFeedbackShader->uniform(uniform.getName(), mVDAnimation->getVec2UniformValueByName(uniform.getName()));
					break;
				case 3:
					// vec3
					mFeedbackShader->uniform(uniform.getName(), mVDAnimation->getVec3UniformValueByName(uniform.getName()));
					break;
				case 4:
					// vec4
					mFeedbackShader->uniform(uniform.getName(), mVDAnimation->getVec4UniformValueByName(uniform.getName()));
					break;
				case 5:
					// int
					mFeedbackShader->uniform(uniform.getName(), mVDAnimation->getIntUniformValueByName(uniform.getName()));
					break;
				case 6:
					// bool
					mFeedbackShader->uniform(uniform.getName(), mVDAnimation->getBoolUniformValueByName(uniform.getName()));
					break;
				default:
					break;
				}
			}
			else {
				if (uniform.getName() != "ciModelViewProjection") {
					mVDSettings->mMsg =  "feedback shader, uniform not found:" + uniform.getName();
					CI_LOG_V(mVDSettings->mMsg);
				}
			}
		}*/
		return mFboTextureShader;
	}
	ci::gl::Texture2dRef VDFbo::getRenderedTexture() {
		if (!isReady) {
			// render once for init
			getFboTexture();
			isReady = true;
		}
		return mRenderedTexture;
	}

	ci::gl::Texture2dRef VDFbo::getFboTexture() {
		// TODO move this:
		getShader();
		//iChannelResolution0 = vec3(mPosX, mPosY, 0.5);
		gl::ScopedFramebuffer fbScp(mFbo);
		gl::clear(Color::black());
		// TODO check mTextureList size for bounds
		if (mInputTextureIndex > mTextureList.size() - 1) mInputTextureIndex = 0;
		mTextureList[mInputTextureIndex]->getTexture()->bind(0);

		gl::ScopedGlslProg glslScope(mFboTextureShader);
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));

		mRenderedTexture = mFbo->getColorTexture();
		return mRenderedTexture;
	}

	void VDFbo::updateThumbFile() {
		/*if (mRenderedTexture) {
			string filename = getShaderName() + ".jpg";
			fs::path fr = getAssetPath("") / "thumbs" / filename;
			getFboTexture();
			Surface s8(mRenderedTexture->createSource());
			writeImage(writeFile(getAssetPath("") / "thumbs" / filename), s8);
		}*/
	}

} // namespace VideoDromm

