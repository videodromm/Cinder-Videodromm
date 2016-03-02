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
		void						update();
		void						load();
		void						save();
		Color						getBackgroundColor() { return mBackgroundColor; };
		float						getExposure() { return mExposure; };
		void						setExposure(float aExposure);
		const int					maxBlendMode = 28;
		// tap tempo
		float						mTempo;
		float						iDeltaTime;
		float						iTempoTime;
		float						iTimeFactor;
		bool						mUseTimeWithTempo;
		// exposure
		float						defaultExposure;
		float						minExposure;
		bool						tExposure;
		bool						autoExposure;
		const float					maxExposure = 3.0;
		//zoom
		float						defaultZoom;
		float						minZoom;
		float						maxZoom;
		bool						tZoom;
		bool						autoZoom;
		//chromatic
		float						defaultChromatic;
		float						minChromatic;
		float						maxChromatic;
		bool						tChromatic;
		bool						autoChromatic;
		//ratio
		float						defaultRatio;
		float						minRatio;
		float						maxRatio;
		bool						tRatio;
		bool						autoRatio;
		// z position
		float						defaultZPos;
		float						minZPos;
		float						maxZPos;
		bool						tZPos;
		bool						autoZPos;
		// Rotation Speed
		float						defaultRotationSpeed;
		float						minRotationSpeed;
		float						maxRotationSpeed;
		bool						tRotationSpeed;
		bool						autoRotationSpeed;
		// animation functions
		void						resetExposure();
		void						tempoExposure();
		void						resetZoom();
		void						tempoZoom();
		void						resetChromatic();
		void						tempoChromatic();
		void						resetRatio();
		void						tempoRatio();
		void						resetZPos();
		void						tempoZPos();
		void						resetRotationSpeed();
		void						tempoRotationSpeed();
		void						lockChromatic() { autoChromatic = !autoChromatic; };
		void						lockRatio() { autoRatio = !autoRatio; };
		void						lockExposure() { autoExposure = !autoExposure; };
		void						lockZoom() { autoZoom = !autoZoom; };
		void						lockZPos() { autoZPos = !autoZPos; };
		void						lockRotationSpeed() { autoRotationSpeed = !autoRotationSpeed; };
		// tempo
		void						tapTempo();
		void						setTimeFactor(const int &aTimeFactor);
		int							iBar;
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Live json params
		Parameter<Color>			mBackgroundColor;
		Parameter<float>			mExposure;
		Parameter<string>			mText;
		// timed animation
		ci::Timer					timer;
		std::deque <double>			buffer;
		void						calculateTempo();
		int							counter;
		double						averageTime;
		double						currentTime;
		double						startTime;
		float						previousTime;
		int							beatIndex; //0 to 3

	};



}