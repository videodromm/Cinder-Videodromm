#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Websocket
#include "VDWebsocket.h"

#if defined( CINDER_MSW )
// MIDI (only on windows for now)
#include "CinderMidi.h"

#endif
// OSC
#include "Osc.h"
#define USE_UDP 1


using namespace ci;
using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip; 
using namespace VideoDromm;
#if USE_UDP
using protocol = asio::ip::udp;
#else
using protocol = asio::ip::tcp;
#endif
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
	struct midiOutput
	{
		string			portName;
		bool			isConnected;
	};

	class VDRouter {
	public:
		VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDWebsocketRef aVDWebsocket);
		static VDRouterRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDWebsocketRef aVDWebsocket)
		{
			return shared_ptr<VDRouter>(new VDRouter(aVDSettings, aVDAnimation, aVDWebsocket));
		}
		void						update();
		void						shutdown();
		// messages
		void						updateParams(int iarg0, float farg1);
		// MIDI
		void						midiSetup();
#if defined( CINDER_MSW )
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		string						getMidiInPortName(int i) { return (i < mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(int i) { return (i < mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
		int							getMidiOutPortsCount() { return mMidiOutputs.size(); };
		string						getMidiOutPortName(int i) { return (i < mMidiOutputs.size()) ? mMidiOutputs[i].portName : "No midi out ports"; };
		bool						isMidiOutConnected(int i) { return (i < mMidiOutputs.size()) ? mMidiOutputs[i].isConnected : false; };
#endif
		void						midiOutSendNoteOn(int i, int channel, int pitch, int velocity);

		void						openMidiInPort(int i);
		void						closeMidiInPort(int i);
		void						openMidiOutPort(int i);
		void						closeMidiOutPort(int i);
		// OSC
		void						setupOSCSender();
		void						setupOSCReceiver();
		void						sendOSCIntMessage(string controlType, int iarg0 = 0, int iarg1 = 0, int iarg2 = 0, int iarg3 = 0, int iarg4 = 0, int iarg5 = 0);
		void						sendOSCColorMessage(string controlType, float val);
		void						sendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						updateAndSendOSCFloatMessage(string controlType, int iarg0 = 0, float farg1 = 0.0, float farg2 = 0.0, float farg3 = 0.0, float farg4 = 0.0, float farg5 = 0.0);
		void						sendOSCStringMessage(string controlType, int iarg0 = 0, string sarg1 = "", string sarg2 = "", string sarg3 = "", string sarg4 = "", string sarg5 = "");
		ivec4						skeleton[20];
		string						getTrack(int i) { return tracks[min(i, MAX)]; };//TODO
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		// VDWebsocket
		VDWebsocketRef				mVDWebsocket;
		// lights4events
		void						colorWrite();
		// MIDI
#if defined( CINDER_MSW )
		vector<midiInput>			mMidiInputs;
		// midi inputs: couldn't make a vector
		midi::MidiInput				mMidiIn0;
		midi::MidiInput				mMidiIn1;
		midi::MidiInput				mMidiIn2;
		midi::MidiInput				mMidiIn3;
		void						midiListener(midi::MidiMessage msg);
		// midi output
		midi::MidiOutput			mMidiOut0;
		midi::MidiOutput			mMidiOut1;
		midi::MidiOutput			mMidiOut2;
		midi::MidiOutput			mMidiOut3;
		vector<midiOutput>			mMidiOutputs;
#endif
		string						midiControlType;
		int							midiControl;
		int							midiPitch;
		int							midiVelocity;
		float						midiNormalizedValue;
		int							midiValue;
		int							midiChannel;

		// osc
		ReceiverRef					mOSCReceiver;

		SenderRef					mOSCSender;
		static const int			MAX = 16;
		// ableton liveOSC
		string						tracks[MAX];

	};
}

