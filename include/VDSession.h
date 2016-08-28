#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
// json
#include "cinder/Json.h"
// Settings
#include "VDSettings.h"
// Logger
#include "VDLog.h"

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
		int							getEndFrame() { return mEndFrame; };
		void						setEndFrame(int frame) { mEndFrame = frame; };
		string						getWaveFileName() { return mWaveFileName; };
		int							getWavePlaybackDelay() { return mWavePlaybackDelay; };
		string						getMovieFileName() { return mMovieFileName; };
		int							getMoviePlaybackDelay() { return mMoviePlaybackDelay; };
		bool						hasMovie() { return mMovieFileName.length() > 0; };
		int							getFadeInDelay() { return mFadeInDelay; };
		int							getFadeOutDelay() { return mFadeOutDelay; };
		string						getImageSequencePath() { return mImageSequencePath; };
		bool						hasImageSequencePath() { return mImageSequencePath.length() > 0; };
		int							iBeatsPerBar;
		string						getText() { return mText; };
		int							getTextStart() { return mTextPlaybackDelay; };
		int							getTextEnd() { return mTextPlaybackEnd; };
		bool						hasText() { return mText.length() > 0; };
		// shaders
		bool						hasShaderLeft() { return mShaderLeft.length() > 0; };
		bool						hasShaderRight() { return mShaderRight.length() > 0; };
		string						getShaderLeft() { return mShaderLeft; };
		string						getShaderRight() { return mShaderRight; };

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
		float						mOriginalBpm;
		// target fps
		float						mTargetFps;
		// files and paths
		string						mWaveFileName;
		string						mMovieFileName;
		string						mImageSequencePath;
		// delay
		int							mWavePlaybackDelay;
		int							mMoviePlaybackDelay;
		int							mFadeInDelay;
		int							mFadeOutDelay;
		int							mEndFrame;
		// font and text 
		string						mText;
		int							mTextPlaybackDelay;
		int							mTextPlaybackEnd;
		// shaders
		string						mShaderLeft;
		string						mShaderRight;
	};

}
