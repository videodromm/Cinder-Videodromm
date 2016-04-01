#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Session
#include "VDSession.h";

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDImageSequence instance
	typedef std::shared_ptr<class VDImageSequence> VDImageSequenceRef;
	typedef std::chrono::high_resolution_clock Clock;

	class VDImageSequence {
	public:
		VDImageSequence(VDSettingsRef aVDSettings, VDAnimationRef aAnimationRef, VDSessionRef aVDSession, string aFilePath);

		static VDImageSequenceRef		create(VDSettingsRef aVDSettings, VDAnimationRef aAnimationRef, VDSessionRef aVDSession, string aFilePath)
		{
			return shared_ptr<VDImageSequence>(new VDImageSequence(aVDSettings, aAnimationRef, aVDSession, aFilePath));
		}
		void						update();
		void						playSequence();
		void						stopSequence();
		void						pauseSequence();
		void						toggleLoadingFromDisk();
		void						stopLoading();
		int							getPlayheadPosition();
		void						setPlayheadPosition(int position);

		int							getSpeed();
		void						setSpeed(int speed);
		void						reverseSequence();
		bool						isLoadingFromDisk();
		bool						isValid(){ return mFramesLoaded > 0; };
		int							getMaxFrames();
		ci::gl::TextureRef			getTexture();

	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Session
		VDSessionRef				mVDSession;

		int							playheadFrameInc;
		void						loadNextImageFromDisk();
		void						updateSequence();
		string						mFilePath;
		char						mFolder[32];
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
		vector<ci::gl::TextureRef>	mSequenceTextures;

	};


}