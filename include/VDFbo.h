#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

// Settings
#include "VDSettings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDFbo instance
	typedef std::shared_ptr<class VDFbo> VDFboRef;

	class VDFbo {
	public:
		VDFbo(VDSettingsRef aVDSettings, string aName);

		static VDFboRef		create(VDSettingsRef aVDSettings, string aName)
		{
			return shared_ptr<VDFbo>(new VDFbo(aVDSettings, aName));
		}
		gl::FboRef					getFboRef();
		ci::gl::TextureRef			getFboTexture();
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		string						getName();
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		gl::FboRef					mFbo;
		string						mName;
		static const int			FBO_WIDTH = 640, FBO_HEIGHT = 480;

	};


}