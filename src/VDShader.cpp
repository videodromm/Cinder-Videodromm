#include "VDShader.h"

using namespace VideoDromm;

VDShader::VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath = "", string aVextexShaderFilePath = "") {
	mFragmentShaderFilePath = aFragmentShaderFilePath;
	mVextexShaderFilePath = aVextexShaderFilePath;
	mValid = false;
	// shadertoy include
	shaderInclude = loadString(loadAsset("shadertoy.inc"));
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mError = "";
	gl::Fbo::Format format;
	mThumbFbo = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, format.depthTexture());

	loadVertexStringFromFile(mVextexShaderFilePath);
	loadFragmentStringFromFile(mFragmentShaderFilePath);
	if (mValid) {
		CI_LOG_V("VDShaders constructor success");
	}
	else {
		CI_LOG_V("VDShaders constructor failed, do not use");
	}
}
void VDShader::loadVertexStringFromFile(string aFileName) {
	// load vertex shader
	try {
		if (aFileName.length() == 0) {
			mVertexFile = getAssetPath("") / "passthru.vert";
		}
		else {
			mVertexFile = aFileName;
		}
		if (!fs::exists(mVertexFile)) {
			mError = mVertexFile.string() + " does not exist";
			CI_LOG_V(mError);
			mVertexFile = getAssetPath("") / "passthru.vert";
		}
		mVextexShaderString = loadString(loadFile(mVertexFile));
		mVextexShaderFilePath = mVertexFile.string();
		CI_LOG_V("successfully loaded " + mVertexFile.string());
	}
	catch (const std::exception &e) {
		mError = "unable to load vertex shader:" + string(e.what());
		CI_LOG_V(string(e.what()));
	}
}
bool VDShader::loadFragmentStringFromFile(string aFileName) {
	mValid = false;
	// load fragment shader
	CI_LOG_V("loadFragmentStringFromFile, loading " + aFileName);

	if (aFileName.length() == 0) {
		mFragFile = getAssetPath("") / "0.glsl";
	}
	else {
		mFragFile = aFileName;
	}
	if (!fs::exists(mFragFile)) {
		mError = mFragFile.string() + " does not exist";
		CI_LOG_V(mError);
		mFragFile = getAssetPath("") / "0.glsl";
	}

	string fileName = mFragFile.filename().string();
	// get filename without extension
	int dotIndex = fileName.find_last_of(".");

	if (dotIndex != std::string::npos) {
		mName = fileName.substr(0, dotIndex);
	}
	else {
		mName = fileName;
	} 
	
	mFragmentShaderFilePath = mFragFile.string();
	mFragmentShaderString = loadString(loadFile(mFragFile));
	mValid = setFragmentString(mFragmentShaderString, mName);

	CI_LOG_V(mFragFile.string() + " loaded and compiled");
	return mValid;
}
bool VDShader::setFragmentString(string aFragmentShaderString, string aName) {
	string mOriginalFragmentString = aFragmentShaderString;
	string mCurrentUniformsString = "";
	string mNotFoundUniformsString = "/*\n";
	if (aName.length() == 0) aName = getElapsedSeconds();
	mName = aName;
	aName += ".frag";
	mValid = false;
	// load fragment shader
	CI_LOG_V("setFragmentString, live loading");
	try
	{
		std::size_t foundUniform = mOriginalFragmentString.find("uniform ");

		if (foundUniform == std::string::npos) {
			CI_LOG_V("loadFragmentStringFromFile, no uniforms found, we add from shadertoy.inc");
			aFragmentShaderString = "/*" + mFragFile.string() + "*/\n" + shaderInclude + mOriginalFragmentString;
		}
		mShader = gl::GlslProg::create(mVextexShaderString, aFragmentShaderString);
		// update only if success
		mFragmentShaderString = aFragmentShaderString;
		CI_LOG_V(mFragFile.string() + " live edited, loaded and compiled");
		mValid = true;
		createThumb();
		auto &uniforms = mShader->getActiveUniforms();
		for (const auto &uniform : uniforms) {
			CI_LOG_V(mShader->getLabel() + ", uniform name:" + uniform.getName());
			if (mVDAnimation->isExistingUniform(uniform.getName())) {
				int uniformType = mVDAnimation->getUniformType(uniform.getName());
				switch (uniformType)
				{
				case 0:
					// float
					mShader->uniform(uniform.getName(), mVDAnimation->getFloatUniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform float " + uniform.getName() + "; // " + toString(mVDAnimation->getFloatUniformValue(uniform.getName())) + "\n";
					break;
				case 1:
					// sampler2D
					mShader->uniform(uniform.getName(), mVDAnimation->getSampler2DUniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform sampler2D " + uniform.getName() + "; // " + toString(mVDAnimation->getSampler2DUniformValue(uniform.getName())) + "\n";
					break;
				case 3:
					// vec3
					mShader->uniform(uniform.getName(), mVDAnimation->getVec3UniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform vec3 " + uniform.getName() + "; // " + toString(mVDAnimation->getVec3UniformValue(uniform.getName())) + "\n";
					break;
				default:
					break;
				}
			}
			else {
				mNotFoundUniformsString += "not found " + uniform.getName() + "\n";
			}
		}
		mNotFoundUniformsString += "*/\n";
		// save .frag file
		fs::path processedFile = getAssetPath("") / "glsl" / "processed" / aName;
		ofstream mFragProcessed(processedFile.string(), std::ofstream::binary);
		mFragProcessed << mNotFoundUniformsString << mCurrentUniformsString << mOriginalFragmentString;
		mFragProcessed.close();
		CI_LOG_V("processed file saved:" + processedFile.string());

	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("setFragmentString, unable to compile live fragment shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("setFragmentString, error on live fragment shader:" + string(e.what()));
	}
	return mValid;
}

void VDShader::fromXml(const XmlTree &xml) {
	mId = xml.getAttributeValue<string>("id", "");
	string mVertfile = xml.getAttributeValue<string>("vertfile", "passthru.vert");
	mName = xml.getAttributeValue<string>("fragfile", "0.glsl");
	CI_LOG_V("shader id " + mId + " name " + mName);
	mVertexFile = getAssetPath("") / mVDSettings->mAssetsPath / mVertfile;
	loadVertexStringFromFile(mVextexShaderFilePath);

	mFragFile = getAssetPath("") / mVDSettings->mAssetsPath / mName;
	loadFragmentStringFromFile(mFragFile.string());
}
void VDShader::createThumb() {
	// draw using the shader
	mThumbFbo->bindFramebuffer();
	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);

	mShader->bind();
	mShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	mShader->uniform("iResolution", vec3(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, 1.0));
	mShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	mShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	mShader->uniform("iChannel0", 0);
	mShader->uniform("iChannel1", 1);
	mShader->uniform("iChannel2", 2);
	mShader->uniform("iAudio0", 0);
	mShader->uniform("iFreq0", 1.0f);
	mShader->uniform("iFreq1", 1.0f);
	mShader->uniform("iFreq2", 1.0f);
	mShader->uniform("iFreq3", 1.0f);
	mShader->uniform("spectrum", vec3(1.0f));
	mShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	mShader->uniform("iColor", vec3(1.0f, 1.0f, 0.0f));
	mShader->uniform("iBackgroundColor", vec3(1.0f, 0.0f, 0.0f));
	mShader->uniform("iSteps", 16.0f);
	mShader->uniform("iRatio", 20.0f);
	mShader->uniform("iRenderXY", mVDSettings->mRenderXY);
	mShader->uniform("iZoom", 1.0f);
	mShader->uniform("iAlpha", 1.0f);

	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
	// stop drawing into the FBO
	mThumbFbo->unbindFramebuffer();
	mThumbTexture = mThumbFbo->getColorTexture();
}
ci::gl::Texture2dRef VDShader::getThumb() {
	if (!mThumbTexture) {
		// init
		mThumbTexture = ci::gl::Texture::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
		string filename = mName + ".jpg";
		fs::path fr = getAssetPath("") / "thumbs" / filename;
		if (!fs::exists(fr)) {
			createThumb();
			Surface s8(mThumbTexture->createSource());
			writeImage(writeFile(getAssetPath("") / "thumbs" / filename), s8);
		}
		else {
			// load from disk
			mThumbTexture = ci::gl::Texture::create(loadImage(fr));
		}
	}
	return mThumbTexture;
}

gl::GlslProgRef VDShader::getShader() {
	return mShader;
};

VDShader::~VDShader() {
	CI_LOG_V("VDShader destructor");
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
