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
	for (int s = 0; s < mVDMix->getFboBlendCount(); s++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((s * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "##s%d",  s);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDMix->getFboThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			
			// select
			sprintf(buf, "B##s%d", s);
			if (ui::Button(buf)){
				mVDMix->useBlendmode(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Use this blend");

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}
}
