#include "soundManager.h"

#ifndef AUTO_LOCK
#define AUTO_LOCK(mtx)	std::lock_guard<std::mutex> lock(mtx)
#endif

using namespace cocos2d;
using namespace experimental;

const int CSoundManager::INVALID_AUDIO_ID = AudioEngine::INVALID_AUDIO_ID;
const float	CSoundManager::TIME_UNKNOWN = AudioEngine::TIME_UNKNOWN;

static unsigned int _hash(const char *key)
{
	unsigned int len = strlen(key);
	const char *end = key + len;
	unsigned int hash;

	for (hash = 0; key < end; key++)
	{
		hash *= 16777619;
		hash ^= (unsigned int)(unsigned char)toupper(*key);
	}
	return (hash);
}

CSoundManager::CSoundManager()
: _isBgmOn(true)
, _isEffectOn(true)
, _bgmVolume(1.0f)
, _effectVolume(1.0f)
{
	AudioEngine::lazyInit();
}

CSoundManager::~CSoundManager()
{
	uncacheAllAudioFiles();
}

bool CSoundManager::init()
{
	/* USER_IMPLEMENTATION_BEGIN */
	// 공유 메모리로 부터 배경음, 효과음의 ON/OFF 여부와 각각의 볼륨값을 셋팅 해준다. (참고로 볼륨 최대값은 1.0f)
	// init 함수를 부르지 않을 것이라면 아래 네개의 함수를 직접 불러줘도 된다.
	bool isBgmOn = true;
	bool isEffectOn = true;
	float bgmVolume = 1.0f;
	float effectVolume = 1.0f;
	/* USER_IMPLEMENTATION_END */
	
	setBgmOn(isBgmOn);
	setEffectOn(isEffectOn);
	setBgmVolume(bgmVolume);
	setEffectVolume(effectVolume);
	
	return true;
}

int CSoundManager::playBgm(const std::string & filePath, bool loop, bool stopPrevBgm, float volumeRatio, const std::function<void(int, const std::string&)> & callbackWhenFinished)
{
	if (!_isBgmOn)
		return INVALID_AUDIO_ID;

	auto hash = _hash(filePath.c_str());
	
	// 같은 BGM 파일이 시간차를 두고 재생 되는 것을 방지한다.
	{
		_mutexForBgm.lock();
		auto it = _bgmAudioIdMMap.find(hash);
		if (it != _bgmAudioIdMMap.end())
		{
			_mutexForBgm.unlock();
			int audioId = it->second;
			
			if (stopPrevBgm)
				stopAllBgms(filePath);

			return audioId;
		}
		_mutexForBgm.unlock();
	}
	
	// 이미 재생 중인 BGM이 있는 경우 모두 정지시킨다. (단, 파라미터로 넘어온 filePath와 같은 파일이라면 그냥 리턴한다.)
	if (stopPrevBgm)
		stopAllBgms();
	
	if (filePath == "")
		return INVALID_AUDIO_ID;

	int audioId = AudioEngine::play2d(filePath, loop, _bgmVolume * volumeRatio);
	if (audioId != INVALID_AUDIO_ID)
	{
		AudioEngine::setFinishCallback(audioId, CC_CALLBACK_2(CSoundManager::bgmPlayFinishedCallback, this));

		AUTO_LOCK(_mutexForBgm);
		_bgmAudioIdMMap.insert(std::pair<unsigned int, int>(hash, audioId));
		_bgmPathHashMap.insert(std::pair<int, unsigned int>(audioId, hash));

		if (callbackWhenFinished)
			_bgmFinishCallbackMap.insert(std::pair<int, std::function<void(int, const std::string &)>>(audioId, callbackWhenFinished));
	}

	return audioId;
}

void CSoundManager::stopBgm(int audioId)
{
	AudioEngine::stop(audioId);

	bgmPlayFinishedCallback(audioId, "");
}

