#include "VDShader.h"

using namespace VideoDromm;

VDShader::VDShader(VDSettingsRef aVDSettings, string aFragmentShaderFilePath = "", string aVextexShaderFilePath = "") {
	mFragmentShaderFilePath = aFragmentShaderFilePath;
	mVextexShaderFilePath = aVextexShaderFilePath;

	// shadertoy include
	shaderInclude = loadString(loadAsset("shadertoy.inc"));

	loadVertexStringFromFile(mVextexShaderFilePath);
	loadFragmentStringFromFile(mFragmentShaderFilePath);
	CI_LOG_V("VDShaders constructor");
	mVDSettings = aVDSettings;
	mError = "";
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
void VDShader::loadFragmentStringFromFile(string aFileName) {
	// load fragment shader
	try
	{
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
		mFragmentShaderFilePath = mFragFile.string();
		mFragmentShaderString = loadString(loadFile(mFragFile));
		std::size_t foundUniform = mFragmentShaderString.find("uniform");

		if (foundUniform == std::string::npos) {
			
			mFragmentShaderString = shaderInclude + mFragmentShaderString;
		}


		mShader = gl::GlslProg::create(mVextexShaderString, mFragmentShaderString);
		mShader->setLabel(mFragFile.string());
		CI_LOG_V(mFragFile.string() + " loaded and compiled");
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile mixfbo fragment shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load mixfbo fragment shader:" + string(e.what()));
	}
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
gl::GlslProgRef VDShader::getShader() { 
	return mShader; 
};
VDShader::~VDShader() {
	CI_LOG_V("VDShader destructor");
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
