#include "VDUIMouse.h"

using namespace videodromm;

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
		mouseX = getValue(mVDSettings->IMOUSEX);
		if (ImGui::SliderFloat("MouseX", &mouseX, 0.0f, mVDSettings->mFboWidth, "%.4f", 3.0f))
		{
			setValue(mVDSettings->IMOUSEX, mouseX);
		}
		mouseY = getValue(mVDSettings->IMOUSEY);
		if (ImGui::SliderFloat("MouseY", &mouseY, 0.0f, mVDSettings->mFboHeight, "%.4f", 0.3f))
		{
			setValue(mVDSettings->IMOUSEY, mouseY);
		}
		mouseZ ^= ImGui::Button("mouse click");
		if (mouseZ)
		{
			setValue(mVDSettings->IMOUSEZ, 1.0f);
		}
		else
		{
			setValue(mVDSettings->IMOUSEZ, 0.0f);
		}
	}
	ImGui::End();
#pragma endregion mouse
}
