//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace VideoDromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;

	// reset no matter what, so we don't miss anything
	reset();

	// check to see if VDSession.xml file exists and restore if it does
	sessionPath = getAssetPath("") / mVDSettings->mAssetsPath / sessionFileName;
	if (fs::exists(sessionPath))
	{
		restore();
	}
	else
	{
		save();
	}
}

VDSessionRef VDSession::create(VDSettingsRef aVDSettings)
{
	return shared_ptr<VDSession>(new VDSession(aVDSettings));
}

bool VDSession::save()
{

	// save in sessionPath


	return true;
}

bool VDSession::restore()
{
	// check to see if json file exists

	if (fs::exists(sessionPath)) {
		// if it does, restore
		
		return true;
		
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
}

void VDSession::reset()
{
	// parameters exposed in json file
	mFlipV = false;
	mFlipH = false;

	resetSomeParams();


}
