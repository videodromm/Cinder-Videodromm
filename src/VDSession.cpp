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

void VDSession::restore()
{
	// check to see if json file exists
	if (!fs::exists(sessionPath)) {
		return;
	}

	try {
		JsonTree doc(loadFile(sessionPath));
		/*JsonTree params(doc.getChild("params"));
		for (JsonTree::ConstIter item = params.begin(); item != params.end(); ++item) {
			const auto& name = item->getKey();
			if (mItems.count(name)) {
				mItems.at(name)->load(name, item);
			}
			
		}*/
	}
	catch (const JsonTree::ExcJsonParserError&)  {
		//CI_LOG_E("Failed to parse json file.");
	}
	//"sequence_folder" : "mandalas"

}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
}

void VDSession::reset()
{
	// parameters exposed in json file
	mFlipV = false;
	mFlipH = false;
	mBpm = 160;
	mFpb = 16;
	resetSomeParams();


}
