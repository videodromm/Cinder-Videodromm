#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include "Resources.h"
// Logger
#include "VDLog.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Watchdog
#include "Watchdog.h"

#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDShader instance
	typedef std::shared_ptr<class VDShader>	VDShaderRef;
	typedef std::vector<VDShaderRef>			VDShaderList;


	class VDShader {
	public:
		VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath, string aVextexShaderFilePath);
		virtual					~VDShader();
		void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath, string aVextexShaderFilePath)
		{
			return shared_ptr<VDShader>(new VDShader(aVDSettings, aVDAnimation, aFragmentShaderFilePath, aVextexShaderFilePath));
		}
		void fromXml(const XmlTree &xml);
		gl::GlslProgRef					getShader();
		string							getName() { return mName; };
		void							loadVertexStringFromFile(string aFileName);
		void							loadFragmentStringFromFile(string aFileName);
		string							getFragmentString() { return mFragmentShaderString; };
		bool							setFragmentString(string aFragmentShaderString);
		// thumb image
		ci::gl::Texture2dRef			getThumb();
		void							createThumb();
        bool							isValid() { return mValid; };
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;

		string							mId;
		gl::GlslProgRef					mShader;
		string							mName;
		string							mText;
		bool							mActive;
		int								mMicroSeconds;
		string							mError;
        bool							mValid;

		//! vertex shader
		std::string						mVextexShaderString;
		std::string						mVextexShaderFilePath;
		fs::path						mVertexFile;
		//! fragment shader
		std::string						mFragmentShaderString;
		std::string						mFragmentShaderFilePath;
		fs::path						mFragFile;
		// include shader lines
		std::string						shaderInclude;
		// fbo
		gl::FboRef						mThumbFbo;
		ci::gl::Texture2dRef			mThumbTexture;
	};
}
