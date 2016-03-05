#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

// parameters
#include "VDSettings.h"
// logger
#include "VDLog.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDUtils instance
	typedef std::shared_ptr<class VDUtils> VDUtilsRef;

	class VDUtils {
	public:
		VDUtils(VDSettingsRef aVDSettings);
		static VDUtilsRef	create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDUtils>(new VDUtils(aVDSettings));
		}
		void						setup();
		// TODO move to private when shadamixa upgraded
		void						createWarpFbos();
		void						splitWarp(int fboWidth, int fboHeight);
		int							getWindowsResolution();
		float						formatFloat(float f);
		Area						getSrcAreaLeftOrTop();
		Area						getSrcAreaRightOrBottom();
		// file system
		fs::path					getPath(string path);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		Area						mSrcAreaLeftOrTop; // left or top
		Area						mSrcAreaRightOrBottom; // right or bottom

	};
}

