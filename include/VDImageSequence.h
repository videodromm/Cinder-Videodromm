#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"

// Settings
#include "VDSettings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDImageSequence instance
	typedef std::shared_ptr<class VDImageSequence> VDImageSequenceRef;
	// structure for loaded image files
	struct sequence {
		string						filePath;
		char						folder[32];
		string						prefix;
		string						ext;
		int							numberOfDigits;
		int							nextIndexFrameToTry;
		int							currentLoadedFrame;
		int							index;
		int							framesLoaded;
		int							playheadPosition;
		bool						loadingPaused;
		bool						loadingFilesComplete;
		bool						playing;
		int							speed;
		vector<ci::gl::TextureRef>	sequenceTextures;
	};
	class VDImageSequence {
	public:		
		VDImageSequence(VDSettingsRef aVDSettings);

		static VDImageSequenceRef		create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDImageSequence>(new VDImageSequence( aVDSettings));
		}
		void						update();
		void						createFromDir(string filePath, int index);
		void						playSequence(int textureIndex);
		void						stopSequence(int textureIndex);
		void						pauseSequence(int textureIndex);
		void						toggleLoadingFromDisk(int textureIndex);
		void						stopLoading();
		int							getPlayheadPosition(int textureIndex);
		void						setPlayheadPosition(int textureIndex, int position);

		int							getSpeed(int textureIndex);
		void						setSpeed(int textureIndex, int speed);
		void						reverseSequence(int textureIndex);
		bool						isLoadingFromDisk(int textureIndex);
		bool						isSequence(int textureIndex);
		int							getMaxFrames(int textureIndex);
		ci::gl::TextureRef			getCurrentSequenceTexture(int sequenceIndex);

	private:
		// Settings
		VDSettingsRef				mVDSettings;

		vector<Texta>				textas;
		int							playheadFrameInc;
		vector<sequence>			sequences;
		void						loadNextImageFromDisk(int currentSeq);
		void						updateSequence(int sequenceIndex);

	};


}