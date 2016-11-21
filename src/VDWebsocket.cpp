#include "VDWebsocket.h"

using namespace VideoDromm;

VDWebsocket::VDWebsocket(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDMixRef aVDMix) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDMix = aVDMix;

	CI_LOG_V("VDWebsocket constructor");
	shaderReceived = false;
	receivedFragString = "";
	// WebSockets
	clientConnected = false;
#if defined( CINDER_MSW )
	if (mVDSettings->mAreWebSocketsEnabledAtStartup) wsConnect();
	mPingTime = getElapsedSeconds();
#endif
}

void VDWebsocket::updateParams(int iarg0, float farg1) {
	if (farg1 > 0.1) {
		//avoid to send twice
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
		mVDAnimation->changeFloatValue(iarg0, farg1);
	}
	if (iarg0 > 10 && iarg0 < 19) {
		// rotary 
		mVDAnimation->changeFloatValue(iarg0, farg1);
		// audio multfactor
		if (iarg0 == 13) mVDAnimation->changeFloatValue(iarg0, (farg1 + 0.01) * 10);
		// exposure
		if (iarg0 == 14) mVDAnimation->changeFloatValue(iarg0, (farg1 + 0.01) * mVDAnimation->getMaxUniformValueByIndex(14));

		wsWrite("{\"params\" :[{\"name\":" + toString(iarg0) + ",\"value\":" + toString(mVDAnimation->getFloatUniformValueByIndex(iarg0)) + "}]}");

	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29) {
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
	if (iarg0 > 40 && iarg0 < 49) {
		// low row 
		mVDAnimation->changeFloatValue(iarg0, farg1);
	}
}

void VDWebsocket::wsPing() {
#if defined( CINDER_MSW )
	if (clientConnected) {
		if (!mVDSettings->mIsWebSocketsServer) {
			mClient.ping();
		}
	}
#endif
}

void VDWebsocket::parseMessage(string msg) {
	mVDSettings->mWebSocketsMsg = "WS onRead";
	mVDSettings->mWebSocketsNewMsg = true;
	if (!msg.empty()) {
		mVDSettings->mWebSocketsMsg += ": " + msg;
		string first = msg.substr(0, 1);
		if (first == "{") {
			// json
			JsonTree json;
			try {
				json = JsonTree(msg);
				if (json.hasChild("params")) {
					JsonTree jsonParams = json.getChild("params");
					for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement) {
						int name = jsonElement->getChild("name").getValue<int>();
						float value = jsonElement->getChild("value").getValue<float>();
						// basic name value 
						mVDAnimation->changeFloatValue(name, value);
					}
				}
				if (json.hasChild("event")) {
					//string evt = json.getChild("event").getValue<string>();
					if (json.hasChild("message")) {
						receivedFragString = json.getChild("message").getValue<string>();
						shaderReceived = true;
					}
				}
				if (json.hasChild("cmd")) {
					JsonTree jsonCmd = json.getChild("cmd");
					for (JsonTree::ConstIter jsonElement = jsonCmd.begin(); jsonElement != jsonCmd.end(); ++jsonElement) {
						receivedType = jsonElement->getChild("type").getValue<int>();
						switch (receivedType)
						{
						case 0:
							// from changeWarpFboIndex
							receivedWarpIndex = jsonElement->getChild("warp").getValue<float>();
							receivedFboIndex = jsonElement->getChild("fbo").getValue<float>();
							receivedSlot = jsonElement->getChild("slot").getValue<float>();
							if (receivedSlot == 0) {
								mVDMix->setWarpAFboIndex(receivedWarpIndex, receivedFboIndex);
							}
							else {
								mVDMix->setWarpBFboIndex(receivedWarpIndex, receivedFboIndex);
							}
							break;
						default:
							break;
						}

					}

				}
			}
			catch (cinder::JsonTree::Exception exception) {
				mVDSettings->mWebSocketsMsg += " error jsonparser exception: ";
				mVDSettings->mWebSocketsMsg += exception.what();
				mVDSettings->mWebSocketsMsg += "  ";
			}
		}
		else if (msg.substr(0, 2) == "/*") {
			// shader with json info				
			unsigned closingCommentPosition = msg.find("*/");
			if (closingCommentPosition > 0) {
				JsonTree json;
				try {
					// create folders if they don't exist
					fs::path pathsToCheck = getAssetPath("") / "glsl";
					if (!fs::exists(pathsToCheck)) fs::create_directory(pathsToCheck);
					pathsToCheck = getAssetPath("") / "glsl" / "received";
					if (!fs::exists(pathsToCheck)) fs::create_directory(pathsToCheck);
					pathsToCheck = getAssetPath("") / "glsl" / "processed";
					if (!fs::exists(pathsToCheck)) fs::create_directory(pathsToCheck);
					// find commented header
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
					std::size_t foundUniform = msg.find("uniform");

					if (foundUniform != std::string::npos) {
						// found uniforms
					}
					else {
						// save glsl file without uniforms as it was received
						fs::path currentFile = getAssetPath("") / "glsl" / "received" / glslFileName;
						ofstream mFrag(currentFile.string(), std::ofstream::binary);
						mFrag << msg;
						mFrag.close();
						CI_LOG_V("received file saved:" + currentFile.string());
						//mVDSettings->mShaderToLoad = currentFile.string(); 
						// uniforms not found, add include
						processedContent += "#include shadertoy.inc";
					}
					processedContent += shader;

					//mShaders->loadLiveShader(processedContent); // need uniforms declared
					// route it to websockets clients
					if (mVDSettings->mIsRouter) {
						wsWrite(msg);
					}

					// save processed file
					fs::path processedFile = getAssetPath("") / "glsl" / "processed" / fragFileName;
					ofstream mFragProcessed(processedFile.string(), std::ofstream::binary);
					mFragProcessed << processedContent;
					mFragProcessed.close();
					CI_LOG_V("processed file saved:" + processedFile.string());
					mVDSettings->mShaderToLoad = processedFile.string();
				}
				catch (cinder::JsonTree::Exception exception) {
					mVDSettings->mWebSocketsMsg += " error jsonparser exception: ";
					mVDSettings->mWebSocketsMsg += exception.what();
					mVDSettings->mWebSocketsMsg += "  ";
				}
			}
		}
		else if (msg.substr(0, 7) == "uniform") {
			// fragment shader from live coding
			//mShaders->loadLiveShader(msg);
			mVDSettings->mShaderToLoad = msg;
			// route it to websockets clients
			if (mVDSettings->mIsRouter) {
				wsWrite(msg);
			}
		}
		else if (msg.substr(0, 7) == "#version") {
			// fragment shader from live coding
			//mShaders->loadLiveShader(msg);
			// route it to websockets clients
			if (mVDSettings->mIsRouter) {
				wsWrite(msg);
			}
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
		else if (first == "I") {

			if (msg == "ImInit") {
				// send ImInit OK
				/*if (!remoteClientActive) { remoteClientActive = true; ForceKeyFrame = true;
				// Send confirmation mServer.write("ImInit"); // Send font texture unsigned char* pixels; int width, height;
				ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height); PreparePacketTexFont(pixels, width, height);SendPacket();
				}*/
			}
			else if (msg.substr(0, 11) == "ImMouseMove") {
				/*string trail = msg.substr(12);
				unsigned commaPosition = trail.find(",");
				if (commaPosition > 0) { mouseX = atoi(trail.substr(0, commaPosition).c_str());
				mouseY = atoi(trail.substr(commaPosition + 1).c_str()); ImGuiIO& io = ImGui::GetIO();
				io.MousePos = toPixels(vec2(mouseX, mouseY)); }*/
			}
			else if (msg.substr(0, 12) == "ImMousePress") {
				/*ImGuiIO& io = ImGui::GetIO(); // 1,0 left click 1,1 right click
				io.MouseDown[0] = false; io.MouseDown[1] = false; int rightClick = atoi(msg.substr(15).c_str());
				if (rightClick == 1) { io.MouseDown[0] = false; io.MouseDown[1] = true; }
				else { io.MouseDown[0] = true; io.MouseDown[1] = false;
				}*/
			}
		}
	}
}
string VDWebsocket::getReceivedShader() {
	shaderReceived = false;
	return receivedFragString;
}
void VDWebsocket::wsConnect() {
#if defined( CINDER_MSW )
	// either a client or a server
	if (mVDSettings->mIsWebSocketsServer) {
		mServer.connectOpenEventHandler([&]() {
			clientConnected = true;
			mVDSettings->mWebSocketsMsg = "Connected";
			mVDSettings->mWebSocketsNewMsg = true;
		});
		mServer.connectCloseEventHandler([&]() {
			clientConnected = false;
			mVDSettings->mWebSocketsMsg = "Disconnected";
			mVDSettings->mWebSocketsNewMsg = true;
		});
		mServer.connectFailEventHandler([&](string err) {
			mVDSettings->mWebSocketsMsg = "WS Error";
			mVDSettings->mWebSocketsNewMsg = true;
			if (!err.empty()) {
				mVDSettings->mWebSocketsMsg += ": " + err;
			}
		});
		mServer.connectInterruptEventHandler([&]() {
			mVDSettings->mWebSocketsMsg = "WS Interrupted";
			mVDSettings->mWebSocketsNewMsg = true;
		});
		mServer.connectPingEventHandler([&](string msg) {
			mVDSettings->mWebSocketsMsg = "WS Pinged";
			mVDSettings->mWebSocketsNewMsg = true;
			if (!msg.empty())
			{
				mVDSettings->mWebSocketsMsg += ": " + msg;
			}
		});
		mServer.connectMessageEventHandler([&](string msg) {
			parseMessage(msg);
		});
		mServer.listen(mVDSettings->mWebSocketsPort);
	}
	else
	{
		mClient.connectOpenEventHandler([&]() {
			clientConnected = true;
			mVDSettings->mWebSocketsMsg = "Connected";
			mVDSettings->mWebSocketsNewMsg = true;
		});
		mClient.connectCloseEventHandler([&]() {
			clientConnected = false;
			mVDSettings->mWebSocketsMsg = "Disconnected";
			mVDSettings->mWebSocketsNewMsg = true;
		});
		mClient.connectFailEventHandler([&](string err) {
			mVDSettings->mWebSocketsMsg = "WS Error";
			mVDSettings->mWebSocketsNewMsg = true;
			if (!err.empty()) {
				mVDSettings->mWebSocketsMsg += ": " + err;
			}
		});
		mClient.connectInterruptEventHandler([&]() {
			mVDSettings->mWebSocketsMsg = "WS Interrupted";
			mVDSettings->mWebSocketsNewMsg = true;
		});
		mClient.connectPingEventHandler([&](string msg) {
			mVDSettings->mWebSocketsMsg = "WS Ponged";
			mVDSettings->mWebSocketsNewMsg = true;
			if (!msg.empty())
			{
				mVDSettings->mWebSocketsMsg += ": " + msg;
			}
		});
		mClient.connectMessageEventHandler([&](string msg) {
			parseMessage(msg);
		});
		wsClientConnect();
	}
	mVDSettings->mAreWebSocketsEnabledAtStartup = true;
	clientConnected = true;
#endif
}
void VDWebsocket::wsClientConnect()
{
#if defined( CINDER_MSW )
	stringstream s;
	s << mVDSettings->mWebSocketsProtocol << mVDSettings->mWebSocketsHost << ":" << mVDSettings->mWebSocketsPort;
	mClient.connect(s.str());
#endif
}
void VDWebsocket::wsClientDisconnect()
{
#if defined( CINDER_MSW )	
	if (clientConnected)
	{
		mClient.disconnect();
	}
#endif
}
void VDWebsocket::wsWrite(string msg)
{
#if defined( CINDER_MSW )
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
#endif
}

void VDWebsocket::sendJSON(string params) {
#if defined( CINDER_MSW )
	wsWrite(params);
#endif
}
void VDWebsocket::toggleAuto(unsigned int aIndex) {
	// toggle
	mVDAnimation->toggleAuto(aIndex);
	// TODO send json	
}
void VDWebsocket::toggleTempo(unsigned int aIndex) {
	// toggle
	mVDAnimation->toggleTempo(aIndex);
	// TODO send json	
}
void VDWebsocket::toggleValue(unsigned int aIndex) {
	// toggle
	mVDAnimation->toggleValue(aIndex);
	stringstream sParams;
	// TODO check boolean value:
	sParams << "{\"params\" :[{\"name\" : " << aIndex << ",\"value\" : " << (int)mVDAnimation->getBoolUniformValueByIndex(aIndex) << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDWebsocket::resetAutoAnimation(unsigned int aIndex) {
	// reset
	mVDAnimation->resetAutoAnimation(aIndex);
	// TODO send json	
}

void VDWebsocket::changeBoolValue(unsigned int aControl, bool aValue) {
	// check if changed
	mVDAnimation->changeBoolValue(aControl, aValue);
	stringstream sParams;
	// TODO check boolean value:
	sParams << "{\"params\" :[{\"name\" : " << aControl << ",\"value\" : " << (int)aValue << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}

void VDWebsocket::changeFloatValue(unsigned int aControl, float aValue) {
	// check if changed
	if (mVDAnimation->changeFloatValue(aControl, aValue)) {
		stringstream sParams;
		// update color vec3
		if (aControl > 0 && aControl < 4) {
			mVDAnimation->changeVec3Value(61, vec3(mVDAnimation->getFloatUniformValueByIndex(1), mVDAnimation->getFloatUniformValueByIndex(2), mVDAnimation->getFloatUniformValueByIndex(3)));
			colorWrite(); //lights4events

		}
		sParams << "{\"params\" :[{\"name\" : " << aControl << ",\"value\" : " << mVDAnimation->getFloatUniformValueByIndex(aControl) << "}]}";
		string strParams = sParams.str();
		sendJSON(strParams);
	}
}
void VDWebsocket::changeWarpFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex, unsigned int aSlot) {
	//aSlot 0 = A, 1 = B,...
	stringstream sParams;
	sParams << "{\"cmd\" :[{\"type\" : 0,\"warp\" : " << aWarpIndex << ",\"fbo\" : " << aWarpFboIndex << ",\"slot\" : " << aSlot << "}]}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDWebsocket::changeFragmentShader(string aFragmentShaderText) {

	stringstream sParams;
	sParams << "{\"event\" : \"frag\",\"message\" : \"" << aFragmentShaderText << "\"}";
	string strParams = sParams.str();
	sendJSON(strParams);
}
void VDWebsocket::colorWrite()
{
#if defined( CINDER_MSW )
	// lights4events
	char col[8];
	int r = mVDAnimation->getFloatUniformValueByIndex(1) * 255;
	int g = mVDAnimation->getFloatUniformValueByIndex(2) * 255;
	int b = mVDAnimation->getFloatUniformValueByIndex(3) * 255;
	sprintf(col, "#%02X%02X%02X", r, g, b);
	wsWrite(col);
#endif
}

void VDWebsocket::update() {

	// websockets
#if defined( CINDER_MSW )
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
			}
		}
	}
#endif
	/* OLD KINECT AND TOUCHOSC 
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
	*/
}
