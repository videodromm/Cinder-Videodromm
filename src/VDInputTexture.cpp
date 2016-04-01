#include "VDInputTexture.h"

using namespace VideoDromm;

VDInputTexture::VDInputTexture(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aIndex, string aFilePath, bool isTopDown, bool isSequence) {
	mVDSettings = aVDSettings;
	mVDAnimation = aAnimation;
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

void VDInputTexture::loadNextImageFromDisk() {
	if (!mLoadingPaused) {

		if (!mLoadingFilesComplete) {
			// thank you Omar!
			char restOfFileName[32];
			if (mNumberOfDigits == 4) {

				sprintf(restOfFileName, "%04d", mNextIndexFrameToTry);
			}
			else {

				sprintf(restOfFileName, "%03d", mNextIndexFrameToTry);
			}

			fs::path fileToLoad = mFilePath + "/" + mPrefix + restOfFileName + "." + mExt;
			if (fs::exists(fileToLoad)) {
				// start profiling
				auto start = Clock::now();

				mSequenceTextures.push_back(ci::gl::Texture::create(loadImage(fileToLoad), gl::Texture::Format().loadTopDown()));
				mCurrentLoadedFrame = mFramesLoaded;
				mFramesLoaded++;
				auto end = Clock::now();
				auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				int milli = msdur.count();

				mVDSettings->mMsg = fileToLoad.string() + " loaded in ms " + toString(milli);
			}
			else {

				mVDSettings->mMsg = fileToLoad.string() + " does not exist";
				if (mFramesLoaded > 0) {
					// if frames have been loaded we hit the last file of the image sequence at this point
					mVDSettings->mMsg = "last image loaded";
					mLoadingFilesComplete = true;
				}
			}
			mVDSettings->newMsg = true;
			// increment counter for next filename
			mNextIndexFrameToTry++;
			if (mNextIndexFrameToTry > 9999 && mNumberOfDigits == 4) mLoadingFilesComplete = true;
			if (mNextIndexFrameToTry > 999 && mNumberOfDigits == 3) mLoadingFilesComplete = true;
		}
	}
}
void VDInputTexture::updateSequence() {
	int newPosition;
	// TODO check: getTexture()
	if (mSequenceTextures.size() > 0) {
		// Call on each frame to update the playhead
		if (mPlaying) {
			newPosition = mPlayheadPosition + (playheadFrameInc * mSpeed);
			if (newPosition < 0) newPosition = mSequenceTextures.size() - 1;
			if (newPosition > mSequenceTextures.size() - 1) newPosition = 0;
		}
		else {
			newPosition = (int)(((int)(mVDAnimation->iBar / mVDAnimation->iBeatIndex)) % mSequenceTextures.size());
		}
		mPlayheadPosition = max(0, min(newPosition, (int)mSequenceTextures.size() - 1));
	}
}

void VDInputTexture::update() {

	updateSequence();
	//if (!mLoadingFilesComplete) loadNextImageFromDisk();
}
//Begins playback of sequence
void VDInputTexture::playSequence() {

	mPlaying = true;
}
// Pauses playback
void VDInputTexture::pauseSequence() {

	mPlaying = false;
}

// Stops playback and resets the playhead to zero
void VDInputTexture::stopSequence() {

	mPlaying = false;
	mPlayheadPosition = 0;
}

int VDInputTexture::getMaxFrames() {

	return mFramesLoaded;
}
int VDInputTexture::getPlayheadPosition() {

	return mPlayheadPosition;
}
// Seek to a new position in the sequence
void VDInputTexture::setPlayheadPosition(int position) {

	mPlayheadPosition = max(0, min(position, (int)mSequenceTextures.size() - 1));
	if (!mLoadingFilesComplete) {
		loadNextImageFromDisk();
	}
}

void VDInputTexture::reverseSequence() {
	mSpeed *= -1;
}
int VDInputTexture::getSpeed() {

	return mSpeed;
}
void VDInputTexture::setSpeed(int speed) {
	mSpeed = speed;
}
bool VDInputTexture::isLoadingFromDisk() {

	return mLoadingFilesComplete;
}

void VDInputTexture::toggleLoadingFromDisk() {

	mLoadingPaused = !mLoadingPaused;
}
ci::gl::TextureRef VDInputTexture::getTexture() {
	if (mSequence) {
		if (mPlayheadPosition > mFramesLoaded) {
			//error
			mPlayheadPosition = 0;
		}
		if (!mLoadingFilesComplete) loadNextImageFromDisk();

		if (mPlaying)  {
			updateSequence();
		}
		mTexture = mSequenceTextures[mPlayheadPosition];
	}

	return mTexture;
}

void VDInputTexture::stopLoading() {

	mLoadingPaused = true;
}