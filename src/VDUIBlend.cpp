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
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		sprintf(buf, "%s", blendModes[s]);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDSession->getFboThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));

			// select blend mode
			if (mVDSettings->iBlendmode == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.3f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.3f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.3f, 0.8f, 0.8f));

			sprintf(buf, "B##s%d", s);
			if (ui::Button(buf)){
				mVDSession->useBlendmode(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Use this blend");
			ui::PopStyleColor(3);

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
		xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;
		//if (xPos > (mVDSettings->mRenderWidth - mVDSettings->uiLargePreviewW))
		if (s % 10 == 9)
		{
			xPos = mVDSettings->uiMargin;
			yPos += mVDSettings->uiPreviewH + mVDSettings->uiMargin;
		}
	}
}
