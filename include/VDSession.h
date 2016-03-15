#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
// json
#include "cinder/json.h"
// Settings
#include "VDSettings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm {

	typedef std::shared_ptr<class VDSession> VDSessionRef;


	class VDSession
	{
	public:
		VDSession(VDSettingsRef aVDSettings);
		static VDSessionRef			create(VDSettingsRef aVDSettings);
		bool						save();
		void						restore();
		void						reset();
		void						resetSomeParams();

		float						getBpm() { return mBpm; };
		float						getTargetFps() { return mTargetFps; };
		void						setBpm(float aBpm) { mBpm = aBpm; };
		string						getWaveFileName() { return mWaveFileName; };
		int							getWavePlaybackDelay() { return mWavePlaybackDelay; };
		string						getMovieFileName() { return mMovieFileName; };
		int							getMoviePlaybackDelay() { return mMoviePlaybackDelay; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		const string				sessionFileName = "session.json";
		fs::path					sessionPath;
		// fbo 
		bool						mFlipV;
		bool						mFlipH;
		// tempo
		float						mBpm, mFpb;
		// target fps
		float						mTargetFps;
		// file names
		string						mWaveFileName;
		string						mMovieFileName;
		// delay
		int							mWavePlaybackDelay;
		int							mMoviePlaybackDelay;
	};

}
