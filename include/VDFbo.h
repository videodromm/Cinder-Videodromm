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
		VDFbo(VDSettingsRef aVDSettings, string aName, int aWidth, int aHeight, int aType);

		static VDFboRef		create(VDSettingsRef aVDSettings, string aName, int aWidth, int aHeight, int aType)
		{
			return shared_ptr<VDFbo>(new VDFbo(aVDSettings, aName, aWidth, aHeight, aType));
		}
		gl::FboRef					getFboRef();
		ci::gl::TextureRef			getTexture();
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		int							getType() { return mType; };
		string						getName();
		bool						isFlipH() { return mFlipH; };
		bool						isFlipV() { return mFlipV; };
		void						setShader(gl::GlslProgRef aShader);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		gl::FboRef					mFbo;
		gl::FboRef					mRenderFbo;
		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		int							mWidth;
		int							mHeight;
		int							mType;
		gl::GlslProgRef				mShader;
	};


}