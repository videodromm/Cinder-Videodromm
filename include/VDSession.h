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


	class VDSession {
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
		void							update(unsigned int aClassIndex = 0);
		bool							save();
		void							restore();
		void							reset();
		void							resetSomeParams();

		string							getWaveFileName() { return mWaveFileName; };
		int								getWavePlaybackDelay() { return mWavePlaybackDelay; };
		string							getMovieFileName() { return mMovieFileName; };
		int								getMoviePlaybackDelay() { return mMoviePlaybackDelay; };
		bool							hasMovie() { return mMovieFileName.length() > 0; };
		unsigned int					getFadeInDelay() { return mFadeInDelay; };
		unsigned int					getFadeOutDelay() { return mFadeOutDelay; };
		string							getImageSequencePath() { return mImageSequencePath; };
		bool							hasImageSequencePath() { return mImageSequencePath.length() > 0; };
		string							getText() { return mText; };
		int								getTextStart() { return mTextPlaybackDelay; };
		int								getTextEnd() { return mTextPlaybackEnd; };
		bool							hasText() { return mText.length() > 0; };
		// control values
		void							toggleValue(unsigned int aCtrl);
		void							toggleAuto(unsigned int aCtrl);
		void							toggleTempo(unsigned int aCtrl);

		void							resetAutoAnimation(unsigned int aIndex);
		float							getMinUniformValueByIndex(unsigned int aIndex);
		float							getMaxUniformValueByIndex(unsigned int aIndex);
		int								getSampler2DUniformValueByName(string aName) {
			return mVDAnimation->getSampler2DUniformValueByName(aName);
		};
		vec2							getVec2UniformValueByName(string aName) {
			return mVDAnimation->getVec2UniformValueByName(aName);
		};
		vec3							getVec3UniformValueByName(string aName) {
			return mVDAnimation->getVec3UniformValueByName(aName);
		};
		vec4							getVec4UniformValueByName(string aName) {
			return mVDAnimation->getVec4UniformValueByName(aName);
		};
		int								getIntUniformValueByName(string aName) {
			return mVDAnimation->getIntUniformValueByName(aName);
		};
		bool							getBoolUniformValueByName(string aName) {
			return mVDAnimation->getBoolUniformValueByName(aName);
		};
		bool							getBoolUniformValueByIndex(unsigned int aCtrl) {
			return mVDAnimation->getBoolUniformValueByIndex(aCtrl);
		}
		float							getFloatUniformValueByIndex(unsigned int aCtrl) {
			return mVDAnimation->getFloatUniformValueByIndex(aCtrl);
		};
		float							getFloatUniformValueByName(string aCtrlName) {
			return mVDAnimation->getFloatUniformValueByName(aCtrlName);
		};
		void							setFloatUniformValueByIndex(unsigned int aCtrl, float aValue) {
			// done in router mVDAnimation->changeFloatValue(aCtrl, aValue);
			mVDWebsocket->changeFloatValue(aCtrl, aValue);
		};

		// tempo
		float							getBpm() { return mVDAnimation->getBpm(); };
		void							setBpm(float aBpm) { mVDAnimation->setBpm(aBpm); };
		void							tapTempo() { mVDAnimation->tapTempo(); };
		// audio
		float							getMaxVolume() { return mVDAnimation->maxVolume; };
		float *							getFreqs() { return mVDAnimation->iFreqs; };
		int								getFreqIndexSize() { return mVDAnimation->getFreqIndexSize(); };
		float							getFreq(unsigned int aFreqIndex) { return mVDAnimation->getFloatUniformValueByIndex(31 + aFreqIndex); };
		int								getFreqIndex(unsigned int aFreqIndex) { return mVDAnimation->getFreqIndex(aFreqIndex); };
		void							setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { mVDAnimation->setFreqIndex(aFreqIndex, aFreq); };
		int								getWindowSize() { return mVDAnimation->mWindowSize; };
		bool							isAudioBuffered() { return mVDAnimation->isAudioBuffered(); };
		void							toggleAudioBuffered() { mVDAnimation->toggleAudioBuffered(); };
		bool							getUseLineIn() { return mVDAnimation->getUseLineIn(); };
		void							setUseLineIn(bool useLineIn) { mVDAnimation->setUseLineIn(useLineIn); };
		void							toggleUseLineIn() { mVDAnimation->toggleUseLineIn(); };
		bool							getFreqWSSend() { return mFreqWSSend; };
		void							toggleFreqWSSend() { mFreqWSSend = !mFreqWSSend; };
		// uniforms
		//void							setCrossfade(float aCrossfade); // TODO use setControlValue

		void							setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex) { mVDMix->setFboFragmentShaderIndex(aFboIndex, aFboShaderIndex); };
		unsigned int					getFboFragmentShaderIndex(unsigned int aFboIndex) { return mVDMix->getFboFragmentShaderIndex(aFboIndex); };
		bool							loadShaderFolder(string aFolder);
		int								loadFragmentShader(string aFilePath);
		unsigned int					getShadersCount() { return mVDMix->getShadersCount(); };
		string							getShaderName(unsigned int aShaderIndex) { return mVDMix->getShaderName(aShaderIndex); };
		ci::gl::TextureRef				getShaderThumb(unsigned int aShaderIndex) { return mVDMix->getShaderThumb(aShaderIndex); };
		void							setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName = "") { mVDMix->setFragmentShaderString(aShaderIndex, aFragmentShaderString, aName); };
		//string							getVertexShaderString(unsigned int aShaderIndex) { return mVDMix->getVertexShaderString(aShaderIndex); };
		string							getFragmentShaderString(unsigned int aShaderIndex) { return mVDMix->getFragmentShaderString(aShaderIndex); };
		void							updateShaderThumbFile(unsigned int aShaderIndex) { mVDMix->updateShaderThumbFile(aShaderIndex); };
		void							removeShader(unsigned int aShaderIndex) { mVDMix->removeShader(aShaderIndex); };
		// utils
		int								getWindowsResolution();
		float							getTargetFps() { return mTargetFps; };
		void							blendRenderEnable(bool render);

		// file operations (filedrop, etc)
		//int								loadFileFromAbsolutePath(string aAbsolutePath, int aIndex = 0);
		void							fileDrop(FileDropEvent event);
		// warps
		string							getWarpName(unsigned int aWarpIndex) { return mVDMix->getWarpName(aWarpIndex); };
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex) { return mVDMix->getWarpAFboIndex(aWarpIndex); };
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex) { return mVDMix->getWarpBFboIndex(aWarpIndex); };
		unsigned int					getWarpAShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpCount() { return mVDMix->getWarpCount(); };

		void							createWarpMix() { mVDMix->createWarp(); };
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade) { mVDMix->setWarpCrossfade(aWarpIndex, aCrossfade); };
		float							getWarpCrossfade(unsigned int aWarpIndex) { return mVDMix->getWarpCrossfade(aWarpIndex); };
		bool							isWarpActive(unsigned int aWarpIndex) { return mVDMix->isWarpActive(aWarpIndex); };
		void							toggleWarpActive(unsigned int aWarpIndex) { mVDMix->toggleWarpActive(aWarpIndex); };
		bool							isWarpSolo(unsigned int aWarpIndex) { return mVDMix->isWarpSolo(aWarpIndex); };
		void							toggleWarpSolo(unsigned int aWarpIndex) { mVDMix->toggleWarpSolo(aWarpIndex); };
		bool							isWarpDeleted(unsigned int aWarpIndex) { return mVDMix->isWarpDeleted(aWarpIndex); };
		void							toggleDeleteWarp(unsigned int aWarpIndex) { mVDMix->toggleDeleteWarp(aWarpIndex); };
		bool							isWarpAnimationActive() { return mVDMix->isWarpAnimationActive(); };
		void							toggleWarpAnimationActive() { mVDMix->toggleWarpAnimationActive(); };
		// common to warps and triangles
		bool							isWarpTriangle();
		void							toggleWarpTriangle();
		int								getSolo() { return mVDMix->getSolo(); };
		unsigned int					getSoloOrActiveIndex() { return mVDMix->getSoloOrActiveIndex(); };
		void							setCurrentEditIndex(unsigned int aIndex) { mVDMix->setCurrentEditIndex(aIndex); };
		unsigned int					getCurrentEditIndex() { return mVDMix->getCurrentEditIndex(); };
		// triangles
		string							getTriangleName(unsigned int aTriangleIndex) { return mVDMix->getTriangleName(aTriangleIndex); };
		unsigned int					getTriangleCount() { return mVDMix->getTriangleCount(); };
		unsigned int					getTriangleAFboIndex(unsigned int aWarpIndex) { return mVDMix->getTriangleAFboIndex(aWarpIndex); };
		unsigned int					getTriangleBFboIndex(unsigned int aWarpIndex) { return mVDMix->getTriangleBFboIndex(aWarpIndex); };
		void							setTriangleAFboIndex(unsigned int aTriangleIndex, unsigned int aTriangleFboIndex);
		void							setTriangleBFboIndex(unsigned int aTriangleIndex, unsigned int aTriangleFboIndex);
		bool							isTriangleActive(unsigned int aTriangleIndex) { return mVDMix->isTriangleActive(aTriangleIndex); };
		void							toggleTriangleActive(unsigned int aTriangleIndex) { mVDMix->toggleTriangleActive(aTriangleIndex); };
		bool							isTriangleSolo(unsigned int aTriangleIndex) { return mVDMix->isTriangleSolo(aTriangleIndex); };
		void							toggleTriangleSolo(unsigned int aTriangleIndex) { mVDMix->toggleTriangleSolo(aTriangleIndex); };
		bool							isTriangleDeleted(unsigned int aTriangleIndex) { return mVDMix->isTriangleDeleted(aTriangleIndex); };
		void							toggleDeleteTriangle(unsigned int aTriangleIndex) { mVDMix->toggleDeleteTriangle(aTriangleIndex); };
		void							setTriangleCrossfade(unsigned int aTriangleIndex, float aCrossfade) { mVDMix->setTriangleCrossfade(aTriangleIndex, aCrossfade); };
		float							getTriangleCrossfade(unsigned int aTriangleIndex) { return mVDMix->getTriangleCrossfade(aTriangleIndex); };
		ci::gl::TextureRef				getTriangleTexture(unsigned int aTriangleFboIndex) { return mVDMix->getTriangleTexture(aTriangleFboIndex); }
		void							createTriangleMix() { mVDMix->createTriangle(); };
		int								getTrianglePointX(unsigned int aTriangleIndex, unsigned int aPointIndex) { return mVDMix->getTrianglePointX(aTriangleIndex, aPointIndex); };
		int								getTrianglePointY(unsigned int aTriangleIndex, unsigned int aPointIndex) { return mVDMix->getTrianglePointY(aTriangleIndex, aPointIndex); };
		void							setTrianglePointX(unsigned int aTriangleIndex, unsigned int aPointIndex, unsigned int aValue) { mVDMix->setTrianglePointX(aTriangleIndex, aPointIndex, aValue); };
		void							setTrianglePointY(unsigned int aTriangleIndex, unsigned int aPointIndex, unsigned int aValue) { mVDMix->setTrianglePointY(aTriangleIndex, aPointIndex, aValue); };
		// fbos
		void							fboFlipV(unsigned int aFboIndex) { mVDMix->fboFlipV(aFboIndex); };
		bool							isFboFlipV(unsigned int aFboIndex) { return mVDMix->isFboFlipV(aFboIndex); };
		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex) { return mVDMix->getFboInputTextureIndex(aFboIndex); };
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex) { mVDMix->setFboInputTexture(aFboIndex, aInputTextureIndex); };
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex = 0) { return mVDMix->getFboTexture(aFboIndex); };
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex) { return mVDMix->getFboRenderedTexture(aFboIndex); };
		ci::gl::TextureRef				getFboThumb(unsigned int aBlendIndex) { return mVDMix->getFboThumb(aBlendIndex); };
		string							getFboName(unsigned int aFboIndex) { return mVDMix->getFboName(aFboIndex); };
		//int								getFboTextureWidth(unsigned int aFboIndex);
		//int								getFboTextureHeight(unsigned int aFboIndex);
		unsigned int					getFboListSize() { return mVDMix->getFboListSize(); };
		//string							getFboFragmentShaderText(unsigned int aFboIndex);
		// feedback get/set
		int								getFeedbackFrames() {
			return mVDMix->getFeedbackFrames();
		};
		void							setFeedbackFrames(int aFeedbackFrames) {
			mVDMix->setFeedbackFrames(aFeedbackFrames);
		};
		string							getMixFboName(unsigned int aMixFboIndex);
		ci::gl::TextureRef				getMixTexture(unsigned int aMixFboIndex = 0);
		unsigned int					getMixFbosCount();
		// RTE in release mode ci::gl::Texture2dRef			getRenderedTexture(bool reDraw = true) { return mVDMix->getRenderedTexture(reDraw); };
		ci::gl::TextureRef				getRenderTexture() { return mVDMix->getRenderTexture(); };
		bool							isEnabledAlphaBlending() { return mEnabledAlphaBlending; };
		void							toggleEnabledAlphaBlending() { mEnabledAlphaBlending = !mEnabledAlphaBlending; }
		bool							isRenderTexture() { return mRenderTexture; };
		void							toggleRenderTexture() { mRenderTexture = !mRenderTexture; }
		bool							isFlipH();
		bool							isFlipV();
		void							flipH();
		void							flipV();
		// blendmodes
		unsigned int					getFboBlendCount() { return mVDMix->getFboBlendCount(); };
		void							useBlendmode(unsigned int aBlendIndex) { return mVDMix->useBlendmode(aBlendIndex); };

		// textures
		ci::gl::TextureRef				getInputTexture(unsigned int aTextureIndex) { return mVDMix->getInputTexture(aTextureIndex); };
		string							getInputTextureName(unsigned int aTextureIndex) { return mVDMix->getInputTextureName(aTextureIndex); };
		unsigned int					getInputTexturesCount() { return mVDMix->getInputTexturesCount(); };
		void							loadImageFile(string aFile, unsigned int aTextureIndex) { mVDMix->loadImageFile(aFile, aTextureIndex); };
		void							loadAudioFile(string aFile) { mVDMix->loadAudioFile(aFile); };
		void							loadMovie(string aFile, unsigned int aTextureIndex) { mVDMix->loadMovie(aFile, aTextureIndex); };
		bool							loadImageSequence(string aFolder, unsigned int aTextureIndex) { return mVDMix->loadImageSequence(aFolder, aTextureIndex); };
		void							toggleSharedOutput(unsigned int aMixFboIndex = 0) { mVDMix->toggleSharedOutput(aMixFboIndex); };
		bool							isSharedOutputActive() { return mVDMix->isSharedOutputActive(); };
		unsigned int					getSharedMixIndex() { return mVDMix->getSharedMixIndex(); };
		// move, rotate, zoom methods
		//void							setPosition(int x, int y);
		//void							setZoom(float aZoom);
		int								getInputTextureXLeft(unsigned int aTextureIndex) { return mVDMix->getInputTextureXLeft(aTextureIndex); };
		void							setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft) { mVDMix->setInputTextureXLeft(aTextureIndex, aXLeft); };
		int								getInputTextureYTop(unsigned int aTextureIndex) { return mVDMix->getInputTextureYTop(aTextureIndex); };
		void							setInputTextureYTop(unsigned int aTextureIndex, int aYTop) { mVDMix->setInputTextureYTop(aTextureIndex, aYTop); };
		int								getInputTextureXRight(unsigned int aTextureIndex) { return mVDMix->getInputTextureXRight(aTextureIndex); };
		void							setInputTextureXRight(unsigned int aTextureIndex, int aXRight) { mVDMix->setInputTextureXRight(aTextureIndex, aXRight); };
		int								getInputTextureYBottom(unsigned int aTextureIndex) { return mVDMix->getInputTextureYBottom(aTextureIndex); };
		void							setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom) { mVDMix->setInputTextureYBottom(aTextureIndex, aYBottom); };
		bool							isFlipVInputTexture(unsigned int aTextureIndex) { return mVDMix->isFlipVInputTexture(aTextureIndex); };
		bool							isFlipHInputTexture(unsigned int aTextureIndex) { return mVDMix->isFlipHInputTexture(aTextureIndex); };
		void							inputTextureFlipV(unsigned int aTextureIndex) { mVDMix->inputTextureFlipV(aTextureIndex); };
		void							inputTextureFlipH(unsigned int aTextureIndex) { mVDMix->inputTextureFlipH(aTextureIndex); };
		bool							getInputTextureLockBounds(unsigned int aTextureIndex) { return mVDMix->getInputTextureLockBounds(aTextureIndex); };
		void							toggleInputTextureLockBounds(unsigned int aTextureIndex) { mVDMix->toggleInputTextureLockBounds(aTextureIndex); };
		unsigned int					getInputTextureOriginalWidth(unsigned int aTextureIndex) { return mVDMix->getInputTextureOriginalWidth(aTextureIndex); };
		unsigned int					getInputTextureOriginalHeight(unsigned int aTextureIndex) { return mVDMix->getInputTextureOriginalHeight(aTextureIndex); };
		void							togglePlayPause(unsigned int aTextureIndex) { mVDMix->togglePlayPause(aTextureIndex); };
		// movie
		bool							isMovie(unsigned int aTextureIndex) { return mVDMix->isMovie(aTextureIndex); };
		// sequence
		bool							isSequence(unsigned int aTextureIndex) { return mVDMix->isSequence(aTextureIndex); };
		bool							isLoadingFromDisk(unsigned int aTextureIndex) { return mVDMix->isLoadingFromDisk(aTextureIndex); };
		void							toggleLoadingFromDisk(unsigned int aTextureIndex) { return mVDMix->toggleLoadingFromDisk(aTextureIndex); };
		void							syncToBeat(unsigned int aTextureIndex) { return mVDMix->syncToBeat(aTextureIndex); };
		void							reverse(unsigned int aTextureIndex) { return mVDMix->reverse(aTextureIndex); };
		float							getSpeed(unsigned int aTextureIndex) { return mVDMix->getSpeed(aTextureIndex); };
		void							setSpeed(unsigned int aTextureIndex, float aSpeed) { mVDMix->setSpeed(aTextureIndex, aSpeed); };
		int								getPosition(unsigned int aTextureIndex) { return mVDMix->getPosition(aTextureIndex); };
		void							setPlayheadPosition(unsigned int aTextureIndex, int aPosition) { mVDMix->setPlayheadPosition(aTextureIndex, aPosition); };
		int								getMaxFrame(unsigned int aTextureIndex) { return mVDMix->getMaxFrame(aTextureIndex); };
		// websockets
		void							wsConnect();
		void							wsPing();
		void							wsWrite(std::string msg);
		void							sendFragmentShader(unsigned int aShaderIndex);
		// midi
		void							midiSetup() { mVDRouter->midiSetup(); };
		void							midiOutSendNoteOn(int i, int channel, int pitch, int velocity) { mVDRouter->midiOutSendNoteOn(i, channel, pitch, velocity); };

		int								getMidiInPortsCount() { return mVDRouter->getMidiInPortsCount(); };
		string							getMidiInPortName(int i) { return mVDRouter->getMidiInPortName(i); };
		bool							isMidiInConnected(int i) { return mVDRouter->isMidiInConnected(i); };
		int								getMidiOutPortsCount() { return mVDRouter->getMidiOutPortsCount(); };
		string							getMidiOutPortName(int i) { return mVDRouter->getMidiOutPortName(i); };
		bool							isMidiOutConnected(int i) { return mVDRouter->isMidiOutConnected(i); };
		void							openMidiInPort(int i) { mVDRouter->openMidiInPort(i); };
		void							closeMidiInPort(int i) { mVDRouter->closeMidiInPort(i); };
		void							openMidiOutPort(int i) { mVDRouter->openMidiOutPort(i); };
		void							closeMidiOutPort(int i) { mVDRouter->closeMidiOutPort(i); };
		//! window management
		void							createWindow() { cmd = 0; };
		void							deleteWindow() { cmd = 1; };
		int								getCmd() { int rtn = cmd; cmd = -1; return rtn; };
		// utils
		float							formatFloat(float f) { return mVDUtils->formatFloat(f); };

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
		// audio
		bool							mFreqWSSend;
		// files and paths
		string							mWaveFileName;
		string							mMovieFileName;
		string							mImageSequencePath;
		// delay
		int								mWavePlaybackDelay;
		int								mMoviePlaybackDelay;
		unsigned int					mFadeInDelay;
		unsigned int					mFadeOutDelay;
		// font and text 
		string							mText;
		int								mTextPlaybackDelay;
		int								mTextPlaybackEnd;
		//! Fbos
		// maintain a list of fbo for right only or left/right or more fbos specific to this mix
		//VDFboList						mFboList;
		fs::path						mFbosFilepath;
		// fbo 
		bool							mFlipV;
		bool							mFlipH;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		bool							mEnabledAlphaBlending;
		bool							mRenderTexture;
		//! Warps
		int								mSelectedWarp;
		//! Shaders
		string							mShaderLeft;
		string							mShaderRight;
		//! textures
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		void							updateWarpName(unsigned int aWarpIndex);
		void							updateStream(string * aStringPtr) { mVDMix->updateStream(aStringPtr); };
		//! window management
		int								cmd;
	};

}
