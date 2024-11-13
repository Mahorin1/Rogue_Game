

    #include <audio.hpp>
    #include <vector>
    #include <set>
    #include <algorithm>

    using namespace sf;
    using namespace std;

    #define MAX_SOUND 200
    
    static vector<Sound> LstSound;
    static set<int> FreeIndices;
    static int SoundLimit;



    void InitSoundVec(int limit) {
        SoundLimit = (limit > MAX_SOUND) ? MAX_SOUND : limit;
        CleanSoundVec();
        LstSound.resize(SoundLimit);
        LstSound.shrink_to_fit();
    }

    void CleanSoundVec() {
        for (Sound& s : LstSound) { s.stop(); }
        FreeIndices.clear();
        for (int i = 0; i < SoundLimit; ++i) {
            FreeIndices.insert(i);
        }
    }

    void PlaySoundVec(SoundBuffer* sb, float volume, float pitch) {
        if (volume > 0.5f && !FreeIndices.empty()) {
            Sound& snd = LstSound[*FreeIndices.begin()];
            FreeIndices.erase(FreeIndices.begin());
            snd.setBuffer(*sb);
            snd.setPitch(max(pitch, 0.1f));
            snd.setVolume(volume);
            snd.play();
        }
    }

    void UpdateSoundVec() {
        for (int i = 0; i < SoundLimit; ++i) {
            if (LstSound[i].getStatus() == SoundSource::Stopped && 
                FreeIndices.find(i) == FreeIndices.end()) {
                FreeIndices.insert(i);
            }
        }
    }

    int NbSoundVec() {
        return count_if(LstSound.begin(), LstSound.end(),
            [](const Sound& s) {
                return s.getStatus() != SoundSource::Stopped;
            });
    }


