#include "MusicPlayer.hpp"

void MusicPlayer::PlayUrl(const char* url)
{
    system("cls"); // Clear layout

    BASS_Init(-1, 44100, 0, NULL, NULL); // Default init of device
    BASS_SetVolume(1); // Set volume for max

    HSTREAM stream = BASS_StreamCreateURL(url, 0, BASS_SAMPLE_MONO, NULL, 0); // Create stream from URL
    BASS_ChannelPlay(stream, false); // Play music

    // Make sure space is not pressed right after launching function
    while (GetAsyncKeyState(VK_SPACE) & 0x8000) 
    {
        Sleep(10);
    }

    QWORD len = BASS_ChannelGetLength(stream, BASS_POS_BYTE); // the length in bytes
    double maxTime = BASS_ChannelBytes2Seconds(stream, len); // the max length of stream in seconds
    
    QWORD songPosition;
    double currentTime;

    float volume = 1.0f;

    bool isPlaying = true;

    std::cout << "\nNow playing: " << actualSong.author << " - " << actualSong.name << "\n\n";
    
    while (true)
    {
        songPosition = BASS_ChannelGetPosition(stream, BASS_POS_BYTE); // Get current position of song
        currentTime = BASS_ChannelBytes2Seconds( stream, songPosition); // Convert bytes to seconds

        // Pause/play music
        if (GetAsyncKeyState(VK_SPACE) & 0x8001)
        {
            if (BASS_ChannelIsActive(stream) == BASS_ACTIVE_PLAYING)
            {
                BASS_ChannelPause(stream);
            }
            else
            {
                BASS_ChannelPlay(stream, false);
            }
        }
        
        if (GetAsyncKeyState(VK_LEFT) & 0x8001) // Go back
        {
            // Refresh currentTime before seeking
            songPosition = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
            currentTime = BASS_ChannelBytes2Seconds(stream, songPosition);

            currentTime = (currentTime - 1 < 0.0) ? 0.0 : currentTime - 1;
            songPosition = BASS_ChannelSeconds2Bytes(stream, currentTime);
            BASS_ChannelSetPosition(stream, songPosition, BASS_POS_BYTE);
        }
        
        if (GetAsyncKeyState(VK_RIGHT) & 0x8001) // Go further
        {
            // Refresh currentTime before seeking
            songPosition = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
            currentTime = BASS_ChannelBytes2Seconds(stream, songPosition);

            currentTime += 1;
            songPosition = BASS_ChannelSeconds2Bytes(stream,currentTime);
            BASS_ChannelSetPosition(stream, songPosition, BASS_POS_BYTE);
        }
        
        if (GetAsyncKeyState(VK_UP) & 0x8001) // Volume up
        {
            volume = (volume + 0.1 > 1.0) ? 1.0 : volume += 0.1;
            BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
        }
        
        if (GetAsyncKeyState(VK_DOWN) & 0x8001) // Volume down
        {
            volume = (volume - 0.1 < 0.0) ? 0.0 : volume -= 0.1;
            BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8001) // Exit music player
        {
            BASS_ChannelStop(stream);
            system("cls");
            break;
        }

        // Build progress bar
        const int barWidth = 30;
        int filled = static_cast<int>((currentTime / maxTime) * barWidth);
        std::string progressBar = "[";
        for (int i = 0; i < barWidth; ++i) {
            progressBar += (i < filled) ? "#" : " ";
        }
        progressBar += "]";

        // Output everything in one line
        std::cout << "\r"
            << "Time: " << formatTime(currentTime)
            << " / " << formatTime(maxTime)
            << "  " << progressBar
            << "  " << (BASS_ChannelIsActive(stream) == BASS_ACTIVE_PLAYING ? "[PLAYING]" : "[PAUSED]")
            << "  Volume: " << std::setw(3) << static_cast<int>(std::ceil(volume * 100)) << "%"
            << "    " << std::flush;

        Sleep(200);
    }
}

void MusicPlayer::Play(int songId)
{
    if (songsLoaded.size() == 0) // Vector with songs is empty
        std::cout << "\nNo songs loaded. Try first using 'search' 'weekpopular' etc. to load some songs.\n" << std::endl;
    else if (songId > songsLoaded.size())
        std::cout << "\nThere are no song with this ID.\n" << std::endl;
    else
        if (songId >= 0 && songId < songsLoaded.size()) {
            AddSongToLastSongs(songsLoaded[songId]);
            actualSong = songsLoaded[songId]; // Add song which is actually being played
            PlayUrl(songsLoaded[songId].link.c_str()); // Play it from URL
        }
        else {
            std::cout << "\nInvalid song index.\n";
        }

}

