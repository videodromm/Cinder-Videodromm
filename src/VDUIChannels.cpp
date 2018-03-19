#include "VDUIChannels.h"

using namespace VideoDromm;

VDUIChannels::VDUIChannels(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}

void VDUIChannels::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiMargin), ImGuiSetCond_Once);

#pragma region channels
	ImGui::Begin("Channels");
	{
		ImGui::Columns(3);
		ImGui::SetColumnOffset(0, 4.0f);// int column_index, float offset)
		ImGui::SetColumnOffset(1, 20.0f);// int column_index, float offset)
		//ImGui::SetColumnOffset(2, 24.0f);// int column_index, float offset)
		ImGui::Text("Chn"); ImGui::NextColumn();
		ImGui::Text("Tex"); ImGui::NextColumn();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Separator();
		for (int i = 0; i < mVDSettings->MAX - 1; i++)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ImGui::Text("c%d", i);
			ImGui::NextColumn();
			sprintf(buf, "%d##chn%d", i, i);
			if (ImGui::SliderInt(buf, &mVDSettings->iChannels[i], 0, mVDSettings->MAX - 1)) {
			}
			ImGui::NextColumn();
			ImGui::PopStyleColor(3);
			ImGui::Text("%s", mVDSession->getInputTextureName(mVDSettings->iChannels[i]).c_str());
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
	}
	ImGui::End();


#pragma endregion channels
}
