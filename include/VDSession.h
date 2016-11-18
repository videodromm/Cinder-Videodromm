#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
// json
#include "cinder/Json.h"
// Warping
#include "Warp.h"
// Settings
#include "VDSettings.h"
// Utils
#include "VDUtils.h"
// Message router
#include "VDRouter.h"
// Animation
#include "VDAnimation.h"
// Textures
#include "VDMix.h"
// Logger
#include "VDLog.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ph::warping;

namespace VideoDromm {

	typedef std::shared_ptr<class VDSession> VDSessionRef;


	class VDSession
	{
	public:
		VDSession(VDSettingsRef aVDSettings);
		static VDSessionRef				create(VDSettingsRef aVDSettings);
		bool							handleMouseMove(MouseEvent &event);
		bool							handleMouseDown(MouseEvent &event);
		bool							handleMouseDrag(MouseEvent &event);
		bool							handleMouseUp(MouseEvent &event);
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		void							resize();
		void							update();
		bool							save();
		void							restore();
		void							reset();
		void							resetSomeParams();

		float							getBpm() { return mBpm; };
		float							getTargetFps() { return mTargetFps; };
		void							setBpm(float aBpm) { mBpm = aBpm; };
		int								getEndFrame() { return mEndFrame; };
		void							setEndFrame(int frame) { mEndFrame = frame; };
		string							getWaveFileName() { return mWaveFileName; };
		int								getWavePlaybackDelay() { return mWavePlaybackDelay; };
		string							getMovieFileName() { return mMovieFileName; };
		int								getMoviePlaybackDelay() { return mMoviePlaybackDelay; };
		bool							hasMovie() { return mMovieFileName.length() > 0; };
		int								getFadeInDelay() { return mFadeInDelay; };
		int								getFadeOutDelay() { return mFadeOutDelay; };
		string							getImageSequencePath() { return mImageSequencePath; };
		bool							hasImageSequencePath() { return mImageSequencePath.length() > 0; };
		int								iBeatsPerBar;
		string							getText() { return mText; };
		int								getTextStart() { return mTextPlaybackDelay; };
		int								getTextEnd() { return mTextPlaybackEnd; };
		bool							hasText() { return mText.length() > 0; };
		// shaders
		bool							hasShaderLeft() { return mShaderLeft.length() > 0; };
		bool							hasShaderRight() { return mShaderRight.length() > 0; };
		string							getShaderLeft() { return mShaderLeft; };
		string							getShaderRight() { return mShaderRight; };
		// utils
		int								getWindowsResolution();
		// warps
		string							getWarpName(unsigned int aWarpIndex) { return mWarpMix[aWarpIndex].Name; };
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex) { return mWarpMix[aWarpIndex].AFboIndex; };
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex) { return mWarpMix[aWarpIndex].BFboIndex; };
		void							createWarp();
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		unsigned int					getWarpCount() { return mWarpMix.size(); };
		void							setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade);
		float							getWarpCrossfade(unsigned int aWarpIndex);
		// fbos
		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex);
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex);
		string							getFboLabel(unsigned int aFboIndex);
		unsigned int					getFboListSize() { return mVDMix->getFboListSize(); };
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex);
		string							getFboName(unsigned int aFboIndex);
		// textures
		ci::gl::Texture2dRef			getInputTexture(unsigned int aTextureIndex);
		ci::gl::TextureRef				getRenderTexture();
		string							getInputTextureName(unsigned int aTextureIndex);
		unsigned int					getInputTexturesCount();
		// websockets
#if defined( CINDER_MSW )
		void						wsConnect();
		void						wsPing();
#endif
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Utils
		VDUtilsRef						mVDUtils;
		// Message router
		VDRouterRef						mVDRouter;
		// Animation
		VDAnimationRef					mVDAnimation;
		// Mix
		VDMixRef						mVDMix;

		const string					sessionFileName = "session.json";
		fs::path						sessionPath;
		// fbo 
		bool							mFlipV;
		bool							mFlipH;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		// tempo
		float							mBpm, mFpb;
		float							mOriginalBpm;
		// target fps
		float							mTargetFps;
		// files and paths
		string							mWaveFileName;
		string							mMovieFileName;
		string							mImageSequencePath;
		// delay
		int								mWavePlaybackDelay;
		int								mMoviePlaybackDelay;
		int								mFadeInDelay;
		int								mFadeOutDelay;
		int								mEndFrame;
		// font and text 
		string							mText;
		int								mTextPlaybackDelay;
		int								mTextPlaybackEnd;
		// shaders
		string							mShaderLeft;
		string							mShaderRight;
		// warping
		gl::TextureRef					mImage;
		WarpList						mWarps;
		string							fileWarpsName;
		fs::path						mWarpSettings;
		gl::FboRef						mRenderFbo;
		int								warpMixToRender;
		map<int, WarpMix>				mWarpMix;
		void							updateWarpName(unsigned int aWarpIndex);
	};

}