Json::Value MusicPlayer::GetSongs(std::string url)
{
    // Send HTTP GET request using cpr
    cpr::Response response = cpr::Get(cpr::Url(url));

    if (response.status_code != 200) {
        std::cerr << "\nRequest failed. Status code: " << response.status_code << '\n' << std::endl;
        return Json::Value(); // return empty JSON object
    }

    // Parse response using JsonCpp
    Json::CharReaderBuilder readerBuilder;
    Json::Value root;
    std::string errs;

    std::istringstream iss(response.text);
    if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
        std::cerr << "\nJSON parsing failed: " << errs << std::endl;
        return Json::Value();
    }

    // Return only the "results" array
    return root["results"];
}

std::vector<Song> MusicPlayer::GetLastSongs()
{
    return this->lastSongs;
}

void MusicPlayer::LoadSongs(Json::Value songs)
{
    Song song;

    this->songsLoaded.clear();

    for (const auto& track : songs) 
    {
        song.name = track["name"].asString();
        song.author = track["artist_name"].asString();
        song.link = track["audio"].asString();

        this->songsLoaded.push_back(song);
    }
}

void MusicPlayer::LoadSongs(std::vector<Song> songs)
{
    songsLoaded = songs;
}

void MusicPlayer::ListLoadedSongs()
{
    // List songs
    for (int i = 1; i < songsLoaded.size(); ++i) {
        std::cout << i << ". " << songsLoaded[i] << std::endl;
    }
}

void MusicPlayer::AddSongToLastSongs(Song song)
{
    for (int i = 0; i < lastSongs.size(); i++)
    {
        if (lastSongs[i].link == song.link)
            lastSongs.erase(lastSongs.begin() + i);
    }

    lastSongs.insert(lastSongs.begin(), song);
}

std::string MusicPlayer::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);

    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    std::string formatted;

    if (minutes < 10) formatted += "0";
    formatted += std::to_string(minutes);
    formatted += ":";

    if (secs < 10) formatted += "0";
    formatted += std::to_string(secs);

    return formatted;
}

void MusicPlayer::SaveLastSongs(User user)
{
    Json::Value root;

    for (Song songInList : lastSongs)
    {
        root.append(songInList.toJson());
    }

    std::string folderPath = "db/lastSongs";
    std::filesystem::create_directories(folderPath);
    std::ofstream file("db/lastSongs/" + user.username + ".json");
    file << root;
}

void MusicPlayer::LoadLastSongs(User user)
{
    std::string folderPath = "db/lastSongs";
    std::filesystem::create_directories(folderPath);

    std::ifstream file(folderPath + "/" + user.username + ".json");
    if (!file.is_open()) 
    {
        std::cout << "\nThere are no last songs." << '\n' << std::endl;
        return;
    }

    Json::Value root;
    file >> root;

    this->lastSongs.clear();

    for (Json::Value songJson : root)
    {
        this->lastSongs.push_back(Song::fromJson(songJson));
    }
}

void MusicPlayer::ListPlaylists(User user)
{
    std::string folderPath = "db/playlists";
    std::string filePath = folderPath + "/" + user.username + ".json";

    // Ensure playlist directory exists
    std::filesystem::create_directories(folderPath);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "\nThere are no playlists created.\n";
        return;
    }

    Json::Value root;
    file >> root;
    file.close();

    if (root.getMemberNames().empty()) {
        std::cout << "\nThere are no playlists created.\n";
        return;
    }

    std::cout << "\nYour playlists:\n";
    int index = 0;
    for (const auto& name : root.getMemberNames()) {
        std::cout << index++ << ". " << name << "\n";
    }
}

void MusicPlayer::ListPlaylistSongs(User user, std::string nameOfPlaylist)
{
    std::string filePath = "db/playlists/" + user.username + ".json";

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "\nNo playlists found for this user.\n";
        return;
    }

    Json::Value root;
    file >> root;
    file.close();

    // Check if playlist exists
    if (!root.isMember(nameOfPlaylist)) {
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" does not exist.\n";
        return;
    }

    Json::Value playlist = root[nameOfPlaylist];

    if (playlist.empty()) {
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" is empty.\n";
        return;
    }

    std::cout << "\nSongs in playlist \"" << nameOfPlaylist << "\":\n";
    this->songsLoaded.clear();
    for (int i = 0; i < playlist.size(); ++i) {
        Song song = Song::fromJson(playlist[i]);
        this->songsLoaded.push_back(song);
        std::cout << i << ". " << song.author << " - " << song.name << "\n";
    }
}

