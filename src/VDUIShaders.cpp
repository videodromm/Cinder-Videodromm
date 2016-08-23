#include "VDUIShaders.h"

using namespace VideoDromm;

VDUIShaders::VDUIShaders(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
}
VDUIShaders::~VDUIShaders() {

}

void VDUIShaders::Run(const char* title) {

	for (int s = 0; s < mVDMix->getShadersCount(); s++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((s * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##s%d", mVDMix->getShaderName(s).c_str(), s);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDMix->getShaderThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			// edit
			sprintf(buf, "E##s%d", s);
			if (ui::Button(buf)){
				mVDMix->editShader(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Edit shader");
			ui::SameLine();
			// thumb
			sprintf(buf, "T##s%d", s);
			if (ui::Button(buf)){
				mVDMix->createShaderThumb(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Create thumb");

			// left
			if (mVDMix->getFboFragmentShaderIndex(1) == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.0f, 0.8f, 0.8f));
			sprintf(buf, "L##s%d", s);
			if (ui::Button(buf)) mVDMix->setFboFragmentShaderIndex(1, s);
			if (ui::IsItemHovered()) ui::SetTooltip("Set shader to left");
			ui::PopStyleColor(3);

			ui::SameLine();
			// right
			if (mVDMix->getFboFragmentShaderIndex(1) == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.3f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.3f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.3f, 0.8f, 0.8f));
			sprintf(buf, "R##s%d", s);
			if (ui::Button(buf)) mVDMix->setFboFragmentShaderIndex(2, s);
			if (ui::IsItemHovered()) ui::SetTooltip("Set shader to right");
			ui::PopStyleColor(3);

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}

}
