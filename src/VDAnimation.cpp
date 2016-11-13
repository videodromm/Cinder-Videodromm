#include "VDAnimation.h"

using namespace VideoDromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	// mix fbo flip
	mFlipH = true;
	mFlipV = true;
	//audio
	mUseLineIn = true;
	maxVolume = 0.0f;
	for (int i = 0; i < 7; i++)
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
	// zoom
	defaultZoom = 1.0f;
	minZoom = -3.1f;
	maxZoom = 3.0f;
	tZoom = autoZoom = false;
	// exposure
	defaultExposure = 1.0;
	minExposure = 0.0001;
	tExposure = autoExposure = false;
	// Chromatic
	defaultChromatic = 0.0;
	minChromatic = 0.000000001;
	maxChromatic = 1.0;
	tChromatic = autoChromatic = false;
	// ratio
	defaultRatio = 20.0;
	minRatio = 0.00000000001;
	maxRatio = 20.0;
	tRatio = autoRatio = false;
	// z position
	defaultZPos = -0.7;
	minZPos = -1.0;
	maxZPos = 1.0;
	tZPos = autoZPos = false;
	// RotationSpeed
	defaultRotationSpeed = 0.0;
	minRotationSpeed = -2.0;
	maxRotationSpeed = 2.0;
	tRotationSpeed = autoRotationSpeed = false;
	//colors
	autoFR = autoFG = autoFB = autoFA = autoBR = autoBG = autoBB = autoBA = false;
	tFR = tFG = tFB = tFA = tBR = tBG = tBB = tBA = false;

	previousTime = 0.0f;
	iBeatIndex = 1;
	counter = 0;
	iTempoTime = 0.0;
	iTimeFactor = 1.0f;
	// tempo
	mUseTimeWithTempo = false;
	// init timer
	timer.start();
	startTime = currentTime = timer.getSeconds();

	iDeltaTime = 60 / mVDSession->getBpm();//mTempo;
	//iBar = 0;
	//iBadTvRunning = false;
	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0.01f;
	}
	int ctrl;
	// not used
	createFloatUniform("iUseless", 0, 1.0f);
	// red
	createFloatUniform("iFR", 1, 1.0f);
	// green
	createFloatUniform("iFG", 2, 0.3f);
	// blue
	createFloatUniform("iFB", 3, 0.0f);
	// Alpha 
	createFloatUniform("iAlpha", 4, 1.0f);
	// background red
	createFloatUniform("iBR", 5, 0.1f);
	// background green
	createFloatUniform("iBG", 6, 0.1f);
	// background blue
	createFloatUniform("iBB", 7, 0.1f);
	// background alpha
	createFloatUniform("iBA", 8, 0.2f);
	// pointsphere zPosition
	createFloatUniform("iZPos", 9, -0.7f);
	// iChromatic
	createFloatUniform("iChromatic", 10, 0.0f);
	// ratio
	createFloatUniform("iRatio", 11, 20.0f);
	// Speed 
	createFloatUniform("iSpeed", 12, 12.0f);
	// Audio multfactor 
	createFloatUniform("iAudioMult", 13, 1.0f);
	// exposure
	createFloatUniform("iExposure", 14, 1.0f);
	// Pixelate
	createFloatUniform("iPixelate", 15, 1.0f);
	// Trixels
	createFloatUniform("iTrixels", 16, 0.0f);
	// GridSize
	createFloatUniform("iGridSize", 17, 0.0f);
	// iCrossfade
	createFloatUniform("iCrossFade", 18, 1.0f);
	// RotationSpeed
	createFloatUniform("iRotationSpeed", 19, 0.0f);
	// Steps
	createFloatUniform("iSteps", 20, 16.0f);
	// iPreviewCrossfade
	createFloatUniform("iPreviewCrossfade", 21, 0.0f);
	// zoom
	createFloatUniform("iZoom", 22, 1.0f);
	// glitch
	createFloatUniform("iGlitch", 45, 0.0f);
	// toggle
	createFloatUniform("iToggle", 46, 0.0f);
	// vignette
	createFloatUniform("iVignette", 47, 0.0f);
	// invert
	createFloatUniform("iInvert", 48, 0.0f);
	// global time in seconds
	createFloatUniform("iGlobalTime", 49, 0.0f);
	// vec3
	createVec3Uniform("iResolution", 0, vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
	createVec3Uniform("iColor", 1, vec3(1.0, 0.5, 0.0));
	createVec3Uniform("iBackgroundColor", 2);
	createVec3Uniform("iChannelResolution[0]", 3, vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
	// boolean
	createBoolUniform("iFlipH", 0);
	createBoolUniform("iFlipV", 1);
	for (size_t i = 0; i < 8; i++)
	{
		createSampler2DUniform("iChannel" + toString(i), i);
	}
	load();
	loadAnimation();
}
void VDAnimation::createSampler2DUniform(string aName, int aTextureIndex) {
	shaderUniforms[aName].textureIndex = aTextureIndex;
	shaderUniforms[aName].uniformType = 1;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createFloatUniform(string aName, int aCtrlIndex, float aValue) {
	controlValues[aCtrlIndex] = aValue;
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].floatValue = aValue;
	shaderUniforms[aName].uniformType = 0;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createVec2Uniform(string aName, int aCtrlIndex, vec2 aValue) {
	vec2Values[aCtrlIndex] = aValue;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 2;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createVec3Uniform(string aName, int aCtrlIndex, vec3 aValue) {
	vec3Values[aCtrlIndex] = aValue;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 3;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createVec4Uniform(string aName, int aCtrlIndex, vec4 aValue) {
	vec4Values[aCtrlIndex] = aValue;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 4;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createIntUniform(string aName, int aCtrlIndex, int aValue) {
	intValues[aCtrlIndex] = aValue;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 5;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createBoolUniform(string aName, int aCtrlIndex, bool aValue) {
	boolValues[aCtrlIndex] = aValue;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 6;
	shaderUniforms[aName].isValid = true;
}
string VDAnimation::getUniformNameForIndex(int aIndex) {
	if (aIndex > controlIndexes.size() - 1) aIndex = 0;
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
bool VDAnimation::changeFloatValue(int aIndex, float aValue) {
	bool rtn = false;
	if (aIndex < controlValues.size() - 1 && aIndex > 0) {
		if (aIndex == 1) {
			CI_LOG_V("changeFloatValue, getUniformNameForIndex(aIndex):" + toString( getUniformNameForIndex(aIndex)));
			CI_LOG_V("changeFloatValue, shaderUniforms[getUniformNameForIndex(aIndex)].floatValue:" + toString( shaderUniforms[getUniformNameForIndex(aIndex)].floatValue));
			CI_LOG_V("changeFloatValue, aValue:" + toString( aValue));
		}
		if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != aValue) {
			controlValues[aIndex] = shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = aValue;
			rtn = true;
		}
	}
	return rtn;
}
float VDAnimation::getFloatUniformValueByIndex(int aIndex) {
	if (aIndex > controlValues.size() - 1) aIndex = 0;
	return controlValues[aIndex];
}
int VDAnimation::getSampler2DUniformValue(string aName) {
	return shaderUniforms[aName].textureIndex;
}
float VDAnimation::getFloatUniformValueByName(string aName) {
	return controlValues[shaderUniforms[aName].index];
}
vec2 VDAnimation::getVec2UniformValue(string aName) {
	return vec2Values[shaderUniforms[aName].index];
}
vec3 VDAnimation::getVec3UniformValue(string aName) {
	return vec3Values[shaderUniforms[aName].index];
}
vec4 VDAnimation::getVec4UniformValue(string aName) {
	return vec4Values[shaderUniforms[aName].index];
}
int VDAnimation::getIntUniformValue(string aName) {
	return intValues[shaderUniforms[aName].index];
}
bool VDAnimation::getBoolUniformValue(string aName) {
	return boolValues[shaderUniforms[aName].index];
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
	catch (const JsonTree::ExcJsonParserError&)  {
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
	float newValue;
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
		timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		break;
	case KeyEvent::KEY_d:
		// save end keyframe
		mVDSession->setEndFrame(getElapsedFrames() - 10);
		break;
	case KeyEvent::KEY_x:
		// trixels
		controlValues[16] = controlValues[16] + 0.05f;
		break;
	case KeyEvent::KEY_r:
		newValue = controlValues[1] + 0.1f;
		if (newValue > 1.0f) newValue = 0.0f;
		changeFloatValue(1, newValue);
		break;
	case KeyEvent::KEY_g:
		newValue = controlValues[2] + 0.1f;
		if (newValue > 1.0f) newValue = 0.0f;
		changeFloatValue(2, newValue);
		break;
	case KeyEvent::KEY_b:
		newValue = controlValues[3] + 0.1f;
		if (newValue > 1.0f) newValue = 0.0f;
		changeFloatValue(3, newValue);
		break;
	case KeyEvent::KEY_e:
		newValue = controlValues[1] - 0.1f;
		if (newValue < 0.0f) newValue = 1.0;
		changeFloatValue(1, newValue);
		break;
	case KeyEvent::KEY_f:
		newValue = controlValues[2] - 0.1f;
		if (newValue < 0.0f) newValue = 1.0;
		changeFloatValue(2, newValue);
		break;
	case KeyEvent::KEY_v:
		newValue = controlValues[3] - 0.1f;
		if (newValue < 0.0f) newValue = 1.0;
		changeFloatValue(3, newValue);
		break;
	case KeyEvent::KEY_t:
		// glitch
		controlValues[45] = 1.0f;
		break;
	case KeyEvent::KEY_i:
		// invert
		controlValues[48] = 1.0f;
		break;
	case KeyEvent::KEY_LEFT:
		if (controlValues[21] > 0.1) controlValues[21] -= 0.1;
		break;
	case KeyEvent::KEY_RIGHT:
		if (controlValues[21] < 1.0) controlValues[21] += 0.1;
		break;
	case KeyEvent::KEY_PAGEDOWN:
		// crossfade right
		if (controlValues[18] < 1.0) controlValues[18] += 0.1;
		break;
	case KeyEvent::KEY_PAGEUP:
		// crossfade left
		if (controlValues[18] > 0.0) controlValues[18] -= 0.1;
		break;
		//case KeyEvent::KEY_x:
	case KeyEvent::KEY_y:
		mVDSettings->iXorY = !mVDSettings->iXorY;
		break;
	case KeyEvent::KEY_c:
		// chromatic
		controlValues[10] = controlValues[10] + 0.05f;
		break;
	case KeyEvent::KEY_p:
		// pixelate
		controlValues[15] = controlValues[15] + 0.05f;
		break;
	case KeyEvent::KEY_o:
		// toggle
		controlValues[46] = !controlValues[46];
		break;
	case KeyEvent::KEY_z:
		// zoom
		controlValues[22] = controlValues[22] - 0.05f;
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
	case KeyEvent::KEY_b:
		// save badtv keyframe
		mBadTV[getElapsedFrames()] = 0.001f;
		break;
	case KeyEvent::KEY_g:
		// glitch
		controlValues[45] = 0.0f;
		break;
	case KeyEvent::KEY_t:
		// trixels
		controlValues[16] = 0.0f;
		break;
	case KeyEvent::KEY_i:
		// invert
		controlValues[48] = 0.0f;
		break;
	case KeyEvent::KEY_c:
		// chromatic
		controlValues[10] = 0.0f;
		break;
	case KeyEvent::KEY_p:
		// pixelate
		controlValues[15] = 1.0f;
		break;
	case KeyEvent::KEY_o:
		// toggle
		controlValues[46] = 0.0f;
		break;
	case KeyEvent::KEY_z:
		// zoom
		controlValues[22] = 1.0f;
		break;
	default:
		handled = false;
	}
	event.setHandled(handled);

	return event.isHandled();
}

void VDAnimation::update() {

	if (mBadTV[getElapsedFrames()] != 0) {
		// duration = 0.2
		timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
	}
	if (controlValues[12] == 0.0)controlValues[12] = 0.01;
	if (mVDSettings->iGreyScale)
	{
		controlValues[1] = controlValues[2] = controlValues[3];
		controlValues[5] = controlValues[6] = controlValues[7];
	}
	mVDSettings->iChannelTime[0] = getElapsedSeconds();
	mVDSettings->iChannelTime[1] = getElapsedSeconds() - 1;
	mVDSettings->iChannelTime[2] = getElapsedSeconds() - 2;
	mVDSettings->iChannelTime[3] = getElapsedSeconds() - 3;
	//
	if (mUseTimeWithTempo)
	{
		mVDSettings->iGlobalTime = iTempoTime*iTimeFactor;
	}
	else
	{
		mVDSettings->iGlobalTime = getElapsedSeconds();
	}
	mVDSettings->iGlobalTime *= mVDSettings->iSpeedMultiplier;
	controlValues[49] = mVDSettings->iGlobalTime;
#pragma region animation

	currentTime = timer.getSeconds();
	// TODO check bounds
	if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = iDeltaTime*1000000.0;
	int elapsedBeatPerBar = iDeltaTime / mVDSession->iBeatsPerBar*1000000.0;
	if (elapsedBeatPerBar > 0)
	{
		double moduloBeatPerBar = (time % elapsedBeatPerBar) / 1000000.0;
		iTempoTimeBeatPerBar = (float)moduloBeatPerBar;
		if (iTempoTimeBeatPerBar < previousTimeBeatPerBar)
		{
			if (iBeatIndex > mVDSession->iBeatsPerBar) iBeatIndex = 1;
			iBeatIndex++;
		}
		previousTimeBeatPerBar = iTempoTimeBeatPerBar;
	}
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		iTempoTime = (float)abs(modulo);
		if (iTempoTime < previousTime)
		{
			//iBar++;
			if (mAutoBeatAnimation) mVDSettings->iBeat++;
		}
		previousTime = iTempoTime;

		//(modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		// exposure
		if (tExposure)
		{
			controlValues[14] = (modulo < 0.1) ? maxExposure : minExposure;
		}
		else
		{
			controlValues[14] = autoExposure ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, minExposure, maxExposure) : controlValues[14];
			//controlValues[14] = autoExposure ? (sin(getElapsedFrames() / (controlValues[12] + 1.0))) : controlValues[14];
		}
		// zoom
		if (tZoom)
		{
			controlValues[22] = (modulo < 0.1) ? maxZoom : minZoom;
		}
		else
		{
			controlValues[22] = autoZoom ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, minZoom, maxZoom) : controlValues[22];
		}
		// ratio
		if (tRatio)
		{
			controlValues[11] = (modulo < 0.1) ? maxRatio : minRatio;
		}
		else
		{
			controlValues[11] = autoRatio ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, minRatio, maxRatio) : controlValues[11];
		}
		// Chromatic
		if (tChromatic)
		{
			controlValues[10] = (modulo < 0.1) ? maxChromatic : minChromatic;
		}
		else
		{
			controlValues[10] = autoChromatic ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, minChromatic, maxChromatic) : controlValues[10];
		}
		// RotationSpeed
		if (tRotationSpeed)
		{
			controlValues[19] = (modulo < 0.1) ? maxRotationSpeed : minRotationSpeed;
		}
		else
		{
			controlValues[19] = autoRotationSpeed ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, minRotationSpeed, maxRotationSpeed) : controlValues[19];
		}
		// ZPos
		if (tZPos)
		{
			controlValues[9] = (modulo < 0.1) ? maxZPos : minZPos;
		}
		else
		{
			controlValues[9] = autoZPos ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, minZPos, maxZPos) : controlValues[9];
		}

		// Front Red
		if (tFR)
		{
			controlValues[1] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[1] = autoFR ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[1];
		}
		// Front Green
		if (tFG)
		{
			controlValues[2] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[2] = autoFG ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[2];
		}
		// front blue
		if (tFB)
		{
			controlValues[3] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[3] = autoFB ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[3];
		}
		// foreground color vec3 update
		vec3Values[1] = vec3(controlValues[1], controlValues[2], controlValues[3]);

		// front alpha
		if (tFA)
		{
			controlValues[4] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[4] = autoFA ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[4];
		}
		// 
		if (tBR)
		{
			controlValues[5] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[5] = autoBR ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[5];
		}
		// 
		if (tBG)
		{
			controlValues[6] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[6] = autoBG ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[6];
		}
		// 
		if (tBB)
		{
			controlValues[7] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[7] = autoBB ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[7];
		}
		// background color vec3 update
		vec3Values[2] = vec3(controlValues[5], controlValues[6], controlValues[7]);
		// 
		if (tBA)
		{
			controlValues[8] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[8] = autoBA ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[8];
		}
		if (mVDSettings->autoInvert)
		{
			controlValues[48] = (modulo < 0.1) ? 1.0 : 0.0;
		}

		if (mVDSettings->tEyePointZ)
		{
			mVDSettings->mCamEyePointZ = (modulo < 0.1) ? mVDSettings->minEyePointZ : mVDSettings->maxEyePointZ;
		}
		else
		{
			mVDSettings->mCamEyePointZ = mVDSettings->autoEyePointZ ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, mVDSettings->minEyePointZ, mVDSettings->maxEyePointZ) : mVDSettings->mCamEyePointZ;
		}

	}
