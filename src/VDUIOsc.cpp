#include "VDUIOsc.h"

using namespace videodromm;

VDUIOsc::VDUIOsc(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIOsc::~VDUIOsc() {

}

void VDUIOsc::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ImGui::Begin("Osc");
	{
		if (mVDSettings->mOSCEnabled) {
			ImGui::Text("OSC enabled");
			if (mVDSettings->mIsOSCSender) {
				/*ImGui::Text("Sending to host %s", mVDSettings->mOSCDestinationHost.c_str());
				ImGui::SameLine();
				ImGui::Text(" on port %d", mVDSettings->mOSCDestinationPort);
				static char str0[128] = "/live/play";
				static int i0 = 0;
				static float f0 = 0.0f;
				ImGui::InputText("address", str0, IM_ARRAYSIZE(str0));
				ImGui::InputInt("track", &i0);
				ImGui::InputFloat("clip", &f0, 0.01f, 1.0f);
				if (ImGui::Button("Send")) { mVDSession->sendOSCIntMessage(str0, i0); }*/
			}
			else {
				ImGui::Text(" Receiving on port %d", mVDSettings->mOSCReceiverPort);
			}
		}
		else {
			ImGui::Text("OSC disabled");

			if (mVDSettings->mIsOSCSender) {
				if (ImGui::Button("sender->receiver"))
				{
					mVDSettings->mIsOSCSender = false;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a OSC receiver");

				static char host[128] = "127.0.0.1";
				std::copy(mVDSettings->mOSCDestinationHost.begin(), (mVDSettings->mOSCDestinationHost.size() >= 128 ? mVDSettings->mOSCDestinationHost.begin() + 128 : mVDSettings->mOSCDestinationHost.end()), host);
				static int senderPort = mVDSettings->mOSCDestinationPort;
				ImGui::InputText("destination address", host, IM_ARRAYSIZE(host));
				if (ImGui::InputInt("destination port", &senderPort)) mVDSettings->mOSCDestinationPort = senderPort;
			}
			else {
				if (ImGui::Button("receiver->sender"))
				{
					mVDSettings->mIsOSCSender = true;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a OSC sender");

				static int receiverPort = mVDSettings->mOSCReceiverPort;
				if (ImGui::InputInt("receiver port", &receiverPort)) mVDSettings->mOSCReceiverPort = receiverPort;
			}
			if (ImGui::Button("Enable"))
			{
				mVDSettings->mOSCEnabled = true;
				/*if (mVDSettings->mIsOSCSender) {
					mVDSession->setupOSCSender();
				}
				else {
					mVDSession->setupOSCReceiver();
				}*/
			}
		}
		ImGui::Text(">%s", mVDSettings->mOSCMsg.c_str());

		/* TODO if useful ImGui::Text("Sending to 2nd host %s", mVDSettings->mOSCDestinationHost2.c_str());
		ImGui::SameLine();
		ImGui::Text(" on port %d", mVDSettings->mOSCDestinationPort2);*/
	}
	ImGui::End();
}
