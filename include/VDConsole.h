#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Mix
#include "VDMix.h"
// Message router
#include "VDRouter.h"
// Session
#include "VDSession.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define snprintf _snprintf
#endif

#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace VideoDromm
{
	// stores the pointer to the VDConsole instance
	typedef std::shared_ptr<class VDConsole> VDConsoleRef;

	class VDConsole
	{
	public:
		VDConsole(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDConsoleRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDConsole>(new VDConsole(aVDSettings, aVDSession));
		}
		~VDConsole();
		void    ClearLog();
		void    AddLog(const char* fmt, ...);
		void    Run(const char* title);
		void    ExecCommand(const char* command_line);
	private:
		// Settings
		VDSettingsRef			mVDSettings;
		// Session
		VDSessionRef				mVDSession;

		char                  InputBuf[256];
		ImVector<char*>       Items;
		bool                  ScrollToBottom;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.size()-1 browsing history.
		ImVector<const char*> Commands;	

		static int TextEditCallbackStub(ImGuiTextEditCallbackData* data)
		{
			VDConsole* console = (VDConsole*)data->UserData;
			return console->TextEditCallback(data);
		}

		int     TextEditCallback(ImGuiTextEditCallbackData* data);
		// Portable helpers
		static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
		static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
		static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

	};
}