#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
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
	typedef std::vector<VDMixRef>			VDMixList;

	class VDMix : public std::enable_shared_from_this < VDMix > {
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		~VDMix(void);
		static VDMixRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) { return std::make_shared<VDMix>(aVDSettings, aVDAnimation); }
		void							update();
		//! returns a shared pointer to this fbo
		VDMixRef						getPtr() { return shared_from_this(); }
		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		std::string						getName();
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
		//!
		void							fromXml(const ci::XmlTree &xml);
		//!
		XmlTree							toXml() const;
		//! read a xml file and pass back a vector of VDMixs
		static VDMixList				readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const ci::DataSourceRef &source);
		//! write a xml file
		static void						writeSettings(const VDMixList &VDMixlist, const ci::DataTargetRef &target);
		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
		// shader
		int								loadFboFragmentShader(string aFilePath, unsigned int aFboIndex = 0);
		// fbos
		ci::gl::Texture2dRef			getTexture();
		ci::gl::Texture2dRef			getLeftFboTexture();
		ci::gl::Texture2dRef			getRightFboTexture();
		void							setLeftFboIndex(unsigned int aFboIndex);
		void							setRightFboIndex(unsigned int aFboIndex);
		unsigned int					getLeftFboIndex() { return mLeftFboIndex; };
		unsigned int					getRightFboIndex() { return mRightFboIndex; };
		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex);
		/*bool							isLeftFboUsed() { return mUseLeftFbo; };
		bool							isRightFboUsed() { return mUseRightFbo; };*/
		bool							isFboUsed() { return mUseFbo; };
		void							toggleFboUsed() { mUseFbo = !mUseFbo; };
		ci::gl::Texture2dRef			getFboTexture(unsigned int aFboIndex);
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aFboInputTextureIndex);
		/*string							getInputTextureName(unsigned int aTextureIndex);*/
		ci::gl::Texture2dRef			getInputTexture(unsigned int aTextureIndex);
		int								getInputTextureXLeft(unsigned int aTextureIndex);
		void							setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft);
		int								getInputTextureYTop(unsigned int aTextureIndex);
		void							setInputTextureYTop(unsigned int aTextureIndex, int aYTop);
		int								getInputTextureXRight(unsigned int aTextureIndex);
		void							setInputTextureXRight(unsigned int aTextureIndex, int aXRight);
		int								getInputTextureYBottom(unsigned int aTextureIndex);
		void							setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom);
		bool							getInputTextureTopDown(unsigned int aTextureIndex);
		void							toggleInputTextureTopDown(unsigned int aTextureIndex);

		bool							getInputTextureLockBounds(unsigned int aTextureIndex);
		void							toggleInputTextureLockBounds(unsigned int aTextureIndex);
		unsigned int					getInputTextureOriginalWidth(unsigned int aTextureIndex);
		unsigned int					getInputTextureOriginalHeight(unsigned int aTextureIndex);


		int								getFboTextureWidth(unsigned int aFboIndex);
		int								getFboTextureHeight(unsigned int aFboIndex);
		unsigned int					getInputTexturesCount();
		unsigned int					getFboCount() { return mFboList.size(); };
		string							getFboName(unsigned int aFboIndex);
		string							getFboLabel(unsigned int aFboIndex);
		string							getFboFragmentShaderText(unsigned int aFboIndex);
		string							getInputTextureName(unsigned int aTextureIndex);
		// uniforms
		void							setCrossfade(float aCrossfade);
		// shaders
		unsigned int					getShadersCount();
		string							getShaderName(unsigned int aShaderIndex);
		ci::gl::Texture2dRef			getShaderThumb(unsigned int aShaderIndex);
		void							editShader(unsigned int aShaderIndex);
		void							createShaderThumb(unsigned int aShaderIndex);
	protected:
		std::string						mName;
		bool							mFlipV;
		bool							mFlipH;
		std::string						mFbosPath;
		//bool							mTopDown;
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		//! default vertex shader
		//std::string						mPassthruVextexShaderString;
		//! default fragment shader
		//std::string						mMixFragmentShaderString;
		//! mix shader
		gl::GlslProgRef					mMixShader;
		// include shader lines
		//std::string						shaderInclude;
		//string							mError;
		// uniforms
		vec3							iChannelResolution0;
	private:
		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;
		// init
		bool							initFboList();
		//! Fbo
		gl::FboRef						mMixFbo, mLeftFbo, mRightFbo;

		unsigned int					mRightFboIndex;
		unsigned int					mLeftFboIndex;
		bool							mUseLeftFbo;
		bool							mUseRightFbo;
		bool							mUseFbo;
		// maintain a list of fbo for right only or left/right or more fbos specific to this mix
		VDFboList						mFboList;
		fs::path						mFbosFilepath;
		//! Shaders
		VDShaderList					mShaderList;
		bool							initShaderList();
		unsigned int					mMixShaderIndex;
		// Textures
		VDTextureList					mTextureList;
		fs::path						mTexturesFilepath;
		bool							initTextureList();
		// blendmodes fbos
		vector<ci::gl::FboRef>			mFboBlend;
		int								mCurrentBlend;
		ci::gl::FboRef					mFboA, mFboB, mFboMix;
		gl::GlslProgRef					mGlslA, mGlslB, mGlslMix, mGlslBlend;
		void							renderSceneA();
		void							renderSceneB();
		void							renderMix();
		void							renderBlend();
	};
}
