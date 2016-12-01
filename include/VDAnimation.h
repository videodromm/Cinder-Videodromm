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

namespace VideoDromm
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
		//float							mTempo;
		float							iDeltaTime;
		float							iTempoTime;
		float							iTimeFactor;
		bool							mUseTimeWithTempo;
		float							iTempoTimeBeatPerBar;
		float							getBpm() { return mBpm; };
		void							setBpm(float aBpm) { iDeltaTime = 60 / mBpm; mBpm = aBpm; };
		void							tapTempo();
		void							setTimeFactor(const int &aTimeFactor);
		int								getEndFrame() { return mEndFrame; };
		void							setEndFrame(int frame) { mEndFrame = frame; };

		//int							iBar;
		int								iBeatIndex; //1 to beatsperbar
		// animation
		int								currentScene;
		//int							getBadTV(int frame);
		// keyboard
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		// audio
		float							maxVolume;
		float							iFreqs[7];
		bool							mUseLineIn;

		// shaders
		bool							isExistingUniform(string aName);
		int								getUniformType(string aName);
		string							getUniformNameForIndex(int aIndex);
		bool							toggleAuto(unsigned int aIndex);
		bool							toggleValue(unsigned int aIndex);
		bool							toggleTempo(unsigned int aIndex);
		void							resetAutoAnimation(unsigned int aIndex);
		bool							changeFloatValue(unsigned int aIndex, float aValue);
		bool							changeBoolValue(unsigned int aIndex, bool aValue);
		void							changeIntValue(unsigned int aIndex, int aValue);
		void							changeVec2Value(unsigned int aIndex, vec2 aValue);
		void							changeVec3Value(unsigned int aIndex, vec3 aValue);
		void							changeVec4Value(unsigned int aIndex, vec4 aValue);
		float							getFloatUniformValueByIndex(unsigned int aIndex);
		bool							getBoolUniformValueByIndex(unsigned int aIndex);
		float							getFloatUniformValueByName(string aName);
		int								getSampler2DUniformValue(string aName);
		vec2							getVec2UniformValue(string aName);
		vec3							getVec3UniformValue(string aName);
		vec4							getVec4UniformValue(string aName);
		int								getIntUniformValue(string aName);
		bool							getBoolUniformValue(string aName);
		float							getMinUniformValueByIndex(unsigned int aIndex);
		float							getMaxUniformValueByIndex(unsigned int aIndex);
		// mix fbo
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };
		void							flipH() { mFlipH = !mFlipH; };
		void							flipV() { mFlipV = !mFlipV; };
		unsigned int					getBlendModesCount() { return mBlendModes; };
		bool							renderBlend() { return mBlendRender; };

		// timed animation
		int								mEndFrame;
		int								iBeatsPerBar;
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Live json params
		fs::path						mJsonFilePath;
		Parameter<Color>				mBackgroundColor;
		Parameter<float>				mExposure;
		Parameter<string>				mText;
		Parameter<bool>					mAutoBeatAnimation;
		// shaders
		map<int, string>				controlIndexes;
		map<string, VDUniform>			shaderUniforms;
		void							createFloatUniform(string aName, int aCtrlIndex, float aValue = 0.01f, float aMin = 0.0f, float aMax = 1.0f);
		void							createSampler2DUniform(string aName, int aTextureIndex = 0);
		void							createVec2Uniform(string aName, int aCtrlIndex, vec2 aValue = vec2(0.0));
		void							createVec3Uniform(string aName, int aCtrlIndex, vec3 aValue = vec3(0.0));
		void							createVec4Uniform(string aName, int aCtrlIndex, vec4 aValue = vec4(0.0));
		void							createIntUniform(string aName, int aCtrlIndex, int aValue = 1);
		void							createBoolUniform(string aName, int aCtrlIndex, bool aValue = false);

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
		bool							mFlipH;
		bool							mFlipV;
		bool							mBlendRender;
		// timed animation
		float							mBpm;

	};
}
