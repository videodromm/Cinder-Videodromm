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
		VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath, string aFragmentShaderString = "");
		//void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFragmentShaderFilePath, string aFragmentShaderString = "")
		{
			return shared_ptr<VDShader>(new VDShader(aVDSettings, aVDAnimation, aFragmentShaderFilePath, aFragmentShaderString));
		}
		//void fromXml(const XmlTree &xml);
		gl::GlslProgRef					getShader();
		string							getName();
		bool							loadFragmentStringFromFile(string aFileName);
		string							getFragmentString() {
			if (mFragmentShaderString.empty()) mFragmentShaderString = "void main(void){vec2 uv = gl_FragCoord.xy / iResolution.xy;fragColor = texture(iChannel0, uv);}";
			if (mFragmentShaderString.size() < 1 || mFragmentShaderString.size() > 256000) mFragmentShaderString = "void main(void){vec2 uv = gl_FragCoord.xy / iResolution.xy;fragColor = texture(iChannel0, uv);}";
			return mFragmentShaderString;
		};
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
