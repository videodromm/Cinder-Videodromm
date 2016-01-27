#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"

// json
#include "cinder/json.h"
// Settings
#include "VDSettings.h"
// Live json params
#include "LiveParam.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace live;

namespace VideoDromm
{
	// stores the pointer to the VDAnimation instance
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;

	class VDAnimation {
	public:		
		VDAnimation(VDSettingsRef aVDSettings);

		static VDAnimationRef		create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDAnimation>(new VDAnimation( aVDSettings));
		}
		void						load();
		void						save();
		Color						getBackgroundColor() { return mBackgroundColor; };
		float						getBpm() { return mBpm; };
		float						getFpb() { return mFpb; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Live json params
		Param<Color>				mBackgroundColor;
		Param<float>				mBpm, mFpb;
	};


}