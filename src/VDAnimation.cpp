#include "VDAnimation.h"

using namespace VideoDromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	// mix fbo flip
	mFlipH = true;
	mFlipV = false;
	mBlendRender = false;
	//audio
	mAudioBuffered = false;
	//setUseLineIn(true);
	maxVolume = 0.0f;
	for (int i = 0; i < 7; i++)
	{
		freqIndexes[i] = i * 7;
	}
	for (int i = 0; i < mWindowSize; i++)
	{
		iFreqs[i] = i;
	}
	// live json params
	mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "live_params.json";
	JsonBag::add(&mBackgroundColor, "background_color");
	JsonBag::add(&mExposure, "exposure", []() {
		app::console() << "Updated exposure" << endl;

	});
	JsonBag::add(&mText, "text", []() {
		app::console() << "Updated text" << endl;
	});
	mAutoBeatAnimation = true;
	JsonBag::add(&mAutoBeatAnimation, "autobeatanimation");
	currentScene = 0;

	previousTime = 0.0f;
	counter = 0;
	iTimeFactor = 1.0f;
	// tempo
	mUseTimeWithTempo = false;
	// init timer
	mTimer.start();
	startTime = currentTime = mTimer.getSeconds();
	mBpm = 166;
	iDeltaTime = 60 / mBpm;//mTempo;
	//iBar = 0;
	//iBadTvRunning = false;
	//int ctrl;

	mUniformsJson = getAssetPath("") / mVDSettings->mAssetsPath / "uniforms.json";
	if (fs::exists(mUniformsJson)) {
		loadUniforms(loadFile(mUniformsJson));
	}
	else {
		// global time in seconds
		createFloatUniform("iGlobalTime", 0, 0.0f);
		// sliders
		// red
		createFloatUniform("iFR", 1, 1.0f);
		// green
		createFloatUniform("iFG", 2, 0.3f);
		// blue
		createFloatUniform("iFB", 3, 0.0f);
		// Alpha 
		createFloatUniform("iAlpha", 4, 1.0f);
		// red multiplier 
		createFloatUniform("iRedMultiplier", 5, 1.0f, 0.0f, 3.0f);
		// green multiplier 
		createFloatUniform("iGreenMultiplier", 6, 1.0f, 0.0f, 3.0f);
		// blue multiplier 
		createFloatUniform("iBlueMultiplier", 7, 1.0f, 0.0f, 3.0f);
		// bad tv
		createFloatUniform("iBadTv", 8, 0.0f, 0.0f, 5.0f);

		// RotationSpeed
		createFloatUniform("iRotationSpeed", 9, 0.0f, -2.0f, 2.0f);
		// Steps
		createFloatUniform("iSteps", 10, 16.0f, 1.0f, 128.0f);

		// rotary
		// ratio
		createFloatUniform("iRatio", 11, 20.0f, 0.00000000001f, 20.0f);
		// zoom
		createFloatUniform("iZoom", 12, 1.0f, -3.0f, 3.0f);
		// Audio multfactor 
		createFloatUniform("iAudioMult", 13, 1.0f, 0.01f, 12.0f);
		// exposure
		createFloatUniform("iExposure", 14, 1.0f, 0.0f, 3.0f);
		// Pixelate
		createFloatUniform("iPixelate", 15, 1.0f, 0.01f);
		// Trixels
		createFloatUniform("iTrixels", 16, 0.0f);
		// iChromatic
		createFloatUniform("iChromatic", 17, 0.0f, 0.000000001f);
		// iCrossfade
		createFloatUniform("iCrossfade", 18, 1.0f);
		// background red
		createFloatUniform("iBR", 19, 0.1f);
		// background green
		createFloatUniform("iBG", 20, 0.5f);
		// background blue
		createFloatUniform("iBB", 21, 0.1f);


		// top row 21 to 28
		// Speed 
		createFloatUniform("iSpeed", 22, 12.0f, 0.01f, 12.0f);
		// background alpha
		createFloatUniform("iBA", 23, 0.2f);
		// tempo time
		createFloatUniform("iTempoTime", 24, 0.1f);
		// fps 25
		createFloatUniform("iFps", mVDSettings->IFPS, 60.0f, 0.0f, 500.0f);
		// contour
		createFloatUniform("iContour", 26, 0.0f, 0.0f, 0.5f);
		// slitscan (or other) Param1 
		createFloatUniform("iParam1", 27, 1.0f, 0.01f, 100.0f);
		// slitscan (or other) Param2 
		createFloatUniform("iParam2", 28, 1.0f, 0.01f, 100.0f);
		// iResolutionX (should be fbowidth) 
		createFloatUniform("iResolutionX", 29, mVDSettings->mFboWidth, 0.01f, 1280.0f);
		// iResolutionY (should be fboheight)  
		createFloatUniform("iResolutionY", 30, mVDSettings->mFboHeight, 0.01f, 800.0f);
		// nanokontrol middle row 31 to 38, bottom row 41 to 88
		// iFreq0  
		createFloatUniform("iFreq0", 31, 0.0f, 0.01f, 256.0f);
		// iFreq1  
		createFloatUniform("iFreq1", 32, 0.0f, 0.01f, 256.0f);
		// iFreq2  
		createFloatUniform("iFreq2", 33, 0.0f, 0.01f, 256.0f);
		// iFreq3  
		createFloatUniform("iFreq3", 34, 0.0f, 0.01f, 256.0f);

		// int
		// blend mode 
		createIntUniform("iBlendmode", 50, 0);
		// greyscale 
		createIntUniform("iGreyScale", 51, 0);
		// current beat
		createIntUniform("iBeat", 52, 1);
		// beats per bar 
		createIntUniform("iBeatsPerBar", 53, 4);

		// vec3
		createVec3Uniform("iResolution", 60, vec3(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"), 1.0));
		createVec3Uniform("iColor", 61, vec3(1.0, 0.5, 0.0));
		createVec3Uniform("iBackgroundColor", 62);
		//createVec3Uniform("iChannelResolution[0]", 63, vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));

		// vec4
		createVec4Uniform("iMouse", 70, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("iDate", 71, vec4(2016.0f, 12.0f, 1.0f, 5.0f));

		// boolean
		// invert
		createBoolUniform("iInvert", 48);
		createBoolUniform("iFlipH", 81);
		createBoolUniform("iFlipV", 82);
		createBoolUniform("iXorY", 83);
		// glitch
		createBoolUniform("iGlitch", 45);
		// toggle
		createBoolUniform("iToggle", 46);
		// vignette
		createBoolUniform("iVignette", 47);
	}
	// textures
	for (size_t i = 0; i < 8; i++)
	{
		createSampler2DUniform("iChannel" + toString(i), 100 + i, i);// TODO verify doesn't mess up type (uint!)
	}
	load();
	loadAnimation();
	CI_LOG_V("VDAnimation, iResX:" + toString(getFloatUniformValueByIndex(29)));
	CI_LOG_V("VDAnimation, iResY:" + toString(getFloatUniformValueByIndex(30)));

	setVec3UniformValueByIndex(60, vec3(getFloatUniformValueByIndex(29), getFloatUniformValueByIndex(30), 1.0));
	CI_LOG_V("VDAnimation, iResolution:" + toString(shaderUniforms[getUniformNameForIndex(60)].vec3Value));

}
void VDAnimation::loadUniforms(const ci::DataSourceRef &source) {

	JsonTree json(source);

	// try to load the specified json file
	if (json.hasChild("uniforms")) {
		JsonTree u(json.getChild("uniforms"));

		// iterate warps
		for (size_t i = 0; i < u.getNumChildren(); i++) {
			JsonTree child(u.getChild(i));

			if (child.hasChild("uniform")) {
				JsonTree w(child.getChild("uniform"));
				// create uniform of the correct type
				int uniformType = (w.hasChild("type")) ? w.getValueForKey<int>("type") : 0;
				switch (uniformType) {
				case 0:
					//float
					floatFromJson(child);
					break;
				case 1:
					// sampler2d
					sampler2dFromJson(child);
					break;
				case 2:
					// vec2
					vec2FromJson(child);
					break;
				case 3:
					// vec3
					vec3FromJson(child);
					break;
				case 4:
					// vec4
					vec4FromJson(child);
					break;
				case 5:
					// int
					intFromJson(child);
					break;
				case 6:
					// boolean
					boolFromJson(child);
					break;
				}
			}
		}
	}
}
void VDAnimation::floatFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	float jValue, jMin, jMax;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 249;
		jValue = (u.hasChild("value")) ? u.getValueForKey<float>("value") : 0.01f;
		jMin = (u.hasChild("min")) ? u.getValueForKey<float>("min") : 0.0f;
		jMax = (u.hasChild("max")) ? u.getValueForKey<float>("max") : 1.0f;
		createFloatUniform(jName, jCtrlIndex, jValue, jMin, jMax);
	}
}
void VDAnimation::sampler2dFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	int jTextureIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 250;
		jTextureIndex = (u.hasChild("textureindex")) ? u.getValueForKey<int>("textureindex") : 0;;
		createSampler2DUniform(jName, jTextureIndex);
	}
}
void VDAnimation::vec2FromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 251;
		createVec2Uniform(jName, jCtrlIndex);
	}
}
void VDAnimation::vec3FromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 252;
		createVec3Uniform(jName, jCtrlIndex);
	}
}
void VDAnimation::vec4FromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 253;
		createVec4Uniform(jName, jCtrlIndex);
	}
}
void VDAnimation::intFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex, jValue;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 254;
		jValue = (u.hasChild("value")) ? u.getValueForKey<int>("value") : 1;
		createIntUniform(jName, jCtrlIndex, jValue);
	}

}
void VDAnimation::boolFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	bool jValue;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 255;
		jValue = (u.hasChild("value")) ? u.getValueForKey<bool>("value") : false;
		createBoolUniform(jName, jCtrlIndex, jValue);
	}
}
//! uniform to json
JsonTree VDAnimation::uniformToJson(int i)
{
	JsonTree		json;
	string s = controlIndexes[i];

	JsonTree u = JsonTree::makeArray("uniform");
	// common
	int uniformType = shaderUniforms[s].uniformType;
	u.addChild(ci::JsonTree("type", uniformType));
	u.addChild(ci::JsonTree("name", s));
	u.addChild(ci::JsonTree("index", i));
	// type specific 
	switch (uniformType) {
	case 0:
		//float
		u.addChild(ci::JsonTree("value", shaderUniforms[s].defaultValue));
		u.addChild(ci::JsonTree("min", shaderUniforms[s].minValue));
		u.addChild(ci::JsonTree("max", shaderUniforms[s].maxValue));
		break;
	case 1:
		// sampler2d
		u.addChild(ci::JsonTree("textureindex", shaderUniforms[s].textureIndex));
		break;
	case 5:
		// int
		u.addChild(ci::JsonTree("value", shaderUniforms[s].intValue));
		break;
	case 6:
		// boolean
		u.addChild(ci::JsonTree("value", shaderUniforms[s].boolValue));
		break;
	default:
		break;
	}

	json.pushBack(u);
	return json;
}
void VDAnimation::saveUniforms()
{
	string jName;
	int jCtrlIndex;
	float jMin, jMax;
	JsonTree		json;
	// create uniforms json
	JsonTree uniformsJson = JsonTree::makeArray("uniforms");

	for (unsigned i = 0; i < controlIndexes.size(); ++i) {
		JsonTree		u(uniformToJson(i));
		// create <uniform>
		uniformsJson.pushBack(u);
	}
	// write file
	json.pushBack(uniformsJson);
	json.write(mUniformsJson);
}

