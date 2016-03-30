#include "VDFbo.h"

using namespace VideoDromm;

VDFbo::VDFbo(VDSettingsRef aVDSettings, string aName, int aWidth, int aHeight, int aType) {
	mVDSettings = aVDSettings;
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

}
void VDFbo::setShader(gl::GlslProgRef aShader) {
	mShader = aShader;
}
ci::gl::TextureRef VDFbo::getTexture() {
	if (mShader) {
		gl::ScopedFramebuffer fbScp(mRenderFbo);
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());
		mShader->bind();
		mShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		mShader->uniform("iResolution", vec3(mWidth, mHeight, 1.0));
		mShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		mShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		mShader->uniform("iZoom", mVDSettings->iZoomRight);
		mShader->uniform("iChannel0", mVDSettings->iChannels[0]);
		mShader->uniform("iChannel1", mVDSettings->iChannels[1]);
		mShader->uniform("iChannel2", mVDSettings->iChannels[2]);
		mShader->uniform("iChannel3", mVDSettings->iChannels[3]);
		mShader->uniform("iChannel4", mVDSettings->iChannels[4]);
		mShader->uniform("iChannel5", mVDSettings->iChannels[5]);
		mShader->uniform("iChannel6", mVDSettings->iChannels[6]);
		mShader->uniform("iChannel7", mVDSettings->iChannels[7]);
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
		mShader->uniform("iRenderXY", mVDSettings->mWarp2RenderXY);
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

		mFbo->getColorTexture()->bind(0);
		gl::drawSolidRect(Rectf(0, 0, mWidth, mHeight));
		mFbo->getColorTexture()->unbind();

		return mRenderFbo->getColorTexture();	
	}
	else
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