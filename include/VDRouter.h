#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Animation
#include "VDAnimation.h"

// MIDI (only on windows for now)
#if defined( CINDER_MSW )
#include "CinderMidi.h"
#endif
// OSC
#include "Osc.h"
#define USE_UDP 1

// WebSockets
#include "WebSocketClient.h"
#include "WebSocketServer.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip;

namespace VideoDromm
{
	// stores the pointer to the VDRouter instance
	typedef std::shared_ptr<class VDRouter> VDRouterRef;
	// midi
	typedef std::shared_ptr<class MIDI> MIDIRef;
	// stores the pointer to the ReceiverUdp or ReceiverTcp instance
#if USE_UDP
	typedef std::shared_ptr<class osc::ReceiverUdp> ReceiverRef;
#else
	typedef std::shared_ptr<class osc::ReceiverTcp> ReceiverRef;
#endif
	// stores the pointer to the SenderUdp instance
	typedef std::shared_ptr<class osc::SenderUdp> SenderRef; 

	struct midiInput
	{
		string			portName;
		bool			isConnected;
	};

	class VDRouter {
	public:
		VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aAnimationRef, VDSessionRef aVDSessionRef);
		static VDRouterRef	create(VDSettingsRef aVDSettings, VDAnimationRef aAnimationRef, VDSessionRef aVDSessionRef)
		{
			return shared_ptr<VDRouter>(new VDRouter(aVDSettings, aAnimationRef, aVDSessionRef));
		}
		void						update();
		void						shutdown();
		// messages
		void						sendJSON(string params);
		void						updateParams(int iarg0, float farg1);
		// MIDI
		void						midiSetup();
        #if defined( CINDER_MSW )
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		string						getMidiInPortName(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(int i) { return (i<mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
        #endif
        void						openMidiInPort(int i);
		void						closeMidiInPort(int i);
		// OSC
		void						setupOSCSender();
		void						sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void						sendOSCColorMessage(string controlType, float val);
		void						sendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						updateAndSendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						sendOSCStringMessage(string controlType, int iarg0 = 0, string sarg1 = "", string sarg2 = "", string sarg3 = "", string sarg4 = "", string sarg5 = "");
		ivec4						skeleton[20];
		string						getTrack(int i) { return tracks[min(i, MAX)]; };//TODO
		// WebSockets
		void						wsWrite(std::string msg);
		void						wsWriteBinary(const void *data, int size);
		void						wsConnect();
		void						wsPing();
		bool						isWsClientConnected() { return clientConnected; };
		void						colorWrite();

	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		// Session
		VDSessionRef				mVDSession;
		// MIDI
        #if defined( CINDER_MSW )
		vector<midiInput>			mMidiInputs;
		// midi inputs: couldn't make a vector
		midi::MidiInput				mMidiIn0;
		midi::MidiInput				mMidiIn1;
		midi::MidiInput				mMidiIn2;
		midi::MidiInput				mMidiIn3;
		void						midiListener(midi::MidiMessage msg);
        #endif
		string						midiControlType;
		int							midiControl;
		int							midiPitch;
		int							midiVelocity;
		float						midiNormalizedValue;
		int							midiValue;
		int							midiChannel;
		// WebSockets
		// Web socket client
		void						wsClientDisconnect();
		WebSocketClient				mClient;
		void						wsClientConnect();
		void						onWsConnect();
		void						onWsDisconnect();
		void						onWsError(std::string err);
		void						onWsInterrupt();
		void						onWsPing(std::string msg);
		void						onWsRead(std::string msg);
		bool						clientConnected;
		// Web socket  server
		WebSocketServer				mServer;
		void						serverConnect();
		void						serverDisconnect();
		double						mPingTime;
		// osc
		ReceiverRef					mOSCReceiver;

		SenderRef					mOSCSender;

		// now using broadcast osc::SenderUdp				mOSCSender2;
		static const int			MAX = 16;
		//int							iargs[MAX];
		//float						fargs[MAX];
		//string						sargs[MAX];
		// ableton liveOSC
		string						tracks[MAX];
	};
}

