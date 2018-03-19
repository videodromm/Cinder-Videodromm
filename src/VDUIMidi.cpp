#include "VDUIMidi.h"

using namespace VideoDromm;

VDUIMidi::VDUIMidi(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIMidi::~VDUIMidi() {

}

void VDUIMidi::Run(const char* title) {
#if defined( CINDER_MSW )
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ImGui::Begin("MIDI");
	{
		sprintf(buf, "Enable");
		if (ImGui::Button(buf)) mVDSession->midiSetup();
		if (ImGui::CollapsingHeader("MidiIn", "20", true, true))
		{
			ImGui::Columns(2, "datain", true);
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Connect"); ImGui::NextColumn();
			ImGui::Separator();
			for (int i = 0; i < mVDSession->getMidiInPortsCount(); i++)
			{
				if (mVDSession->getMidiInPortName(i) != "Ableton Push 2 1") {
					ImGui::Text(mVDSession->getMidiInPortName(i).c_str()); ImGui::NextColumn();

					if (mVDSession->isMidiInConnected(i))
					{
						sprintf(buf, "Disconnect %d", i);
					}
					else
					{
						sprintf(buf, "Connect %d", i);
					}

					if (ImGui::Button(buf))
					{
						if (mVDSession->isMidiInConnected(i))
						{
							mVDSession->closeMidiInPort(i);
						}
						else
						{
							mVDSession->openMidiInPort(i);
						}
					}
					ImGui::NextColumn();
					ImGui::Separator();
				}
			}
			ImGui::Columns(1);
		}
		// Midi out
		if (ImGui::CollapsingHeader("MidiOut", "20", true, true))
		{
			ImGui::Columns(2, "dataout", true);
			ImGui::Text("Name"); ImGui::NextColumn();
			ImGui::Text("Connect"); ImGui::NextColumn();
			ImGui::Separator();
			for (int i = 0; i < mVDSession->getMidiOutPortsCount(); i++)
			{
				ImGui::Text(mVDSession->getMidiOutPortName(i).c_str()); ImGui::NextColumn();

				if (mVDSession->isMidiOutConnected(i))
				{
					sprintf(buf, "Disconnect  %d", i);
				}
				else
				{
					sprintf(buf, "Connect  %d", i);
				}

				if (ImGui::Button(buf))
				{
					if (mVDSession->isMidiOutConnected(i))
					{
						mVDSession->closeMidiOutPort(i);
					}
					else
					{
						mVDSession->openMidiOutPort(i);
					}
				}
				ImGui::NextColumn();
				ImGui::Separator();
			}
			ImGui::Columns(1);
		}
		ImGui::TextWrapped(">%s", mVDSettings->mMidiMsg.c_str());
	}
	ImGui::End();
#endif
}
