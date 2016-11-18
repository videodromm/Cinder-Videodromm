#include "VDUIColor.h"

using namespace VideoDromm;

VDUIColor::VDUIColor(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIColor::~VDUIColor() {
	
}

void VDUIColor::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));

		ui::Begin("Color");
		{
			// foreground color
			color[0] = mVDAnimation->controlValues[1];
			color[1] = mVDAnimation->controlValues[2];
			color[2] = mVDAnimation->controlValues[3];
			color[3] = mVDAnimation->controlValues[4];
			ui::ColorEdit4("f", color);

			// red
			ctrl = 1;
			if (ui::Button("a##red")) { mVDAnimation->lockRed(); }
			ui::SameLine();
			if (ui::Button("t##red")) { mVDAnimation->tempoRed(); }
			ui::SameLine();
			if (ui::Button("x##red")) { mVDAnimation->resetRed(); }
			ui::SameLine();
			if (ui::SliderFloat("red/min/max", &color[ctrl], 0.0f, 1.0f))
			{
				// TODO color[0] not mVDAnimation->controlValues[ctrl]??
			}
			for (int i = 0; i < 4; i++)
			{
				if (mVDAnimation->controlValues[i + 1] != color[i])
				{
					mVDRouter->changeControlValue(i + 1, color[i]);
				}
			}

			// background color
			backcolor[0] = mVDAnimation->controlValues[5];
			backcolor[1] = mVDAnimation->controlValues[6];
			backcolor[2] = mVDAnimation->controlValues[7];
			backcolor[3] = mVDAnimation->controlValues[8];
			ui::ColorEdit4("g", backcolor);
			for (int i = 0; i < 4; i++)
			{
				if (mVDAnimation->controlValues[i + 5] != backcolor[i])
				{
					mVDRouter->changeControlValue(i + 5, backcolor[i]);
				}

			}
			// color multipliers
			if (ui::Button("x##RedX")) { mVDSettings->iRedMultiplier = 1.0f; }
			ui::SameLine();
			if (ui::SliderFloat("RedX", &mVDSettings->iRedMultiplier, 0.0f, 3.0f))
			{
			}
			if (ui::Button("x##GreenX")) { mVDSettings->iGreenMultiplier = 1.0f; }
			ui::SameLine();
			if (ui::SliderFloat("GreenX", &mVDSettings->iGreenMultiplier, 0.0f, 3.0f))
			{
			}
			if (ui::Button("x##BlueX")) { mVDSettings->iBlueMultiplier = 1.0f; }
			ui::SameLine();
			if (ui::SliderFloat("BlueX", &mVDSettings->iBlueMultiplier, 0.0f, 3.0f))
			{
			}


		}
		ui::End();

}
