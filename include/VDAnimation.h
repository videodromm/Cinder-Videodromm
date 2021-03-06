#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"

// json
#include "cinder/Json.h"
// Settings
#include "VDSettings.h"
// Live json params
#include "LiveParam.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace live;

namespace videodromm
{
	// stores the pointer to the VDAnimation instance
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;

	//enum class UniformTypes { FLOAT, SAMPLER2D, VEC2, VEC3, VEC4, INT, BOOL };

	struct VDUniform
	{
		int								uniformType;
		int								index;
		float							defaultValue;
		float							floatValue;
		bool							boolValue;
		int								intValue;
		vec2							vec2Value;
		vec3							vec3Value;
		vec4							vec4Value;
		float							minValue;
		float							maxValue;
		bool							autotime;
		bool							automatic;
		bool							autobass;
		bool							automid;
		bool							autotreble;
		int								textureIndex;
		bool							isValid;
	};

	class VDAnimation {
	public:		
		VDAnimation(VDSettingsRef aVDSettings);

		static VDAnimationRef			create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDAnimation>(new VDAnimation(aVDSettings));
		}
		void							update();
		void							load();
		void							save();

		Color							getBackgroundColor() { return mBackgroundColor; };
		float							getExposure() { return mExposure; };
		void							setExposure(float aExposure);
		bool							getAutoBeatAnimation() { return mAutoBeatAnimation; };
		void							setAutoBeatAnimation( bool aAutoBeatAnimation);

		const int						mBlendModes = 28;
		void							blendRenderEnable(bool render) { mBlendRender = render; };
		// tap tempo
		bool							mUseTimeWithTempo;
		void							toggleUseTimeWithTempo() { mUseTimeWithTempo = !mUseTimeWithTempo; };
		void							useTimeWithTempo() { mUseTimeWithTempo = true; };
		bool							getUseTimeWithTempo() {return mUseTimeWithTempo; };
		float							iTempoTimeBeatPerBar;
		float							getBpm() { return getFloatUniformValueByIndex(mVDSettings->IBPM); };
		void							setBpm(float aBpm) {
			if(aBpm > 0.0f) {
				setFloatUniformValueByIndex(mVDSettings->IBPM, aBpm);
				setFloatUniformValueByIndex(mVDSettings->IDELTATIME, 60 / aBpm);
			}
		};
		void							tapTempo();
		void							setTimeFactor(const int &aTimeFactor);
		int								getEndFrame() { return mEndFrame; };
		void							setEndFrame(int frame) { mEndFrame = frame; };

		int								mLastBeat = 0;
		// animation
		int								currentScene;
		//int							getBadTV(int frame);
		// keyboard
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		// audio
		float							maxVolume;		
		static const int				mWindowSize = 128; // fft window size
		float							iFreqs[mWindowSize];
		void							preventLineInCrash(); // at next launch
		void							saveLineIn();
		bool							getUseAudio() {
			return mVDSettings->mUseAudio;
		};
		bool							getUseLineIn() {
			return mVDSettings->mUseLineIn;
		};
		void							setUseLineIn(bool useLineIn) {
			mVDSettings->mUseLineIn = useLineIn;
		};
		void							toggleUseLineIn() { mVDSettings->mUseLineIn =  !mVDSettings->mUseLineIn; };

		// audio
		bool							isAudioBuffered() { return mAudioBuffered; };
		void							toggleAudioBuffered() { mAudioBuffered = !mAudioBuffered; };

		// shaders
		bool							isExistingUniform(string aName);
		int								getUniformType(string aName);
		string							getUniformNameForIndex(int aIndex);
		bool							toggleAuto(unsigned int aIndex);
		bool							toggleValue(unsigned int aIndex);
		bool							toggleTempo(unsigned int aIndex);
		bool							toggleBass(unsigned int aIndex);
		bool							toggleMid(unsigned int aIndex);
		bool							toggleTreble(unsigned int aIndex);
		void							resetAutoAnimation(unsigned int aIndex);
		bool							setFloatUniformValueByIndex(unsigned int aIndex, float aValue);

