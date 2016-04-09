#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
// audio
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"

#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/Rand.h"
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
		VDInputTexture(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aFboIndex, string aFilePathOrText, bool isTopDown, int aType);

		static VDInputTextureRef		create(VDSettingsRef aVDSettings, VDAnimationRef aAnimation, int aFboIndex, string aFilePathOrText, bool isTopDown, int aType)
		{
			return shared_ptr<VDInputTexture>(new VDInputTexture(aVDSettings, aAnimation, aFboIndex, aFilePathOrText, isTopDown, aType));
		}

		ci::gl::TextureRef			getTexture();
		//void						setTexture(ci::gl::TextureRef aTexture);
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		int							getType() { return mType; };
		int							getFboIndex() { return mFboIndex; };
		void						setFboIndex(int aFboIndex) { mFboIndex = aFboIndex; };
		string						getFolder() { return mFolder; };
		string						getName();
		bool						isFlipH() { return mFlipH; };
		bool						isFlipV() { return mFlipV; };
		int							getTextureWidth();
		int							getTextureHeight();
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
		void						rewindMovie();
		void						fastforwardMovie();
		float						movieTime;
		void						playMovie();
		// audio
		void						loadWaveFile(string aFilePath);
		float*						getSmallSpectrum() { return arr; };
		bool						isAudio() { return mIsAudio; };

	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;

		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		int							mType;
		int							mFboIndex; // fbo index
		string						mFilePathOrText;
		bool						mTopDown;
		int							mWidth;
		int							mHeight;
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
		// audio
		audio::InputDeviceNodeRef		mLineIn;
		audio::MonitorSpectralNodeRef	mMonitorLineInSpectralNode;
		audio::MonitorSpectralNodeRef	mMonitorWaveSpectralNode;
		audio::SamplePlayerNodeRef		mSamplePlayerNode;
		audio::SourceFileRef			mSourceFile;
		audio::MonitorSpectralNodeRef	mScopeLineInFmt;

		vector<float>					mMagSpectrum;

		float							arr[7];
		// number of frequency bands of our spectrum
		static const int				kBands = 1024;

		// audio texture
		unsigned char					dTexture[1024];
		bool							mIsAudio;
	};


}