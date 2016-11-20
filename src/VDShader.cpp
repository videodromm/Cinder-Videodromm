#include "VDShader.h"

using namespace VideoDromm;

VDShader::VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath, string aVextexShaderFilePath) {
	mFragmentShaderFilePath = aFragmentShaderFilePath;
	mVertexShaderFilePath = aVextexShaderFilePath;
	mValid = false;
	// shadertoy include
	shaderInclude = loadString(loadAsset("shadertoy.inc"));
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mError = "";
	gl::Fbo::Format format;
	mThumbFbo = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, format.depthTexture());

	loadVertexStringFromFile(mVertexShaderFilePath);
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
		mVertexShaderString = loadString(loadFile(mVertexFile));
		mVertexShaderFilePath = mVertexFile.string();
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
		mFragFile = getAssetPath("") / "mixfbo.frag";
	}
	else {
		mFragFile = aFileName;
	}
	if (!fs::exists(mFragFile)) {
		mError = mFragFile.string() + " does not exist";
		CI_LOG_V(mError);
		mFragFile = getAssetPath("") / "0.frag";
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
	// we would like a name
	if (aName.length() == 0) aName = toString((int)getElapsedSeconds());
	// name of the shader
	mName = aName;
	// filename to save
	aName += ".frag";
	mValid = false;
	// load fragment shader
	CI_LOG_V("setFragmentString, live loading" + mName);
	try
	{
		CI_LOG_V(mOriginalFragmentString);
		// shadertoy: 
		// change void mainImage( out vec4 fragColor, in vec2 fragCoord ) to void main(void)
		std::regex pattern{ "mainImage" };
		std::string replacement{ "main" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		CI_LOG_V("1 " + mOriginalFragmentString);
		pattern = { "out vec4 fragColor," };
		replacement = { "void" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		CI_LOG_V("2 " + mOriginalFragmentString);
		//pattern = { "fragColor" };
		//replacement = { "gl_FragColor" };
		//mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		//CI_LOG_V("3 " + mOriginalFragmentString);
		pattern = { "in vec2 fragCoord" };
		replacement = { "" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		CI_LOG_V("4 " + mOriginalFragmentString);
		pattern = { "vec2 fragCoord" };
		replacement = { "" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		CI_LOG_V("5 " + mOriginalFragmentString);
		//pattern = { "fragCoord" };
		//replacement = { "gl_FragCoord.xy" };
		//mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		// html glslEditor:
		// change vec2 u_resolution to vec3 iResolution
		pattern = { "2 u_r" };
		replacement = { "3 iR" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		pattern = { "u_r" };
		replacement = { "iR" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		pattern = { "u_tex" };
		replacement = { "iChannel" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		pattern = { "2 u_mouse" };
		replacement = { "4 iMouse" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		pattern = { "u_m" };
		replacement = { "iM" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		pattern = { "u_time" };
		replacement = { "iGlobalTime" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		pattern = { "u_" };
		replacement = { "i" };
		mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
		CI_LOG_V(mOriginalFragmentString);

		// change texture2D to texture for version > 150?
		// change fragCoord to gl_FragCoord
		// change gl_FragColor to fragColor

		// check if uniforms were declared in the file
		std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
		if (foundUniform == std::string::npos) {
			CI_LOG_V("loadFragmentStringFromFile, no uniforms found, we add from shadertoy.inc");
			aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mOriginalFragmentString;
		}
		else {
			aFragmentShaderString = "/* " + aName + " */\n" + mOriginalFragmentString;
		}

		// before compilation save .frag file to inspect errors
		fs::path receivedFile = getAssetPath("") / "glsl" / "received" / aName;
		ofstream mFragReceived(receivedFile.string(), std::ofstream::binary);
		mFragReceived << aFragmentShaderString;
		mFragReceived.close();
		CI_LOG_V("Received file saved:" + receivedFile.string());

		// try to compile
		mShader = gl::GlslProg::create(mVertexShaderString, aFragmentShaderString);
		// update only if success
		mFragmentShaderString = aFragmentShaderString;
		CI_LOG_V(aName + " live edited, loaded and compiled");
		mValid = true;
		auto &uniforms = mShader->getActiveUniforms();
		for (const auto &uniform : uniforms) {
			CI_LOG_V(mShader->getLabel() + ", uniform name:" + uniform.getName());
			// if uniform is handled
			if (mVDAnimation->isExistingUniform(uniform.getName())) {
				int uniformType = mVDAnimation->getUniformType(uniform.getName());
				switch (uniformType)
				{
				case 0:
					// float
					mShader->uniform(uniform.getName(), mVDAnimation->getFloatUniformValueByName(uniform.getName()));
					mCurrentUniformsString += "uniform float " + uniform.getName() + "; // " + toString(mVDAnimation->getFloatUniformValueByName(uniform.getName())) + "\n";
					break;
				case 1:
					// sampler2D
					mShader->uniform(uniform.getName(), mVDAnimation->getSampler2DUniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform sampler2D " + uniform.getName() + "; // " + toString(mVDAnimation->getSampler2DUniformValue(uniform.getName())) + "\n";
					break;
				case 2:
					// vec2
					mShader->uniform(uniform.getName(), mVDAnimation->getVec2UniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform vec2 " + uniform.getName() + "; // " + toString(mVDAnimation->getVec2UniformValue(uniform.getName())) + "\n";
					break;
				case 3:
					// vec3
					mShader->uniform(uniform.getName(), mVDAnimation->getVec3UniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform vec3 " + uniform.getName() + "; // " + toString(mVDAnimation->getVec3UniformValue(uniform.getName())) + "\n";
					break;
				case 4:
					// vec4
					mShader->uniform(uniform.getName(), mVDAnimation->getVec4UniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform vec4 " + uniform.getName() + "; // " + toString(mVDAnimation->getVec4UniformValue(uniform.getName())) + "\n";
					break;
				case 5:
					// int
					mShader->uniform(uniform.getName(), mVDAnimation->getIntUniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform int " + uniform.getName() + "; // " + toString(mVDAnimation->getIntUniformValue(uniform.getName())) + "\n";
					break;
				case 6:
					// bool
					mShader->uniform(uniform.getName(), mVDAnimation->getBoolUniformValue(uniform.getName()));
					mCurrentUniformsString += "uniform bool " + uniform.getName() + "; // " + toString(mVDAnimation->getBoolUniformValue(uniform.getName())) + "\n";
					break;
				default:
					break;
				}
			}
			else {
				if (uniform.getName() != "ciModelViewProjection") {
					mNotFoundUniformsString += "not found " + uniform.getName() + "\n";
				}
			}
		}
		mNotFoundUniformsString += "*/\n";
		// save .frag file to migrate old shaders
		fs::path processedFile = getAssetPath("") / "glsl" / "processed" / aName;
		ofstream mFragProcessed(processedFile.string(), std::ofstream::binary);
		//mFragProcessed << mNotFoundUniformsString << mCurrentUniformsString << mOriginalFragmentString;
		mFragProcessed << mNotFoundUniformsString << mOriginalFragmentString;
		mFragProcessed.close();
		CI_LOG_V("processed file saved:" + processedFile.string());
		createThumb();

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
	mVDSettings->mWebSocketsMsg = mError;
	return mValid;
}

/*void VDShader::fromXml(const XmlTree &xml) {
	mId = xml.getAttributeValue<string>("id", "");
	string mVertfile = xml.getAttributeValue<string>("vertfile", "passthru.vert");
	mName = xml.getAttributeValue<string>("fragfile", "0.frag");
	CI_LOG_V("shader id " + mId + " name " + mName);
	mVertexFile = getAssetPath("") / mVDSettings->mAssetsPath / mVertfile;
	loadVertexStringFromFile(mVertexShaderFilePath);

	mFragFile = getAssetPath("") / mVDSettings->mAssetsPath / mName;
	loadFragmentStringFromFile(mFragFile.string());
}*/
void VDShader::createThumb() {
	// draw using the shader
	mThumbFbo->bindFramebuffer();
	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);

	mShader->bind();
	mShader->uniform("iGlobalTime", mVDAnimation->getFloatUniformValueByIndex(49));
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
}
string VDShader::getName() {
	return mName;
}

VDShader::~VDShader() {
	CI_LOG_V("VDShader destructor");
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
