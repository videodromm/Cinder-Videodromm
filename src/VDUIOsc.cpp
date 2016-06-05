#include "VDUIOsc.h"

using namespace VideoDromm;

VDUIOsc::VDUIOsc(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
}
VDUIOsc::~VDUIOsc() {

}

void VDUIOsc::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));

	ui::Begin("Osc");
	{
		ui::Text("Sending to host %s", mVDSettings->mOSCDestinationHost.c_str());
		ui::SameLine();
		ui::Text(" on port %d", mVDSettings->mOSCDestinationPort);
		ui::Text("Sending to 2nd host %s", mVDSettings->mOSCDestinationHost2.c_str());
		ui::SameLine();
		ui::Text(" on port %d", mVDSettings->mOSCDestinationPort2);
		ui::Text(" Receiving on port %d", mVDSettings->mOSCReceiverPort);

		static char str0[128] = "/live/play";
		static int i0 = 0;
		static float f0 = 0.0f;
		ui::InputText("address", str0, IM_ARRAYSIZE(str0));
		ui::InputInt("track", &i0);
		ui::InputFloat("clip", &f0, 0.01f, 1.0f);
		if (ui::Button("Send")) { mVDRouter->sendOSCIntMessage(str0, i0); }
	}
	ui::End();
}