void VDAnimation::createFloatUniform(string aName, int aCtrlIndex, float aValue, float aMin, float aMax) {
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].minValue = aMin;
	shaderUniforms[aName].maxValue = aMax;
	shaderUniforms[aName].defaultValue = aValue;
	shaderUniforms[aName].boolValue = false;
	shaderUniforms[aName].autotime = false;
	shaderUniforms[aName].automatic = false;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].floatValue = aValue;
	shaderUniforms[aName].uniformType = 0;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createSampler2DUniform(string aName, int aCtrlIndex, int aTextureIndex) {
	shaderUniforms[aName].textureIndex = aTextureIndex;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 1;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createVec2Uniform(string aName, int aCtrlIndex, vec2 aValue) {
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 2;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].vec2Value = aValue;
}
void VDAnimation::createVec3Uniform(string aName, int aCtrlIndex, vec3 aValue) {
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 3;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].vec3Value = aValue;
}
void VDAnimation::createVec4Uniform(string aName, int aCtrlIndex, vec4 aValue) {
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 4;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].vec4Value = aValue;
}
void VDAnimation::createIntUniform(string aName, int aCtrlIndex, int aValue) {
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 5;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].intValue = aValue;
}
void VDAnimation::createBoolUniform(string aName, int aCtrlIndex, bool aValue) {
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].minValue = 0;
	shaderUniforms[aName].maxValue = 1;
	shaderUniforms[aName].defaultValue = aValue;
	shaderUniforms[aName].boolValue = aValue;
	shaderUniforms[aName].autotime = false;
	shaderUniforms[aName].automatic = false;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].floatValue = aValue;
	shaderUniforms[aName].uniformType = 6;
	shaderUniforms[aName].isValid = true;
}
string VDAnimation::getUniformNameForIndex(int aIndex) {
	return controlIndexes[aIndex];
}
/*bool VDAnimation::hasFloatChanged(int aIndex) {
	if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != controlValues[aIndex]) {
	//CI_LOG_V("hasFloatChanged, aIndex:" + toString(aIndex));
	CI_LOG_V("hasFloatChanged, shaderUniforms[getUniformNameForIndex(aIndex)].floatValue:" + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue));
	CI_LOG_V("hasFloatChanged, controlValues[aIndex]:" + toString(controlValues[aIndex]));
	//CI_LOG_W("hasFloatChanged, getUniformNameForIndex(aIndex):" + toString(getUniformNameForIndex(aIndex)));
	}
	return (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != controlValues[aIndex]);
	}*/
