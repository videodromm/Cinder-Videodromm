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
// shaders
#include "VDShaders.h"
// Animation
#include "VDAnimation.h"
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
		void						renderWarpFbos();
		// shaders
		void						renderShadaThumbFbo();
		int							currentShadaThumbIndex;
		// textures
		void						setTexture(int index, string fileName);
		ci::gl::TextureRef			getTexture(int index);
		int							getTextureCount() { return sTextures.size(); };
		void						flipTexture(int index);
		void						setTexture(int index, ci::gl::TextureRef texture);
		char*						getTextureName(int index);
		void						setTextureName(int index, char* name);
		// from audio
		void						setAudioTexture(unsigned char *signal);
		// fbos
		ci::gl::TextureRef			getFboTexture(int index);
		GLuint						getFboTextureId(int index);
		ci::gl::FboRef				getFbo(int index);
		int							getFboCount() { return mVDFbos.size(); };
		int							getThumbsFboCount() { return mThumbFbos.size(); };
		void						flipFboV(int index);
		void						flipFboH(int index);
		const char*					getFboName(int index);

		// shaders
		GLuint						getShaderThumbTextureId(int index);
		// spout
		void						setSenderTextureSize(int index, int width, int height);
		int							createSpoutTexture(char name[256], unsigned int width, unsigned int height);

		// image
		void						loadImageFile(int index, string aFile);
		// quicktime video
		void						loadMovieFile(int index, string aFile);
		void						update();
		void						draw();
		//void						createWarpInput();
		char*						getPreviewTime() { return previewTime; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Shaders
		VDShadersRef				mVDShaders;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Fbos
		vector<VDFboRef>			mVDFbos;

		char						buf[32];

		//static const int			mTexturesCount = 12; //match MAX from mVDSettings

		//ci::gl::Texture				previousTexture;
		//ci::gl::Texture				currentTexture;
		unsigned char				dTexture[1024];
		vector<ci::gl::TextureRef>	sTextures;
		//vector<Texta>				textas;
		char						spoutSenderName[256];
		string						fileName;
		// fbo
		//vector<FrameBuffa>			mFbos;
		//! thumb fbos
		vector<FrameBuffa>			mThumbFbos;

		//! shader fbos
		//vector<ShadaFbo>			mShadaFbos;
		// shaders
		gl::GlslProgRef				aShader;

		// profiling
		char						previewTime[256];
		// quicktime movie
		//qtime::MovieGlRef			mMovie;
		int							mMovieIndex;

	};
}