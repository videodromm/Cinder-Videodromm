#include "VDUIChannels.h"

using namespace VideoDromm;

VDUIChannels::VDUIChannels(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}

void VDUIChannels::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiMargin), ImGuiSetCond_Once);

#pragma region channels
	ui::Begin("Channels");
	{
		ui::Columns(3);
		ui::SetColumnOffset(0, 4.0f);// int column_index, float offset)
		ui::SetColumnOffset(1, 20.0f);// int column_index, float offset)
		//ui::SetColumnOffset(2, 24.0f);// int column_index, float offset)
		ui::Text("Chn"); ui::NextColumn();
		ui::Text("Tex"); ui::NextColumn();
		ui::Text("Name"); ui::NextColumn();
		ui::Separator();
		for (int i = 0; i < mVDSettings->MAX - 1; i++)
		{
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ui::Text("c%d", i);
			ui::NextColumn();
			sprintf(buf, "%d##chn%d", i, i);
			if (ui::SliderInt(buf, &mVDSettings->iChannels[i], 0, mVDSettings->MAX - 1)) {
			}
			ui::NextColumn();
			ui::PopStyleColor(3);
			ui::Text("%s", mVDSession->getInputTextureName(mVDSettings->iChannels[i]).c_str());
			ui::NextColumn();
		}
		ui::Columns(1);
	}
	ui::End();


#pragma endregion channels
}
