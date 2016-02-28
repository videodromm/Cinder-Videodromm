#include "VDUtils.h"

using namespace VideoDromm;

VDUtils::VDUtils(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;
	CI_LOG_V("VDUtils constructor");
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
	// tempo
	counter = 0;
	startTime = timer.getSeconds();
	currentTime = timer.getSeconds();
	mVDSettings->iDeltaTime = 60 / mVDSettings->mTempo;
	previousTime = 0.0f;
	beatIndex = 0;
}
float VDUtils::formatFloat(float f)
{
	int i;
	f *= 100;
	i = ((int)f) / 100;
	return (float)i;
}
void VDUtils::shutdown() {
	
}
void VDUtils::setup()
{
	createWarpFbos();
}

void VDUtils::createWarpFbos()
{
	// vector + dynamic resize
	/*for (int a = 0; a < 12; a++)
	{
		WarpFbo newWarpFbo;
		if (a == 0)
		{
			newWarpFbo.textureIndex = 0; // spout
			newWarpFbo.textureMode = mVDSettings->TEXTUREMODEINPUT;
			newWarpFbo.active = true;
			newWarpFbo.fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		}
		else
		{
			newWarpFbo.textureIndex = 0; // index of MixFbo for shadamixa
			newWarpFbo.textureMode = mVDSettings->TEXTUREMODESHADER;
			newWarpFbo.active = false;
			newWarpFbo.fbo = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
		}
		mVDSettings->mWarpFbos.push_back(newWarpFbo);
	}*/
}


void VDUtils::update()
{
	if (mVDSettings->controlValues[12] == 0.0) mVDSettings->controlValues[12] = 0.01;
	if (mVDSettings->iGreyScale)
	{
		mVDSettings->controlValues[1] = mVDSettings->controlValues[2] = mVDSettings->controlValues[3];
		mVDSettings->controlValues[5] = mVDSettings->controlValues[6] = mVDSettings->controlValues[7];
	}
#pragma region animation
	// check this line position: can't remember
	currentTime = timer.getSeconds();

	int time = (currentTime - startTime)*1000000.0;
	int elapsed = mVDSettings->iDeltaTime*1000000.0;
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		mVDSettings->iTempoTime = (float)modulo;
		if (mVDSettings->iTempoTime < previousTime)
		{
			beatIndex++;
			if (beatIndex > 3) {
				beatIndex = 0;
				mVDSettings->iBeat++;
			} 
		}
		previousTime = mVDSettings->iTempoTime;

		//(modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		// exposure
		if (tExposure)
		{
			mVDSettings->controlValues[14] = (modulo < 0.1) ? mVDSettings->maxExposure : minExposure;
		}
		else
		{
			mVDSettings->controlValues[14] = autoExposure ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, minExposure, mVDSettings->maxExposure) : mVDSettings->controlValues[14];
			//mVDSettings->controlValues[14] = autoExposure ? (sin(getElapsedFrames() / (mVDSettings->controlValues[12] + 1.0))) : mVDSettings->controlValues[14];
		}
		// zoom
		if (tZoom)
		{
			mVDSettings->controlValues[22] = (modulo < 0.1) ? maxZoom : minZoom;
		}
		else
		{
			mVDSettings->controlValues[22] = autoZoom ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, minZoom, maxZoom) : mVDSettings->controlValues[22];
		}
		// ratio
		if (tRatio)
		{
			mVDSettings->controlValues[11] = (modulo < 0.1) ? maxRatio : minRatio;
		}
		else
		{
			mVDSettings->controlValues[11] = autoRatio ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, minRatio, maxRatio) : mVDSettings->controlValues[11];
		}
		// Chromatic
		if (tChromatic)
		{
			mVDSettings->controlValues[10] = (modulo < 0.1) ? maxChromatic : minChromatic;
		}
		else
		{
			mVDSettings->controlValues[10] = autoChromatic ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, minChromatic, maxChromatic) : mVDSettings->controlValues[10];
		}
		// RotationSpeed
		if (tRotationSpeed)
		{
			mVDSettings->controlValues[19] = (modulo < 0.1) ? maxRotationSpeed : minRotationSpeed;
		}
		else
		{
			mVDSettings->controlValues[19] = autoRotationSpeed ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, minRotationSpeed, maxRotationSpeed) : mVDSettings->controlValues[19];
		}
		// ZPos
		if (tZPos)
		{
			mVDSettings->controlValues[9] = (modulo < 0.1) ? maxZPos : minZPos;
		}
		else
		{
			mVDSettings->controlValues[9] = autoZPos ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, minZPos, maxZPos) : mVDSettings->controlValues[9];
		}

		// Front Red
		if (mVDSettings->tFR)
		{
			mVDSettings->controlValues[1] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[1] = mVDSettings->mLockFR ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[1];
		}
		// Front Green
		if (mVDSettings->tFG)
		{
			mVDSettings->controlValues[2] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[2] = mVDSettings->mLockFG ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[2];
		}
		// front blue
		if (mVDSettings->tFB)
		{
			mVDSettings->controlValues[3] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[3] = mVDSettings->mLockFB ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[3];
		}
		// front alpha
		if (mVDSettings->tFA)
		{
			mVDSettings->controlValues[4] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[4] = mVDSettings->mLockFA ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[4];
		}
		// 
		if (mVDSettings->tBR)
		{
			mVDSettings->controlValues[5] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[5] = mVDSettings->mLockBR ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[5];
		}
		// 
		if (mVDSettings->tBG)
		{
			mVDSettings->controlValues[6] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[6] = mVDSettings->mLockBG ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[6];
		}
		// 
		if (mVDSettings->tBB)
		{
			mVDSettings->controlValues[7] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[7] = mVDSettings->mLockBB ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[7];
		}
		// 
		if (mVDSettings->tBA)
		{
			mVDSettings->controlValues[8] = (modulo < 0.1) ? 1.0 : 0.0;
		}
		else
		{
			mVDSettings->controlValues[8] = mVDSettings->mLockBA ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, 0.0, 1.0) : mVDSettings->controlValues[8];
		}
		if (mVDSettings->autoInvert)
		{
			mVDSettings->controlValues[48] = (modulo < 0.1) ? 1.0 : 0.0;
		}

		if (mVDSettings->tEyePointZ)
		{
			mVDSettings->mCamEyePointZ = (modulo < 0.1) ? mVDSettings->minEyePointZ : mVDSettings->maxEyePointZ;
		}
		else
		{
			mVDSettings->mCamEyePointZ = mVDSettings->autoEyePointZ ? lmap<float>(mVDSettings->iTempoTime, 0.00001, mVDSettings->iDeltaTime, mVDSettings->minEyePointZ, mVDSettings->maxEyePointZ) : mVDSettings->mCamEyePointZ;
		}

	}
