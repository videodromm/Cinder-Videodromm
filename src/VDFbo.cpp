#include "VDFbo.h"

using namespace VideoDromm;

VDFbo::VDFbo(VDSettingsRef aVDSettings, string aName) {
	mVDSettings = aVDSettings;
	mName = aName;
	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo::create(FBO_WIDTH, FBO_HEIGHT, format.depthTexture());
}

ci::gl::TextureRef VDFbo::getFboTexture() {

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