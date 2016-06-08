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
			// thumb
			sprintf(buf, "T##s%d", s);
			if (ui::Button(buf)){
				mVDMix->createShaderThumb(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Create thumb");

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}
	
}
