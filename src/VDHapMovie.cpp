#include "VDHapMovie.h"

using namespace VideoDromm;

VDHapMovie::VDHapMovie(VDSettingsRef aVDSettings, string aFilePath) {
	mVDSettings = aVDSettings;
	mFilePath = aFilePath;

	// movie
	mLoopVideo = false;

	// Loads all files contained in the supplied folder and creates Textures from them
	// init the Movie
	playheadFrameInc = 1;
	//mIndex = 0;
	mFramesLoaded = 0;
	mCurrentLoadedFrame = 0;
	mPlaying = false;
	mSpeed = 1;
	mExt = "png";
	mPrefix = "none";
	mPlayheadPosition = 0;

	sprintf_s(mFolder, "");
	// find the folder name for display in the ui
	if (mFilePath.find_last_of("\\") != std::string::npos) {
		int slashIndex = mFilePath.find_last_of("\\") + 1;
		string folder = mFilePath.substr(slashIndex);
		sprintf_s(mFolder, "%s", folder.c_str());
	}


}
void VDHapMovie::loadMovieFile(const fs::path &moviePath)
{
	try {
		mMovie.reset();
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGlHap::create(moviePath);
		mLoopVideo = (mMovie->getDuration() < 30.0f);
		mMovie->setLoop(mLoopVideo);
		mMovie->play();
	}
	catch (ci::Exception &e)
	{
		console() << string(e.what()) << std::endl;
		console() << "Unable to load the movie." << std::endl;
	}

}

void VDHapMovie::update() {
	int newPosition;
	if (mMovieTextures.size() > 0) {
		// Call on each frame to update the playhead
		if (mPlaying) {
			newPosition = mPlayheadPosition + (playheadFrameInc * mSpeed);
			if (newPosition < 0) newPosition = mMovieTextures.size() - 1;
			if (newPosition > mMovieTextures.size() - 1) newPosition = 0;
		}
		else {
			newPosition = (int)(mVDSettings->iBeat % mMovieTextures.size());
		}
		mPlayheadPosition = max(0, min(newPosition, (int)mMovieTextures.size() - 1));
	}
}
//Begins playback of Movie
void VDHapMovie::playMovie() {

	mPlaying = true;
}
// Pauses playback
void VDHapMovie::pauseMovie() {

	mPlaying = false;
}

// Stops playback and resets the playhead to zero
void VDHapMovie::stopMovie() {

	mPlaying = false;
	mPlayheadPosition = 0;
}

int VDHapMovie::getMaxFrames() {

	return mFramesLoaded;
}
int VDHapMovie::getPlayheadPosition() {

	return mPlayheadPosition;
}
// Seek to a new position in the Movie
void VDHapMovie::setPlayheadPosition(int position) {

	mPlayheadPosition = max(0, min(position, (int)mMovieTextures.size() - 1));
	
}


int VDHapMovie::getSpeed() {

	return mSpeed;
}
void VDHapMovie::setSpeed(int speed) {
	mSpeed = speed;
}


ci::gl::TextureRef VDHapMovie::getTexture() {

	if (mPlayheadPosition > mFramesLoaded) {
		//error
	}

	return mMovieTextures[mPlayheadPosition];
}
