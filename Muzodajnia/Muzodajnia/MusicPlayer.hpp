#pragma once
#include "IMusicPlayer.hpp"
#include <iostream>
#include <vector>
#include <bass.h>				// From BASS (playing music)
#include <filesystem>

#include <windows.h>			// For gettings keys
#include <limits>

#include "Auth.hpp"

class MusicPlayer : public IMusicPlayer
{
private:
	std::string formatTime(double seconds);
	std::vector<Song> songsLoaded;
	std::vector<Song> lastSongs;
	std::unordered_map<std::string, std::vector<Song>> playlists;
	Song actualSong;
	void PlayUrl(const char* url);
	void AddSongToLastSongs(Song song);

public:
	Json::Value GetSongs(std::string url);
	std::vector<Song> GetLastSongs();
	
	void LoadSongs(Json::Value songs); 
	void LoadSongs(std::vector<Song> songs);

	void ListLoadedSongs();
	
	void Play(int songId);

	void SaveLastSongs(User user);
	void LoadLastSongs(User user);

	void ListPlaylists(User user);
	void ListPlaylistSongs(User user, std::string nameOfPlaylist);
	void DeleteSongFromPlaylist(User user, int songId, std::string nameOfPlaylist);
	void CreatePlaylist(User user, std::string nameOfPlaylist);
	void DeletePlaylist(User user, std::string nameOfPlaylist);
	void AddSongToPlaylist(User user, int songId, std::string nameOfPlaylist);
};

