#include "VDUITriangles.h"

using namespace VideoDromm;

VDUITriangles::VDUITriangles(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUITriangles::~VDUITriangles() {

}

void VDUITriangles::Run(const char* title) {
	static int currentNode = 0;

	for (int w = 0; w < mVDSession->getTriangleCount(); w++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2((w * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3), ImGuiSetCond_Once);
		//int hue = 0;
		sprintf(buf, "%s##tn%d", mVDSession->getTriangleName(w).c_str(), w);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(w);
			ui::Image((void*)mVDSession->getMixTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) ui::SetTooltip(mVDSession->getTriangleName(w).c_str());
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0 && (a%6 != 0)) ui::SameLine();
				if (mVDSession->getTriangleAFboIndex(w) == a) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##tia%d%d", a, w, a);
				if (ui::Button(buf)) mVDSession->setTriangleAFboIndex(w, a);
				sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(3);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
				if (b > 0 && (b % 6 != 0)) ui::SameLine();
				if (mVDSession->getTriangleBFboIndex(w) == b) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##tib%d%d", b, w, b);
				if (ui::Button(buf)) mVDSession->setTriangleBFboIndex(w, b);
				sprintf(buf, "Set input fbo B to %s", mVDSession->getShaderName(b).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(3);
			}

			// crossfade
			float xFade = mVDSession->getTriangleCrossfade(w);
			sprintf(buf, "xfade##txf%d", w);
			if (ui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDSession->setTriangleCrossfade(w, xFade);
			}
			// nodes
			if (currentNode == w) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.8f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "N##tnod%d", w);
			if (ui::Button(buf)){
				if (w == currentNode) {
					// if the same button pressed we hide the nodes
					currentNode = -1;
				}
				else{
					currentNode = w;
				}
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Show nodes");
			ui::SameLine();
			// active
			if (mVDSession->isTriangleActive(w) == w) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "A##a%d", w);
			if (ui::Button(buf)) {
				mVDSession->toggleTriangleActive(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Triangle Active");
			ui::SameLine();
			// solo
			if (mVDSession->isTriangleSolo(w) == w) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "S##s%d", w);
			if (ui::Button(buf)) {
				mVDSession->toggleTriangleSolo(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Triangle Solo");
			ui::SameLine();
			// delete
			if (mVDSession->isTriangleDeleted(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "X##x%d", w);
			if (ui::Button(buf)) {
				mVDSession->toggleDeleteTriangle(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Delete Triangle");

			ui::PopID();
			ui::PopItemWidth();
		}

		ui::End();
	}
}
