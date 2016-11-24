#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Xml.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Fbos
#include "VDFbo.h"
// Warping
#include "Warp.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ph::warping; 
using namespace VideoDromm;

namespace VideoDromm
{
	// stores the pointer to the VDMix instance
	typedef std::shared_ptr<class VDMix> 	VDMixRef;

	class VDMix {
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList, VDShaderList aShaderList, VDFboList aFboList);
		static VDMixRef					create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList, VDShaderList aShaderList, VDFboList aFboList)
		{
			return shared_ptr<VDMix>(new VDMix(aVDSettings, aVDAnimation, aTextureList, aShaderList, aFboList));
		}
		void							update();
		void							resize();
		bool							handleMouseMove(MouseEvent &event);
		bool							handleMouseDown(MouseEvent &event);
		bool							handleMouseDrag(MouseEvent &event);
		bool							handleMouseUp(MouseEvent &event);
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };

		unsigned int					getMixFbosCount() { return mMixFbos.size(); };
		string							getMixFboLabel(unsigned int aMixFboIndex);
		// blendmodes
		unsigned int					getFboBlendCount();
		ci::gl::TextureRef				getFboThumb(unsigned int aBlendIndex);
		void							useBlendmode(unsigned int aBlendIndex);

		ci::gl::TextureRef				getMixTexture(unsigned int aMixFboIndex = 0);
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex = 0);
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex);
		unsigned int					getBlendFbosCount() { return mBlendFbos.size(); }
		// warps
		string							getWarpName(unsigned int aWarpIndex);
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex);
		unsigned int					getWarpAShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBShaderIndex(unsigned int aWarpIndex);
		float							getWarpCrossfade(unsigned int aWarpIndex);
		unsigned int					getWarpCount();
		void							createWarp(string wName = "warp", unsigned int aFboIndex = 1, unsigned int aShaderIndex = 4, unsigned int bFboIndex = 2, unsigned int bShaderIndex = 5, float xFade = 1.0f);
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade);
		void							updateWarpName(unsigned int aWarpIndex);
		ci::gl::Texture2dRef				getRenderTexture();
		void							crossfadeWarp(unsigned int aWarpIndex, float aValue);
		void							save();
		void							load();
		ci::gl::Texture2dRef			getRenderedTexture();

	private:
		bool							mFlipV;
		bool							mFlipH;
		std::string						mFbosPath;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;

		//! mix shader
		gl::GlslProgRef					mMixShader;
		// uniforms
		vec3							iChannelResolution0;

		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;

		//! Fbos
		// maintain a list of fbos specific to this mix
		VDFboList						mFboList;
		vector<ci::gl::FboRef>			mMixFbos;

		//! Shaders
		VDShaderList					mShaderList;
		// Textures
		VDTextureList					mTextureList; 

		// blendmodes fbos
		vector<ci::gl::FboRef>			mBlendFbos;
		int								mCurrentBlend;
		gl::GlslProgRef					mGlslMix, mGlslBlend;
		// render
		void							renderMix();
		void							renderBlend();
		// warping
		string							fileWarpsName;
		fs::path						mWarpSettings;
		fs::path						mWarpJson;
		WarpList						mWarps;
		gl::FboRef						mRenderFbo;

		int								warpMixToRender;
		map<int, WarpMix>				mWarpMix;
		// temp
		ci::gl::Texture2dRef			mRenderedTexture;
	};
}
