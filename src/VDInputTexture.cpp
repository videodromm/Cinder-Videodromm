#include "VDInputTexture.h"

using namespace VideoDromm;

VDInputTexture::VDInputTexture(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aIndex, string aFilePathOrText, bool isTopDown, int aType) {

	CI_LOG_V("VDInputTexture constructor");
	mVDSettings = aVDSettings;
	mVDAnimation = aAnimation;
	mIndex = aIndex;
	mFilePathOrText = aFilePathOrText;
	mTopDown = isTopDown;
	mType = aType;

	mIsSequence = (mType == 1);
	mIsText = (mType == 2);

	mFlipV = false;
	mFlipH = true;

	// Loads all files contained in the supplied folder and creates Textures from them
	// init the sequence vars
	playheadFrameInc = 1;
	mLoadingFilesComplete = true;
	mLoadingPaused = false;
	mFramesLoaded = 0;
	mCurrentLoadedFrame = 0;
	mNextIndexFrameToTry = 0;
	mPlaying = false;
	mSpeed = 1;
	mExt = "png";
	mPrefix = "none";
	mNextIndexFrameToTry = 0;
	mPlayheadPosition = 0;
	mNumberOfDigits = 4;
	mSyncToBeat = false;

	if (mIsSequence) {
		// find the folder name for display in the ui
		if (mFilePathOrText.find_last_of("\\") != std::string::npos) {
			int slashIndex = mFilePathOrText.find_last_of("\\") + 1;
			mFolder = mFilePathOrText.substr(slashIndex);
		}
		bool noValidFile = true; // if no valid files in the folder, we keep existing vector
		bool firstIndexFound = false;
		int i = 0;
		string anyImagefileName = "";
		// loading 2000 files takes a while, I load only the first one
		for (fs::directory_iterator it(mFilePathOrText); it != fs::directory_iterator(); ++it)
		{
			if (fs::is_regular_file(*it))
			{
				string fileName = it->path().filename().string();
				if (fileName.find_last_of(".") != std::string::npos) {
					int dotIndex = fileName.find_last_of(".");
					mExt = fileName.substr(dotIndex + 1);
					if (mExt == "png" || mExt == "jpg") {
						anyImagefileName = fileName;
					}
					// get the prefix for the image sequence
					// the files are named from p0000.jpg to p2253.jpg for instance
					// sometimes only 3 digits : l000 this time
					// find the first digit
					int firstDigit = fileName.find_first_of("0123456789");
					// if valid image sequence (contains a digit)
					if (firstDigit > -1) {
						mNumberOfDigits = dotIndex - firstDigit;
						int prefixIndex = fileName.find_last_of(".") - mNumberOfDigits;//-4 or -3
						mPrefix = fileName.substr(0, prefixIndex);
						if (!firstIndexFound) {
							firstIndexFound = true;
							mNextIndexFrameToTry = std::stoi(fileName.substr(prefixIndex, dotIndex));
						}
					}
				}
				// only if proper image sequence
				if (firstIndexFound) {
					if (mExt == "png" || mExt == "jpg") {
						if (noValidFile) {
							// we found a valid file
							noValidFile = false;
							mSequenceTextures.clear();
							// reset playhead to the start
							//mPlayheadPosition = 0;
							mLoadingFilesComplete = false;
							loadNextImageFromDisk();
							mPlaying = true;
						}
					}
				}

			}
		}
		// init if no valid file found with the last image found in the folder
		if (noValidFile) {
			if (anyImagefileName.length() > 0) {
				mIsSequence = false;
				if (mTopDown) {
					mTexture = gl::Texture::create(loadImage(anyImagefileName), gl::Texture::Format().loadTopDown());
				}
				else {
					mTexture = gl::Texture::create(loadImage(anyImagefileName));
				}
			}
		}
	}
	else if (mIsText) {
		TextLayout layout;
		layout.clear(ColorA(0.0f, 0.0f, 0.0f, 1.0f));
		//layout.setFont(Font("Flamingo Solide Normal", 24));
		fs::path fontFile = getAssetPath("") / "fonts" / "FlamingoSolide.otf";
		layout.setFont(Font(loadFile(fontFile), 72));
		//layout.setFont(Font("Arial", 24));
		layout.setColor(Color(1.0f, 1.0f, 1.0f));
		layout.addCenteredLine(mFilePathOrText);
	
		layout.setFont(Font(loadFile(fontFile), 24));
		layout.addCenteredLine("W H E N   O N E   S U N   R I S E S");

		layout.addCenteredLine("E V E R Y T H I N G   I S   I L L U M I N A T E D");

		Surface8u rendered = layout.render(false, false);
		mTexture = gl::Texture::create(rendered, gl::Texture::Format().loadTopDown());
	}
	else {
		if (mTopDown) {
			mTexture = gl::Texture::create(loadImage(mFilePathOrText));
		}
		else {
			mTexture = gl::Texture::create(loadImage(mFilePathOrText));
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

			fs::path fileToLoad = mFilePathOrText + "/" + mPrefix + restOfFileName + "." + mExt;
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
			if (mSyncToBeat) {
				newPosition = (int)(((int)(mVDAnimation->iBar / mVDAnimation->iBeatIndex)) % mSequenceTextures.size());
			}
			else {
				newPosition = mPlayheadPosition;
			}
		}
		mPlayheadPosition = max(0, min(newPosition, (int)mSequenceTextures.size() - 1));
	}
}

void VDInputTexture::update() {
	if (mIsSequence) {
		updateSequence();
		//if (!mLoadingFilesComplete) loadNextImageFromDisk();
	}
}
// play/pause sequence
void VDInputTexture::playPauseSequence() {

	mPlaying = !mPlaying;
}
// sync to beat
void VDInputTexture::syncToBeat() {

	mSyncToBeat = !mSyncToBeat;
}

// Stops playback and resets the playhead to zero
void VDInputTexture::stopSequence() {

	mPlaying = false;
	mPlayheadPosition = 0;
}

int VDInputTexture::getMaxFrame() {

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
	if (mIsSequence) {
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