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
		VDFbo(VDSettingsRef aVDSettings, string aName, int aWidth, int aHeight);

		static VDFboRef		create(VDSettingsRef aVDSettings, string aName, int aWidth, int aHeight)
		{
			return shared_ptr<VDFbo>(new VDFbo(aVDSettings, aName, aWidth, aHeight));
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
		bool						mFlipV;
		bool						mFlipH;
		int							mWidth;
		int							mHeight;
	};


}