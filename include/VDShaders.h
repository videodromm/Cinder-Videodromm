#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include "Resources.h"
// Settings
#include "VDSettings.h"
// Logger
#include "VDLog.h"
// Watchdog
#include "Watchdog.h"

#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDShaders instance
	typedef std::shared_ptr<class VDShaders> VDShadersRef;

	class VDShaders {
	public:
		VDShaders(VDSettingsRef aVDSettings);
		virtual					~VDShaders();
		void update();
		static VDShadersRef	create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDShaders>(new VDShaders(aVDSettings));
		}

		string							getFragFileName() { return mFragFileName; };
		string							getFragFullPath() { return mFragFile; };

		bool							setFragString(string pixelFrag);
		int								setGLSLString(string pixelFrag, string name);
		int								setGLSLStringAtIndex(string pixelFrag, string name, int index);
		int								setGLSLPixelShaderAtIndex(gl::GlslProgRef pixelFrag, string name, int index);
		bool							loadTextFile(string aFilePath);

		string							getFragError();

		bool							isValidFrag() { return validFrag; };
		bool							isValidVert() { return validVert; };

		gl::GlslProgRef					getMixShader() { return mMixShader; };
		gl::GlslProgRef					getWarpShader() { return mWarpShader; };
		gl::GlslProgRef					getLiveShader() { return mLiveShader; };
		void							setupLiveShader();
		string							loadLiveShader(string frag);
		Shada							getShader(int aIndex) { return mFragmentShaders[aIndex]; };
		int								getCount() { return mFragmentShaders.size(); };

		int								loadPixelFragmentShader(string aFilePath);
		void							removePixelFragmentShaderAtIndex(int index);
		int								loadPixelFragmentShaderAtIndex(string aFilePath, int index);
		string							getFileName(string aFilePath);
		string							getNewFragFileName(string aFilePath);
		string							getFragStringFromFile(string fileName);
		void							renderPreviewShader();

		void							createThumbsFromDir(string filePath);
		void							setShaderMicroSeconds(int index, int micro);

		// get the include shader
		std::string						getShaderInclude() { return shaderInclude; };

	private:
		// Settings
		VDSettingsRef					mVDSettings;

		string							mFragFile;
		string							mFragFileName;

		bool							validFrag;
		bool							validVert;
		bool							liveError;
		bool							isLiveShaderSetup;
		fs::path						liveFragFile;
		// new
		int								mCurrentRenderShader;
		int								mCurrentPreviewShader;
		int								mShaderIndex;
		//! shaders
		vector<Shada>					mFragmentShaders;

		string							fileName, previousFileName, currentFileName, mixFileName;
		string							mError;

		//! find index for insert/update in mFragmentShaders
		int								findFragmentShaderIndex(int index, string name);

		// default vertex shader
		std::string						mPassthruVextexShader;
		// include shader lines
		std::string						shaderInclude;
		// mix shader
		gl::GlslProgRef					mMixShader;
		// live coding shader
		gl::GlslProgRef					mLiveShader;
		//! warp shader
		gl::GlslProgRef					mWarpShader;
		//! PassThru shader
		gl::GlslProgRef					mPassThruShader;

	};
}
