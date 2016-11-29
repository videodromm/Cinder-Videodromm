#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
// json
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Utils
#include "VDUtils.h"
// Message router
#include "VDRouter.h"
// Websocket
#include "VDWebsocket.h"
// Animation
#include "VDAnimation.h"
// Mix
#include "VDMix.h"
// Logger
#include "VDLog.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm {

	typedef std::shared_ptr<class VDSession> VDSessionRef;


	class VDSession	{
	public:
		VDSession(VDSettingsRef aVDSettings);
		static VDSessionRef				create(VDSettingsRef aVDSettings);

		//!
		void							fromXml(const ci::XmlTree &xml);
		//!
		//XmlTree							toXml() const;
		//! read a xml file and pass back a vector of VDMixs
		void							readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const ci::DataSourceRef &source);
		//! write a xml file
		//static void						writeSettings(const VDMixList &VDMixlist, const ci::DataTargetRef &target);

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

		string							getWaveFileName() { return mWaveFileName; };
		int								getWavePlaybackDelay() { return mWavePlaybackDelay; };
		string							getMovieFileName() { return mMovieFileName; };
		int								getMoviePlaybackDelay() { return mMoviePlaybackDelay; };
		bool							hasMovie() { return mMovieFileName.length() > 0; };
		int								getFadeInDelay() { return mFadeInDelay; };
		int								getFadeOutDelay() { return mFadeOutDelay; };
		string							getImageSequencePath() { return mImageSequencePath; };
		bool							hasImageSequencePath() { return mImageSequencePath.length() > 0; };
		string							getText() { return mText; };
		int								getTextStart() { return mTextPlaybackDelay; };
		int								getTextEnd() { return mTextPlaybackEnd; };
		bool							hasText() { return mText.length() > 0; };
		// control values
		float							getControlValue(unsigned int aCtrl);
		void							setControlValue(unsigned int aCtrl, float aValue);
		void							toggleValue(unsigned int aCtrl);
		void							toggleAuto(unsigned int aCtrl);
		void							toggleTempo(unsigned int aCtrl);
		bool							getBoolValue(unsigned int aCtrl);
		void							resetAutoAnimation(unsigned int aIndex);
		float							getMinUniformValueByIndex(unsigned int aIndex);
		float							getMaxUniformValueByIndex(unsigned int aIndex);
		// tempo
		float							getBpm() { return mVDAnimation->getBpm(); };
		void							setBpm(float aBpm) { mVDAnimation->setBpm(aBpm); };
		void							tapTempo() { mVDAnimation->tapTempo(); };
		// audio
		float							getMaxVolume() { return mVDAnimation->maxVolume; };
		float *							getFreqs() { return mVDAnimation->iFreqs; };
		// uniforms
		//void							setCrossfade(float aCrossfade); // TODO use setControlValue

		int								loadFboFragmentShader(string aFilePath, unsigned int aFboIndex = 0);
		void							setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex);
		unsigned int					getFboFragmentShaderIndex(unsigned int aFboIndex);
		bool							loadShaderFolder(string aFolder);
		int								loadFragmentShader(string aFilePath);
		unsigned int					getShadersCount();
		string							getShaderName(unsigned int aShaderIndex);
		ci::gl::TextureRef				getShaderThumb(unsigned int aShaderIndex);
		void							setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName = "");
		string							getVertexShaderString(unsigned int aShaderIndex);
		string							getFragmentShaderString(unsigned int aShaderIndex);
		//void							renderShaderThumb(unsigned int aShaderIndex);
		void							updateShaderThumbFile(unsigned int aShaderIndex);
		void							removeShader(unsigned int aShaderIndex);
		// utils
		int								getWindowsResolution();
		float							getTargetFps() { return mTargetFps; };
		void							blendRenderEnable(bool render);

		// file operations (filedrop, etc)
		//int								loadFileFromAbsolutePath(string aAbsolutePath, int aIndex = 0);
		void							fileDrop(FileDropEvent event);
		// warps
		string							getWarpName(unsigned int aWarpIndex);
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex);
		unsigned int					getWarpAShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpCount();
		void							createWarpMix();
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade);
		float							getWarpCrossfade(unsigned int aWarpIndex);
		fs::path						mMixesFilepath;
		// fbos
		void							fboFlipV(unsigned int aFboIndex);
		bool							isFboFlipV(unsigned int aFboIndex);
		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex);
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex);
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex = 0) { return mVDMix->getFboTexture(aFboIndex); };
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex) { return mVDMix->getFboRenderedTexture(aFboIndex); };
		ci::gl::TextureRef				getFboThumb(unsigned int aBlendIndex) { return mVDMix->getFboThumb(aBlendIndex); };
		string							getFboName(unsigned int aFboIndex);
		int								getFboTextureWidth(unsigned int aFboIndex);
		int								getFboTextureHeight(unsigned int aFboIndex);
		unsigned int					getFboListSize() { return mFboList.size(); };
		string							getFboFragmentShaderText(unsigned int aFboIndex);
		string							getMixFboName(unsigned int aMixFboIndex);
		ci::gl::TextureRef				getMixTexture(unsigned int aMixFboIndex = 0);
		unsigned int					getMixFbosCount();
		bool							isFlipH();
		bool							isFlipV();
		void							flipH();
		void							flipV();
		// blendmodes
		unsigned int					getFboBlendCount() { return mVDMix->getFboBlendCount(); };
		void							useBlendmode(unsigned int aBlendIndex) { return mVDMix->useBlendmode(aBlendIndex); };

		// textures
		ci::gl::TextureRef				getInputTexture(unsigned int aTextureIndex);
		ci::gl::TextureRef				getRenderTexture();
		string							getInputTextureName(unsigned int aTextureIndex);
		unsigned int					getInputTexturesCount();
		int								getTextureWidth();
		int								getTextureHeight();
		void							loadImageFile(string aFile, unsigned int aTextureIndex);
		void							loadAudioFile(string aFile);
		void							loadMovie(string aFile, unsigned int aTextureIndex);
		bool							loadImageSequence(string aFolder, unsigned int aTextureIndex);
		//ci::gl::TextureRef				getStreamedTexture();

		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
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
		// websockets
