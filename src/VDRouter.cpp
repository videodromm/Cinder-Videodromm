#include "VDRouter.h"

using namespace VideoDromm;

VDRouter::VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDWebsocketRef aVDWebsocket) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDWebsocket = aVDWebsocket;
	CI_LOG_V("VDRouter constructor");
	// kinect
	for (int i = 0; i < 20; i++) {
		skeleton[i] = ivec4(0);
	}
	// OSC
	if (mVDSettings->mOSCEnabled) {
		if (mVDSettings->mIsOSCSender) {
			// OSC sender
			setupOSCSender();
		}
		else {
			// OSC receiver
			setupOSCReceiver();
		}
	}

	// midi
	if (mVDSettings->mMIDIOpenAllInputPorts) midiSetup();

}
void VDRouter::setupOSCSender() {
	// OSC sender with broadcast
	osc::UdpSocketRef mSocket(new udp::socket(App::get()->io_service(), udp::endpoint(udp::v4(), 9999)));
	mSocket->set_option(asio::socket_base::broadcast(true));
	mOSCSender = shared_ptr<osc::SenderUdp>(new osc::SenderUdp(mSocket, udp::endpoint(address_v4::broadcast(), mVDSettings->mOSCDestinationPort)));
	// OSC sender without broadcast
	//mOSCSender = shared_ptr<osc::SenderUdp>(new osc::SenderUdp(10003, mVDSettings->mOSCDestinationHost, mVDSettings->mOSCDestinationPort));
	//mOSCSender->bind();

	osc::Message msg("/start");
	msg.append(1);
	msg.append(2);

	mOSCSender->send(msg);
}
void VDRouter::setupOSCReceiver() {
#if USE_UDP
	mOSCReceiver = shared_ptr<osc::ReceiverUdp>(new osc::ReceiverUdp(mVDSettings->mOSCReceiverPort));
#else
	mOSCReceiver = shared_ptr<osc::ReceiverTcp>(new osc::ReceiverTcp(mVDSettings->mOSCReceiverPort));
#endif
	/* TODO
	Load addresses from json file
	for (auto & address : jsonList["addresses"]) {
	mOSCReceiver->setListener(address.getValue(), myFuncForLayerClips);
	}
	TODO use pattern matching
	mOSCReceiver->setListener("/layer1/clip*", myFuncForLayerClips);*/
	mOSCReceiver->setListener("/cc",
		[&](const osc::Message &msg) {
		mVDSettings->mOSCMsg = "/cc";
		mVDSettings->mOSCNewMsg = true;
		mVDAnimation->setFloatUniformValueByIndex(msg[0].int32(), msg[1].flt());
		updateParams(msg[0].int32(), msg[1].flt());
	});
	mOSCReceiver->setListener("/fader3/out",
		[&](const osc::Message &msg) {
		mVDSettings->mOSCMsg = "/fader3/out" + toString(msg[0].flt());
		mVDSettings->mOSCNewMsg = true;
	});
	mOSCReceiver->setListener("/Freq1",
		[&](const osc::Message &msg) {
		//float f1 = msg[0].flt();
	});
	mOSCReceiver->setListener("/backgroundcolor",
		[&](const osc::Message &msg) {
		// background red
		mVDAnimation->setFloatUniformValueByIndex(5, msg[0].flt());
		// background green
		mVDAnimation->setFloatUniformValueByIndex(6, msg[1].flt());
		// background blue
		mVDAnimation->setFloatUniformValueByIndex(7, msg[2].flt());
	});
	mOSCReceiver->setListener("/live/beat",
		[&](const osc::Message &msg) {
		mVDAnimation->setAutoBeatAnimation(false);
		mVDSettings->iBeat = msg[0].int32();
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
	});
	// artcraft
	mOSCReceiver->setListener("/bar",
		[&](const osc::Message &msg) {
		mVDSettings->iBeat = msg[0].int32();
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
	});
	/*mOSCReceiver->setListener("/live/tempo",
		[&](const osc::Message &msg) {
		mVDAnimation->setAutoBeatAnimation(false);
		// Animation
		mVDSession->setBpm(msg[0].flt());
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
		});
		// artcraft
		mOSCReceiver->setListener("/tempo",
		[&](const osc::Message &msg) {
		// Animation
		mVDSession->setBpm(msg[0].flt());
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
		});*/
	mOSCReceiver->setListener("/live/track/meter",
		[&](const osc::Message &msg) {
		mVDAnimation->setAutoBeatAnimation(false);
		mVDSettings->liveMeter = msg[2].flt();
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
	});
	// artcraft
	mOSCReceiver->setListener("/Audio1",
		[&](const osc::Message &msg) {
		mVDSettings->liveMeter = msg[0].flt()*100.0f;
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
	});
	mOSCReceiver->setListener("/live/name/trackblock",
		[&](const osc::Message &msg) {
		mVDAnimation->setAutoBeatAnimation(false);
		mVDSettings->mTrackName = msg[0].string();
		for (int a = 0; a < MAX; a++)
		{
			tracks[a] = msg[a].string();
		}
	});
	mOSCReceiver->setListener("/live/play",
		[&](const osc::Message &msg) {
		mVDSettings->mOSCMsg = "/live/play";
		mVDSettings->mOSCNewMsg = true;

		mVDAnimation->setAutoBeatAnimation(false);
		osc::Message m;
		m.setAddress("/tracklist");

		for (int a = 0; a < MAX; a++)
		{
			if (tracks[a] != "") m.append(tracks[a]);
		}
		if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(m);

	});
	// kinect
	mOSCReceiver->setListener("/sumMovement",
		[&](const osc::Message &msg) {
		float sumMovement = msg[0].flt();
		//exposure
		mVDAnimation->setFloatUniformValueByIndex(14, sumMovement);
		//greyScale
		if (sumMovement < 0.1)
		{
			mVDSettings->iGreyScale = 1.0f;
		}
		else
		{
			mVDSettings->iGreyScale = 0.0f;
		}
	});
	// kinect
	mOSCReceiver->setListener("/handsHeadHeight",
		[&](const osc::Message &msg) {
		float handsHeadHeight = msg[0].flt();
		if (handsHeadHeight > 0.3)
		{
			// glitch
			mVDAnimation->setFloatUniformValueByIndex(45, 1.0f);
		}
		else
		{
			// glitch
			mVDAnimation->setFloatUniformValueByIndex(45, 0.0f);
		}
		// background red
		mVDAnimation->setFloatUniformValueByIndex(5, handsHeadHeight*3.0);
	});
	mOSCReceiver->setListener("/centerXY",
		[&](const osc::Message &msg) {
		float x = msg[0].flt();
		float y = msg[1].flt();
		// background green
		mVDAnimation->setFloatUniformValueByIndex(6, y);
		// green
		mVDAnimation->setFloatUniformValueByIndex(2, x);
	});
	mOSCReceiver->setListener("/selectShader",
		[&](const osc::Message &msg) {
		//selectShader(msg[0].int32(), msg[1].int32());
	});
	// kinect
	mOSCReceiver->setListener("/joint",
		[&](const osc::Message &msg) {
		//int skeletonIndex = msg[0].int32();
		int jointIndex = msg[1].int32();
		if (jointIndex < 20)
		{
			skeleton[jointIndex] = ivec4(msg[2].int32(), msg[3].int32(), msg[4].int32(), msg[5].int32());
		}
	});
	// json
	/*mOSCReceiver->setListener("/json/params",
		[&](const osc::Message &msg) {
		mVDSettings->mOSCMsg = "/json/params:" + msg[0].string();
		mVDSettings->mOSCNewMsg = true;

		parseMessage(msg[0].string());

	});*/

	mOSCReceiver->bind();
	mOSCReceiver->listen([](asio::error_code error, protocol::endpoint endpoint) -> bool {
		if (error) {
			CI_LOG_E("Error Listening: " << error.message() << " val: " << error.value() << " endpoint: " << endpoint);
			return false;
		}
		else
			return true;
	});
}
void VDRouter::shutdown() {

	mMidiIn0.closePort();
	mMidiIn1.closePort();
	mMidiIn2.closePort();
	mMidiOut0.closePort();
	mMidiOut1.closePort();
	mMidiOut2.closePort();

}

