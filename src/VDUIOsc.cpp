#include "VDUIOsc.h"

using namespace VideoDromm;

VDUIOsc::VDUIOsc(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIOsc::~VDUIOsc() {

}

void VDUIOsc::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));

	ui::Begin("Osc");
	{
		if (mVDSettings->mOSCEnabled) {
			ui::Text("OSC enabled");
			if (mVDSettings->mIsOSCSender) {
				ui::Text("Sending to host %s", mVDSettings->mOSCDestinationHost.c_str());
				ui::SameLine();
				ui::Text(" on port %d", mVDSettings->mOSCDestinationPort);
				static char str0[128] = "/live/play";
				static int i0 = 0;
				static float f0 = 0.0f;
				ui::InputText("address", str0, IM_ARRAYSIZE(str0));
				ui::InputInt("track", &i0);
				ui::InputFloat("clip", &f0, 0.01f, 1.0f);
				if (ui::Button("Send")) { mVDRouter->sendOSCIntMessage(str0, i0); }
			}
			else {
				ui::Text(" Receiving on port %d", mVDSettings->mOSCReceiverPort);
			}
		}
		else {
			ui::Text("OSC disabled");

			if (mVDSettings->mIsOSCSender) {
				if (ui::Button("sender->receiver"))
				{
					mVDSettings->mIsOSCSender = false;
				}
				if (ui::IsItemHovered()) ui::SetTooltip("Change to a OSC receiver");

				static char host[128] = "127.0.0.1";
				std::copy(mVDSettings->mOSCDestinationHost.begin(), (mVDSettings->mOSCDestinationHost.size() >= 128 ? mVDSettings->mOSCDestinationHost.begin() + 128 : mVDSettings->mOSCDestinationHost.end()), host);
				static int senderPort = mVDSettings->mOSCDestinationPort;
				ui::InputText("destination address", host, IM_ARRAYSIZE(host));
				if (ui::InputInt("destination port", &senderPort)) mVDSettings->mOSCDestinationPort = senderPort;
			}
			else {
				if (ui::Button("receiver->sender"))
				{
					mVDSettings->mIsOSCSender = true;
				}
				if (ui::IsItemHovered()) ui::SetTooltip("Change to a OSC sender");

				static int receiverPort = mVDSettings->mOSCReceiverPort;
				if (ui::InputInt("receiver port", &receiverPort)) mVDSettings->mOSCReceiverPort = receiverPort;
			}
			if (ui::Button("Enable"))
			{
				mVDSettings->mOSCEnabled = true;
				if (mVDSettings->mIsOSCSender) {
					mVDRouter->setupOSCSender();
				}
				else {
					mVDRouter->setupOSCReceiver();
				}
			}
		}
		ui::Text(">%s", mVDSettings->mOSCMsg.c_str());

		/* TODO if useful ui::Text("Sending to 2nd host %s", mVDSettings->mOSCDestinationHost2.c_str());
		ui::SameLine();
		ui::Text(" on port %d", mVDSettings->mOSCDestinationPort2);*/
	}
	ui::End();
}
