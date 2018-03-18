#include "VDUIMouse.h"

using namespace VideoDromm;

VDUIMouse::VDUIMouse(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIMouse::~VDUIMouse() {
	
}
float VDUIMouse::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIMouse::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
void VDUIMouse::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	// Mouse
#pragma region mouse

	ui::Begin("Mouse");
	{
		ui::Text("Position: %.1f,%.1f", ui::GetIO().MousePos.x, ui::GetIO().MousePos.y);
		ui::Text("Clic %d", ui::GetIO().MouseDown[0]);
		mouseGlobal ^= ui::Button("mouse gbl");
		if (mouseGlobal)
		{
			mVDSettings->mRenderPosXY.x = ui::GetIO().MousePos.x; ui::SameLine();
			mVDSettings->mRenderPosXY.y = ui::GetIO().MousePos.y;
		}
		// mouse
		mouseX = getValue(35);
		if (ui::SliderFloat("MouseX", &mouseX, 0.0f, mVDSettings->mFboWidth))
		{
			setValue(35, mouseX);
		}
		mouseY = getValue(36);
		if (ui::SliderFloat("MouseY", &mouseY, 0.0f, mVDSettings->mFboHeight))
		{
			setValue(36, mouseY);
		}
		mouseZ ^= ui::Button("mouse click");
		if (mouseZ)
		{
			setValue(37, 1.0f);
		}
		else
		{
			setValue(37, 0.0f);
		}
	}
	ui::End();
#pragma endregion mouse
}