void VDRouter::midiSetup() {
	stringstream ss;
	ss << "setupMidi: ";

	if (mMidiIn0.getNumPorts() > 0)
	{
		mMidiIn0.listPorts();
		for (int i = 0; i < mMidiIn0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiInputs.size(); j++)
			{
				if (mMidiInputs[j].portName == mMidiIn0.getPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiInput mIn;
				mIn.portName = mMidiIn0.getPortName(i);
				mMidiInputs.push_back(mIn);
				if (mVDSettings->mMIDIOpenAllInputPorts) {
					openMidiInPort(i);
					mMidiInputs[i].isConnected = true;
					ss << "Opening MIDI in port " << i << " " << mMidiInputs[i].portName;
				}
				else {
					mMidiInputs[i].isConnected = false;
					ss << "Available MIDI in port " << i << " " << mMidiIn0.getPortName(i);
				}
			}
		}
	}
	else {
		ss << "No MIDI in ports found!" << std::endl;
	}
	ss << std::endl;

	mVDSettings->mNewMsg = true;
	mVDSettings->mMsg = ss.str();
	// midi out
	//mMidiOut0.getPortList();
	if (mMidiOut0.getNumPorts() > 0) {
		for (int i = 0; i < mMidiOut0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiOutputs.size(); j++)
			{
				if (mMidiOutputs[j].portName == mMidiOut0.getPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiOutput mOut;
				mOut.portName = mMidiOut0.getPortName(i);
				mMidiOutputs.push_back(mOut);

				mMidiOutputs[i].isConnected = false;
				ss << "Available MIDI output port " << i << " " << mMidiOut0.getPortName(i);

			}
		}
	}
	else {
		ss << "No MIDI Out Ports found!!!!" << std::endl;
	}
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiNormalizedValue = midiValue = midiChannel = 0;
}

void VDRouter::openMidiInPort(int i) {

	// HACK Push2 has 2 midi ports, we keep the internal one not useable 
	if (mMidiIn0.getPortName(i) != "Ableton Push 2 1") {

		stringstream ss;
		if (i < mMidiIn0.getNumPorts()) {
			if (i == 0) {
				mMidiIn0.openPort(i);
				mMidiIn0.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
			}
			if (i == 1) {
				mMidiIn1.openPort(i);
				mMidiIn1.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
			}
			if (i == 2) {
				mMidiIn2.openPort(i);
				mMidiIn2.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
			}
		}
		mMidiInputs[i].isConnected = true;
		ss << "Opening MIDI in port " << i << " " << mMidiInputs[i].portName << std::endl;
		mVDSettings->mMsg = ss.str();
		mVDSettings->mNewMsg = true;
	}

}
void VDRouter::closeMidiInPort(int i) {

	if (i == 0)
	{
		mMidiIn0.closePort();
	}
	if (i == 1)
	{
		mMidiIn1.closePort();
	}
	if (i == 2)
	{
		mMidiIn2.closePort();
	}
	mMidiInputs[i].isConnected = false;

}
void VDRouter::midiOutSendNoteOn(int i, int channel, int pitch, int velocity) {

	if (i == 0)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut0.sendNoteOn(channel, pitch, velocity);
	}
	if (i == 1)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut1.sendNoteOn(channel, pitch, velocity);
	}
	if (i == 2)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut2.sendNoteOn(channel, pitch, velocity);
	}

}
void VDRouter::openMidiOutPort(int i) {

	stringstream ss;
	ss << "Port " << i << " ";
	if (i < mMidiOutputs.size()) {
		if (i == 0) {
			if (mMidiOut0.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << "Opened MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
				mMidiOut0.sendNoteOn(1, 40, 64);
			}
			else {
				ss << "Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
			}
		}
		if (i == 1) {
			if (mMidiOut1.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << "Opened MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
				mMidiOut1.sendNoteOn(1, 40, 64);
			}
			else {
				ss << "Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
			}
		}
		if (i == 2) {
			if (mMidiOut2.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << "Opened MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
				mMidiOut2.sendNoteOn(1, 40, 64);
			}
			else {
				ss << "Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
			}
		}
	}
	mVDSettings->mMsg = ss.str();
	mVDSettings->mNewMsg = true;

}
void VDRouter::closeMidiOutPort(int i) {

	if (i == 0)
	{
		mMidiOut0.closePort();
	}
	if (i == 1)
	{
		mMidiOut1.closePort();
	}
	if (i == 2)
	{
		mMidiOut2.closePort();
	}
	mMidiOutputs[i].isConnected = false;

}

