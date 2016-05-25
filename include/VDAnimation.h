#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"

// json
#include "cinder/json.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
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
		VDAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession);

		static VDAnimationRef		create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDAnimation>(new VDAnimation( aVDSettings, aVDSession));
		}
		void						update();
		void						load();
		void						save();
		Color						getBackgroundColor() { return mBackgroundColor; };
		float						getExposure() { return mExposure; };
		void						setExposure(float aExposure);
		bool						getAutoBeatAnimation() { return mAutoBeatAnimation; };
		const int					maxBlendMode = 28;
		// tap tempo
		//float						mTempo;
		float						iDeltaTime;
		float						iTempoTime;
		float						iTimeFactor;
		bool						mUseTimeWithTempo;
		float						iTempoTimeBeatPerBar;
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
		//int							iBar;
		int							iBeatIndex; //1 to beatsperbar
		// animation
		int							currentScene;
		//int							getBadTV(int frame);
		// keyboard
		bool						handleKeyDown(KeyEvent &event);
		bool						handleKeyUp(KeyEvent &event);
		// audio
		float						maxVolume;
		bool						mUseLineIn;
		float						iFreqs[7];
		// OSC/MIDI/JSON controlled UI and params
		map<int, float>				controlValues;

	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// Live json params
		fs::path					mJsonFilePath;
		Parameter<Color>			mBackgroundColor;
		Parameter<float>			mExposure;
		Parameter<string>			mText;
		Parameter<bool>				mAutoBeatAnimation;

		// timed animation
		ci::Timer					timer;
		std::deque <double>			buffer;
		void						calculateTempo();
		int							counter;
		double						averageTime;
		double						currentTime;
		double						startTime;
		float						previousTime;
		float						previousTimeBeatPerBar;
		JsonTree					mData;
		void						loadAnimation();
		void						saveAnimation();

		std::unordered_map<int, float>	mBadTV;

	};
}