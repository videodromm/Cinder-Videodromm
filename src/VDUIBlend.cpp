#include "VDUIBlend.h"

using namespace VideoDromm;

VDUIBlend::VDUIBlend(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIBlend::~VDUIBlend() {

}

void VDUIBlend::Run(const char* title) {
	const char* blendModes[] = { "mix", "multiply", "colorBurn", "linearBurn", "darkerColor", "lighten", "screen", "colorDodge", "linearDodge", "lighterColor", "overlay", "softLight", "hardLight", "vividLight", "linearLight", "pinLight", "hardMix", "difference", "exclusion", "subtract", "divide", "hue", "color", "saturation", "luminosity", "darken", "left", "right" };

	xPos = mVDSettings->uiMargin;
	yPos = mVDSettings->uiYPosRow3;
	for (int s = 0; s < mVDSession->getFboBlendCount(); s++) {
		ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		sprintf(buf, "%s", blendModes[s]);
		ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ImGui::PushID(s);
			ImGui::Image((void*)mVDSession->getFboThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));

			// select blend mode
			if (mVDSettings->iBlendmode == s) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.3f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.3f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.3f, 0.8f, 0.8f));

			sprintf(buf, "B##s%d", s);
			if (ImGui::Button(buf)){
				mVDSession->useBlendmode(s);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Use this blend");
			ImGui::PopStyleColor(3);

			ImGui::PopID();
			ImGui::PopItemWidth();
		}
		ImGui::End();
		xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;
		//if (xPos > (mVDSettings->mRenderWidth - mVDSettings->uiLargePreviewW))
		if (s % 10 == 9)
		{
			xPos = mVDSettings->uiMargin;
			yPos += mVDSettings->uiPreviewH + mVDSettings->uiMargin;
		}
	}
}
