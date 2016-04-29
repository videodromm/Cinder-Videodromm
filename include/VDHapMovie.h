#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"

// Settings
#include "VDSettings.h"
// hap codec movie
#include "MovieHap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDHapMovie instance
	typedef std::shared_ptr<class VDHapMovie> VDHapMovieRef;
	// structure for loaded image files
	//struct Movie {
	//};
	class VDHapMovie {
	public:
		VDHapMovie(VDSettingsRef aVDSettings, string aFilePath);

		static VDHapMovieRef		create(VDSettingsRef aVDSettings, string aFilePath)
		{
			return shared_ptr<VDHapMovie>(new VDHapMovie(aVDSettings, aFilePath));
		}
		void						update();
		void						playMovie();
		void						stopMovie();
		void						pauseMovie();
		int							getPlayheadPosition();
		void						setPlayheadPosition(int position);

		int							getSpeed();
		void						setSpeed(int speed);
		bool						isValid(){ return mFramesLoaded > 0; };
		int							getMaxFrames();
		ci::gl::TextureRef			getTexture();

	private:
		// Settings
		VDSettingsRef				mVDSettings;

		qtime::MovieGlHapRef		mMovie;
		void						loadMovieFile(const fs::path &path);
		bool						mLoopVideo;

		int							playheadFrameInc;
		void						updateMovie();
		string						mFilePath;
		char						mFolder[32];
		string						mPrefix;
		string						mExt;
		int							mCurrentLoadedFrame;
		int							mFramesLoaded;
		int							mPlayheadPosition;
		bool						mPlaying;
		int							mSpeed;
		vector<ci::gl::TextureRef>	mMovieTextures;

	};


}