#include "VDUIMidi.h"

using namespace VideoDromm;

VDUIMidi::VDUIMidi(VDSettingsRef aVDSettings, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDRouter = aVDRouter;
}
VDUIMidi::~VDUIMidi() {
	
}

void VDUIMidi::Run(const char* title) {
#if defined( CINDER_MSW )
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));

	ui::Begin("MIDI");
	{
		sprintf(buf, "Enable");
		if (ui::Button(buf)) mVDRouter->midiSetup();
		if (ui::CollapsingHeader("MidiIn", "20", true, true))
		{
			ui::Columns(2, "data", true);
			ui::Text("Name"); ui::NextColumn();
			ui::Text("Connect"); ui::NextColumn();
			ui::Separator();
			for (int i = 0; i < mVDRouter->getMidiInPortsCount(); i++)
			{
				ui::Text(mVDRouter->getMidiInPortName(i).c_str()); ui::NextColumn();

				if (mVDRouter->isMidiInConnected(i))
				{
					sprintf(buf, "Disconnect %d", i);
				}
				else
				{
					sprintf(buf, "Connect %d", i);
				}

				if (ui::Button(buf))
				{
					if (mVDRouter->isMidiInConnected(i))
					{
						mVDRouter->closeMidiInPort(i);
					}
					else
					{
						mVDRouter->openMidiInPort(i);
					}
				}
				ui::NextColumn();
				ui::Separator();
			}
			ui::Columns(1);
		}
	}
	ui::End();
#endif
}
