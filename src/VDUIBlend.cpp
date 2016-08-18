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
	xPos = mVDSettings->uiMargin;
	yPos = mVDSettings->uiYPosRow3;
	for (int s = 0; s < mVDMix->getFboBlendCount(); s++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
		ui::SetNextWindowPos(ImVec2(xPos, yPos));
		int hue = 0;
		sprintf(buf, "bm##s%d",  s);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDMix->getFboThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));

			// select blend mode
			if (mVDSettings->iBlendMode == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.3f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.3f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.3f, 0.8f, 0.8f));

			sprintf(buf, "B##s%d", s);
			if (ui::Button(buf)){
				mVDMix->useBlendmode(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Use this blend");
			ui::PopStyleColor(3);

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
		xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;
		if (xPos > (mVDSettings->mRenderWidth - mVDSettings->uiLargePreviewW) )
		{
			xPos = mVDSettings->uiMargin;
			yPos += mVDSettings->uiPreviewH + mVDSettings->uiMargin;
		}
	}
}
