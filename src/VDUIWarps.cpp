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
			for (unsigned int a = 0; a < mVDMix->getMixFboCount(); a++) {
				if (a > 0) ui::SameLine();
				if (mVDMix->getWarpAFboIndex(w) == a) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wia%d%d", a, w, a);
				if (ui::Button(buf)) mVDMix->setWarpAFboIndex(w, a);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input fbo A");
				ui::PopStyleColor(3);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDMix->getMixFboCount(); b++) {
				if (b > 0) ui::SameLine();
				if (mVDMix->getWarpBFboIndex(w) == b) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wib%d%d", b, w, b);
				if (ui::Button(buf)) mVDMix->setWarpBFboIndex(w, b);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input fbo B");
				ui::PopStyleColor(3);
			}
			// crossfade
			float xFade = mVDMix->getWarpCrossfade(w);
			sprintf(buf, "xfade##xf%d", w);
			if (ui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDMix->setWarpCrossfade(w, xFade);
			}
			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}
}