#pragma endregion animation
}

int VDUtils::getWindowsResolution()
{
	mVDSettings->mDisplayCount = 0;
	int w = Display::getMainDisplay()->getWidth();
	int h = Display::getMainDisplay()->getHeight();

	// Display sizes
	if (mVDSettings->mAutoLayout)
	{
		mVDSettings->mMainWindowWidth = w;
		mVDSettings->mMainWindowHeight = h;
		mVDSettings->mRenderX = mVDSettings->mMainWindowWidth;
		// for MODE_MIX and triplehead(or doublehead), we only want 1/3 of the screen centered	
		for (auto display : Display::getDisplays())
		{
			CI_LOG_V("Window #" + toString(mVDSettings->mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
			mVDSettings->mDisplayCount++;
			mVDSettings->mRenderWidth = display->getWidth();
			mVDSettings->mRenderHeight = display->getHeight();
		}
	}
	else
	{
		for (auto display : Display::getDisplays())
		{
			CI_LOG_V("Window #" + toString(mVDSettings->mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
			mVDSettings->mDisplayCount++;

		}
	}
	mVDSettings->mRenderY = 0;

	CI_LOG_V(" mMainDisplayWidth:" + toString(mVDSettings->mMainWindowWidth) + " mMainDisplayHeight:" + toString(mVDSettings->mMainWindowHeight));
	CI_LOG_V(" mRenderX:" + toString(mVDSettings->mRenderX) + " mRenderY:" + toString(mVDSettings->mRenderY));
	mVDSettings->mRenderResoXY = vec2(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
	// in case only one screen , render from x = 0
	if (mVDSettings->mDisplayCount == 1) mVDSettings->mRenderX = 0;
	return w;
}

void VDUtils::tempoZoom()
{
	tZoom = !tZoom;
	if (!tZoom) resetZoom();
}
void VDUtils::resetZoom()
{
	autoZoom = false;
	tZoom = false;
	mVDSettings->controlValues[22] = defaultZoom;
}

void VDUtils::tempoZPos()
{
	tZPos = !tZPos;
	if (!tZPos) resetZPos();
}
void VDUtils::resetZPos()
{
	autoZPos = false;
	tZPos = false;
	mVDSettings->controlValues[9] = defaultZPos;
}
void VDUtils::tempoRotationSpeed()
{
	tRotationSpeed = !tRotationSpeed;
	if (!tRotationSpeed) resetRotationSpeed();
}
void VDUtils::resetRotationSpeed()
{
	autoRotationSpeed = false;
	tRotationSpeed = false;
	mVDSettings->controlValues[19] = defaultRotationSpeed;
}

void VDUtils::tempoExposure()
{
	tExposure = !tExposure;
	if (!tExposure) resetExposure();
}
void VDUtils::resetExposure()
{
	autoExposure = false;
	tExposure = false;
	mVDSettings->controlValues[14] = defaultExposure;
}
// chromatic
void VDUtils::tempoChromatic()
{
	tChromatic = !tChromatic;
	if (!tChromatic) resetChromatic();
}
void VDUtils::resetChromatic()
{
	autoChromatic = false;
	tChromatic = false;
	mVDSettings->controlValues[10] = defaultChromatic;
}
// ratio
void VDUtils::tempoRatio()
{
	tRatio = !tRatio;
	if (!tRatio) resetRatio();
}
void VDUtils::resetRatio()
{
	autoRatio = false;
	tRatio = false;
	mVDSettings->controlValues[11] = defaultRatio;
}
// tempo
void VDUtils::tapTempo()
{
	startTime = currentTime = timer.getSeconds();

	timer.stop();
	timer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time budder is going to be discarded....
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
void VDUtils::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	mVDSettings->iDeltaTime = averageTime;
	mVDSettings->mTempo = 60 / averageTime;
}

void VDUtils::setTimeFactor(const int &aTimeFactor)
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
fs::path VDUtils::getPath(string path)
{
	fs::path p = app::getAssetPath("");
	if (path.length() > 0) { p += fs::path("/" + path); }
	return p;
}
