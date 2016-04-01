#include "VDInputTexture.h"

using namespace VideoDromm;

VDInputTexture::VDInputTexture(VDSettingsRef aVDSettings, int aIndex, string aFilePath, bool isTopDown, bool isSequence) {
	mVDSettings = aVDSettings;
	mIndex = aIndex;
	mFilePath = aFilePath;
	mTopDown = isTopDown;
	mSequence = isSequence;
	CI_LOG_V("VDInputTexture constructor");

	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	//mRenderFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
	mFlipV = false;
	mFlipH = true;

	if (mSequence) {
		//TODO
	}
	else {
		if (mTopDown) {
			mTexture = gl::Texture::create(loadImage(mFilePath), gl::Texture::Format().loadTopDown());
		}
		else {
			mTexture = gl::Texture::create(loadImage(mFilePath));
		}
	}

}


void VDInputTexture::setTexture(ci::gl::TextureRef aTexture) {
	mTexture = aTexture;
}


ci::gl::TextureRef VDInputTexture::getTexture() {

	return mTexture;
}
ivec2 VDInputTexture::getSize() {

	return mTexture->getSize();
}

Area VDInputTexture::getBounds() {

	return mTexture->getBounds();
}
GLuint VDInputTexture::getId() {

	return mTexture->getId();
}
string VDInputTexture::getName(){

	return mName;
}