bool VDAnimation::toggleValue(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].boolValue = !shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
	return shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
}
bool VDAnimation::toggleAuto(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automatic = !shaderUniforms[getUniformNameForIndex(aIndex)].automatic;
	return shaderUniforms[getUniformNameForIndex(aIndex)].automatic;
}
bool VDAnimation::toggleTempo(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].autotime = !shaderUniforms[getUniformNameForIndex(aIndex)].autotime;
	return shaderUniforms[getUniformNameForIndex(aIndex)].autotime;
}
void VDAnimation::resetAutoAnimation(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automatic = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autotime = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = shaderUniforms[getUniformNameForIndex(aIndex)].defaultValue;
}

bool VDAnimation::setFloatUniformValueByIndex(unsigned int aIndex, float aValue) {
	bool rtn = false;
	// we can't change iGlobalTime at index 0
	if (aIndex > 0) {
		/*if (aIndex == 31) {
			CI_LOG_V("old value " + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue) + " newvalue " + toString(aValue));
		}*/
		if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != aValue) {
			if (aValue >= shaderUniforms[getUniformNameForIndex(aIndex)].minValue && aValue <= shaderUniforms[getUniformNameForIndex(aIndex)].maxValue) {
				shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = aValue;
				rtn = true;
			}
		}
		// not all controls are from 0.0 to 1.0
		/* not working float lerpValue = lerp<float, float>(shaderUniforms[getUniformNameForIndex(aIndex)].minValue, shaderUniforms[getUniformNameForIndex(aIndex)].maxValue, aValue);
		if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != lerpValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = lerpValue;
			rtn = true;
		}*/
	}
	return rtn;
}

