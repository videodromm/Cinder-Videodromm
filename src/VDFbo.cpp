#include "VDFbo.h"

using namespace VideoDromm;

VDFbo::VDFbo(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, string aName, int aWidth, int aHeight, int aType) {
	mVDSettings = aVDSettings;
	mVDShaders = aShadersRef;
	mName = aName;
	mWidth = aWidth;
	mHeight = aHeight;
	mType = aType;
	CI_LOG_V("VDFbo constructor");

	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	//mRenderFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	mFlipV = false;
	mFlipH = true;
	mShaderIndex = 0;
	gl::enableDepthRead();
	gl::enableDepthWrite();
	// temp
	mShaderName = "passthru";
	try
	{
		fs::path vertexFile = getAssetPath("") / "passthru.vert";
		if (fs::exists(vertexFile)) {
			mPassthruVextexShaderString = loadString(loadAsset("passthru.vert"));
		}
		else
		{
			CI_LOG_V("passthru.vert does not exist, should quit");
		}
		fs::path fragFile = getAssetPath("") / "passthru.frag";
		if (fs::exists(fragFile)) {		
			mPassthruFragmentShaderString = loadString(loadAsset("passthru.frag"));
		}
		else
		{
			CI_LOG_V("passthru.frag does not exist, should quit");
		}
		mPassThruShader = gl::GlslProg::create(mPassthruVextexShaderString, mPassthruFragmentShaderString);
		mShader = gl::GlslProg::create(mPassthruVextexShaderString, mPassthruFragmentShaderString);
	
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile passthru shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load passthru shader:" + string(e.what()));
	}
	shaderInclude = loadString(loadAsset("shadertoy.inc"));
	//load mix shader
	/*try
	{
		fs::path mixFragFile = getAssetPath("") / "mix.frag";
		if (fs::exists(mixFragFile))
		{
			mShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(mixFragFile));
		}
		else
		{
			mVDSettings->mMsg = "mix.frag does not exist, should quit";
			mVDSettings->newMsg = true;
			CI_LOG_V("mix.frag does not exist, should quit");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		mVDSettings->mMsg = "mix.frag unable to load/compile shader: " + mError;
		mVDSettings->newMsg = true;
		CI_LOG_V("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		mVDSettings->mMsg = "mix.frag unable to load shader: " + mError;
		mVDSettings->newMsg = true;
		CI_LOG_V("unable to load shader:" + string(e.what()));
	}*/

	std::string fs = shaderInclude + loadString(loadAsset("10.glsl"));
	//mShader = gl::GlslProg::create(mPassthruVextexShaderString, fs);
	mTexture = gl::Texture::create(loadImage(loadAsset("0.jpg")));
	mTexture1 = gl::Texture::create(loadImage(loadAsset("0.jpg")), gl::Texture::Format().loadTopDown());

}
int VDFbo::loadPixelFragmentShader(string aFilePath)
{
	int rtn = -1;
	// reset 
	mVDSettings->iFade = false;
	mVDSettings->controlValues[22] = 1.0f;
	try
	{
		fs::path fr = aFilePath;
		string name = "unknown";
		string mFile = fr.string();
		if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
		mFragFileName = name;
		if (fs::exists(fr))
		{
			validFrag = false;
			std::string fs = shaderInclude + loadString(loadFile(aFilePath));
			rtn = setGLSLString(fs, name);
			if (rtn > -1)
			{
				CI_LOG_V(mFragFile + " loaded and compiled");
				mVDSettings->mMsg = name + " loadPixelFragmentShader success";
				mVDSettings->newMsg = true;
				//mFragmentShadersNames[rtn] = name;
			}
		}
		else
		{
			CI_LOG_V(mFragFile + " does not exist");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V(aFilePath + " unable to load/compile shader err:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(aFilePath + " unable to load shader err:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}

	return rtn;
}

int VDFbo::setGLSLString(string pixelFrag, string name)
{
	int foundIndex = 0;

	try
	{
		mShader = gl::GlslProg::create(mPassthruVextexShaderString, pixelFrag);
		mShaderName = name;
		
		//preview the new loaded shader
		mVDSettings->mPreviewFragIndex = foundIndex;
		CI_LOG_V("setGLSLString success");
		mVDSettings->mMsg = name + " setGLSLString success";
		mVDSettings->newMsg = true;
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc exc)
	{
		validFrag = false;
		// TODO CI_LOG_E("Problem Compiling ImGui::Renderer shader " << exc.what());
		foundIndex = -1;
		mError = string(exc.what());
		mVDSettings->mMsg = "setGLSLString file: " + name + " error:" + mError;
		mVDSettings->newMsg = true;
		CI_LOG_V(mVDSettings->mMsg);
	}
	return foundIndex;
}

void VDFbo::setTexture(ci::gl::TextureRef aTexture) {
	mTexture = aTexture;
}

void VDFbo::setShaderIndex(int aShaderIndex) {
	mShaderIndex = aShaderIndex;
}
/*ci::gl::TextureRef VDFbo::getTexture() {
	return mFbo->getColorTexture();
}*/
ci::gl::TextureRef VDFbo::getTexture() {

	gl::ScopedFramebuffer fbScp(mFbo);
	gl::clear(Color(0.25, 0.5f, 1.0f));// Color::black());
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());//ivec2(1024,480));
	// hehe gl::ScopedViewport scpVp(ivec2(40 * mVDSettings->iGlobalTime), mFbo->getSize());//ivec2(1024,480));
	//gl::viewport(getWindowSize());

	//mShader = mVDShaders->getShader(mShaderIndex).shader;
	gl::ScopedGlslProg shaderScp(mShader);
	//mShader->bind();
	mShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	mShader->uniform("iResolution", vec3(mWidth, mHeight, 1.0));
	mShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	mShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	mShader->uniform("iZoom", 1.0f);
	mShader->uniform("iChannel0", 0);
	mShader->uniform("iChannel1", 1);
	/*mShader->uniform("iChannel0", mVDSettings->iChannels[0]);
	mShader->uniform("iChannel1", mVDSettings->iChannels[1]);
	mShader->uniform("iChannel2", mVDSettings->iChannels[2]);
	mShader->uniform("iChannel3", mVDSettings->iChannels[3]);
	mShader->uniform("iChannel4", mVDSettings->iChannels[4]);
	mShader->uniform("iChannel5", mVDSettings->iChannels[5]);
	mShader->uniform("iChannel6", mVDSettings->iChannels[6]);
	mShader->uniform("iChannel7", mVDSettings->iChannels[7]);*/
	mShader->uniform("iAudio0", 0);
	mShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	mShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	mShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	mShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	mShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	mShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
	mShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
	mShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	mShader->uniform("iRatio", mVDSettings->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio); 
	mShader->uniform("width", 1);
	mShader->uniform("height", 1);
	mShader->uniform("iRenderXY", vec2(0.0, 0.0));
	mShader->uniform("iAlpha", mVDSettings->controlValues[4]);
	mShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	mShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	mShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
	mShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	mShader->uniform("iExposure", mVDSettings->controlValues[14]);
	mShader->uniform("iFade", (int)mVDSettings->iFade);
	mShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	mShader->uniform("iLight", (int)mVDSettings->iLight);
	mShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	mShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	mShader->uniform("iTransition", mVDSettings->iTransition);
	mShader->uniform("iAnim", mVDSettings->iAnim.value());
	mShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	mShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
	mShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	mShader->uniform("iDebug", (int)mVDSettings->iDebug);
	mShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	mShader->uniform("iFps", mVDSettings->iFps);
	// TODO mShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	// TODO mShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	mShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	mShader->uniform("iBeat", mVDSettings->iBeat);
	mShader->uniform("iSeed", mVDSettings->iSeed);
	mShader->uniform("iFlipH", mFlipH);
	mShader->uniform("iFlipV", mFlipV);
	//mTexture->bind(0);
	//mTexture1->bind(1);

	gl::ScopedTextureBind tex(mTexture);
	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight));	
	//gl::drawSolidRect(Rectf(2, 2, mWidth*1.9, mHeight*1.9));

	return mFbo->getColorTexture();
}
ivec2 VDFbo::getSize() {

	return mFbo->getSize();
}
gl::FboRef VDFbo::getFboRef() {

	return mFbo;
}
Area VDFbo::getBounds() {

	return mFbo->getBounds();
}
GLuint VDFbo::getId() {

	return mFbo->getId();
}
string VDFbo::getName(){

	return mName;
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS