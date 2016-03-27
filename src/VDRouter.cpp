#include "VDRouter.h"

using namespace VideoDromm;

VDRouter::VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aAnimationRef, VDSessionRef aVDSessionRef) {
	mVDSettings = aVDSettings;
	mVDAnimation = aAnimationRef;
	mVDSession = aVDSessionRef;

	CI_LOG_V("VDRouter constructor");
	// kinect
	for (int i = 0; i < 20; i++)
	{
		skeleton[i] = ivec4(0.0f);
	}
	// OSC
	if (mVDSettings->mOSCEnabled) {
		if (mVDSettings->mIsOSCSender) {
			// OSC sender with broadcast
			osc::UdpSocketRef mSocket(new udp::socket(App::get()->io_service(), udp::endpoint(udp::v4(), mVDSettings->mOSCDestinationPort)));
			mSocket->set_option(asio::socket_base::broadcast(true));
			mOSCSender = shared_ptr<osc::SenderUdp>(new osc::SenderUdp(mSocket, udp::endpoint(address_v4::broadcast(), mVDSettings->mOSCDestinationPort)));
			mOSCSender->bind();

			//mOSCSender.setup(mVDSettings->mOSCDestinationHost, mVDSettings->mOSCDestinationPort, true);
			//mOSCSender2.setup(mVDSettings->mOSCDestinationHost2, mVDSettings->mOSCDestinationPort2, true);
			osc::Message msg("/start");
			msg.append(1);
			msg.append(2);

			mOSCSender->send(msg);
		}
		else {
			// OSC receiver
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
				[&](const osc::Message &msg){
				mVDSettings->controlValues[msg[0].int32()] = msg[1].flt();
				updateParams(msg[0].int32(), msg[1].flt());
			});
			mOSCReceiver->setListener("/Freq1",
				[&](const osc::Message &msg){
				float f1 = msg[0].flt();
			});
			mOSCReceiver->setListener("/backgroundcolor",
				[&](const osc::Message &msg){
				// background red
				mVDSettings->controlValues[5] = msg[0].flt();
				// background green
				mVDSettings->controlValues[6] = msg[1].flt();
				// background blue
				mVDSettings->controlValues[7] = msg[2].flt();

			});
			mOSCReceiver->setListener("/live/beat",
				[&](const osc::Message &msg){
				mVDSettings->iBeat = msg[0].int32();
				if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
			});
			mOSCReceiver->setListener("/live/tempo",
				[&](const osc::Message &msg){
				// Animation
				mVDSession->setBpm( msg[0].flt() );
				if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
			});
			mOSCReceiver->setListener("/live/track/meter",
				[&](const osc::Message &msg){
				mVDSettings->liveMeter = msg[2].flt();
				if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(msg);
			});
			mOSCReceiver->setListener("/live/name/trackblock",
				[&](const osc::Message &msg){
				mVDSettings->mTrackName = msg[0].string();
				for (int a = 0; a < MAX; a++)
				{
					tracks[a] = msg[a].string();
				}

			});
			mOSCReceiver->setListener("/live/play",
				[&](const osc::Message &msg){
				osc::Message m;
				m.setAddress("/tracklist");

				for (int a = 0; a < MAX; a++)
				{
					if (tracks[a] != "") m.append(tracks[a]);
				}
				if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender->send(m);

			});
			mOSCReceiver->setListener("/sumMovement",
				[&](const osc::Message &msg){
				float sumMovement = msg[0].flt();
				//exposure
				mVDSettings->controlValues[14] = sumMovement;
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
			mOSCReceiver->setListener("/handsHeadHeight",
				[&](const osc::Message &msg){
				float handsHeadHeight = msg[0].flt();
				if (handsHeadHeight > 0.3)
				{
					// glitch
					mVDSettings->controlValues[45] = 1.0f;
				}
				else
				{
					// glitch
					mVDSettings->controlValues[45] = 0.0f;
				}
				// background red
				mVDSettings->controlValues[5] = handsHeadHeight*3.0;
			});
			mOSCReceiver->setListener("/centerXY",
				[&](const osc::Message &msg){
				float x = msg[0].flt();
				float y = msg[1].flt();
				// background green
				mVDSettings->controlValues[6] = y;
				// green
				mVDSettings->controlValues[2] = x;
			});
			mOSCReceiver->setListener("/selectShader",
				[&](const osc::Message &msg){
				//selectShader(msg[0].int32(), msg[1].int32());
			});

			mOSCReceiver->setListener("/joint",
				[&](const osc::Message &msg){
				int skeletonIndex = msg[0].int32();
				int jointIndex = msg[1].int32();
				if (jointIndex < 20)
				{
					skeleton[jointIndex] = ivec4(msg[2].int32(), msg[3].int32(), msg[4].int32(), msg[5].int32());
				}
			});
			mOSCReceiver->bind();
			mOSCReceiver->listen();
		}

		// ws
		clientConnected = false;
		if (mVDSettings->mAreWebSocketsEnabledAtStartup) wsConnect();
		mPingTime = getElapsedSeconds();
		if (mVDSettings->mMIDIOpenAllInputPorts) midiSetup();
	}
}

void VDRouter::shutdown() {
	mMidiIn0.ClosePort();
	mMidiIn1.ClosePort();
	mMidiIn2.ClosePort();
}

void VDRouter::midiSetup()
{
	stringstream ss;
	ss << "setupMidi: ";

	mMidiIn0.GetPortList();
	if (mMidiIn0.mPortCount > 0)
	{
		for (int i = 0; i < mMidiIn0.mPortCount; i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiInputs.size(); j++)
			{
				if (mMidiInputs[j].portName == mMidiIn0.GetPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiInput mIn;
				mIn.portName = mMidiIn0.GetPortName(i);
				mMidiInputs.push_back(mIn);
				if (mVDSettings->mMIDIOpenAllInputPorts)
				{
					openMidiInPort(i);
					mMidiInputs[i].isConnected = true;
					ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName;
				}
				else
				{
					mMidiInputs[i].isConnected = false;
					ss << "Available MIDI port " << i << " " << mMidiIn0.GetPortName(i);
				}

			}
		}
	}
	else
	{
		ss << "No MIDI Ports found!!!!" << std::endl;
	}
	ss << std::endl;

	mVDSettings->newMsg = true;
	mVDSettings->mMsg = ss.str();
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiNormalizedValue = midiValue = midiChannel = 0;
}
void VDRouter::openMidiInPort(int i) {
	stringstream ss;
	if (i < mMidiIn0.mPortCount) {
		if (i == 0)
		{
			mMidiIn0.OpenPort(i);
			mMidiIn0.mMidiInCallback = std::bind(&VDRouter::midiListener, this, std::placeholders::_1);
		}
		if (i == 1)
		{
			mMidiIn1.OpenPort(i);
			mMidiIn1.mMidiInCallback = std::bind(&VDRouter::midiListener, this, std::placeholders::_1);
		}
		if (i == 2)
		{
			mMidiIn2.OpenPort(i);
			mMidiIn2.mMidiInCallback = std::bind(&VDRouter::midiListener, this, std::placeholders::_1);
		}
	}
	mMidiInputs[i].isConnected = true;
	ss << "Opening MIDI port " << i << " " << mMidiInputs[i].portName << std::endl;
	mVDSettings->mMsg = ss.str();
	mVDSettings->newMsg = true;
}
void VDRouter::closeMidiInPort(int i) {
	if (i == 0)
	{
		mMidiIn0.ClosePort();
	}
	if (i == 1)
	{
		mMidiIn1.ClosePort();
	}
	if (i == 2)
	{
		mMidiIn2.ClosePort();
	}
	mMidiInputs[i].isConnected = false;
}

void VDRouter::midiListener(midi::MidiMessage msg)
{
	stringstream ss;
	midiChannel = msg.Channel;
	switch (msg.StatusCode)
	{
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.Control;
		midiValue = msg.Value;
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
		midiPitch = msg.Pitch;
		midiVelocity = msg.Velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		mVDSettings->controlValues[14] = 1.0f + midiNormalizedValue; // quick hack!
		break;
	case MIDI_NOTE_OFF:
		midiControlType = "/off";
		midiPitch = msg.Pitch;
		midiVelocity = msg.Velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	default:
		break;
	}
	ss << "MIDI Chn: " << midiChannel << " type: " << midiControlType << " CC: " << midiControl << " Pitch: " << midiPitch << " Vel: " << midiVelocity << " Val: " << midiValue << " NVal: " << midiNormalizedValue << std::endl;
	mVDSettings->mMsg = ss.str();
	mVDSettings->newMsg = true;
}
void VDRouter::updateParams(int iarg0, float farg1)
{
	if (farg1 > 0.1)
	{
		//avoid to send twice
		if (iarg0 == 51) {
			sendOSCIntMessage("/live/prev/cue", 0);		// previous cue	
			// left assign
			mVDSettings->mLeftFragIndex = mVDSettings->iTrack;
		}
		if (iarg0 == 52) {
			sendOSCIntMessage("/live/next/cue", 0);		// next cue 
			// right assign
			mVDSettings->mRightFragIndex = mVDSettings->iTrack;
		}
		if (iarg0 == 53) {
			sendOSCIntMessage("/live/stop", 0);			// stop
			// preview assign
			mVDSettings->mPreviewFragIndex = mVDSettings->iTrack;
		}
		if (iarg0 == 54) sendOSCIntMessage("/live/play", 0);			// play
		if (iarg0 == 58)
		{
			// track left		
			mVDSettings->iTrack--;
			if (mVDSettings->iTrack < 0) mVDSettings->iTrack = 0;
		}
		if (iarg0 == 59)
		{
			// track right
			mVDSettings->iTrack++;
			if (mVDSettings->iTrack > 7) mVDSettings->iTrack = 7;
		}
		if (iarg0 == 61)
		{
			// right arrow
			mVDSettings->iBlendMode--;
			if (mVDSettings->iBlendMode < 0) mVDSettings->iBlendMode = mVDAnimation->maxBlendMode;
		}
		if (iarg0 == 62)
		{
			// left arrow
			mVDSettings->iBlendMode++;
			if (mVDSettings->iBlendMode > mVDAnimation->maxBlendMode) mVDSettings->iBlendMode = 0;
		}
	}
	if (iarg0 > 0 && iarg0 < 9)
	{
		// sliders 
		mVDSettings->controlValues[iarg0] = farg1;
	}
	if (iarg0 > 10 && iarg0 < 19)
	{
		// rotary 
		mVDSettings->controlValues[iarg0] = farg1;
		// audio multfactor
		if (iarg0 == 13) mVDSettings->controlValues[iarg0] = (farg1 + 0.01) * 10;
		// exposure
		if (iarg0 == 14) mVDSettings->controlValues[iarg0] = (farg1 + 0.01) * mVDAnimation->maxExposure;
	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29)
	{
		// select index
		mVDSettings->selectedWarp = iarg0 - 21;
	}
	/*if (iarg0 > 30 && iarg0 < 39)
	{
		// select input
		mVDSettings->mWarpFbos[mVDSettings->selectedWarp].textureIndex = iarg0 - 31;
		// activate
		mVDSettings->mWarpFbos[mVDSettings->selectedWarp].active = !mVDSettings->mWarpFbos[mVDSettings->selectedWarp].active;
	}*/
	if (iarg0 > 40 && iarg0 < 49)
	{
		// low row 
		mVDSettings->controlValues[iarg0] = farg1;
	}

}

void VDRouter::sendOSCIntMessage(string controlType, int iarg0, int iarg1, int iarg2, int iarg3, int iarg4, int iarg5)
{
	osc::Message m(controlType);
	m.append(iarg0);
	m.append(iarg1);
	m.append(iarg2);
	m.append(iarg3);
	m.append(iarg4);
	m.append(iarg5);
	mOSCSender->send(m);
}
void VDRouter::sendOSCStringMessage(string controlType, int iarg0, string sarg1, string sarg2, string sarg3, string sarg4, string sarg5)
{
	osc::Message m(controlType);
	m.append(iarg0);
	if (sarg1 != "") m.append(sarg1);
	if (sarg2 != "") m.append(sarg2);
	if (sarg3 != "") m.append(sarg3);
	if (sarg4 != "") m.append(sarg4);
	if (sarg5 != "") m.append(sarg5);
	mOSCSender->send(m);
}
void VDRouter::sendOSCColorMessage(string controlType, float val)
{
	osc::Message m(controlType);
	m.append(val);
	mOSCSender->send(m);
}
void VDRouter::sendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5)
{
	osc::Message m(controlType);
	m.append(iarg0);
	m.append(farg1);
	m.append(farg2);
	m.append(farg3);
	m.append(farg4);
	m.append(farg5);
	mOSCSender->send(m);
}
void VDRouter::updateAndSendOSCFloatMessage(string controlType, int iarg0, float farg1, float farg2, float farg3, float farg4, float farg5)
{
	updateParams(iarg0, farg1);
	sendOSCFloatMessage(controlType, iarg0, farg1, farg2);
}

void VDRouter::wsPing()
{
	if (clientConnected)
	{
		if (!mVDSettings->mIsWebSocketsServer)
		{
			mClient.ping();
		}
	}
}

void VDRouter::wsConnect()
{
	// either a client or a server
	if (mVDSettings->mIsWebSocketsServer)
	{
		mServer.connectOpenEventHandler([&]()
		{
			clientConnected = true;
			mVDSettings->mMsg = "Connected";
			mVDSettings->newMsg = true;
		});
		mServer.connectCloseEventHandler([&]()
		{
			clientConnected = false;
			mVDSettings->mMsg = "Disconnected";
			mVDSettings->newMsg = true;
		});
		mServer.connectFailEventHandler([&](string err)
		{
			mVDSettings->mMsg = "WS Error";
			mVDSettings->newMsg = true;
			if (!err.empty()) {
				mVDSettings->mMsg += ": " + err;
			}

		});
		mServer.connectInterruptEventHandler([&]()
		{
			mVDSettings->mMsg = "WS Interrupted";
			mVDSettings->newMsg = true;
		});
		mServer.connectPingEventHandler([&](string msg)
		{
			mVDSettings->mMsg = "WS Pinged";
			mVDSettings->newMsg = true;
			if (!msg.empty())
			{
				mVDSettings->mMsg += ": " + msg;
			}
		});
		mServer.connectMessageEventHandler([&](string msg)
		{
			int left;
			int index;
			mVDSettings->mMsg = "WS onRead";
			mVDSettings->newMsg = true;
			if (!msg.empty())
			{
				mVDSettings->mMsg += ": " + msg;
				string first = msg.substr(0, 1);
				if (first == "{")
				{
					// json
					JsonTree json;
					try
					{
						json = JsonTree(msg);
						JsonTree jsonParams = json.getChild("params");
						for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement)
						{
							int name = jsonElement->getChild("name").getValue<int>();
							float value = jsonElement->getChild("value").getValue<float>();
							if (name > mVDSettings->controlValues.size()) {
								switch (name)
								{
								case 300:
									//selectShader
									left = jsonElement->getChild("left").getValue<int>();
									index = jsonElement->getChild("index").getValue<int>();
									//selectShader(left, index);
									break;
								default:
									break;
								}

							}
							else {
								// basic name value 
								mVDSettings->controlValues[name] = value;
							}
						}
						JsonTree jsonSelectShader = json.getChild("selectShader");
						for (JsonTree::ConstIter jsonElement = jsonSelectShader.begin(); jsonElement != jsonSelectShader.end(); ++jsonElement)
						{
						}
					}
					catch (cinder::JsonTree::Exception exception)
					{
						mVDSettings->mMsg += " error jsonparser exception: ";
						mVDSettings->mMsg += exception.what();
						mVDSettings->mMsg += "  ";
					}
				}
				else if (msg.substr(0, 2) == "/*")
				{
					// shader with json info				
					unsigned closingCommentPosition = msg.find("*/");
					if (closingCommentPosition > 0) {
						JsonTree json;
						try
						{
							string jsonHeader = msg.substr(2, closingCommentPosition - 2);
							ci::JsonTree::ParseOptions parseOptions;
							parseOptions.ignoreErrors(false);
							json = JsonTree(jsonHeader, parseOptions);
							string title = json.getChild("title").getValue<string>();
							string fragFileName = title + ".frag"; // with uniforms
							string glslFileName = title + ".glsl"; // without uniforms, need to include shadertoy.inc
							string shader = msg.substr(closingCommentPosition + 2);

							string processedContent = "/*" + jsonHeader + "*/";
							// check uniforms presence
							unsigned uniformPosition = msg.find("uniform");
							if (uniformPosition < 1) {
								// uniforms not found, add include
								processedContent += "#include shadertoy.inc";
							}
							processedContent += shader;


							//mShaders->loadLiveShader(processedContent); // need uniforms declared
							mVDSettings->mShaderToLoad = shader; // CHECK if useless?
							// route it to websockets clients
							if (mVDSettings->mIsRouter) {
								wsWrite(msg);
							}
							// save it as is
							fs::path currentFile = getAssetPath("") / "glsl" / "received" / fragFileName;
							ofstream mFrag(currentFile.string(), std::ofstream::binary);
							mFrag << msg;
							mFrag.close();
							CI_LOG_V("received file saved:" + currentFile.string());



							// check vertex.uv presence

							// save processed file
							//processedContent = msg;
							fs::path processedFile = getAssetPath("") / "glsl" / "processed" / fragFileName;
							ofstream mFragProcessed(processedFile.string(), std::ofstream::binary);
							mFragProcessed << processedContent;
							mFragProcessed.close();
							CI_LOG_V("processed file saved:" + processedFile.string());
						}
						catch (cinder::JsonTree::Exception exception)
						{
							mVDSettings->mMsg += " error jsonparser exception: ";
							mVDSettings->mMsg += exception.what();
							mVDSettings->mMsg += "  ";
						}


					}
				}
				else if (msg.substr(0, 7) == "uniform")
				{
					// fragment shader from live coding
					//mShaders->loadLiveShader(msg);
					mVDSettings->mShaderToLoad = msg;
					// route it to websockets clients
					if (mVDSettings->mIsRouter) {
						wsWrite(msg);
					}
				}
				else if (msg.substr(0, 7) == "#version")
				{
					// fragment shader from live coding
					//mShaders->loadLiveShader(msg);
					// route it to websockets clients
					if (mVDSettings->mIsRouter) {
						wsWrite(msg);
					}

				}
				else if (first == "I")
				{

					if (msg == "ImInit") {
						// send ImInit OK
						/*if (!remoteClientActive)
						{
						remoteClientActive = true;
						ForceKeyFrame = true;
						// Send confirmation
						mServer.write("ImInit");
						// Send font texture
						unsigned char* pixels;
						int width, height;
						ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

						PreparePacketTexFont(pixels, width, height);
						SendPacket();
						}*/
					}
					else if (msg.substr(0, 11) == "ImMouseMove") {
						/*string trail = msg.substr(12);
						unsigned commaPosition = trail.find(",");
						if (commaPosition > 0) {
						mouseX = atoi(trail.substr(0, commaPosition).c_str());
						mouseY = atoi(trail.substr(commaPosition + 1).c_str());
						ImGuiIO& io = ImGui::GetIO();
						io.MousePos = toPixels(vec2(mouseX, mouseY));

						}*/

					}
					else if (msg.substr(0, 12) == "ImMousePress") {
						/*ImGuiIO& io = ImGui::GetIO(); // 1,0 left click 1,1 right click
						io.MouseDown[0] = false;
						io.MouseDown[1] = false;
						int rightClick = atoi(msg.substr(15).c_str());
						if (rightClick == 1) {

						io.MouseDown[0] = false;
						io.MouseDown[1] = true;
						}
						else {
						io.MouseDown[0] = true;
						io.MouseDown[1] = false;
						}*/
					}
				}
			}
		});
		mServer.listen(mVDSettings->mWebSocketsPort);
	}
	else
	{
		mClient.connectOpenEventHandler([&]()
		{
			clientConnected = true;
			mVDSettings->mMsg = "Connected";
			mVDSettings->newMsg = true;
		});
		mClient.connectCloseEventHandler([&]()
		{
			clientConnected = false;
			mVDSettings->mMsg = "Disconnected";
			mVDSettings->newMsg = true;
		});
		mClient.connectFailEventHandler([&](string err)
		{
			mVDSettings->mMsg = "WS Error";
			mVDSettings->newMsg = true;
			if (!err.empty()) {
				mVDSettings->mMsg += ": " + err;
			}

		});
		mClient.connectInterruptEventHandler([&]()
		{
			mVDSettings->mMsg = "WS Interrupted";
			mVDSettings->newMsg = true;
		});
		mClient.connectPingEventHandler([&](string msg)
		{
			mVDSettings->mMsg = "WS Ponged";
			mVDSettings->newMsg = true;
			if (!msg.empty())
			{
				mVDSettings->mMsg += ": " + msg;
			}
		});
		mClient.connectMessageEventHandler([&](string msg)
		{
			int left;
			int index;
			mVDSettings->mMsg = "WS onRead";
			mVDSettings->newMsg = true;
			if (!msg.empty())
			{
				mVDSettings->mMsg += ": " + msg;
				string first = msg.substr(0, 1);
				if (first == "{")
				{
					// json
					JsonTree json;
					try
					{
						json = JsonTree(msg);
						JsonTree jsonParams = json.getChild("params");
						for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement)
						{
							int name = jsonElement->getChild("name").getValue<int>();
							float value = jsonElement->getChild("value").getValue<float>();
							if (name > mVDSettings->controlValues.size()) {
								switch (name)
								{
								case 300:
									//selectShader
									left = jsonElement->getChild("left").getValue<int>();
									index = jsonElement->getChild("index").getValue<int>();
									//selectShader(left, index);
									break;
								default:
									break;
								}

							}
							else {
								// basic name value 
								mVDSettings->controlValues[name] = value;
							}
						}
						JsonTree jsonSelectShader = json.getChild("selectShader");
						for (JsonTree::ConstIter jsonElement = jsonSelectShader.begin(); jsonElement != jsonSelectShader.end(); ++jsonElement)
						{
						}
					}
					catch (cinder::JsonTree::Exception exception)
					{
						mVDSettings->mMsg += " error jsonparser exception: ";
						mVDSettings->mMsg += exception.what();
						mVDSettings->mMsg += "  ";
					}
				}
				else if (first == "#")
				{
					// fragment shader from live coding
					//mBatchass->getShadersRef()->loadLiveShader(msg);

				}
				else if (first == "/")
				{
					// osc from videodromm-nodejs-router
					int f = msg.size();
					const char c = msg[9];
					int s = msg[12];
					int t = msg[13];
					int u = msg[14];
					CI_LOG_V(msg);

				}
			}
		});
		wsClientConnect();
	}
	mVDSettings->mAreWebSocketsEnabledAtStartup = true;
	clientConnected = true;
}
void VDRouter::wsClientConnect()
{
	stringstream s;
	s << "ws://" << mVDSettings->mWebSocketsHost << ":" << mVDSettings->mWebSocketsPort;
	mClient.connect(s.str());
}
void VDRouter::wsClientDisconnect()
{
	if (clientConnected)
	{
		mClient.disconnect();
	}
}
void VDRouter::wsWrite(string msg)
{
	if (mVDSettings->mAreWebSocketsEnabledAtStartup)
	{
		if (mVDSettings->mIsWebSocketsServer)
		{
			mServer.write(msg);
		}
		else
		{
			if (clientConnected) mClient.write(msg);
		}
	}
}
void VDRouter::sendJSON(string params) {
	wsWrite(params);
	if (mVDSettings->mOSCEnabled) {

	}
}
void VDRouter::colorWrite()
{
	if (mVDSettings->mOSCEnabled && mVDSettings->mIsOSCSender) {
		sendOSCColorMessage("/fr", mVDSettings->controlValues[1]);
		sendOSCColorMessage("/fg", mVDSettings->controlValues[2]);
		sendOSCColorMessage("/fb", mVDSettings->controlValues[3]);
		sendOSCColorMessage("/fa", mVDSettings->controlValues[4]);
	}
	char col[8];
	int r = mVDSettings->controlValues[1] * 255;
	int g = mVDSettings->controlValues[2] * 255;
	int b = mVDSettings->controlValues[3] * 255;
	//sprintf_s(col, sizeof(col), "#%02X%02X%02X", r, g, b);
	//wsWrite(col);
}

void VDRouter::update() {
	// websockets
	if (mVDSettings->mAreWebSocketsEnabledAtStartup)
	{
		if (mVDSettings->mIsWebSocketsServer)
		{
			mServer.poll();
		}
		else
		{
			if (clientConnected)
			{
				mClient.poll();
				/*double e = getElapsedSeconds();
				if (e - mPingTime > 20.0) {
				mClient.ping();
				mPingTime = e;
				}*/

			}
		}
	}
	/*
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
	mVDSettings->newMsg = true;
	mVDSettings->mMsg = ss.str();
	// filter messages
	if (routeMessage)
	{
	// avoid LiveOSC infinite loop
	if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort != 9000) mOSCSender.sendMessage(message);
	if (mVDSettings->mIsOSCSender && mVDSettings->mOSCDestinationPort2 != 9000) mOSCSender2.sendMessage(message);

	}
	*/

}