bool VDAnimation::isExistingUniform(string aName) {
	return shaderUniforms[aName].isValid;
}
int VDAnimation::getUniformType(string aName) {
	return shaderUniforms[aName].uniformType;
}
void VDAnimation::load() {
	// Create json file if it doesn't already exist.
#if defined( CINDER_MSW )
	if (fs::exists(mJsonFilePath)) {
		bag()->load(mJsonFilePath);
	}
	else {
		bag()->save(mJsonFilePath);
		bag()->load(mJsonFilePath);
	}
#endif
}
void VDAnimation::save() {
#if defined( CINDER_MSW )
	bag()->save(mJsonFilePath);
	saveAnimation();
	saveUniforms();
#endif
}
void VDAnimation::saveAnimation() {
	// save 
	fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
	JsonTree doc;
	JsonTree badtv = JsonTree::makeArray("badtv");

	for (const auto& item : mBadTV) {
		if (item.second > 0.0001) badtv.addChild(ci::JsonTree(ci::toString(item.first), ci::toString(item.second)));
	}

	doc.pushBack(badtv);
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions());
	// backup save
	string fileName = "animation" + toString(getElapsedFrames()) + ".json";
	mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / fileName;
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions());
}
void VDAnimation::loadAnimation() {

	fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
	// Create json file if it doesn't already exist.
	if (!fs::exists(mJsonFilePath)) {
		std::ofstream oStream(mJsonFilePath.string());
		oStream.close();
	}
	if (!fs::exists(mJsonFilePath)) {
		return;
	}
	try {
		JsonTree doc(loadFile(mJsonFilePath));
		JsonTree badtv(doc.getChild("badtv"));
		for (JsonTree::ConstIter item = badtv.begin(); item != badtv.end(); ++item) {
			const auto& key = std::stoi(item->getKey());
			const auto& value = item->getValue<float>();
			mBadTV[key] = value;

		}
	}
	catch (const JsonTree::ExcJsonParserError&) {
		CI_LOG_W("Failed to parse json file.");
	}
}

