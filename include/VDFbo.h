#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

// Settings
#include "VDSettings.h"
// shaders
#include "VDShaders.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDFbo instance
	typedef std::shared_ptr<class VDFbo> VDFboRef;

	class VDFbo {
	public:
		VDFbo(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, string aName, int aWidth, int aHeight, int aType);

		static VDFboRef		create(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, string aName, int aWidth, int aHeight, int aType)
		{
			return shared_ptr<VDFbo>(new VDFbo(aVDSettings, aShadersRef, aName, aWidth, aHeight, aType));
		}
		gl::FboRef					getFboRef();
		ci::gl::TextureRef			getTexture();
		void						setTexture(ci::gl::TextureRef aTexture);
		ci::gl::TextureRef			getProcessedTexture();
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		int							getType() { return mType; };
		string						getName();
		bool						isFlipH() { return mFlipH; };
		bool						isFlipV() { return mFlipV; };
		void						setShaderIndex(int aShaderIndex);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Shaders
		VDShadersRef				mVDShaders;

		gl::FboRef					mFbo;
		gl::FboRef					mRenderFbo;
		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		int							mWidth;
		int							mHeight;
		int							mType;
		gl::GlslProgRef				aShader;
		int							mShaderIndex;

		// temporary
		// default vertex shader
		std::string						mPassthruVextexShader;
		// include shader lines
		std::string						shaderInclude;
		ci::gl::TextureRef				mTexture, mTexture1;
	};


}