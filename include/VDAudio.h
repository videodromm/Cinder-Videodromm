#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
// audio
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"
// Settings
#include "VDSettings.h"
// Logger
#include "VDLog.h"


using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDAudio instance
	typedef std::shared_ptr<class VDAudio> VDAudioRef;

	class VDAudio {
	public:
		VDAudio(VDSettingsRef aVDSettings);

		static VDAudioRef		create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDAudio>(new VDAudio(aVDSettings));
		}
		ci::gl::TextureRef				getTexture();
		void							update();
		//void							loadWaveFile(string aFilePath);
		float*							getSmallSpectrum() { return arr; };

	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// audio
		audio::InputDeviceNodeRef		mLineIn;
		audio::MonitorSpectralNodeRef	mMonitorLineInSpectralNode;
		//audio::MonitorSpectralNodeRef	mMonitorWaveSpectralNode;

		vector<float>					mMagSpectrum;

		//audio::SamplePlayerNodeRef		mSamplePlayerNode;
		//audio::SourceFileRef			mSourceFile;
		audio::MonitorSpectralNodeRef	mScopeLineInFmt;
		float							arr[7];
		// number of frequency bands of our spectrum
		static const int				kBands = 1024;

		// texture
		gl::TextureRef					mTexture;
		unsigned char					dTexture[1024];
	};


}