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
// std regex
#include <regex>

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
		//void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath, string aVextexShaderFilePath)
		{
			return shared_ptr<VDShader>(new VDShader(aVDSettings, aVDAnimation, aFragmentShaderFilePath, aVextexShaderFilePath));
		}
		//void fromXml(const XmlTree &xml);
		gl::GlslProgRef					getShader();
		string							getName();
		void							loadVertexStringFromFile(string aFileName);
		bool							loadFragmentStringFromFile(string aFileName);
		string							getFragmentString() { return mFragmentShaderString; };
		string							getVertexString() { return mVertexShaderString; };
		bool							setFragmentString(string aFragmentShaderString, string aName = "");
		// thumb image
		//ci::gl::Texture2dRef			getThumb();
		bool							isValid() { return mValid; };
		bool							isActive() { return mActive; };
		void							setActive(bool active) { mActive = active; };
		void							removeShader();
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
		std::string						mVertexShaderString;
		std::string						mVertexShaderFilePath;
		fs::path						mVertexFile;
		//! fragment shader
		std::string						mFragmentShaderString;
		std::string						mFragmentShaderFilePath;
		fs::path						mFragFile;
		// include shader lines
		std::string						shaderInclude;
		// fbo
		//gl::FboRef						mThumbFbo;
		//ci::gl::Texture2dRef			mThumbTexture;
	};
}