void VDRouter::midiListener(midi::Message msg) {
	stringstream ss;
	midiChannel = msg.channel;
	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.control;
		midiValue = msg.value;
		midiNormalizedValue = lmap<float>(midiValue, 0.0, 127.0, 0.0, 1.0);
		if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) {
			updateAndSendOSCFloatMessage(midiControlType, midiControl, midiNormalizedValue, midiChannel);
		}
		else {
			updateParams(midiControl, midiNormalizedValue);
		}
		//mWebSockets->write("{\"params\" :[{" + controlType);
		break;
	case MIDI_NOTE_ON:
		midiControlType = "/on";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		// quick hack!
		mVDAnimation->setFloatUniformValueByIndex(14, 1.0f + midiNormalizedValue);
		break;
	case MIDI_NOTE_OFF:
		midiControlType = "/off";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	default:
		break;
	}
	ss << "MIDI Chn: " << midiChannel << " type: " << midiControlType << " CC: " << midiControl << " Pitch: " << midiPitch << " Vel: " << midiVelocity << " Val: " << midiValue << " NVal: " << midiNormalizedValue << std::endl;
	CI_LOG_V("Midi: " + ss.str());

	mVDSettings->mMidiMsg = ss.str();
}

void VDRouter::updateParams(int iarg0, float farg1) {
	if (farg1 > 0.1) {
		//avoid to send twice
		if (iarg0 == 51) {
			if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) sendOSCIntMessage("/live/prev/cue", 0); // previous cue	
			// left assign
			//mVDSettings->mLeftFragIndex = mVDSettings->iTrack;
		}
		if (iarg0 == 52) {
			if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) sendOSCIntMessage("/live/next/cue", 0); // next cue 
			// right assign
			//mVDSettings->mRightFragIndex = mVDSettings->iTrack;
		}
		if (iarg0 == 53) {
			if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) sendOSCIntMessage("/live/stop", 0); // stop
			// preview assign
			//mVDSettings->mPreviewFragIndex = mVDSettings->iTrack;
		}
		if (iarg0 == 54) {
			if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) sendOSCIntMessage("/live/play", 0); // play
		}
		if (iarg0 == 58) {
			// track left		
			mVDSettings->iTrack--;
			if (mVDSettings->iTrack < 0) mVDSettings->iTrack = 0;
		}
		if (iarg0 == 59) {
			// track right
			mVDSettings->iTrack++;
			if (mVDSettings->iTrack > 7) mVDSettings->iTrack = 7;
		}
		if (iarg0 == 60) {
			// set (reset blendmode)
			mVDSettings->iBlendmode = 0;
		}
		if (iarg0 == 61) {
			// right arrow
			mVDSettings->iBlendmode--;
			if (mVDSettings->iBlendmode < 0) mVDSettings->iBlendmode = mVDAnimation->getBlendModesCount() - 1;
		}
		if (iarg0 == 62) {
			// left arrow
			mVDSettings->iBlendmode++;
			if (mVDSettings->iBlendmode > mVDAnimation->getBlendModesCount() - 1) mVDSettings->iBlendmode = 0;
		}
	}
	if (iarg0 > 0 && iarg0 < 9) {
		// sliders 
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
	if (iarg0 > 10 && iarg0 < 19) {
		// rotary 
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
		// audio multfactor
		if (iarg0 == 13) mVDAnimation->setFloatUniformValueByIndex(iarg0, (farg1 + 0.01) * 10);
		// exposure
		if (iarg0 == 14) mVDAnimation->setFloatUniformValueByIndex(iarg0, (farg1 + 0.01) * mVDAnimation->getMaxUniformValueByIndex(14));
		// xfade
		if (iarg0 == 18) {
			mVDSettings->xFade = farg1;
			mVDSettings->xFadeChanged = true;
		}
		mVDWebsocket->wsWrite("{\"params\" :[{\"name\":" + toString(iarg0) + ",\"value\":" + toString(mVDAnimation->getFloatUniformValueByIndex(iarg0)) + "}]}");

	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29) {
		// top row
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
	if (iarg0 > 30 && iarg0 < 39)
	{
		// middle row
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
	if (iarg0 > 40 && iarg0 < 49) {
		// low row 
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
}
void VDRouter::sendOSCIntMessage(string controlType, int iarg0, int iarg1, int iarg2, int iarg3, int iarg4, int iarg5) {
	osc::Message m(controlType);
	m.append(iarg0);
	m.append(iarg1);
	m.append(iarg2);
	m.append(iarg3);
	m.append(iarg4);
	m.append(iarg5);
	mOSCSender->send(m);
}
void VDRouter::sendOSCStringMessage(string controlType, int iarg0, string sarg1, string sarg2, string sarg3, string sarg4, string sarg5) {
	osc::Message m(controlType);
	m.append(iarg0);
	if (sarg1 != "") m.append(sarg1);
	if (sarg2 != "") m.append(sarg2);
	if (sarg3 != "") m.append(sarg3);
	if (sarg4 != "") m.append(sarg4);
	if (sarg5 != "") m.append(sarg5);
	mOSCSender->send(m);
}
void VDRouter::sendOSCColorMessage(string controlType, float val) {
	osc::Message m(controlType);
	m.append(val);
	mOSCSender->send(m);
}
void VDRouter::sendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5) {
	osc::Message m(controlType);
	m.append(iarg0);
	m.append(farg1);
	m.append(farg2);
	m.append(farg3);
	m.append(farg4);
	m.append(farg5);
	mOSCSender->send(m);
}
void VDRouter::updateAndSendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5) {
	updateParams(iarg0, farg1);
	sendOSCFloatMessage(controlType, iarg0, farg1, farg2);
}

void VDRouter::colorWrite()
{
	if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) {
		sendOSCColorMessage("/fr", mVDAnimation->getFloatUniformValueByIndex(1));
		sendOSCColorMessage("/fg", mVDAnimation->getFloatUniformValueByIndex(2));
		sendOSCColorMessage("/fb", mVDAnimation->getFloatUniformValueByIndex(3));
		sendOSCColorMessage("/fa", mVDAnimation->getFloatUniformValueByIndex(4));
	}
#if defined( CINDER_MSW )
	// lights4events
	char col[8];
	int r = mVDAnimation->getFloatUniformValueByIndex(1) * 255;
	int g = mVDAnimation->getFloatUniformValueByIndex(2) * 255;
	int b = mVDAnimation->getFloatUniformValueByIndex(3) * 255;
	sprintf(col, "#%02X%02X%02X", r, g, b);
	mVDWebsocket->wsWrite(col);
#endif
}

