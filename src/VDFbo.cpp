#include "VDFbo.h"

using namespace VideoDromm;

VDFbo::VDFbo(VDSettingsRef aVDSettings, string aName, int aWidth, int aHeight) {
	mVDSettings = aVDSettings;
	mName = aName;
	mWidth = aWidth;
	mHeight = aHeight;
	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	mFlipV = false;
	mFlipH = false;
}

ci::gl::TextureRef VDFbo::getTexture() {

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