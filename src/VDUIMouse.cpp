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
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	// Mouse
#pragma region mouse

	ImGui::Begin("Mouse");
	{
		ImGui::Text("Position: %.1f,%.1f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		ImGui::Text("Clic %d", ImGui::GetIO().MouseDown[0]);
		/* mouseGlobal ^= ImGui::Button("mouse gbl");
		if (mouseGlobal)
		{
			mVDSettings->mRenderPosXY.x = ImGui::GetIO().MousePos.x; ImGui::SameLine();
			mVDSettings->mRenderPosXY.y = ImGui::GetIO().MousePos.y;
		} */
		// mouse
		mouseX = getValue(35);
		if (ImGui::SliderFloat("MouseX", &mouseX, 0.0f, mVDSettings->mFboWidth, "%.4f", 3.0f))
		{
			setValue(35, mouseX);
		}
		mouseY = getValue(36);
		if (ImGui::SliderFloat("MouseY", &mouseY, 0.0f, mVDSettings->mFboHeight, "%.4f", 0.3f))
		{
			setValue(36, mouseY);
		}
		mouseZ ^= ImGui::Button("mouse click");
		if (mouseZ)
		{
			setValue(37, 1.0f);
		}
		else
		{
			setValue(37, 0.0f);
		}
	}
	ImGui::End();
#pragma endregion mouse
}
