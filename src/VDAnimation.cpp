#include "VDAnimation.h"

using namespace videodromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	// mix fbo flip
	/*mFlipH = false;
	mFlipV = false;*/
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
	// tempo
	mUseTimeWithTempo = false;
	// init timer
	mTimer.start();
	startTime = currentTime = mTimer.getSeconds();
	//mBpm = 166;
	setFloatUniformValueByIndex(mVDSettings->IDELTATIME, 60.0f / 166.0f);

	mUniformsJson = getAssetPath("") / mVDSettings->mAssetsPath / "uniforms.json";
	if (fs::exists(mUniformsJson)) {
		loadUniforms(loadFile(mUniformsJson));
	}
	else {
		// global time in seconds
		createFloatUniform("iTime", mVDSettings->ITIME, 0.0f); // 0
		// sliders
		// red
		createFloatUniform("iFR", mVDSettings->IFR, 1.0f); // 1
		// green
		createFloatUniform("iFG", mVDSettings->IFG, 0.3f); // 2
		// blue
		createFloatUniform("iFB", mVDSettings->IFB, 0.0f); // 3
		// Alpha 
		createFloatUniform("iAlpha", mVDSettings->IFA, 1.0f); // 4
		// red multiplier 
		createFloatUniform("iRedMultiplier", mVDSettings->IFRX, 1.0f, 0.0f, 3.0f); // 5
		// green multiplier 
		createFloatUniform("iGreenMultiplier", mVDSettings->IFGX, 1.0f, 0.0f, 3.0f); // 6 
		// blue multiplier 
		createFloatUniform("iBlueMultiplier", mVDSettings->IFBX, 1.0f, 0.0f, 3.0f); // 7 
		// gstnsmk
		createFloatUniform("iSobel", mVDSettings->ISOBEL, 0.02f, 0.02f, 1.0f); // 8
		// bad tv
		createFloatUniform("iBadTv", mVDSettings->IBADTV, 0.0f, 0.0f, 5.0f); // 9
		// Steps
		createFloatUniform("iSteps", mVDSettings->ISTEPS, 16.0f, 1.0f, 128.0f); // 10

		// rotary
		// ratio
		createFloatUniform("iRatio", mVDSettings->IRATIO, 1.0f, 0.01f, 1.0f); // 11
		// zoom
		createFloatUniform("iZoom", mVDSettings->IZOOM, 1.0f, 0.95f, 1.1f); // 12
		// Audio multfactor 
		createFloatUniform("iAudioMult", mVDSettings->IAUDIOX, 1.0f, 0.01f, 12.0f); // 13
		// exposure
		createFloatUniform("iExposure", mVDSettings->IEXPOSURE, 1.0f, 0.0f, 3.0f); // 14
		// Pixelate
		createFloatUniform("iPixelate", mVDSettings->IPIXELATE, 1.0f, 0.01f); // 15
		// Trixels
		createFloatUniform("iTrixels", mVDSettings->ITRIXELS, 0.0f); // 16
		// iChromatic
		createFloatUniform("iChromatic", mVDSettings->ICHROMATIC, 0.0f, 0.000000001f); // 17
		// iCrossfade
		createFloatUniform("iCrossfade", mVDSettings->IXFADE, 1.0f); // 18
		// tempo time
		createFloatUniform("iTempoTime", mVDSettings->ITEMPOTIME, 0.1f); // 19
		// fps
		createFloatUniform("iFps", mVDSettings->IFPS, 60.0f, 0.0f, 500.0f); // 20

		// iBpm 
		createFloatUniform("iBpm", mVDSettings->IBPM, 165.0f, 0.000000001f, 400.0f); // 21
		// Speed 
		createFloatUniform("iSpeed", mVDSettings->ISPEED, 12.0f, 0.01f, 12.0f); // 22
		// slitscan (or other) Param1 
		createFloatUniform("iParam1", mVDSettings->IPARAM1, 1.0f, 0.01f, 100.0f); // 23
		// slitscan (or other) Param2 
		createFloatUniform("iParam2", mVDSettings->IPARAM2, 1.0f, 0.01f, 100.0f); // 24
		// delta time in seconds
		createFloatUniform("iDeltaTime", mVDSettings->IDELTATIME, 0.0f); // 25
		// background red
		createFloatUniform("iBR", mVDSettings->IBR, 0.1f);  // 26
		// background green
		createFloatUniform("iBG", mVDSettings->IBG, 0.5f); // 27
		// background blue
		createFloatUniform("iBB", mVDSettings->IBB, 0.1f); // 28


		// iResolutionX (should be fbowidth) 
		createFloatUniform("iResolutionX", mVDSettings->IRESX, mVDSettings->mFboWidth, 0.01f, 1280.0f); // 29
		// iResolutionY (should be fboheight)  
		createFloatUniform("iResolutionY", mVDSettings->IRESY, mVDSettings->mFboHeight, 0.01f, 800.0f); // 30
		// weight mix fbo texture 0
		createFloatUniform("iWeight0", mVDSettings->IWEIGHT0, 1.0f); // 31
		// weight texture 1
		createFloatUniform("iWeight1", mVDSettings->IWEIGHT1, 0.0f); // 32
		// weight texture 2
		createFloatUniform("iWeight2", mVDSettings->IWEIGHT2, 0.0f); // 33
		// weight texture 3
		createFloatUniform("iWeight3", mVDSettings->IWEIGHT3, 0.0f); // 34
		// weight texture 4
		createFloatUniform("iWeight4", mVDSettings->IWEIGHT4, 0.0f); // 35
		// weight texture 5
		createFloatUniform("iWeight5", mVDSettings->IWEIGHT5, 0.0f); // 36
		// weight texture 6
		createFloatUniform("iWeight6", mVDSettings->IWEIGHT6, 0.0f); // 37
		// weight texture 7
		createFloatUniform("iWeight7", mVDSettings->IWEIGHT7, 0.0f); // 38
		// elapsed in bar 
		createFloatUniform("iElapsed", mVDSettings->IELAPSED, 0.0f); // 39


		// contour
		createFloatUniform("iContour", mVDSettings->ICONTOUR, 0.0f, 0.0f, 0.5f); // 40
		// RotationSpeed
		createFloatUniform("iRotationSpeed", mVDSettings->IROTATIONSPEED, 0.0f, -2.0f, 2.0f); // 41

		// iMouseX  
		createFloatUniform("iMouseX", mVDSettings->IMOUSEX, (float)mVDSettings->mFboWidth / 2, 0.0f, mVDSettings->mFboWidth); // 42
		// iMouseY  
		createFloatUniform("iMouseY", mVDSettings->IMOUSEY, (float)mVDSettings->mFboHeight / 2, 0.0f, mVDSettings->mFboHeight); // 43
		// iMouseZ  
		createFloatUniform("iMouseZ", mVDSettings->IMOUSEZ, 0.0f, 0.0f, 1.0f); // 44
		// vignette amount
		createFloatUniform("iVAmount", mVDSettings->IVAMOUNT, 0.91f, 0.0f, 1.0f); // 45
		// vignette falloff
		createFloatUniform("iVFallOff", mVDSettings->IVFALLOFF, 0.31f, 0.0f, 1.0f); // 46
		// hydra time
		createFloatUniform("time", mVDSettings->TIME, 0.0f); // 47
		// current beat
		createFloatUniform("iPhase", mVDSettings->IPHASE, 0.0f); // 48;
		// int
		// blend mode 
		createIntUniform("iBlendmode", mVDSettings->IBLENDMODE, 0);
		// greyscale 
		createIntUniform("iGreyScale", 51, 0);
		// current beat
		createIntUniform("iPhase", mVDSettings->IPHASE, 0); // 52
		// beats per bar 
		createIntUniform("iBeatsPerBar", 53, 4);
		// fbo A
		createIntUniform("iFboA", mVDSettings->IFBOA, 0); // 54
		// fbo B
		createIntUniform("iFboB", mVDSettings->IFBOB, 1); // 55
		// iOutW
		createIntUniform("iOutW", mVDSettings->IOUTW, mVDSettings->mRenderWidth); // 56
		// iOutH  
		createIntUniform("iOutH", mVDSettings->IOUTH, mVDSettings->mRenderHeight); // 57
		// beat 
		createIntUniform("iBeat", mVDSettings->IBEAT, 1); // 58
		// bar 
		createIntUniform("iBar", mVDSettings->IBAR, 0); // 59

		// vec3
		createVec3Uniform("iResolution", 60, vec3(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"), 1.0));
		createVec3Uniform("iColor", 61, vec3(1.0, 0.5, 0.0));
		createVec3Uniform("iBackgroundColor", 62);
		//createVec3Uniform("iChannelResolution[0]", 63, vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));

		// vec4
		createVec4Uniform("iMouse", 70, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("iDate", 71, vec4(2016.0f, 12.0f, 1.0f, 5.0f));

		// boolean
		// glitch
		createBoolUniform("iGlitch", mVDSettings->IGLITCH); // 81
		// vignette
		createBoolUniform("iVignette", mVDSettings->IVIGN); // 82 toggle
		// toggle
		createBoolUniform("iToggle", mVDSettings->ITOGGLE); // 83
		// invert
		createBoolUniform("iInvert", mVDSettings->IINVERT); // 86
		createBoolUniform("iXorY", mVDSettings->IXORY); // 101 was 87
		createBoolUniform("iFlipH", mVDSettings->IFLIPH); // 100 toggle was 90
		createBoolUniform("iFlipV", mVDSettings->IFLIPV); // 103 toggle was 92
		// iFreq0  
		createFloatUniform("iFreq0", mVDSettings->IFREQ0, 0.0f, 0.01f, 256.0f); //  140 was 25
		// iFreq1  
		createFloatUniform("iFreq1", mVDSettings->IFREQ1, 0.0f, 0.01f, 256.0f); // 141 was  39 was  26
		// iFreq2  
		createFloatUniform("iFreq2", mVDSettings->IFREQ2, 0.0f, 0.01f, 256.0f); //  142 was 49 was  27
		// iFreq3  
		createFloatUniform("iFreq3", mVDSettings->IFREQ3, 0.0f, 0.01f, 256.0f); //  143 was 50 was  28

	}
	// vec4 kinect2
	createVec4Uniform("iSpineBase", 200, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("SpineMid", 201, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("Neck", 202, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("Head", 203, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ShldrL", 204, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ElbowL", 205, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("WristL", 206, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandL", 207, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ShldrR", 208, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ElbowR", 209, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("WristR", 210, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandR", 211, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HipL", 212, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("KneeL", 213, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("AnkleL", 214, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("FootL", 215, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HipR", 216, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("KneeR", 217, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("AnkleR", 218, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("FootR", 219, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("SpineShldr", 220, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandTipL", 221, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ThumbL", 222, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("HandTipR", 223, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	createVec4Uniform("ThumbR", 224, vec4(320.0f, 240.0f, 0.0f, 0.0f));
	// textures
	for (size_t i = 0; i < 8; i++)
	{
		createSampler2DUniform("iChannel" + toString(i), 300 + i, i);// TODO verify doesn't mess up type (uint!)
	}
	load();
	loadAnimation();

	setVec3UniformValueByIndex(60, vec3(getFloatUniformValueByIndex(mVDSettings->IRESX), getFloatUniformValueByIndex(mVDSettings->IRESY), 1.0));
}
void VDAnimation::loadUniforms(const ci::DataSourceRef &source) {

	JsonTree json(source);

	// try to load the specified json file
	if (json.hasChild("uniforms")) {
		JsonTree u(json.getChild("uniforms"));

		// iterate uniforms
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
	shaderUniforms[aName].autobass = false;
	shaderUniforms[aName].automid = false;
	shaderUniforms[aName].autotreble = false;
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
	shaderUniforms[aName].autobass = false;
	shaderUniforms[aName].automid = false;
	shaderUniforms[aName].autotreble = false;
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
bool VDAnimation::toggleBass(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].autobass = !shaderUniforms[getUniformNameForIndex(aIndex)].autobass;
	return shaderUniforms[getUniformNameForIndex(aIndex)].autobass;
}
bool VDAnimation::toggleMid(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automid = !shaderUniforms[getUniformNameForIndex(aIndex)].automid;
	return shaderUniforms[getUniformNameForIndex(aIndex)].automid;
}
bool VDAnimation::toggleTreble(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].autotreble = !shaderUniforms[getUniformNameForIndex(aIndex)].autotreble;
	return shaderUniforms[getUniformNameForIndex(aIndex)].autotreble;
}
void VDAnimation::resetAutoAnimation(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automatic = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autotime = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autobass = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].automid = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autotreble = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = shaderUniforms[getUniformNameForIndex(aIndex)].defaultValue;
}

bool VDAnimation::setFloatUniformValueByIndex(unsigned int aIndex, float aValue) {
	bool rtn = false;
	// we can't change iTime at index 0
	if (aIndex > 0) {
		/*if (aIndex ==  mVDSettings->IXFADE) {
			CI_LOG_V("v18 old value " + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue) + " newvalue " + toString(aValue));
		}*/
		string uniformName = getUniformNameForIndex(aIndex);
		if (shaderUniforms[uniformName].floatValue != aValue) {
			if ((aValue >= shaderUniforms[uniformName].minValue && aValue <= shaderUniforms[uniformName].maxValue) || shaderUniforms[uniformName].autobass || shaderUniforms[uniformName].automid || shaderUniforms[uniformName].autotreble) {
				shaderUniforms[uniformName].floatValue = aValue;
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
	else {
		// no max 
		if (aIndex == 0) shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = aValue;
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
	/*fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
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
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions()); */
}
void VDAnimation::loadAnimation() {

	/*fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
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
	} */
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
	case KeyEvent::KEY_u:
		// save badtv keyframe
		// not uused for now mBadTV[getElapsedFrames() - 10] = 1.0f;
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
	case KeyEvent::KEY_u:
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
	// iTime
	if (mUseTimeWithTempo)
	{
		// Ableton Link from openframeworks websockets
		/* 20190803 obsolete MAYBE REMOVE iTimeFactor
		float f = getFloatUniformValueByName("iTime");
		float g = shaderUniforms["iTime"].floatValue;
		shaderUniforms["iTime"].floatValue = shaderUniforms["iTempoTime"].floatValue*iTimeFactor;
		CI_LOG_W(" shaderUniforms[iTime].floatValue:" + toString(g));
		CI_LOG_W(" getFloatUniformValueByName(iTime):" + toString(f)); */
		shaderUniforms["iTime"].floatValue = shaderUniforms["iTempoTime"].floatValue * mVDSettings->iSpeedMultiplier * mVDSettings->iTimeFactor;
		shaderUniforms["iElapsed"].floatValue = shaderUniforms["iPhase"].floatValue * mVDSettings->iSpeedMultiplier * mVDSettings->iTimeFactor;
	}
	else
	{
		shaderUniforms["iTime"].floatValue = getElapsedSeconds() * mVDSettings->iSpeedMultiplier * mVDSettings->iTimeFactor;
		shaderUniforms["iElapsed"].floatValue = getElapsedSeconds() * mVDSettings->iSpeedMultiplier * mVDSettings->iTimeFactor;
	}
	// iDate
	time_t now = time(0);
	tm *   t = gmtime(&now);
	shaderUniforms["iDate"].vec4Value = vec4(float(t->tm_year + 1900), float(t->tm_mon + 1), float(t->tm_mday), float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec));

#pragma region animation

	currentTime = mTimer.getSeconds();
	// TODO check bounds
	if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = shaderUniforms["iDeltaTime"].floatValue * 1000000.0;
	int elapsedBeatPerBar = shaderUniforms["iDeltaTime"].floatValue / shaderUniforms["iBeatsPerBar"].intValue*1000000.0;
	/* if (elapsedBeatPerBar > 0)
	{
		double moduloBeatPerBar = (time % elapsedBeatPerBar) / 1000000.0;
		iTempoTimeBeatPerBar = (float)moduloBeatPerBar;
		if (iTempoTimeBeatPerBar < previousTimeBeatPerBar)
		{
			if (shaderUniforms["iPhase"].intValue > shaderUniforms["iBeatsPerBar"].intValue) shaderUniforms["iPhase"].intValue = 0;
			shaderUniforms["iPhase"].intValue++;
		}
		previousTimeBeatPerBar = iTempoTimeBeatPerBar;
	} */
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		shaderUniforms["iTempoTime"].floatValue = (float)abs(modulo);
		/* if (shaderUniforms["iTempoTime"].floatValue < previousTime)
		{
			//iBar++;
			//if (mAutoBeatAnimation) mVDSettings->iPhase++;
		} */
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
					setFloatUniformValueByIndex(anim, lmap<float>(shaderUniforms["iTempoTime"].floatValue, 0.00001, getFloatUniformValueByIndex(mVDSettings->IDELTATIME), shaderUniforms[getUniformNameForIndex(anim)].minValue, shaderUniforms[getUniformNameForIndex(anim)].maxValue));
				}
				else
				{
					if (shaderUniforms[getUniformNameForIndex(anim)].autobass) {
						setFloatUniformValueByIndex(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getFloatUniformValueByIndex(mVDSettings->IFREQ0) / 25.0f);
					}
					else
					{
						if (shaderUniforms[getUniformNameForIndex(anim)].automid) {
							setFloatUniformValueByIndex(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getFloatUniformValueByIndex(mVDSettings->IFREQ1) / 5.0f);
						}
						else
						{
							if (shaderUniforms[getUniformNameForIndex(anim)].autotreble) {
								setFloatUniformValueByIndex(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getFloatUniformValueByIndex(mVDSettings->IFREQ2) / 2.0f);
							}
						}
					}
				}
			}
		}

		// foreground color vec3 update
		shaderUniforms["iColor"].vec3Value = vec3(shaderUniforms[getUniformNameForIndex(mVDSettings->IFR)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IFG)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IFB)].floatValue);
		// background color vec3 update
		shaderUniforms["iBackgroundColor"].vec3Value = vec3(shaderUniforms[getUniformNameForIndex(mVDSettings->IBR)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IBG)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IBB)].floatValue);
		// mouse vec4 update
		shaderUniforms["iMouse"].vec4Value = vec4(shaderUniforms[getUniformNameForIndex(mVDSettings->IMOUSEX)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IMOUSEY)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IMOUSEZ)].floatValue, 0.0f);
		// TODO migrate:
		if (mVDSettings->autoInvert)
		{
			setBoolUniformValueByIndex(mVDSettings->IINVERT, (modulo < 0.1) ? 1.0 : 0.0);
		}

		if (mVDSettings->tEyePointZ)
		{
			mVDSettings->mCamEyePointZ = (modulo < 0.1) ? mVDSettings->minEyePointZ : mVDSettings->maxEyePointZ;
		}
		else
		{
			mVDSettings->mCamEyePointZ = mVDSettings->autoEyePointZ ? lmap<float>(shaderUniforms["iTempoTime"].floatValue, 0.00001, getFloatUniformValueByIndex(mVDSettings->IDELTATIME), mVDSettings->minEyePointZ, mVDSettings->maxEyePointZ) : mVDSettings->mCamEyePointZ;
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
	setFloatUniformValueByIndex(mVDSettings->IDELTATIME, averageTime);
	setBpm(60 / averageTime);
}
void VDAnimation::setTimeFactor(const int &aTimeFactor)
{
	switch (aTimeFactor)
	{
	case 0:
		mVDSettings->iTimeFactor = 0.0001;
		break;
	case 1:
		mVDSettings->iTimeFactor = 0.125;
		break;
	case 2:
		mVDSettings->iTimeFactor = 0.25;
		break;
	case 3:
		mVDSettings->iTimeFactor = 0.5;
		break;
	case 4:
		mVDSettings->iTimeFactor = 0.75;
		break;
	case 5:
		mVDSettings->iTimeFactor = 1.0;
		break;
	case 6:
		mVDSettings->iTimeFactor = 2.0;
		break;
	case 7:
		mVDSettings->iTimeFactor = 4.0;
		break;
	case 8:
		mVDSettings->iTimeFactor = 16.0;
		break;
	default:
		mVDSettings->iTimeFactor = 1.0;
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