#if defined( CINDER_MSW )
		void							wsConnect();
		void							wsPing();
		void							wsWrite(std::string msg);
		void							sendFragmentShader(unsigned int aShaderIndex);
#endif
		// midi
		void						midiSetup() { mVDRouter->midiSetup(); };
#if defined( CINDER_MSW )
		int							getMidiInPortsCount() { return mVDRouter->getMidiInPortsCount(); };
		string						getMidiInPortName(int i) { return mVDRouter->getMidiInPortName(i); };
		bool						isMidiInConnected(int i) { return mVDRouter->isMidiInConnected(i); };
		int							getMidiOutPortsCount() { return mVDRouter->getMidiOutPortsCount(); };
		string						getMidiOutPortName(int i) { return mVDRouter->getMidiOutPortName(i); };
		bool						isMidiOutConnected(int i) { return mVDRouter->isMidiOutConnected(i); };
#endif
		void						openMidiInPort(int i) { mVDRouter->openMidiInPort(i); };
		void						closeMidiInPort(int i) { mVDRouter->closeMidiInPort(i); };
		void						openMidiOutPort(int i) { mVDRouter->openMidiOutPort(i); };
		void						closeMidiOutPort(int i) { mVDRouter->closeMidiOutPort(i); };
		ci::gl::Texture2dRef		getRenderedTexture()  { return mVDMix->getRenderedTexture(); };

	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Utils
		VDUtilsRef						mVDUtils;
		// Message router
		VDRouterRef						mVDRouter;
		// VDWebsocket
		VDWebsocketRef					mVDWebsocket;
		// Animation
		VDAnimationRef					mVDAnimation;
		// Mix
		VDMixRef						mVDMix;
		// Log
		VDLogRef						mVDLog;

		const string					sessionFileName = "session.json";
		fs::path						sessionPath;
		// tempo
		float							mFpb;
		float							mOriginalBpm;
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
		// font and text 
		string							mText;
		int								mTextPlaybackDelay;
		int								mTextPlaybackEnd;
		//! Fbos
		// maintain a list of fbo for right only or left/right or more fbos specific to this mix
		VDFboList						mFboList;
		fs::path						mFbosFilepath;
		// fbo 
		bool							mFlipV;
		bool							mFlipH;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;

		//! Shaders
		VDShaderList					mShaderList;
		bool							initShaderList();
		string							mShaderLeft;
		string							mShaderRight;
		// textures
		VDTextureList					mTextureList;
		fs::path						mTexturesFilepath;
		bool							initTextureList();
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		void							updateWarpName(unsigned int aWarpIndex);
		void							updateStream();
		//ci::gl::TextureRef			mStreamedTexture;
	};

}
