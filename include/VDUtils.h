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
		void						update();
		void						shutdown();
		// TODO move to private when shadamixa upgraded
		void						createWarpFbos();
		int							getWindowsResolution();
		float						formatFloat(float f);

		// animation
		// exposure
		float						defaultExposure;
		float						minExposure;
		bool						tExposure;
		bool						autoExposure;
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
		// file system
		fs::path					getPath(string path);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// tempo
		void						calculateTempo();
		std::deque <double>			buffer;
		ci::Timer					timer;
		int							counter;
		double						averageTime;
		double						currentTime;
		double						startTime;
		float						previousTime;
		int							beatIndex; //0 to 3

	};
}

