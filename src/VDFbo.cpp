#include "VDFbo.h"

using namespace VideoDromm;

VDFbo::VDFbo(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, string aName, int aWidth, int aHeight, int aType) {
	mVDSettings = aVDSettings;
	mVDShaders = aShadersRef;
	mName = aName;
	mWidth = aWidth;
	mHeight = aHeight;
	mType = aType;
	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	mRenderFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	mFlipV = false;
	mFlipH = true;
	mShaderIndex = 0;
	gl::enableDepthRead();
	gl::enableDepthWrite();
	// temp
	mPassthruVextexShader = loadString(loadAsset("shadertoy.vert"));
	shaderInclude = loadString(loadAsset("shadertoy.inc"));
	std::string fs = shaderInclude + loadString(loadAsset("10.glsl"));
	aShader = gl::GlslProg::create(mPassthruVextexShader, fs);
	mTexture = gl::Texture::create(loadImage(loadAsset("iam.jpg")));
	mTexture1 = gl::Texture::create(loadImage(loadAsset("noise.jpg")), gl::Texture::Format().loadTopDown());

}
void VDFbo::setTexture(ci::gl::TextureRef aTexture) {
	mTexture = aTexture;
}

void VDFbo::setShaderIndex(int aShaderIndex) {
	mShaderIndex = aShaderIndex;
}
ci::gl::TextureRef VDFbo::getTexture() {
	return mFbo->getColorTexture();
}
ci::gl::TextureRef VDFbo::getProcessedTexture() {

	gl::ScopedFramebuffer fbScp(mRenderFbo);
	gl::clear(Color(0.25, 0.5f, 1.0f));// Color::black());
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());

	//aShader = mVDShaders->getShader(mShaderIndex).shader;
	

	gl::ScopedGlslProg shaderScp(aShader);
	//aShader->bind();
	aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	aShader->uniform("iResolution", vec3(mWidth, mHeight, 1.0));
	//aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	//aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", 1.0f);
	aShader->uniform("iChannel0", 0);
	aShader->uniform("iChannel1", 1);
	/*aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
	aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
	aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
	aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
	aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
	aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
	aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
	aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
	aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
	aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	aShader->uniform("iRatio", mVDSettings->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio); 
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);*/
	aShader->uniform("iRenderXY", vec2(0.0, 0.0));
	/*aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
	aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
	aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	aShader->uniform("iExposure", mVDSettings->controlValues[14]);
	aShader->uniform("iFade", (int)mVDSettings->iFade);
	aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	aShader->uniform("iLight", (int)mVDSettings->iLight);
	aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	aShader->uniform("iTransition", mVDSettings->iTransition);
	aShader->uniform("iAnim", mVDSettings->iAnim.value());
	aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
	aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	aShader->uniform("iDebug", (int)mVDSettings->iDebug);
	aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	aShader->uniform("iFps", mVDSettings->iFps);
	// TODO aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	// TODO aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	aShader->uniform("iBeat", mVDSettings->iBeat);
	aShader->uniform("iSeed", mVDSettings->iSeed);
	aShader->uniform("iFlipH", mFlipH);
	aShader->uniform("iFlipV", mFlipV);*/
	mTexture->bind(0);
	mTexture1->bind(1);

	//mFbo->getColorTexture()->bind(0);
	gl::drawSolidRect(Rectf(20, 20, mWidth*2, mHeight*2));
	//mFbo->getColorTexture()->unbind();

	return mRenderFbo->getColorTexture();
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