void VDAnimation::setExposure(float aExposure) {
	mExposure = aExposure;
}
void VDAnimation::setAutoBeatAnimation(bool aAutoBeatAnimation) {
	mAutoBeatAnimation = aAutoBeatAnimation;
}
bool VDAnimation::handleKeyDown(KeyEvent &event)
{
	//float newValue;
	bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_s:
		// save animation
		save();
		break;
	case KeyEvent::KEY_a:
		// save badtv keyframe
		mBadTV[getElapsedFrames() - 10] = 1.0f;
		//iBadTvRunning = true;
		// duration = 0.2
		shaderUniforms["iBadTv"].floatValue = 5.0f;
		//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		break;
	case KeyEvent::KEY_d:
		// save end keyframe
		setEndFrame(getElapsedFrames() - 10);
		break;

		//case KeyEvent::KEY_x:
	case KeyEvent::KEY_y:
		mVDSettings->iXorY = !mVDSettings->iXorY;
		break;

	default:
		handled = false;
	}
	event.setHandled(handled);

	return event.isHandled();
}
bool VDAnimation::handleKeyUp(KeyEvent &event)
{
	bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_a:
		// save badtv keyframe
		mBadTV[getElapsedFrames()] = 0.001f;
		shaderUniforms["iBadTv"].floatValue = 0.0f;
		break;

	default:
		handled = false;
	}
	event.setHandled(handled);

	return event.isHandled();
}

void VDAnimation::update() {

	if (mBadTV[getElapsedFrames()] == 0) {
		// TODO check shaderUniforms["iBadTv"].floatValue = 0.0f;
	}
	else {
		// duration = 0.2
		//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		shaderUniforms["iBadTv"].floatValue = 5.0f;
	}

	mVDSettings->iChannelTime[0] = getElapsedSeconds();
	mVDSettings->iChannelTime[1] = getElapsedSeconds() - 1;
	mVDSettings->iChannelTime[2] = getElapsedSeconds() - 2;
	mVDSettings->iChannelTime[3] = getElapsedSeconds() - 3;
	// iGlobalTime
	if (mUseTimeWithTempo)
	{
		shaderUniforms["iGlobalTime"].floatValue = shaderUniforms["iTempoTime"].floatValue*iTimeFactor;
	}
	else
	{
		shaderUniforms["iGlobalTime"].floatValue = getElapsedSeconds();
	}
	shaderUniforms["iGlobalTime"].floatValue *= mVDSettings->iSpeedMultiplier;
	// iDate
	time_t now = time(0);
	tm *   t = gmtime(&now);
	shaderUniforms["iDate"].vec4Value = vec4(float(t->tm_year + 1900), float(t->tm_mon + 1), float(t->tm_mday), float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec));

