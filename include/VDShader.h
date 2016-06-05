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
	typedef std::shared_ptr<class VDShaders>	VDShadersRef;
	typedef std::vector<VDShadersRef>			VDShaderList;

	struct VDShader
	{
		gl::GlslProgRef				shader;
		string						name;
		string						text;
		bool						active;
		int							microseconds;
	};
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
		// TO CHECK text int								setGLSLPixelShaderAtIndex(gl::GlslProgRef pixelFrag, string name, int index);
		bool							loadTextFile(string aFilePath);

		string							getFragError();

		bool							isValidFrag() { return validFrag; };
		bool							isValidVert() { return validVert; };

		//Shada							getShader(int aIndex) { return mFragmentShaders[aIndex]; };
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

		std::string						getShaderString(int index);
		// get the include shader
		std::string						getShaderInclude() { return shaderInclude; };
		// fbo shader
		std::string						loadFboPixelFragmentShader(string aFilePath);

	private:
		// Settings
		VDSettingsRef					mVDSettings;

		string							mFragFile;
		string							mFragFileName;

		bool							validFrag;
		bool							validVert;

		// new
		int								mCurrentRenderShader;
		int								mCurrentPreviewShader;
		int								mShaderIndex;
		//! shaders
		vector<VDShader>				mFragmentShaders;

		string							fileName, previousFileName, currentFileName, mixFileName;
		string							mError;

		//! find index for insert/update in mFragmentShaders
		int								findFragmentShaderIndex(int index, string name);

		//! default vertex shader
		std::string						mPassthruVextexShaderString;
		//! default fragment shader
		std::string						mPassthruFragmentShaderString;
		//! passthru shader
		gl::GlslProgRef					mPassThruShader;

		// include shader lines
		std::string						shaderInclude;

	};
}