#pragma endregion animation
}

#pragma region utility
void VDAnimation::tempoZoom()
{
	tZoom = !tZoom;
	if (!tZoom) resetZoom();
}
void VDAnimation::resetZoom()
{
	autoZoom = false;
	tZoom = false;
	controlValues[22] = defaultZoom;
}

void VDAnimation::tempoZPos()
{
	tZPos = !tZPos;
	if (!tZPos) resetZPos();
}
void VDAnimation::resetZPos()
{
	autoZPos = false;
	tZPos = false;
	controlValues[9] = defaultZPos;
}
void VDAnimation::tempoRotationSpeed()
{
	tRotationSpeed = !tRotationSpeed;
	if (!tRotationSpeed) resetRotationSpeed();
}
void VDAnimation::resetRotationSpeed()
{
	autoRotationSpeed = false;
	tRotationSpeed = false;
	controlValues[19] = defaultRotationSpeed;
}

void VDAnimation::tempoExposure()
{
	tExposure = !tExposure;
	if (!tExposure) resetExposure();
}
void VDAnimation::resetExposure()
{
	autoExposure = false;
	tExposure = false;
	controlValues[14] = defaultExposure;
}
// chromatic
void VDAnimation::tempoChromatic()
{
	tChromatic = !tChromatic;
	if (!tChromatic) resetChromatic();
}
void VDAnimation::resetChromatic()
{
	autoChromatic = false;
	tChromatic = false;
	controlValues[10] = defaultChromatic;
}
// ratio
void VDAnimation::tempoRatio()
{
	tRatio = !tRatio;
	if (!tRatio) resetRatio();
}
void VDAnimation::resetRatio()
{
	autoRatio = false;
	tRatio = false;
	controlValues[11] = defaultRatio;
}
// red
void VDAnimation::tempoRed()
{
	tFR = !tFR;
	if (!tFR) resetRed();
}
void VDAnimation::resetRed()
{
	autoFR = false;
	tFR = false;
}
#pragma endregion utility

// tempo
void VDAnimation::tapTempo()
{
	startTime = currentTime = timer.getSeconds();

	timer.stop();
	timer.start();

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
	mVDSession->setBpm(60 / averageTime);
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
