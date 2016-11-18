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
// Message router
#include "VDRouter.h"

// Fbos
#include "VDFbo.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

namespace VideoDromm
{
	// stores the pointer to the VDMix instance
	typedef std::shared_ptr<class VDMix> 	VDMixRef;

	class VDMix{
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDRouterRef aVDRouter);
		static VDMixRef					create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDRouterRef aVDRouter)
		{
			return shared_ptr<VDMix>(new VDMix(aVDSettings, aVDAnimation, aVDRouter));
		}
		void							update();
		void							save();

		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };
		int								getTextureWidth();
		int								getTextureHeight();
		// file operations (filedrop, etc)
		int								loadFileFromAbsolutePath(string aAbsolutePath, int aIndex = 0);
		// textures
		void							loadImageFile(string aFile, unsigned int aTextureIndex, bool right);
		void							loadAudioFile(string aFile);
		void							loadMovie(string aFile, unsigned int aTextureIndex);
		bool							loadImageSequence(string aFolder, unsigned int aTextureIndex);
		//!
		void							fromXml(const ci::XmlTree &xml);
		//!
		//XmlTree							toXml() const;
		//! read a xml file and pass back a vector of VDMixs
		void							readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDRouterRef aVDRouter, const ci::DataSourceRef &source);
		//! write a xml file
		//static void						writeSettings(const VDMixList &VDMixlist, const ci::DataTargetRef &target);
		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
		// fbos
		ci::gl::Texture2dRef			getInputTexture(unsigned int aTextureIndex);
		int								getInputTextureXLeft(unsigned int aTextureIndex);
		void							setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft);
		int								getInputTextureYTop(unsigned int aTextureIndex);
		void							setInputTextureYTop(unsigned int aTextureIndex, int aYTop);
		int								getInputTextureXRight(unsigned int aTextureIndex);
		void							setInputTextureXRight(unsigned int aTextureIndex, int aXRight);
		int								getInputTextureYBottom(unsigned int aTextureIndex);
		void							setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom);
		bool							isFlipVInputTexture(unsigned int aTextureIndex);
		bool							isFlipHInputTexture(unsigned int aTextureIndex);
		void							inputTextureFlipV(unsigned int aTextureIndex);
		void							inputTextureFlipH(unsigned int aTextureIndex);

		bool							getInputTextureLockBounds(unsigned int aTextureIndex);
		void							toggleInputTextureLockBounds(unsigned int aTextureIndex);
		unsigned int					getInputTextureOriginalWidth(unsigned int aTextureIndex);
		unsigned int					getInputTextureOriginalHeight(unsigned int aTextureIndex);
		void							togglePlayPause(unsigned int aTextureIndex);
		// movie
		bool							isMovie(unsigned int aTextureIndex);
		// sequence
		bool							isSequence(unsigned int aTextureIndex);
		bool							isLoadingFromDisk(unsigned int aTextureIndex);
		void							toggleLoadingFromDisk(unsigned int aTextureIndex);
		void							syncToBeat(unsigned int aTextureIndex);
		void							reverse(unsigned int aTextureIndex);
		float							getSpeed(unsigned int aTextureIndex);
		void							setSpeed(unsigned int aTextureIndex, float aSpeed);
		int								getPlayheadPosition(unsigned int aTextureIndex);
		void							setPlayheadPosition(unsigned int aTextureIndex, int aPosition);
		int								getMaxFrame(unsigned int aTextureIndex);

		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex);
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex);
		int								getFboTextureWidth(unsigned int aFboIndex);
		int								getFboTextureHeight(unsigned int aFboIndex);
		unsigned int					getInputTexturesCount();
		unsigned int					getFboListSize() { return mFboList.size(); };
		string							getFboName(unsigned int aFboIndex);
		string							getFboLabel(unsigned int aFboIndex);
		string							getFboFragmentShaderText(unsigned int aFboIndex);
		string							getInputTextureName(unsigned int aTextureIndex);
		unsigned int					getMixFbosCount() { return mMixFbos.size(); };
		string							getMixFboLabel(unsigned int aMixFboIndex);
		// uniforms
		void							setCrossfade(float aCrossfade);
		// shaders
		int								loadFboFragmentShader(string aFilePath, unsigned int aFboIndex = 0);
		void							setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex);
		unsigned int					getFboFragmentShaderIndex(unsigned int aFboIndex);
		bool							loadShaderFolder(string aFolder);
		int								loadFragmentShader(string aFilePath);
		unsigned int					getShadersCount();
		string							getShaderName(unsigned int aShaderIndex);
		ci::gl::Texture2dRef			getShaderThumb(unsigned int aShaderIndex);
		void							setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName="");
		string							getVertexShaderString(unsigned int aShaderIndex);
		string							getFragmentShaderString(unsigned int aShaderIndex);
		void							createShaderThumb(unsigned int aShaderIndex);
		// blendmodes
		unsigned int					getFboBlendCount();
		ci::gl::Texture2dRef			getFboThumb(unsigned int aBlendIndex);
		void							useBlendmode(unsigned int aBlendIndex);
		//const unsigned int				MAXBLENDMODES = 27;
		ci::gl::Texture2dRef			getMixTexture(unsigned int aMixFboIndex = 0);
		ci::gl::Texture2dRef			getFboTexture(unsigned int aFboIndex = 0);
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex);
		unsigned int					getBlendFbosCount() { return mBlendFbos.size(); }

	protected:
		bool							mFlipV;
		bool							mFlipH;
		std::string						mFbosPath;
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;

		//! mix shader
		gl::GlslProgRef					mMixShader;
		// uniforms
		vec3							iChannelResolution0;
	private:
		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;
		// Router
		VDRouterRef						mVDRouter;

		fs::path						mMixesFilepath;
		//! Fbos
		// maintain a list of fbo for right only or left/right or more fbos specific to this mix
		VDFboList						mFboList;
		fs::path						mFbosFilepath;
		vector<ci::gl::FboRef>			mMixFbos;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		//! Shaders
		VDShaderList					mShaderList;
		bool							initShaderList();
		// Textures
		VDTextureList					mTextureList;
		fs::path						mTexturesFilepath;
		bool							initTextureList();
		// blendmodes fbos
		vector<ci::gl::FboRef>			mBlendFbos;
		int								mCurrentBlend;
		gl::GlslProgRef					mGlslMix, mGlslBlend;
		// render
		void							renderMix();
		void							renderBlend();
	};
}