/*void VDRouter::update() {

	// check for mouse moved message
	if(m.getAddress() == "/mouse/position"){
	// both the arguments are int32's
	Vec2i pos = Vec2i( m.getArgAsInt32(0), m.getArgAsInt32(1));
	Vec2f mouseNorm = Vec2f( pos ) / getWindowSize();
	Vec2f mouseVel = Vec2f( pos - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = pos;
	if ( m.getArgAsInt32(2) == 1 )
	{
	mMouseDown = true;
	}
	else
	{
	mMouseDown = false;
	}
	if ( mMouseDown )
	{
	mArcball.mouseDown( pos );
	mCurrentMouseDown = mInitialMouseDown = pos;
	}
	}
	// check for mouse button message
	else if(m.getAddress() == "/mouse/button"){
	// the single argument is a string
	Vec2i pos = Vec2i( m.getArgAsInt32(0), m.getArgAsInt32(1));
	mArcball.mouseDown( pos );
	mCurrentMouseDown = mInitialMouseDown = pos;
	if ( m.getArgAsInt32(2) == 1 )
	{
	mMouseDown = true;
	}
	else
	{
	mMouseDown = false;
	}
	}
	else if(m.getAddress() == "/fluid/drawfluid"){
	drawFluid = !drawFluid;
	}
	else if(m.getAddress() == "/fluid/drawfluidtex"){
	drawFluidTex = !drawFluidTex;
	}
	else if(m.getAddress() == "/fluid/drawparticles"){
	drawParticles = ! drawParticles;
	}
	else if(m.getAddress() == "/fluid/randomizecolor"){
	fluidSolver.randomizeColor();
	}
	else if(m.getAddress() == "/window/position"){
	// window position
	setWindowPos(m.getArgAsInt32(0), m.getArgAsInt32(1));
	}
	else if(m.getAddress() == "/window/setfullscreen"){
	// fullscreen
	//setFullScreen( ! isFullScreen() );
	}
	else if(m.getAddress() == "/quit"){
	quitProgram();
	}
	else{
	// unrecognized message
	//cout << "not recognized:" << m.getAddress() << endl;

	}

	}
	// osc
	while (mOSCReceiver.hasWaitingMessages())
	{
	osc::Message message;
	bool routeMessage = false;
	mOSCReceiver.getNextMessage(&message);
	for (int a = 0; a < MAX; a++)
	{
	iargs[a] = 0;
	fargs[a] = 0.0;
	sargs[a] = "";
	}
	string oscAddress = message.getAddress();

	int numArgs = message.getNumArgs();
	// get arguments
	for (int i = 0; i < message.getNumArgs(); i++)
	{
	if (i < MAX)
	{
	if (message.getArgType(i) == osc::TYPE_INT32) {
	try
	{
	iargs[i] = message.getArgAsInt32(i);
	sargs[i] = toString(iargs[i]);
	}
	catch (...) {
	cout << "Exception reading argument as int32" << std::endl;
	}
	}
	if (message.getArgType(i) == osc::TYPE_FLOAT) {
	try
	{
	fargs[i] = message.getArgAsFloat(i);
	sargs[i] = toString(fargs[i]);
	}
	catch (...) {
	cout << "Exception reading argument as float" << std::endl;
	}
	}
	if (message.getArgType(i) == osc::TYPE_STRING) {
	try
	{
	sargs[i] = message.getArgAsString(i);
	}
	catch (...) {
	cout << "Exception reading argument as string" << std::endl;
	}
	}
	}
	}



	{
	console() << "OSC message received: " << oscAddress << std::endl;
	// is it a layer msg?
	int layer = 0;
	unsigned layerFound = oscAddress.find("layer");
	if (layerFound == 1)
	{
	unsigned clipFound = oscAddress.find("/clip");
	if (clipFound == 7) // layer must be < 10
	{
	cout << "clipFound " << clipFound;
	layer = atoi(oscAddress.substr(6, 1).c_str());
	int clip = atoi(oscAddress.substr(12, 1).c_str());
	string fileName = toString((layer * 10) + clip) + ".fragjson";
	fs::path fragFile = getAssetPath("") / "shaders" / "fragjson" / fileName;
	if (fs::exists(fragFile))
	{
	//mShaders->loadFragJson(fragFile.string());
	}
	}
	else
	{
	if (clipFound == 8)
	{
	layer = atoi(oscAddress.substr(6, 2).c_str());
	}
	}
	// connect or preview
	unsigned connectFound = oscAddress.find("connect");
	if (connectFound != string::npos) cout << "connectFound " << connectFound;
	}
	//if ( layerFound != string::npos ) cout << "layerFound " << layerFound;

	unsigned found = oscAddress.find_last_of("/");
	int name = atoi(oscAddress.substr(found + 1).c_str());
	}
	stringstream ss;
	ss << message.getRemoteIp() << " adr:" << oscAddress << " ";
	for (int a = 0; a < MAX; a++)
	{
	ss << a << ":" << sargs[a] << " ";
	}
	ss << std::endl;
	mVDSettings->mWebSocketsNewMsg = true;
	mVDSettings->mWebSocketsMsg = ss.str();
	
}*/
