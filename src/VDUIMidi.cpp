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
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ui::Begin("MIDI");
	{
		sprintf(buf, "Enable");
		if (ui::Button(buf)) mVDSession->midiSetup();
		if (ui::CollapsingHeader("MidiIn", "20", true, true))
		{
			ui::Columns(2, "datain", true);
			ui::Text("Name"); ui::NextColumn();
			ui::Text("Connect"); ui::NextColumn();
			ui::Separator();
			for (int i = 0; i < mVDSession->getMidiInPortsCount(); i++)
			{
				if (mVDSession->getMidiInPortName(i) != "Ableton Push 2 1") {
					ui::Text(mVDSession->getMidiInPortName(i).c_str()); ui::NextColumn();

					if (mVDSession->isMidiInConnected(i))
					{
						sprintf(buf, "Disconnect %d", i);
					}
					else
					{
						sprintf(buf, "Connect %d", i);
					}

					if (ui::Button(buf))
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
					ui::NextColumn();
					ui::Separator();
				}
			}
			ui::Columns(1);
		}
		// Midi out
		if (ui::CollapsingHeader("MidiOut", "20", true, true))
		{
			ui::Columns(2, "dataout", true);
			ui::Text("Name"); ui::NextColumn();
			ui::Text("Connect"); ui::NextColumn();
			ui::Separator();
			for (int i = 0; i < mVDSession->getMidiOutPortsCount(); i++)
			{
				ui::Text(mVDSession->getMidiOutPortName(i).c_str()); ui::NextColumn();

				if (mVDSession->isMidiOutConnected(i))
				{
					sprintf(buf, "Disconnect  %d", i);
				}
				else
				{
					sprintf(buf, "Connect  %d", i);
				}

				if (ui::Button(buf))
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
				ui::NextColumn();
				ui::Separator();
			}
			ui::Columns(1);
		}
		ui::TextWrapped(">%s", mVDSettings->mMidiMsg.c_str());
	}
	ui::End();
#endif
}
