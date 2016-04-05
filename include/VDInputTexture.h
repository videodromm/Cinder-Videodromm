#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

// Settings
#include "VDSettings.h"
// Logger
#include "VDLog.h"
// Animation
#include "VDAnimation.h"
// hap codec movie
#include "MovieHap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDInputTexture instance
	typedef std::shared_ptr<class VDInputTexture> VDInputTextureRef;
	typedef std::chrono::high_resolution_clock Clock;

	class VDInputTexture {
	public:
		VDInputTexture(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aIndex, string aFilePathOrText, bool isTopDown, int aType);

		static VDInputTextureRef		create(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aIndex, string aFilePathOrText, bool isTopDown, int aType)
		{
			return shared_ptr<VDInputTexture>(new VDInputTexture(aVDSettings, aAnimation, aIndex, aFilePathOrText, isTopDown, aType));
		}

		ci::gl::TextureRef			getTexture();
		void						setTexture(ci::gl::TextureRef aTexture);
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		int							getType() { return mType; };
		int							getIndex() { return mIndex; };
		void						setIndex(int aIndex) { mIndex = aIndex; };
		string						getFolder() { return mFolder; };
		string						getName();
		bool						isFlipH() { return mFlipH; };
		bool						isFlipV() { return mFlipV; };
		// image sequence
		bool						isSequence() { return mIsSequence; };
		void						update();
		void						playPauseSequence();
		void						syncToBeat();
		void						stopSequence();
		void						toggleLoadingFromDisk();
		void						stopLoading();
		int							getPlayheadPosition();
		void						setPlayheadPosition(int position);

		int							getSpeed();
		void						setSpeed(int speed);
		void						reverseSequence();
		bool						isLoadingFromDisk();
		bool						isValid(){ return mFramesLoaded > 0; };
		int							getMaxFrame();
		// text
		bool						isText() { return mIsText; };
		// hap codec movie
		bool						isMovie() { return mIsMovie; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;

		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		int							mType;
		int							mIndex; // check it's fbo index
		string						mFilePathOrText;
		bool						mTopDown;
		//! Textures
		ci::gl::TextureRef			mTexture;
		// text
		bool						mIsText;

		// hap codec movie
		qtime::MovieGlHapRef		mMovie;
		void						loadMovieFile(const fs::path &path);
		bool						mLoopVideo;
		bool						mIsMovie;
		// Image sequence
		int							playheadFrameInc;
		void						loadNextImageFromDisk();
		void						updateSequence();
		bool						mIsSequence;
		string						mFolder;
		string						mPrefix;
		string						mExt;
		int							mNumberOfDigits;
		int							mNextIndexFrameToTry;
		int							mCurrentLoadedFrame;
		int							mFramesLoaded;
		int							mPlayheadPosition;
		bool						mLoadingPaused;
		bool						mLoadingFilesComplete;
		bool						mPlaying;
		int							mSpeed;
		bool						mSyncToBeat;
		vector<ci::gl::TextureRef>	mSequenceTextures;
	};


}