void MusicPlayer::DeleteSongFromPlaylist(User user, int songId, std::string nameOfPlaylist)
{
    // Ensure playlists folder exists
    std::string folderPath = "db/playlists";
    std::filesystem::create_directories(folderPath);

    std::string filePath = "db/playlists/" + user.username + ".json";

    // Read file with users playlist
    std::ifstream fileIn(filePath);
    if (!fileIn.is_open()) {
        std::cout << "\nNo playlists found for this user.\n";
        return;
    }

    Json::Value root;
    fileIn >> root;
    fileIn.close();

    // Check if playlist exists
    if (!root.isMember(nameOfPlaylist)) {
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" does not exist.\n";
        return;
    }

    Json::Value& playlist = root[nameOfPlaylist];

    // Make sure songId is not out of range
    if (songId < 0 || songId >= static_cast<int>(playlist.size())) {
        std::cout << "\nInvalid song ID.\n";
        return;
    }

    // Make new json and append there song but not append the one with id we want to delete
    Json::Value updatedPlaylist(Json::arrayValue);
    for (int i = 0; i < playlist.size(); ++i) {
        if (i != songId) {
            updatedPlaylist.append(playlist[i]);
        }
    }
    root[nameOfPlaylist] = updatedPlaylist;

    // Save modified json to file
    std::ofstream fileOut(filePath);
    if (fileOut.is_open()) {
        fileOut << root;
        fileOut.close();
        std::cout << "\nSong removed from playlist \"" << nameOfPlaylist << "\".\n";
    }
    else {
        std::cout << "\nFailed to write updated playlist file.\n";
    }
}


void MusicPlayer::CreatePlaylist(User user, std::string nameOfPlaylist)
{
    std::string folderPath = "db/playlists";
    std::string filePath = folderPath + "/" + user.username + ".json";

    // Ensure playlists folder exists
    std::filesystem::create_directories(folderPath);

    Json::Value root;

    // Load existing playlists
    std::ifstream fileIn(filePath);
    if (fileIn.is_open()) {
        fileIn >> root;
        fileIn.close();
    }

    // Check if playlist already exists
    if (root.isMember(nameOfPlaylist)) {
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" already exists.\n";
        return;
    }

    // Create an empty playlist
    root[nameOfPlaylist] = Json::arrayValue;

    // Save updated playlists
    std::ofstream fileOut(filePath);
    fileOut << root;
    fileOut.close();

    std::cout << "\nSuccessfully created playlist \"" << nameOfPlaylist << "\".\n";
}

void MusicPlayer::DeletePlaylist(User user, std::string nameOfPlaylist)
{
    std::string folderPath = "db/playlists";
    std::string filePath = folderPath + "/" + user.username + ".json";

    // Ensure playlists folder exists
    std::filesystem::create_directories(folderPath);

    Json::Value root;

    // Load existing playlists
    std::ifstream fileIn(filePath);
    if (fileIn.is_open()) {
        fileIn >> root;
        fileIn.close();
    }
    else {
        std::cout << "\nNo playlists found for this user.\n";
        return;
    }

    // Check if this playlist even exsits in list
    if (!root.isMember(nameOfPlaylist)) {
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" does not exist.\n";
        return;
    }

    // Remove playlist
    root.removeMember(nameOfPlaylist);

    // Write modified playlist to file
    std::ofstream fileOut(filePath);
    if (fileOut.is_open()) {
        fileOut << root;
        fileOut.close();
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" has been deleted.\n";
    }
    else {
        std::cout << "\nFailed to write updated playlist file.\n";
    }
}

void MusicPlayer::AddSongToPlaylist(User user, int songId, std::string nameOfPlaylist)
{
    if (songsLoaded.empty()) {
        std::cout << "\nNo songs loaded. Use 'search' or 'weekpopular' first.\n";
        return;
    }

    if (songId < 0 || songId >= songsLoaded.size()) {
        std::cout << "\nInvalid song ID.\n";
        return;
    }

    std::string filePath = "db/playlists/" + user.username + ".json";
    Json::Value root;

    // Load existing playlists if file exists
    std::ifstream fileIn(filePath);
    if (fileIn.is_open()) {
        fileIn >> root;
        fileIn.close();
    }

    // Check if playlist exists
    if (!root.isMember(nameOfPlaylist)) {
        std::cout << "\nPlaylist \"" << nameOfPlaylist << "\" does not exist.\n";
        return;
    }

    // Add song to playlist
    const Song& songToAdd = songsLoaded[songId];
    root[nameOfPlaylist].append(songToAdd.toJson());

    // Save updated JSON
    std::ofstream fileOut(filePath);
    fileOut << root;
    fileOut.close();

    std::cout << "\nAdded \"" << songToAdd.author << " - " << songToAdd.name
        << "\" to playlist \"" << nameOfPlaylist << "\".\n";
}
