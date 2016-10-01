#include "VDUIWarps.h"

using namespace VideoDromm;

VDUIWarps::VDUIWarps(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
	
}
VDUIWarps::~VDUIWarps() {

}

void VDUIWarps::Run(const char* title) {
	static int shaderToEdit = -1;
	for (int s = 0; s < mVDMix->getMixFbosCount(); s++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((s * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##sh%d", mVDMix->getWarpName(s).c_str(), s);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDMix->getTexture(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			
			
			// left
			if (mVDMix->getFboFragmentShaderIndex(1) == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.0f, 0.8f, 0.8f));
			sprintf(buf, "L##sl%d", s);
			if (ui::Button(buf)) mVDMix->setFboFragmentShaderIndex(1, s);
			if (ui::IsItemHovered()) ui::SetTooltip("Set shader to left");
			ui::PopStyleColor(3);

			ui::SameLine();
			

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}
}
