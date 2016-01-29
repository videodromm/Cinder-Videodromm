#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"

// Settings
#include "VDSettings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDImageSequence instance
	typedef std::shared_ptr<class VDImageSequence> VDImageSequenceRef;
	// structure for loaded image files
	//struct sequence {
	//};
	class VDImageSequence {
	public:
		VDImageSequence(VDSettingsRef aVDSettings, string aFilePath, int aIndex);

		static VDImageSequenceRef		create(VDSettingsRef aVDSettings, string aFilePath, int aIndex)
		{
			return shared_ptr<VDImageSequence>(new VDImageSequence(aVDSettings, aFilePath, aIndex));
		}
		void						update();
		//void						createFromDir(string filePath, int index);
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
		ci::gl::TextureRef			getCurrentSequenceTexture();

	private:
		// Settings
		VDSettingsRef				mVDSettings;

		vector<Texta>				textas;
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
		int							mIndex;
		int							mFramesLoaded;
		int							mPlayheadPosition;
		bool						mLoadingPaused;
		bool						mLoadingFilesComplete;
		bool						mPlaying;
		int							mSpeed;
		vector<ci::gl::TextureRef>	mSequenceTextures;

	};


}