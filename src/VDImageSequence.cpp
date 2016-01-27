#include "VDImageSequence.h"

using namespace VideoDromm;
	
VDImageSequence::VDImageSequence(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	for (int j = 0; j < mVDSettings->MAX - 1; j++)
	{
		Texta tex;
		sprintf_s(tex.name, "tex%d", j);
		tex.sequenceIndex = 0;
		tex.isSequence = false;
		textas.push_back(tex);
	}
	// image sequence
	playheadFrameInc = 1;
	// init with one sequence
	sequence seq;
	seq.filePath = "none";
	sprintf_s(seq.folder, "none");
	seq.index = 0;
	seq.loadingFilesComplete = true;
	seq.loadingPaused = true;
	seq.framesLoaded = 0;
	seq.currentLoadedFrame = 0;
	seq.nextIndexFrameToTry = 0;
	seq.playing = false;
	seq.speed = 1;
	seq.ext = "png";
	seq.prefix = "none";
	seq.nextIndexFrameToTry = 0;
	seq.playheadPosition = 0;
	sequences.push_back(seq);

	sequences[0].sequenceTextures.push_back(gl::Texture::create(loadImage(loadAsset("logo/videodromm-logo.png"))));
}
void VDImageSequence::update() {
	if (sequences.size() > 0) {
		for (int i = 0; i < sequences.size(); i++) {
			updateSequence(i);
			if (!sequences[i].loadingFilesComplete) {
				loadNextImageFromDisk(i);
			}
		}
	}
}
//Begins playback of sequence
void VDImageSequence::playSequence(int textureIndex)
{
	sequences[textas[textureIndex].sequenceIndex].playing = true;
}
// Pauses playback
void VDImageSequence::pauseSequence(int textureIndex)
{

	sequences[textas[textureIndex].sequenceIndex].playing = false;
}

// Stops playback and resets the playhead to zero
void VDImageSequence::stopSequence(int textureIndex)
{
	sequences[textas[textureIndex].sequenceIndex].playing = false;
	sequences[textas[textureIndex].sequenceIndex].playheadPosition = 0;
}

int VDImageSequence::getMaxFrames(int textureIndex) {

	return sequences[textas[textureIndex].sequenceIndex].framesLoaded;
}
int VDImageSequence::getPlayheadPosition(int textureIndex) {

	return sequences[textas[textureIndex].sequenceIndex].playheadPosition;
}
// Seek to a new position in the sequence
void VDImageSequence::setPlayheadPosition(int textureIndex, int position) {

	sequences[textas[textureIndex].sequenceIndex].playheadPosition = max(0, min(position, (int)sequences[textas[textureIndex].sequenceIndex].sequenceTextures.size() - 1));
}

void VDImageSequence::reverseSequence(int textureIndex) {
	sequences[textas[textureIndex].sequenceIndex].speed *= -1;
}
int VDImageSequence::getSpeed(int textureIndex) {

	return sequences[textas[textureIndex].sequenceIndex].speed;
}
void VDImageSequence::setSpeed(int textureIndex, int speed) {
	sequences[textas[textureIndex].sequenceIndex].speed = speed;
}
bool VDImageSequence::isLoadingFromDisk(int textureIndex) {
	return sequences[textas[textureIndex].sequenceIndex].loadingFilesComplete;
}

void VDImageSequence::toggleLoadingFromDisk(int textureIndex) {

	sequences[textas[textureIndex].sequenceIndex].loadingPaused = !sequences[textas[textureIndex].sequenceIndex].loadingPaused;
}
/**
*  -- Loads all files contained in the supplied folder and creates Textures from them
*/
void VDImageSequence::createFromDir(string filePath, int index)
{
	fs::path p(filePath);
	sequence seq;
	seq.filePath = filePath;
	seq.index = index;
	seq.loadingFilesComplete = false;
	seq.loadingPaused = false;
	seq.framesLoaded = 0;
	seq.currentLoadedFrame = 0;
	seq.nextIndexFrameToTry = 0;
	seq.playing = false;
	seq.speed = 1;
	seq.numberOfDigits = 4;
	sprintf_s(seq.folder, "");
	// find the folder name for display in the ui
	if (filePath.find_last_of("\\") != std::string::npos) {
		int slashIndex = filePath.find_last_of("\\") + 1;
		string folder = filePath.substr(slashIndex);
		sprintf_s(seq.folder, "%s", folder.c_str());
	}
	bool noValidFile = true; // if no valid files in the folder, we keep existing vector
	bool firstIndexFound = false;
	int i = 0;
	// loading 2000 files takes a while, I load only 2
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName = it->path().filename().string();
			if (fileName.find_last_of(".") != std::string::npos) {
				int dotIndex = fileName.find_last_of(".");
				seq.ext = fileName.substr(dotIndex + 1);
				// get the prefix for the image sequence
				// the files are named from p0000.jpg to p2253.jpg for instance
				// sometimes only 3 digits : l000 this time
				// find the first digit
				int firstDigit = fileName.find_first_of("0123456789");
				// if valid image sequence (contains a digit)
				if (firstDigit > -1) {
					seq.numberOfDigits = dotIndex - firstDigit;
					int prefixIndex = fileName.find_last_of(".") - seq.numberOfDigits;//-4 or -3
					seq.prefix = fileName.substr(0, prefixIndex);
					if (!firstIndexFound) {
						firstIndexFound = true;
						seq.nextIndexFrameToTry = std::stoi(fileName.substr(prefixIndex, dotIndex));
					}
				}
			}
			// only if proper image sequence
			if (firstIndexFound) {
				if (seq.ext == "png" || seq.ext == "jpg")
				{
					if (noValidFile)
					{
						// we found a valid file
						noValidFile = false;
						seq.sequenceTextures.clear();
						// reset playhead to the start
						seq.playheadPosition = 0;
						sequences.push_back(seq);
						textas[seq.index].sequenceIndex = sequences.size() - 1;
						sprintf_s(textas[seq.index].name, "%s", seq.folder);
						textas[seq.index].isSequence = true;
						loadNextImageFromDisk(sequences.size() - 1);
						seq.playing = true;
					}
				}
			}
		}
	}

}
bool VDImageSequence::isSequence(int textureIndex) {
	return textas[textureIndex].isSequence;
}