void CSoundManager::stopBgm(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	std::multimap<unsigned int, int>::iterator it;
	
	// mutex 처리 때문데 multimap의 equal_range를 사용하지 않고 하나씩 처리함
	while (true)
	{
		_mutexForBgm.lock();
		it = _bgmAudioIdMMap.find(hash);

		if (it == _bgmAudioIdMMap.end())
		{
			_mutexForBgm.unlock();
			break;
		}
		_mutexForBgm.unlock();

		stopBgm(it->second);
	}	
}

void CSoundManager::stopAllBgms(const std::string & exceptFilePath)
{
	if (exceptFilePath == "")
	{
		std::multimap<unsigned int, int>::iterator it;

		while (true)
		{
			_mutexForBgm.lock();
			it = _bgmAudioIdMMap.begin();

			if (it == _bgmAudioIdMMap.end())
			{
				_mutexForBgm.unlock();
				break;
			}
			_mutexForBgm.unlock();

			stopBgm(it->second);
		}
	}
	else
	{
		std::vector<int> audioIdToStopVec;
		int hash = _hash(exceptFilePath.c_str());

		_mutexForBgm.lock();
		for (auto & it : _bgmAudioIdMMap)
		{			
			if (it.first != hash)
				audioIdToStopVec.push_back(it.second);
		}
		_mutexForBgm.unlock();

		for (auto it : audioIdToStopVec)
			stopBgm(it);
	}
}

void CSoundManager::pauseBgm(int audioId)
{
	AudioEngine::pause(audioId);
}

void CSoundManager::pauseBgm(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	AUTO_LOCK(_mutexForBgm);

	auto range = _bgmAudioIdMMap.equal_range(hash);
	for (auto it = range.first; it != range.second; ++it)
		pauseBgm(it->second);
}

void CSoundManager::pauseAllBgm()
{
	AUTO_LOCK(_mutexForBgm);

	for (auto & it : _bgmAudioIdMMap)
		pauseBgm(it.second);
}

void CSoundManager::resumeBgm(int audioId)
{
	AudioEngine::resume(audioId);
}

void CSoundManager::resumeBgm(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	AUTO_LOCK(_mutexForBgm);

	auto range = _bgmAudioIdMMap.equal_range(hash);
	for (auto it = range.first; it != range.second; ++it)
		resumeBgm(it->second);
}

void CSoundManager::resumeAllBgm()
{
	AUTO_LOCK(_mutexForBgm);

	for (auto & it : _bgmAudioIdMMap)
		resumeBgm(it.second);
}

bool CSoundManager::isBgmPlaying(int audioId)
{
	return AudioEngine::getState(audioId) == AudioEngine::AudioState::PLAYING;
}

bool CSoundManager::isBgmPlaying(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	AUTO_LOCK(_mutexForBgm);

	auto range = _bgmAudioIdMMap.equal_range(hash);
	for (auto it = range.first; it != range.second; ++it)
	{
		if (AudioEngine::getState(it->second) == AudioEngine::AudioState::PLAYING)
			return true;
	}

	return false;
}

bool CSoundManager::isAnyBgmPlaying()
{
	AUTO_LOCK(_mutexForBgm);

	for (auto & it : _bgmAudioIdMMap)
	{
		if (AudioEngine::getState(it.second) == AudioEngine::AudioState::PLAYING)
			return true;
	}

	return false;
}

int CSoundManager::playEffect(const std::string & filePath, bool loop, float volumeRatio, const std::function<void(int, const std::string&)> & callbackWhenFinished)
{
	if (!_isEffectOn)
		return INVALID_AUDIO_ID;

	if (filePath == "")
		return INVALID_AUDIO_ID;

	int audioId = AudioEngine::play2d(filePath, loop, _effectVolume * volumeRatio);
	if (audioId != INVALID_AUDIO_ID)
	{
		AudioEngine::setFinishCallback(audioId, CC_CALLBACK_2(CSoundManager::effectPlayFinishedCallback, this));
		auto hash = _hash(filePath.c_str());

		AUTO_LOCK(_mutexForEffect);
		_effectAudioIdMMap.insert(std::pair<unsigned int, int>(hash, audioId));
		_effectPathHashMap.insert(std::pair<int, unsigned int>(audioId, hash));

		if (callbackWhenFinished)
			_effectFinishCallbackMap.insert(std::pair<int, std::function<void(int, const std::string &)>>(audioId, callbackWhenFinished));
	}

	return audioId;
}