#pragma region animation

	currentTime = mTimer.getSeconds();
	// TODO check bounds
	if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = iDeltaTime*1000000.0;
	int elapsedBeatPerBar = iDeltaTime / shaderUniforms["iBeatsPerBar"].intValue*1000000.0;
	if (elapsedBeatPerBar > 0)
	{
		double moduloBeatPerBar = (time % elapsedBeatPerBar) / 1000000.0;
		iTempoTimeBeatPerBar = (float)moduloBeatPerBar;
		if (iTempoTimeBeatPerBar < previousTimeBeatPerBar)
		{
			if (shaderUniforms["iBeat"].intValue > shaderUniforms["iBeatsPerBar"].intValue ) shaderUniforms["iBeat"].intValue = 0;
			shaderUniforms["iBeat"].intValue++;
		}
		previousTimeBeatPerBar = iTempoTimeBeatPerBar;
	}
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		shaderUniforms["iTempoTime"].floatValue = (float)abs(modulo);
		if (shaderUniforms["iTempoTime"].floatValue < previousTime)
		{
			//iBar++;
			if (mAutoBeatAnimation) mVDSettings->iBeat++;
		}
		previousTime = shaderUniforms["iTempoTime"].floatValue;

		// TODO (modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		for (unsigned int anim = 1; anim < 29; anim++)
		{
			if (shaderUniforms[getUniformNameForIndex(anim)].autotime)
			{
				setFloatUniformValueByIndex(anim, (modulo < 0.1) ? shaderUniforms[getUniformNameForIndex(anim)].maxValue : shaderUniforms[getUniformNameForIndex(anim)].minValue);
			}
			else
			{
				if (shaderUniforms[getUniformNameForIndex(anim)].automatic) {
					setFloatUniformValueByIndex(anim, lmap<float>(shaderUniforms["iTempoTime"].floatValue, 0.00001, iDeltaTime, shaderUniforms[getUniformNameForIndex(anim)].minValue, shaderUniforms[getUniformNameForIndex(anim)].maxValue));
				}
			}
		}

		// foreground color vec3 update
		shaderUniforms["iColor"].vec3Value = vec3(shaderUniforms[getUniformNameForIndex(1)].floatValue, shaderUniforms[getUniformNameForIndex(2)].floatValue, shaderUniforms[getUniformNameForIndex(3)].floatValue);

		// background color vec3 update
		shaderUniforms["iBackgroundColor"].vec3Value = vec3(shaderUniforms[getUniformNameForIndex(19)].floatValue, shaderUniforms[getUniformNameForIndex(20)].floatValue, shaderUniforms[getUniformNameForIndex(21)].floatValue);
		// TODO migrate:
		if (mVDSettings->autoInvert)
		{
			setBoolUniformValueByIndex(48, (modulo < 0.1) ? 1.0 : 0.0);
		}

		if (mVDSettings->tEyePointZ)
		{
			mVDSettings->mCamEyePointZ = (modulo < 0.1) ? mVDSettings->minEyePointZ : mVDSettings->maxEyePointZ;
		}
		else
		{
			mVDSettings->mCamEyePointZ = mVDSettings->autoEyePointZ ? lmap<float>(shaderUniforms["iTempoTime"].floatValue, 0.00001, iDeltaTime, mVDSettings->minEyePointZ, mVDSettings->maxEyePointZ) : mVDSettings->mCamEyePointZ;
		}

	}
#pragma endregion animation
}

// tempo
void VDAnimation::tapTempo()
{
	startTime = currentTime = mTimer.getSeconds();

	mTimer.stop();
	mTimer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time buffer is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void VDAnimation::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	iDeltaTime = averageTime;
	setBpm(60 / averageTime);
}
void VDAnimation::setTimeFactor(const int &aTimeFactor)
{
	switch (aTimeFactor)
	{
	case 0:
		iTimeFactor = 0.0001;
		break;
	case 1:
		iTimeFactor = 0.125;
		break;
	case 2:
		iTimeFactor = 0.25;
		break;
	case 3:
		iTimeFactor = 0.5;
		break;
	case 4:
		iTimeFactor = 0.75;
		break;
	case 5:
		iTimeFactor = 1.0;
		break;
	case 6:
		iTimeFactor = 2.0;
		break;
	case 7:
		iTimeFactor = 4.0;
		break;
	case 8:
		iTimeFactor = 16.0;
		break;
	default:
		iTimeFactor = 1.0;
		break;
	}
}
void VDAnimation::preventLineInCrash() {
	setUseLineIn(false);
	mVDSettings->save();
}
void VDAnimation::saveLineIn() {
	setUseLineIn(true);
	mVDSettings->save();
}