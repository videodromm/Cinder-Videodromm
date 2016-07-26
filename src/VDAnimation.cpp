#include "VDAnimation.h"

using namespace VideoDromm;

void VDAnimation::setExposure(float aExposure) {
	mExposure = aExposure;
}
void VDAnimation::setAutoBeatAnimation(bool aAutoBeatAnimation) {
	mAutoBeatAnimation = aAutoBeatAnimation;
}

bool VDAnimation::handleKeyDown(KeyEvent &event)
{
	bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_s:
		// save animation
		save();
		break;
	case KeyEvent::KEY_b:
		// save badtv keyframe
		mBadTV[getElapsedFrames() - 10] = 1.0f;
		//iBadTvRunning = true;
		// duration = 0.2
		timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		break;
	case KeyEvent::KEY_e:
		// save end keyframe
		mVDSession->setEndFrame(getElapsedFrames() - 10);
		break;
	case KeyEvent::KEY_t:
		// trixels
		controlValues[16] = controlValues[16] + 0.05f;
		break;
	case KeyEvent::KEY_g:
		// glitch
		controlValues[45] = 1.0f;
		break;
	case KeyEvent::KEY_i:
		// invert
		controlValues[48] = 1.0f;
		break;
	case KeyEvent::KEY_LEFT:		
		if (controlValues[21]>0.1) controlValues[21] -= 0.1;
		break;
	case KeyEvent::KEY_RIGHT:
		if (controlValues[21]<1.0) controlValues[21] += 0.1;
		break;
	case KeyEvent::KEY_x:
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
void VDAnimation::update() {

	//chromatic
	if (!controlValues[42]) {
		lockChromatic();
	}
	else {
		resetChromatic();
	}
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
#pragma region animation

	currentTime = timer.getSeconds();
	// TODO check bounds
	if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = iDeltaTime*1000000.0;
	int elapsedBeatPerBar = iDeltaTime/mVDSession->iBeatsPerBar*1000000.0;
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
		iTempoTime = (float)modulo;
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
		if (mVDSettings->tFR)
		{
			controlValues[1] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[1] = mVDSettings->mLockFR ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[1];
		}
		// Front Green
		if (mVDSettings->tFG)
		{
			controlValues[2] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[2] = mVDSettings->mLockFG ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[2];
		}
		// front blue
		if (mVDSettings->tFB)
		{
			controlValues[3] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[3] = mVDSettings->mLockFB ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[3];
		}
		// front alpha
		if (mVDSettings->tFA)
		{
			controlValues[4] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[4] = mVDSettings->mLockFA ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[4];
		}
		// 
		if (mVDSettings->tBR)
		{
			controlValues[5] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[5] = mVDSettings->mLockBR ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[5];
		}
		// 
		if (mVDSettings->tBG)
		{
			controlValues[6] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[6] = mVDSettings->mLockBG ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[6];
		}
		// 
		if (mVDSettings->tBB)
		{
			controlValues[7] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[7] = mVDSettings->mLockBB ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[7];
		}
		// 
		if (mVDSettings->tBA)
		{
			controlValues[8] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			controlValues[8] = mVDSettings->mLockBA ? lmap<float>(iTempoTime, 0.00001, iDeltaTime, 0.0, 1.0) : controlValues[8];
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

VDAnimation::VDAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
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
	minZoom = 0.1;
	maxZoom = 3.0;
	tZoom = autoZoom = false;
	// exposure
	defaultExposure = 1.0;
	minExposure = 0.0001;
	tExposure = autoExposure = false;
	// Chromatic
	defaultChromatic = 0.0;
	minChromatic = 0.0;
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
	// red
	controlValues[1] = 1.0f;
	// green
	controlValues[2] = 0.3f;
	// blue
	controlValues[3] = 0.0f;
	// Alpha 
	controlValues[4] = 1.0f;
	// background red
	controlValues[5] = 0.1f;
	// background green
	controlValues[6] = 0.1f;
	// background blue
	controlValues[7] = 0.1f;
	// background alpha
	controlValues[8] = 0.2f;
	// pointsphere zPosition
	controlValues[9] = -0.7f;
	// iChromatic
	controlValues[10] = 0.0f;
	// ratio
	controlValues[11] = 20.0f;
	// Speed 
	controlValues[12] = 12.0f;
	// Audio multfactor 
	controlValues[13] = 1.0f;
	// exposure
	controlValues[14] = 1.0f;
	// Pixelate
	controlValues[15] = 1.0f;
	// Trixels
	controlValues[16] = 0.0f;
	// GridSize
	controlValues[17] = 0.0f;
	// iCrossfade
	controlValues[18] = 1.0f;
	// RotationSpeed
	controlValues[19] = 0.0f;
	// Steps
	controlValues[20] = 16.0f;
	// iPreviewCrossfade
	controlValues[21] = 0.0f;
	// zoom
	controlValues[22] = 1.0f;
	// glitch
	controlValues[45] = 0.0f;
	// toggle
	controlValues[46] = 0.0f;
	// vignette
	controlValues[47] = 0.0f;
	// invert
	controlValues[48] = 0.0f;
	load();
	loadAnimation();
}
void VDAnimation::load() {
	// Create json file if it doesn't already exist.
	if (fs::exists(mJsonFilePath)) {
		bag()->load(mJsonFilePath);
	}
	else {
		bag()->save(mJsonFilePath);
		bag()->load(mJsonFilePath);
	}

}
void VDAnimation::save() {
	bag()->save(mJsonFilePath);
	saveAnimation();
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
	mVDSession->setBpm( 60 / averageTime);
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