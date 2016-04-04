#pragma once

#include <string>
#include <vector>

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/Capture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Input textures
#include "VDInputTexture.h"
// fbo
#include "VDFbo.h"
// Logger
#include "VDLog.h"

using namespace ci;
using namespace ci::app;
using namespace std;


namespace VideoDromm
{
	// stores the pointer to the VDTextures instance
	typedef std::shared_ptr<class VDTextures> VDTexturesRef;
	typedef std::chrono::high_resolution_clock Clock;

	class VDTextures {
	public:
		VDTextures(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, VDAnimationRef aAnimationRef);
		virtual					~VDTextures();
		static VDTexturesRef	create(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, VDAnimationRef aAnimationRef)
		{
			return shared_ptr<VDTextures>(new VDTextures(aVDSettings, aShadersRef, aAnimationRef));
		}

		ci::gl::TextureRef			getWarpTexture(int index);
		//WarpFbo						getWarpFbo(int index) { return mVDSettings->mWarpFbos[min((int)(mVDSettings->mWarpFbos.size() - 1), index)]; };
		//void						renderWarpFbos();
		// shaders
		void						renderShadaThumbFbo();
		int							currentShadaThumbIndex;
		// textures
		//void						setTexture(int index, string fileName);
		//ci::gl::TextureRef			getTexture(int index);
		void						setFboTexture(int index, ci::gl::TextureRef aTexture);
			//int							getTextureCount() { return sTextures.size(); };
		void						flipTexture(int index);
		//void						setTexture(int index, ci::gl::TextureRef texture);
		string						getTextureName(int index);
		void						setTextureName(int index, char* name);
		ci::gl::TextureRef			getInputTexture(int index);
		int							getInputTexturesCount() { return mVDInputTextures.size(); };
		// from audio
		//void						setAudioTexture(unsigned char *signal);
		// fbos
		ci::gl::TextureRef			getFboTexture(int index);
		GLuint						getFboTextureId(int index);
		ci::gl::FboRef				getFbo(int index);
		int							getFboCount() { return mVDFbos.size(); };
		//int							getThumbsFboCount() { return mThumbFbos.size(); };
		void						flipFboV(int index);
		void						flipFboH(int index);
		void						saveThumb(int index);
		void						setFragForFbo(int fragIndex, int fboIndex);
		//const char*					getFboName(int index);

		// shaders
		//GLuint						getShaderThumbTextureId(int index);
		// spout
		//void						setSenderTextureSize(int index, int width, int height);
		//int							createSpoutTexture(char name[256], unsigned int width, unsigned int height);

		// image
		void						loadImageFile(int index, string aFile);
		// quicktime video
		void						loadMovieFile(int index, string aFile);
		// image sequence
		int							loadImageSequence(int index, string aFile);
		// text
		int							loadText(int index, string text, int start, int end);

		void						update();
		void						draw();
		bool						inputTextureIsSequence(int index);
		bool						inputTextureIsLoadingFromDisk(int index);
		void						inputTextureToggleLoadingFromDisk(int index);
		void						inputTexturePlayPauseSequence(int index);
		void						inputTextureSyncToBeatSequence(int index);
		void						inputTextureReverseSequence(int index);
		int							inputTextureGetPlayheadPosition(int index);
		void						inputTextureSetPlayheadPosition(int index, int position);
		int							inputTextureGetMaxFrame(int index);
		int							inputTextureGetSpeed(int index);
		void						inputTextureSetSpeed(int index, int speed);
		// shaders
		int							loadPixelFragmentShaderAtIndex(int index, string mFile);
		//void						createWarpInput();
		char*						getPreviewTime() { return previewTime; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Shaders
		VDShadersRef				mVDShaders;
		// Fbos
		vector<VDFboRef>			mVDFbos;
		// Input textures
		vector<VDInputTextureRef>	mVDInputTextures;

		char						buf[32];

		unsigned char				dTexture[1024];
		//vector<ci::gl::TextureRef>	sTextures;
		char						spoutSenderName[256];
		string						fileName;
		// shaders
		//gl::GlslProgRef				aShader;

		// profiling
		char						previewTime[256];
		// quicktime movie
		//qtime::MovieGlRef			mMovie;
		int							mMovieIndex;

	};
}