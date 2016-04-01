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
		VDInputTexture(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aIndex, string aFilePath, bool isTopDown, bool isSequence);

		static VDInputTextureRef		create(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aIndex, string aFilePath, bool isTopDown, bool isSequence)
		{
			return shared_ptr<VDInputTexture>(new VDInputTexture(aVDSettings, aAnimation, aIndex, aFilePath, isTopDown, isSequence));
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
		bool						isSequence() { return mSequence; };
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
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;

		//gl::FboRef					mFbo;
		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		//int							mWidth;
		//int							mHeight;
		int							mType;
		int							mIndex; // check it's fbo index
		string						mFilePath;
		bool						mTopDown;
		bool						mSequence;
		//! Textures
		ci::gl::TextureRef			mTexture;
		// Image sequence
		int							playheadFrameInc;
		void						loadNextImageFromDisk();
		void						updateSequence();

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
		vector<ci::gl::TextureRef>	mSequenceTextures;
	};


}