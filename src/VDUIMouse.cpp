#include "VDUIMouse.h"

using namespace VideoDromm;

VDUIMouse::VDUIMouse(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIMouse::~VDUIMouse() {
	
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
			//mVDSettings->iMouse.z = ui::GetIO().MouseDown[0];
		}
		else
		{
			//mVDSettings->iMouse.z = ui::Button("mouse click");
		}
		ui::SliderFloat("MouseX", &mVDSettings->mRenderPosXY.x, 0, mVDSettings->mFboWidth);
		ui::SliderFloat("MouseY", &mVDSettings->mRenderPosXY.y, 0, mVDSettings->mFboHeight);
	}
	ui::End();
#pragma endregion mouse
}
