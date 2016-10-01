#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"

// json
#include "cinder/Json.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
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
		int							uniformType;
		int							index;
		float						floatValue;
		int							textureIndex;
		bool						isValid;
	};

	class VDAnimation {
	public:		
		VDAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession);

		static VDAnimationRef		create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDAnimation>(new VDAnimation( aVDSettings, aVDSession));
		}
		void						update();
		void						load();
		void						save();

		Color						getBackgroundColor() { return mBackgroundColor; };
		float						getExposure() { return mExposure; };
		void						setExposure(float aExposure);
		bool						getAutoBeatAnimation() { return mAutoBeatAnimation; };
		void						setAutoBeatAnimation( bool aAutoBeatAnimation);

		const int					maxBlendMode = 28;
		// tap tempo
		//float						mTempo;
		float						iDeltaTime;
		float						iTempoTime;
		float						iTimeFactor;
		bool						mUseTimeWithTempo;
		float						iTempoTimeBeatPerBar;
		// exposure
		float						defaultExposure;
		float						minExposure;
		bool						tExposure;
		bool						autoExposure;
		const float					maxExposure = 3.0;
		//zoom
		float						defaultZoom;
		float						minZoom;
		float						maxZoom;
		bool						tZoom;
		bool						autoZoom;
		//chromatic
		float						defaultChromatic;
		float						minChromatic;
		float						maxChromatic;
		bool						tChromatic;
		bool						autoChromatic;
		//ratio
		float						defaultRatio;
		float						minRatio;
		float						maxRatio;
		bool						tRatio;
		bool						autoRatio;
		// z position
		float						defaultZPos;
		float						minZPos;
		float						maxZPos;
		bool						tZPos;
		bool						autoZPos;
		// Rotation Speed
		float						defaultRotationSpeed;
		float						minRotationSpeed;
		float						maxRotationSpeed;
		bool						tRotationSpeed;
		bool						autoRotationSpeed;
		// colors
		bool						tFR, tFG, tFB, tFA, tBR, tBG, tBB, tBA;
		bool						autoFR, autoFG, autoFB, autoFA, autoBR, autoBG, autoBB, autoBA;

		// animation functions
		void						resetChromatic();
		void						tempoChromatic();
		void						lockChromatic() { autoChromatic = !autoChromatic; };
		void						resetRatio();
		void						tempoRatio();
		void						lockRatio() { autoRatio = !autoRatio; };
		void						resetExposure();
		void						tempoExposure();
		void						lockExposure() { autoExposure = !autoExposure; };
		void						resetZoom();
		void						tempoZoom();
		void						lockZoom() { autoZoom = !autoZoom; };
		void						resetZPos();
		void						tempoZPos();
		void						lockZPos() { autoZPos = !autoZPos; };
		void						resetRotationSpeed();
		void						tempoRotationSpeed();
		void						lockRotationSpeed() { autoRotationSpeed = !autoRotationSpeed; };
		void						resetRed();
		void						tempoRed();
		void						lockRed() { autoFR = !autoFR; };
		// tempo
		void						tapTempo();
		void						setTimeFactor(const int &aTimeFactor);
		//int							iBar;
		int							iBeatIndex; //1 to beatsperbar
		// animation
		int							currentScene;
		//int							getBadTV(int frame);
		// keyboard
		bool						handleKeyDown(KeyEvent &event);
		bool						handleKeyUp(KeyEvent &event);
		// audio
		float						maxVolume;
		bool						mUseLineIn;
		float						iFreqs[7];
		// OSC/MIDI/JSON controlled UI and params
		map<int, string>			controlIndexes;
		map<int, float>				controlValues;
		map<int, vec2>				vec2Values;
		map<int, vec3>				vec3Values;
		map<int, vec4>				vec4Values;
		map<int, int>				intValues;
		map<int, bool>				boolValues;
		int							getValuesSize() { return controlValues.size(); };
		// shaders
		bool						isExistingUniform(string aName);
		int							getUniformType(string aName);
		string						getUniformNameForIndex(int aIndex);
		bool						hasFloatChanged(int aIndex);
		bool						changeFloatValue(int aIndex, float aValue);
		float						getFloatUniformValueByIndex(int aIndex);
		float						getFloatUniformValueByName(string aName);
		int							getSampler2DUniformValue(string aName);
		vec2						getVec2UniformValue(string aName);
		vec3						getVec3UniformValue(string aName);
		vec4						getVec4UniformValue(string aName);
		int							getIntUniformValue(string aName);
		bool						getBoolUniformValue(string aName);
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;
		// Live json params
		fs::path					mJsonFilePath;
		Parameter<Color>			mBackgroundColor;
		Parameter<float>			mExposure;
		Parameter<string>			mText;
		Parameter<bool>				mAutoBeatAnimation;
		// shaders
		map<string, VDUniform>		shaderUniforms;
		void						createFloatUniform(string aName, int aCtrlIndex, float aValue = 0.01f);
		void						createSampler2DUniform(string aName, int aTextureIndex = 0);
		void						createVec2Uniform(string aName, int aCtrlIndex, vec2 aValue = vec2(0.0));
		void						createVec3Uniform(string aName, int aCtrlIndex, vec3 aValue = vec3(0.0));
		void						createVec4Uniform(string aName, int aCtrlIndex, vec4 aValue = vec4(0.0));
		void						createIntUniform(string aName, int aCtrlIndex, int aValue = 1);
		void						createBoolUniform(string aName, int aCtrlIndex, bool aValue = false);

		// timed animation
		ci::Timer					timer;
		std::deque <double>			buffer;
		void						calculateTempo();
		int							counter;
		double						averageTime;
		double						currentTime;
		double						startTime;
		float						previousTime;
		float						previousTimeBeatPerBar;
		JsonTree					mData;
		void						loadAnimation();
		void						saveAnimation();

		std::unordered_map<int, float>	mBadTV;
	};
}
