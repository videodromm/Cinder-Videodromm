#include "VDUIBlend.h"

using namespace VideoDromm;

VDUIBlend::VDUIBlend(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDAnimation = aVDAnimation;
}
VDUIBlend::~VDUIBlend() {
	
}

void VDUIBlend::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiMargin), ImGuiSetCond_Once);

	ui::Begin("Blend");
	{
		// blend modes
		if (ui::Button("x##blendmode")) { mVDSettings->iBlendMode = 0.0f; }
		ui::SameLine();
		ui::SliderInt("blendmode", &mVDSettings->iBlendMode, 0, mVDAnimation->maxBlendMode);

	}
	ui::End();
}
