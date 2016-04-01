#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

// Settings
#include "VDSettings.h"
// shaders
#include "VDShaders.h"
// Logger
#include "VDLog.h"

#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS

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
		//gl::FboRef					getFboRef();
		ci::gl::TextureRef			getTexture();
		void						setTexture(ci::gl::TextureRef aTexture);
		ivec2						getSize();
		Area						getBounds();
		GLuint						getId();
		int							getType() { return mType; };
		string						getName();
		bool						isFlipH() { return mFlipH; };
		bool						isFlipV() { return mFlipV; };
		void						setShaderIndex(int aShaderIndex);
		int							loadPixelFragmentShader(string aFilePath);
		int							setGLSLString(string pixelFrag, string name);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Shaders
		VDShadersRef				mVDShaders;

		gl::FboRef					mFbo;
		string						mName;
		bool						mFlipV;
		bool						mFlipH;
		int							mWidth;
		int							mHeight;
		int							mType;

		//! Shaders
		gl::GlslProgRef				mShader;
		int							mShaderIndex;
		string						mError;
		string						mFragFile;
		string						mFragFileName;
		string						mShaderName;
		bool						validFrag;
		//! default vertex shader
		std::string					mPassthruVextexShaderString;
		//! default fragment shader
		std::string					mPassthruFragmentShaderString;
		//! passthru shader
		gl::GlslProgRef				mPassThruShader;
		// include shader lines
		std::string					shaderInclude;

		//! Textures
		ci::gl::TextureRef			mTexture, mTexture1;
	};


}