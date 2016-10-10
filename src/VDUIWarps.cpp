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

	for (int w = 0; w < mVDMix->getWarpCount(); w++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((w * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##sh%d", mVDMix->getWarpName(w).c_str(), w);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(w);
			ui::Image((void*)mVDMix->getWarpTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			// loop on the fbos
			for (unsigned int s = 0; s < mVDMix->getShadersCount(); s++) {
				if (s > 0) ui::SameLine();
				if (mVDMix->getWarpFboIndex(w) == s) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(s / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(s / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(s / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(s / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wis%d%d", s, w, s);
				if (ui::Button(buf)) mVDMix->setWarpFboIndex(w, s);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input fbo");
				ui::PopStyleColor(3);
			}
			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}
}