ci::gl::TextureRef VDImageSequence::getCurrentSequenceTexture(int sequenceIndex) {
	if (sequenceIndex > sequences.size()-1) {
		sequenceIndex = 0;
	}
	if (sequences[sequenceIndex].playheadPosition > sequences[sequenceIndex].framesLoaded) {
		//error
	}
	return sequences[sequenceIndex].sequenceTextures[sequences[sequenceIndex].playheadPosition];
}
void VDImageSequence::stopLoading() {
	for (int i = 0; i < sequences.size(); i++)
	{
		sequences[i].loadingPaused = true;
	}
}
void VDImageSequence::loadNextImageFromDisk(int currentSeq) {
	if (!sequences[currentSeq].loadingPaused) {

		if (!sequences[currentSeq].loadingFilesComplete) {
			// thank you Omar!
			char restOfFileName[32];
			if (sequences[currentSeq].numberOfDigits == 4) {
				sprintf(restOfFileName, "%04d", sequences[currentSeq].nextIndexFrameToTry);

			}
			else {
				sprintf(restOfFileName, "%03d", sequences[currentSeq].nextIndexFrameToTry);

			}

			fs::path fileToLoad = sequences[currentSeq].filePath + sequences[currentSeq].prefix + restOfFileName + "." + sequences[currentSeq].ext;
			if (fs::exists(fileToLoad)) {
				sequences[currentSeq].sequenceTextures.push_back(ci::gl::Texture::create(loadImage(fileToLoad)));
				sequences[currentSeq].currentLoadedFrame = sequences[currentSeq].framesLoaded;
				sequences[currentSeq].framesLoaded++;
				mVDSettings->mMsg = fileToLoad.string() + " loaded";
			}
			else {
				mVDSettings->mMsg = fileToLoad.string() + " does not exist";
				if (sequences[currentSeq].framesLoaded > 0) {
					// if frames have been loaded we hit the last file of the image sequence at this point
					mVDSettings->mMsg = "last image loaded";
					sequences[currentSeq].loadingFilesComplete = true;
				}
			}
			mVDSettings->newMsg = true;
			// increment counter for next filename
			sequences[currentSeq].nextIndexFrameToTry++;
			if (sequences[currentSeq].nextIndexFrameToTry > 9999) sequences[currentSeq].loadingFilesComplete = true;

		}
	}
}
void VDImageSequence::updateSequence(int sequenceIndex)
{
	int newPosition;
	if (sequences[sequenceIndex].sequenceTextures.size() > 0) {
		// Call on each frame to update the playhead
		if (sequences[sequenceIndex].playing) {
			newPosition = sequences[sequenceIndex].playheadPosition + (playheadFrameInc * sequences[sequenceIndex].speed);
			if (newPosition < 0) newPosition = sequences[sequenceIndex].sequenceTextures.size() - 1;
			if (newPosition > sequences[sequenceIndex].sequenceTextures.size() - 1) newPosition = 0;
		}
		else {
			newPosition = (int)mVDSettings->iBeat % (sequences[sequenceIndex].sequenceTextures.size());
		}
		sequences[sequenceIndex].playheadPosition = max(0, min(newPosition, (int)sequences[sequenceIndex].sequenceTextures.size() - 1));
		//sTextures[sequences[sequenceIndex].index] = getCurrentSequenceTexture(sequenceIndex);
	}

}
