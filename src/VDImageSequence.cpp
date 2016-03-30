#include "VDImageSequence.h"
/*
TODO load each file between two ticks
TODO ensure an image texture exists
TODO pass the folder to loadfrom in the constructor
*/
using namespace VideoDromm;

VDImageSequence::VDImageSequence(VDSettingsRef aVDSettings, VDAnimationRef aAnimationRef, string aFilePath) {
	mVDSettings = aVDSettings;
	mVDAnimation = aAnimationRef;
	mFilePath = aFilePath;

	// Loads all files contained in the supplied folder and creates Textures from them
	// init the sequence
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
	sprintf_s(mFolder, "");
	// find the folder name for display in the ui
	if (mFilePath.find_last_of("\\") != std::string::npos) {
		int slashIndex = mFilePath.find_last_of("\\") + 1;
		string folder = mFilePath.substr(slashIndex);
		sprintf_s(mFolder, "%s", folder.c_str());
	}
	bool noValidFile = true; // if no valid files in the folder, we keep existing vector
	bool firstIndexFound = false;
	int i = 0;
	// loading 2000 files takes a while, I load only the first one
	for (fs::directory_iterator it(mFilePath); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName = it->path().filename().string();
			if (fileName.find_last_of(".") != std::string::npos) {
				int dotIndex = fileName.find_last_of(".");
				mExt = fileName.substr(dotIndex + 1);
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
}
void VDImageSequence::loadNextImageFromDisk() {
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

				mSequenceTextures.push_back(ci::gl::Texture::create(loadImage(fileToLoad)));
				mCurrentLoadedFrame = mFramesLoaded;
				mFramesLoaded++;
				auto end = Clock::now();
				auto nsdur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				int micro = nsdur.count();

				mVDSettings->mMsg = fileToLoad.string() + " loaded in ns " + toString(micro);
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
void VDImageSequence::updateSequence() {
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
			newPosition = (int)(mVDAnimation->iBar % mSequenceTextures.size());
		}
		mPlayheadPosition = max(0, min(newPosition, (int)mSequenceTextures.size() - 1));
	}
}

void VDImageSequence::update() {

	updateSequence();
	if (!mLoadingFilesComplete) loadNextImageFromDisk();
}
//Begins playback of sequence
void VDImageSequence::playSequence() {

	mPlaying = true;
}
// Pauses playback
void VDImageSequence::pauseSequence() {

	mPlaying = false;
}

// Stops playback and resets the playhead to zero
void VDImageSequence::stopSequence() {

	mPlaying = false;
	mPlayheadPosition = 0;
}

int VDImageSequence::getMaxFrames() {

	return mFramesLoaded;
}
int VDImageSequence::getPlayheadPosition() {

	return mPlayheadPosition;
}
// Seek to a new position in the sequence
void VDImageSequence::setPlayheadPosition(int position) {

	mPlayheadPosition = max(0, min(position, (int)mSequenceTextures.size() - 1));
	if (!mLoadingFilesComplete) {
		loadNextImageFromDisk();
	}
}

void VDImageSequence::reverseSequence() {
	mSpeed *= -1;
}
int VDImageSequence::getSpeed() {

	return mSpeed;
}
void VDImageSequence::setSpeed(int speed) {
	mSpeed = speed;
}
bool VDImageSequence::isLoadingFromDisk() {

	return mLoadingFilesComplete;
}

void VDImageSequence::toggleLoadingFromDisk() {

	mLoadingPaused = !mLoadingPaused;
}

ci::gl::TextureRef VDImageSequence::getTexture() {
	if (mPlayheadPosition > mFramesLoaded) {
		//error
		mPlayheadPosition = 0;
	}
	// TODO temporary:
	if (mPlaying)  {
		if (!mLoadingFilesComplete) loadNextImageFromDisk();
		mPlayheadPosition = (int)(mVDAnimation->iBar % mSequenceTextures.size());

	}
	mPlayheadPosition = max(0, min(mPlayheadPosition, (int)mSequenceTextures.size() - 1));

	return mSequenceTextures[mPlayheadPosition];
}
void VDImageSequence::stopLoading() {

	mLoadingPaused = true;
}
