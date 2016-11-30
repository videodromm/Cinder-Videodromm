#include "VDUIColor.h"

using namespace VideoDromm;

VDUIColor::VDUIColor(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	for (int c = 0; c < 9; c++)
	{
		localValues[c] = 0.01f;
	}
}
VDUIColor::~VDUIColor() {
	
}
float VDUIColor::getValue(unsigned int aCtrl) {
	return mVDSession->getControlValue(aCtrl);
}
void VDUIColor::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setControlValue(aCtrl, aValue);
}
void VDUIColor::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIColor::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
void VDUIColor::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

		ui::Begin("Color");
		{
			// foreground color
			color[0] = getValue(1);
			color[1] = getValue(2);
			color[2] = getValue(3);
			color[3] = getValue(4);
			ui::ColorEdit4("f", color);

			// red
			ctrl = 1;
			if (ui::Button("a##red")) { toggleAuto(ctrl); }
			ui::SameLine();
			if (ui::Button("t##red")) { toggleAuto(ctrl); }
			ui::SameLine();
			if (ui::Button("x##red")) { resetAutoAnimation(ctrl); }
			ui::SameLine();
			if (ui::SliderFloat("red/min/max", &color[ctrl], 0.0f, 1.0f))
			{
				// TODO color[0] not mVDAnimation->controlValues[ctrl]??
			}
			for (int i = 0; i < 4; i++)
			{
				if (getValue(i + 1) != color[i])
				{
					setValue(i + 1, color[i]);
				}
			}

			// background color
			backcolor[0] = getValue(5);
			backcolor[1] = getValue(6);
			backcolor[2] = getValue(7);
			backcolor[3] = getValue(8);
			ui::ColorEdit4("g", backcolor);
			for (int i = 0; i < 4; i++)
			{
				if (getValue(i + 5) != backcolor[i])
				{
					setValue(i + 5, backcolor[i]);
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