		bool							setBoolUniformValueByIndex(unsigned int aIndex, bool aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].boolValue = aValue;
			return aValue;
		}
		void							setIntUniformValueByName(string aName, int aValue) {
			shaderUniforms[aName].intValue = aValue;
		};
		void							setIntUniformValueByIndex(unsigned int aIndex, int aValue) {
			if (mVDSettings->IBEAT == aIndex) {
				if (aValue != mLastBeat) {
					mLastBeat = aValue;
					if (aValue == 0) setIntUniformValueByIndex(mVDSettings->IBAR, getIntUniformValueByIndex(mVDSettings->IBAR) + 1);
				}
			}
			shaderUniforms[getUniformNameForIndex(aIndex)].intValue = aValue;
		}
		void							setFloatUniformValueByName(string aName, float aValue) {
			shaderUniforms[aName].floatValue = aValue;
		}
		void setVec2UniformValueByName(string aName, vec2 aValue) {
			shaderUniforms[aName].vec2Value = aValue;
		}
		void setVec2UniformValueByIndex(unsigned int aIndex, vec2 aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].vec2Value = aValue;
		}
		void setVec3UniformValueByName(string aName, vec3 aValue) {
			shaderUniforms[aName].vec3Value = aValue;
		}
		void setVec3UniformValueByIndex(unsigned int aIndex, vec3 aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].vec3Value = aValue;
		}
		void setVec4UniformValueByName(string aName, vec4 aValue) {
			shaderUniforms[aName].vec4Value = aValue;
		}
		void setVec4UniformValueByIndex(unsigned int aIndex, vec4 aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].vec4Value = aValue;
		}
		bool							getBoolUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
		}
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].minValue;
		}
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].maxValue;
		}
		float							getMinUniformValueByName(string aName) {
			return shaderUniforms[aName].minValue;
		}
		float							getMaxUniformValueByName(string aName) {
			return shaderUniforms[aName].maxValue;
		}
		bool							getBoolUniformValueByName(string aName) {
			return shaderUniforms[aName].boolValue;
		}
		vec2							getVec2UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].vec2Value;
		};
		vec3							getVec3UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].vec3Value;
		};
		vec4							getVec4UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].vec4Value;
		};
		float							getFloatUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].floatValue;
		}
		float							getFloatUniformDefaultValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].defaultValue;
		}
		int								getIntUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].intValue;
		}
		int								getSampler2DUniformValueByName(string aName) {
			return shaderUniforms[aName].textureIndex;
		}
		float							getFloatUniformValueByName(string aName) {
			if (aName.length() > 0) {
				return shaderUniforms[aName].floatValue;
			}
			else {
				CI_LOG_V("getFloatUniformValueByName name empty");
				return 1.0f;
			}
		}
		vec2							getVec2UniformValueByName(string aName) {
			return shaderUniforms[aName].vec2Value;
		}
		vec3							getVec3UniformValueByName(string aName) {
			return shaderUniforms[aName].vec3Value;
		}
		vec4							getVec4UniformValueByName(string aName) {
			return shaderUniforms[aName].vec4Value;
		}
		int								getIntUniformValueByName(string aName) {
			return shaderUniforms[aName].intValue;
		};

		// mix fbo
		/* bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };
		void							flipH() { mFlipH = !mFlipH; };
		void							flipV() { mFlipV = !mFlipV; }; */
		bool							isFlipH() { return getBoolUniformValueByIndex(mVDSettings->IFLIPH); };
		bool							isFlipV() { return getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		void							flipH() { setBoolUniformValueByIndex(mVDSettings->IFLIPH, !getBoolUniformValueByIndex(mVDSettings->IFLIPH)); };
		void							flipV() { setBoolUniformValueByIndex(mVDSettings->IFLIPV, !getBoolUniformValueByIndex(mVDSettings->IFLIPV)); };
		
		unsigned int					getBlendModesCount() { return mBlendModes; };
		bool							renderBlend() { return mBlendRender; };

		// timed animation
		int								mEndFrame;
		int								getFreqIndexSize() { return freqIndexes.size(); };
		int								getFreqIndex(unsigned int aFreqIndex) { return freqIndexes[aFreqIndex]; };
		void							setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { freqIndexes[aFreqIndex] = aFreq; };
		//float							getFreq(unsigned int aFreqIndex) { return iFreqs[freqIndexes[aFreqIndex]]; };
		// public for hydra
		void							createFloatUniform(string aName, int aCtrlIndex, float aValue = 0.01f, float aMin = 0.0f, float aMax = 1.0f);
		void							createSampler2DUniform(string aName, int aCtrlIndex, int aTextureIndex = 0);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		map<int, int>					freqIndexes;
		bool							mAudioBuffered;
		// Live json params
		fs::path						mJsonFilePath;
		Parameter<Color>				mBackgroundColor;
		Parameter<float>				mExposure;
		Parameter<string>				mText;
		Parameter<bool>					mAutoBeatAnimation;
		// shaders
		map<int, string>				controlIndexes;
		map<string, VDUniform>			shaderUniforms;
		//! read a uniforms json file 
		void							loadUniforms(const ci::DataSourceRef &source);
		void							floatFromJson(const ci::JsonTree &json);
		void							sampler2dFromJson(const ci::JsonTree &json);
		void							vec2FromJson(const ci::JsonTree &json);
		void							vec3FromJson(const ci::JsonTree &json);
		void							vec4FromJson(const ci::JsonTree &json);
		void							intFromJson(const ci::JsonTree &json);
		void							boolFromJson(const ci::JsonTree &json);
		fs::path						mUniformsJson;


		void							createVec2Uniform(string aName, int aCtrlIndex, vec2 aValue = vec2(0.0));
		void							createVec3Uniform(string aName, int aCtrlIndex, vec3 aValue = vec3(0.0));
		void							createVec4Uniform(string aName, int aCtrlIndex, vec4 aValue = vec4(0.0));
		void							createIntUniform(string aName, int aCtrlIndex, int aValue = 1);
		void							createBoolUniform(string aName, int aCtrlIndex, bool aValue = false);
		//! write a uniforms json file
		void							saveUniforms();
		ci::JsonTree					uniformToJson(int i);

		// time
		ci::Timer						mTimer;
		std::deque <double>				buffer;
		void							calculateTempo();
		int								counter;
		double							averageTime;
		double							currentTime;
		double							startTime;
		float							previousTime;
		float							previousTimeBeatPerBar;
		JsonTree						mData;
		void							loadAnimation();
		void							saveAnimation();

		std::unordered_map<int, float>	mBadTV;
		//bool							mFlipH;
		//bool							mFlipV;
		bool							mBlendRender;
		// timed animation
		//float							mBpm;

	};
}
