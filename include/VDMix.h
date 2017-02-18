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
// Triangles
#include "VDTriangle.h"
// Warping
#include "Warp.h"

// Syphon
#if defined( CINDER_MAC )
#include "cinderSyphon.h"
#endif

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
	struct VDMixFbo
	{
		ci::gl::FboRef					fbo;
		ci::gl::Texture2dRef			texture;
		string							name;
	};
	class VDMix {
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDMixRef					create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDMix>(new VDMix(aVDSettings, aVDAnimation));
		}
		void							update();
		void							resize();
		bool							handleMouseMove(MouseEvent &event);
		bool							handleMouseDown(MouseEvent &event);
		bool							handleMouseDrag(MouseEvent &event);
		bool							handleMouseUp(MouseEvent &event);
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };

		unsigned int					getMixFbosCount() { return mMixFbos.size(); };
		string							getMixFboName(unsigned int aMixFboIndex);

		// blendmodes
		unsigned int					getFboBlendCount();
		ci::gl::TextureRef				getFboThumb(unsigned int aBlendIndex);
		void							useBlendmode(unsigned int aBlendIndex);
		// fbolist
		unsigned int					getFboListSize() { return mFboList.size(); };

		ci::gl::TextureRef				getMixTexture(unsigned int aMixFboIndex = 0);
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex = 0);
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex);
		unsigned int					getBlendFbosCount() { return mBlendFbos.size(); }
		// warps
		string							getWarpName(unsigned int aWarpIndex) { return mWarps[aWarpIndex]->getName(); };
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex) { return mWarps[aWarpIndex]->getAFboIndex(); };
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex) { return mWarps[aWarpIndex]->getBFboIndex(); };
		unsigned int					getWarpAShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpCount() { return mWarps.size(); };
		void							createWarp(string wName = "warp", unsigned int aFboIndex = 0, unsigned int aShaderIndex = 0, unsigned int bFboIndex = 0, unsigned int bShaderIndex = 0, float xFade = 1.0f);
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		float							getWarpCrossfade(unsigned int aWarpIndex) { if (aWarpIndex > mWarps.size() - 1) aWarpIndex = 0; return mWarps[aWarpIndex]->ABCrossfade; };
		void							setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade) { if (aWarpIndex < mWarps.size()) mWarps[aWarpIndex]->ABCrossfade = aCrossfade; };
		void							updateWarpName(unsigned int aWarpIndex);
		void							crossfadeWarp(unsigned int aWarpIndex, float aValue) { timeline().apply(&mWarps[aWarpIndex]->ABCrossfade, aValue, 2.0f); };
		bool							isWarpActive(unsigned int aWarpIndex) { return mWarps[aWarpIndex]->isActive(); };
		void							toggleWarpActive(unsigned int aWarpIndex) { mWarps[aWarpIndex]->toggleWarpActive(); };
		bool							isWarpSolo(unsigned int aWarpIndex) { CI_LOG_V(toString((mSolo == aWarpIndex))); return (mSolo == aWarpIndex); };
		void							toggleWarpSolo(unsigned int aWarpIndex) { mSolo = (aWarpIndex == mSolo) ? -1 : aWarpIndex; CI_LOG_V(toString(mSolo) + " " + toString(aWarpIndex)); };
		bool							isWarpDeleted(unsigned int aWarpIndex) { return mWarps[aWarpIndex]->isDeleted(); };
		void							toggleDeleteWarp(unsigned int aWarpIndex) { mWarps[aWarpIndex]->toggleDeleteWarp(); };
		bool							isWarpAnimationActive() { return mWarpAnimationActive; };
		void							toggleWarpAnimationActive();
		// common to warps and triangles
		bool							isWarpTriangle();
		void							toggleWarpTriangle();
		int								getSolo() { return mSolo; };
		unsigned int					getSoloOrActiveIndex();
		unsigned int					getCurrentEditIndex() { return mCurrentEditIndex; };
		void							setCurrentEditIndex(unsigned int aIndex);

		// triangles
		void							createTriangle(string wName = "triangle", unsigned int aFboIndex = 0, unsigned int aShaderIndex = 0, unsigned int bFboIndex = 0, unsigned int bShaderIndex = 0, float xFade = 1.0f);
		void							updateTriangleName(unsigned int aTriangleIndex);
		string							getTriangleName(unsigned int aTriangleIndex) { return mVDTriangles[aTriangleIndex]->getName(); };
		unsigned int					getTriangleCount() { return mVDTriangles.size(); };
		unsigned int					getTriangleAFboIndex(unsigned int aTriangleIndex) { return mVDTriangles[aTriangleIndex]->getAFboIndex(); };
		unsigned int					getTriangleBFboIndex(unsigned int aTriangleIndex) { return mVDTriangles[aTriangleIndex]->getBFboIndex(); };
		void							setTriangleAFboIndex(unsigned int aTriangleIndex, unsigned int aTriangleFboIndex);
		void							setTriangleBFboIndex(unsigned int aTriangleIndex, unsigned int aTriangleFboIndex);
		bool							isTriangleActive(unsigned int aTriangleIndex) { return mVDTriangles[aTriangleIndex]->isActive(); };
		void							toggleTriangleActive(unsigned int aTriangleIndex) { mVDTriangles[aTriangleIndex]->toggleTriangleActive(); };
		bool							isTriangleSolo(unsigned int aTriangleIndex) { return (mSolo == aTriangleIndex); };
		void							toggleTriangleSolo(unsigned int aTriangleIndex) {
			mSolo = (aTriangleIndex == mSolo) ? -1 : aTriangleIndex; CI_LOG_V(toString(mSolo) + " " + toString(aTriangleIndex));
		};
		bool							isTriangleDeleted(unsigned int aTriangleIndex) { return mVDTriangles[aTriangleIndex]->isDeleted(); };
		void							toggleDeleteTriangle(unsigned int aTriangleIndex) { mVDTriangles[aTriangleIndex]->toggleDeleteTriangle(); };
		float							getTriangleCrossfade(unsigned int aTriangleIndex) { if (aTriangleIndex > mVDTriangles.size() - 1) aTriangleIndex = 0; return mVDTriangles[aTriangleIndex]->ABCrossfade; };
		void							setTriangleCrossfade(unsigned int aTriangleIndex, float aCrossfade) { if (aTriangleIndex < mVDTriangles.size()) mVDTriangles[aTriangleIndex]->ABCrossfade = aCrossfade; };
		ci::gl::TextureRef				getTriangleTexture(unsigned int aTriangleFboIndex);
		unsigned int					getTriangleFbosCount() { return mTriangleFbos.size(); };
		string							getTriangleFboName(unsigned int aTriangleFboIndex);
		int								getTrianglePointX(unsigned int aTriangleIndex, unsigned int aPointIndex) { return mVDTriangles[aTriangleIndex]->getControlPoint(aPointIndex).x; };
		int								getTrianglePointY(unsigned int aTriangleIndex, unsigned int aPointIndex) { return mVDTriangles[aTriangleIndex]->getControlPoint(aPointIndex).y; };
		void							setTrianglePointX(unsigned int aTriangleIndex, unsigned int aPointIndex, unsigned int aValue) { mVDTriangles[aTriangleIndex]->setControlPoint(aTriangleIndex, vec2(aValue, getTrianglePointY(aTriangleIndex, aPointIndex))); };
		void							setTrianglePointY(unsigned int aTriangleIndex, unsigned int aPointIndex, unsigned int aValue) { mVDTriangles[aTriangleIndex]->setControlPoint(aTriangleIndex, vec2(getTrianglePointX(aTriangleIndex, aPointIndex), aValue)); };

		// RTE in release mode ci::gl::Texture2dRef			getRenderedTexture(bool reDraw = true);
		ci::gl::Texture2dRef			getRenderTexture();
		void							save();
		void							load();
		// fbos
		unsigned int 					createShaderFbo(string aShaderFilename, unsigned int aInputTextureIndex = 0);
		unsigned int					createShaderFboFromString(string aFragmentShaderString, string aShaderFilename);
		string							getFboName(unsigned int aFboIndex) { return mFboList[aFboIndex]->getName(); };
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex);
		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex);
		void							fboFlipV(unsigned int aFboIndex);
		bool							isFboFlipV(unsigned int aFboIndex);
		void							setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex);
		unsigned int					getFboFragmentShaderIndex(unsigned int aFboIndex);
		// feedback get/set
		unsigned int					getFeedbackFrames(unsigned int aFboIndex) {
			if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
			return mFboList[aFboIndex]->getFeedbackFrames();
		};
		void							setFeedbackFrames(unsigned int aFboIndex, unsigned int aFeedbackFrames) {
			if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
			mFboList[aFboIndex]->setFeedbackFrames(aFeedbackFrames);
		};
		// textures
		ci::gl::TextureRef				getInputTexture(unsigned int aTextureIndex);
		string							getInputTextureName(unsigned int aTextureIndex);
		unsigned int					getInputTexturesCount();

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
		void							loadImageFile(string aFile, unsigned int aTextureIndex);
		void							loadAudioFile(string aFile);
		void							loadMovie(string aFile, unsigned int aTextureIndex);
		bool							loadImageSequence(string aFolder, unsigned int aTextureIndex);
		void							updateStream(string * aStringPtr);

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
		// shaders
		void							updateShaderThumbFile(unsigned int aShaderIndex);
		void							removeShader(unsigned int aShaderIndex);
		void							setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName = "");
		string							getVertexShaderString(unsigned int aShaderIndex);
		string							getFragmentShaderString(unsigned int aShaderIndex);
		unsigned int					getShadersCount() { return mShaderList.size(); };
		string							getShaderName(unsigned int aShaderIndex);
		ci::gl::TextureRef				getShaderThumb(unsigned int aShaderIndex);
		string							getFragmentString(unsigned int aShaderIndex) { return mShaderList[aShaderIndex]->getFragmentString(); };
		// spout output
		void							toggleSharedOutput(unsigned int aMixFboIndex = 0);
		bool							isSharedOutputActive() { return mSharedOutputActive; };
		unsigned int					getSharedMixIndex() { return mSharedFboIndex; };

	private:
		bool							mFlipV;
		bool							mFlipH;
		std::string						mFbosPath;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;

		//! mix shader
		gl::GlslProgRef					mMixShader;

		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;

		//! Fbos
		map<int, VDMixFbo>				mMixFbos;
		map<int, VDMixFbo>				mTriangleFbos;
		// maintain a list of fbos specific to this mix
		VDFboList						mFboList;
		fs::path						mMixesFilepath;

		//! Shaders
		VDShaderList					mShaderList;
		void							initShaderList();
		//! Textures
		VDTextureList					mTextureList;
		fs::path						mTexturesFilepath;
		bool							initTextureList();
		// blendmodes fbos
		map<int, ci::gl::FboRef>		mBlendFbos;
		int								mCurrentBlend;
		gl::GlslProgRef					mGlslMix, mGlslBlend;
		// render
		void							renderMix();
		void							renderBlend();
		// warping
		string							fileWarpsName;
		//fs::path						mWarpSettings;
		fs::path						mWarpJson;
		WarpList						mWarps;
		gl::FboRef						mRenderFbo;
		int								warpMixToRender;
		int								mSolo;
		bool							mWarpAnimationActive;
		unsigned int					mWarpActiveIndex;
		// warp rendered texture
		ci::gl::Texture2dRef			mRenderedTexture;
		// triangles
		VDTriangleList					mVDTriangles;
		fs::path						mTrianglesJson;
		void							renderTriangles();
		bool							mUseTriangles;
		int								triangleMixToRender;
		// common to warps and triangles
		unsigned int					mCurrentEditIndex;
		// old to refactor:
		//ci::vec2						mStartPt, mVertices[3];
		// shared texture output
		bool							mSharedOutputActive;
		unsigned int					mSharedFboIndex;
		bool							mSpoutInitialized;
		char							mSenderName[256];
#if defined( CINDER_MSW )
		// spout output
		SpoutSender						mSpoutSender;
#endif
		// syphon output
#if defined( CINDER_MAC )
		syphonServer                    mSyphonServer;
#endif
	};
}