void CSoundManager::stopEffect(int audioId)
{
	AudioEngine::stop(audioId);
	
	effectPlayFinishedCallback(audioId, "");
}

void CSoundManager::stopEffect(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	std::multimap<unsigned int, int>::iterator it;

	// mutex 처리 때문데 multimap의 equal_range를 사용하지 않고 하나씩 처리함
	while (true)
	{
		_mutexForEffect.lock();
		it = _effectAudioIdMMap.find(hash);

		if (it == _effectAudioIdMMap.end())
		{
			_mutexForEffect.unlock();
			break;
		}
		_mutexForEffect.unlock();

		stopEffect(it->second);
	}
}

void CSoundManager::stopAllEffects()
{
	std::multimap<unsigned int, int>::iterator it;

	while (true)
	{
		_mutexForEffect.lock();
		it = _effectAudioIdMMap.begin();

		if (it == _effectAudioIdMMap.end())
		{
			_mutexForEffect.unlock();
			break;
		}
		_mutexForEffect.unlock();

		stopEffect(it->second);
	}
}

void CSoundManager::pauseEffect(int audioId)
{
	AudioEngine::pause(audioId);
}

void CSoundManager::pauseEffect(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	AUTO_LOCK(_mutexForEffect);

	auto range = _effectAudioIdMMap.equal_range(hash);
	for (auto it = range.first; it != range.second; ++it)
		pauseEffect(it->second);
}

void CSoundManager::pauseAllEffects()
{
	AUTO_LOCK(_mutexForEffect);

	for (auto & it : _effectAudioIdMMap)
		pauseEffect(it.second);
}

void CSoundManager::resumeEffect(int audioId)
{
	AudioEngine::resume(audioId);
}

void CSoundManager::resumeEffect(const std::string & filePath)
{
	auto hash = _hash(filePath.c_str());

	AUTO_LOCK(_mutexForEffect);

	auto range = _effectAudioIdMMap.equal_range(hash);
	for (auto it = range.first; it != range.second; ++it)
		resumeEffect(it->second);
}

void CSoundManager::resumeAllEffects()
{
	AUTO_LOCK(_mutexForEffect);

	for (auto & it : _effectAudioIdMMap)
		resumeEffect(it.second);
}

void CSoundManager::setBgmOn(bool on)
{
	/* USER_IMPLEMENTATION_BEGIN */
	// 이 함수를 호출하기 전 또는 후에 DB 파일에 설정 정보를 쓰도록 했다면 여기는 그냥 넘어가세요.
	// 자신의 DB 파일에 파라미터로 넘어온 on 값에 따라 설정값 저장.
	// 따로 저장해두지 않으면 앱을 재실행 할 때마다 BGM이 켜져 있게 됩니다.
	/* USER_IMPLEMENTATION_END */

	_isBgmOn = on;
	if (!_isBgmOn)
		stopAllBgms();
}

void CSoundManager::setEffectOn(bool on)
{
	/* USER_IMPLEMENTATION_BEGIN */
	// 이 함수를 호출하기 전 또는 후에 DB 파일에 설정 정보를 쓰도록 했다면 여기는 그냥 넘어가세요.
	// 자신의 DB 파일에 파라미터로 넘어온 on 값에 따라 설정값 저장.
	// 따로 저장해두지 않으면 앱을 재실행 할 때마다 효과음이 켜져 있게 됩니다.
	/* USER_IMPLEMENTATION_END */

	_isEffectOn = on;
	if (!_isEffectOn)
		stopAllEffects();
}

