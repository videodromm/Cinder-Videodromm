#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

// Settings
#include "VDSettings.h"
// Logger
#include "VDLog.h"


using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDInputTexture instance
	typedef std::shared_ptr<class VDInputTexture> VDInputTextureRef;

	class VDInputTexture {
	public:
		VDInputTexture(VDSettingsRef aVDSettings, int aIndex, string aFilePath, bool isTopDown, bool isSequence);

		static VDInputTextureRef		create(VDSettingsRef aVDSettings, int aIndex, string aFilePath, bool isTopDown, bool isSequence)
		{
			return shared_ptr<VDInputTexture>(new VDInputTexture(aVDSettings, aIndex, aFilePath, isTopDown, isSequence));
		}

		ci::gl::TextureRef			getTexture();
		void						setTexture(ci::gl::TextureRef aTexture);
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		int							getType() { return mType; };
		string						getName();
		bool						isFlipH() { return mFlipH; };
		bool						isFlipV() { return mFlipV; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;

		gl::FboRef					mFbo;
		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		int							mWidth;
		int							mHeight;
		int							mType;
		int							mIndex;
		string						mFilePath;
		bool						mTopDown;
		bool						mSequence;
		//! Textures
		ci::gl::TextureRef			mTexture;
	};


}