void CSoundManager::setBgmVolume(float volume)
{
	if (volume < 0.0f)
		volume = 0.0f;
	else if (volume > 1.0f)
		volume = 1.0f;

	/* USER_IMPLEMENTATION_BEGIN */
	// 이 함수를 호출하기 전 또는 후에 DB 파일에 설정 정보를 쓰도록 했다면 여기는 그냥 넘어가세요.
	// 자신의 DB 파일에 파라미터로 넘어온 volume 값에 따라 설정값 저장.
	// 따로 저장해두지 않으면 앱을 재실행 할 때마다 BGM의 볼륨이 최대로 설정됩니다.
	/* USER_IMPLEMENTATION_END */

	_bgmVolume = volume;
	for (auto & it : _bgmAudioIdMMap)
		AudioEngine::setVolume(it.second, _bgmVolume);
}

void CSoundManager::setEffectVolume(float volume)
{
	if (volume < 0.0f)
		volume = 0.0f;
	else if (volume > 1.0f)
		volume = 1.0f;

	/* USER_IMPLEMENTATION_BEGIN */
	// 이 함수를 호출하기 전 또는 후에 DB 파일에 설정 정보를 쓰도록 했다면 여기는 그냥 넘어가세요.
	// 자신의 DB 파일에 파라미터로 넘어온 volume 값에 따라 설정값 저장.
	// 따로 저장해두지 않으면 앱을 재실행 할 때마다 BGM의 볼륨이 최대로 설정됩니다.
	/* USER_IMPLEMENTATION_END */

	_effectVolume = volume;
	for (auto & it : _effectAudioIdMMap)
		AudioEngine::setVolume(it.second, _effectVolume);
}

void CSoundManager::preloadAudioFile(const std::string & filePath)
{
	auto it = _preloadedAudioFileSet.find(filePath);
	if (it != _preloadedAudioFileSet.end())
		return;

	_preloadedAudioFileSet.insert(filePath);

	AudioEngine::preload(filePath);
}

void CSoundManager::preloadAudioFiles(const std::vector<std::string> & filePathVec)
{
	for (auto it : filePathVec)
		preloadAudioFile(it);
}

void CSoundManager::uncacheAudioFile(const std::string & filePath)
{
	if (filePath == "")
		return;

	stopBgm(filePath);
	stopEffect(filePath);
	AudioEngine::uncache(filePath);
	auto it = _preloadedAudioFileSet.find(filePath);
	if (it != _preloadedAudioFileSet.end())
		_preloadedAudioFileSet.erase(it);	
}

void CSoundManager::uncacheAllAudioFiles()
{
	stopAllBgms();
	stopAllEffects();
	AudioEngine::uncacheAll();
	_preloadedAudioFileSet.clear();
}

void CSoundManager::bgmPlayFinishedCallback(int audioId, const std::string & filePath)
{
	AUTO_LOCK(_mutexForBgm);

	auto callbackIt = _bgmFinishCallbackMap.find(audioId);
	if (callbackIt != _bgmFinishCallbackMap.end())
	{
		if (filePath != "")
			callbackIt->second(audioId, filePath);
		_bgmFinishCallbackMap.erase(callbackIt);
	}

	auto hashIt = _bgmPathHashMap.find(audioId);
	if (hashIt != _bgmPathHashMap.end())
	{
		auto range = _bgmAudioIdMMap.equal_range(hashIt->second);
		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second == audioId)
			{
				_bgmAudioIdMMap.erase(it);
				break;
			}
		}

		_bgmPathHashMap.erase(hashIt);
	}
}

void CSoundManager::effectPlayFinishedCallback(int audioId, const std::string & filePath)
{
	AUTO_LOCK(_mutexForEffect);

	auto callbackIt = _effectFinishCallbackMap.find(audioId);
	if (callbackIt != _effectFinishCallbackMap.end())
	{
		if (filePath != "")
			callbackIt->second(audioId, filePath);
		_effectFinishCallbackMap.erase(callbackIt);
	}

	auto hashIt = _effectPathHashMap.find(audioId);
	if (hashIt != _effectPathHashMap.end())
	{
		auto range = _effectAudioIdMMap.equal_range(hashIt->second);
		for (auto it = range.first; it != range.second; ++it)
		{
			if (it->second == audioId)
			{
				_effectAudioIdMMap.erase(it);
				break;
			}
		}

		_effectPathHashMap.erase(hashIt);
	}
}
