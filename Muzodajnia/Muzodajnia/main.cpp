#pragma once
#include "Auth.hpp"
#include "MusicPlayer.hpp"
#include "Utils.hpp"

int main()
{
    std::string prompt;
    std::vector<std::string> splittedPrompt;

    Auth authorization;
    MusicPlayer musicPlayer;

    Utils::WelcomeMessage();

    // 'Logging in' loop
    while (!authorization.loggedIn)
    {
        std::getline(std::cin, prompt); // Get prompt from user

        splittedPrompt = Utils::split_sentence(prompt); // Split prompt on parts

        if (splittedPrompt.empty()) {
            continue;
        }

        switch (Utils::translatePrompt(splittedPrompt[0]))
        {
        case Utils::Commands::Register: {
            if (splittedPrompt.size() == 1) // No arguments passed after 'register' command
                Utils::RegisterHelpMessage();
            else
                authorization.InteractiveRegister(splittedPrompt[1]); // Pass username from prompt to function that handles registering
            
            break;
        }
        case Utils::Commands::LogIn: {
            if (splittedPrompt.size() == 1)
                Utils::LoginHelpMessage();
            else
                authorization.InteractiveLogIn(splittedPrompt[1]); // Pass username from prompt to function that handles loggin in

            break;
        }
        case Utils::Commands::Help: {
            Utils::LoginScreenHelpMessage();
            break;
        }
        case Utils::Commands::Exit: {
            // Save last songs at the end of program for user
            musicPlayer.SaveLastSongs(authorization.getUserInfo());
            return 0;
        }
        default: {
            std::cout << "Unknown command!\n";
            break;
        }
        }
    }

    // Load last songs from user that logged in
    musicPlayer.LoadLastSongs(authorization.getUserInfo());

    // 'Music player' loop
    while (authorization.loggedIn)
    {
        std::getline(std::cin, prompt); // Get prompt from user

        splittedPrompt = Utils::split_sentence(prompt); // Split prompt on parts

        if (splittedPrompt.empty()) {
            continue;
        }

        switch (Utils::translatePrompt(splittedPrompt[0]))
        {
        case Utils::Commands::Play: {
            if (splittedPrompt.size() == 1)
                std::cout << "Invalid ID of the song." << std::endl;
            else
            {
                try {
                    int songId = std::stoi(splittedPrompt[1]);
                    musicPlayer.Play(songId);
                }
                catch (const std::invalid_argument& e) {
                    std::cout << "\nInvalid number format.\n";
                }
                catch (const std::out_of_range& e) {
                    std::cout << "\nNumber out of range.\n";
                }
            }

            break;
        }
        case Utils::Commands::Help: {
            Utils::MusicPlayerHelpMessage();
            break;
        }
        case Utils::Commands::WeekPopular: {
            std::cout << "List of most popular songs this week:\n";

            Json::Value results = musicPlayer.GetSongs("https://api.jamendo.com/v3.0/tracks?client_id=07bfe5ca&audioformat=ogg&order=popularity_week&limit=20");
            musicPlayer.LoadSongs(results); // Load songs to list of actual songs that can be played

            // List songs
            musicPlayer.ListLoadedSongs();

            break;
        }
        case Utils::Commands::MonthPopular: {
            std::cout << "List of most popular songs this month:\n";

            Json::Value results = musicPlayer.GetSongs("https://api.jamendo.com/v3.0/tracks?client_id=07bfe5ca&audioformat=ogg&order=popularity_month&limit=20");
            musicPlayer.LoadSongs(results); // Load songs to list of actual songs that can be played

            // List songs
            musicPlayer.ListLoadedSongs();

            break;
        }
        case Utils::Commands::LastSongs: {
            std::vector<Song> lastSongs = musicPlayer.GetLastSongs();

            if (lastSongs.size() <= 0)
            {
                std::cout << "There are no songs you recently listened to..." << std::endl;
                break;
            }

            std::cout << "List of last songs you listened to:\n";
            musicPlayer.LoadSongs(lastSongs);

            // List songs
            musicPlayer.ListLoadedSongs();

            break;
        }
        case Utils::Commands::Search: {
            if (splittedPrompt.size() < 2)
            {
                std::cout << "What do you want to search?" << std::endl;
                break;
            }

            std::string combinedQuery;

            for (int i = 1; i < splittedPrompt.size(); i++)
            {
                combinedQuery += splittedPrompt[i] + ((i != splittedPrompt.size() - 1) ? " " : "");
            }

            std::cout << "\nSearch results for '" << combinedQuery <<"':\n";

            Json::Value results = musicPlayer.GetSongs("https://api.jamendo.com/v3.0/tracks?client_id=07bfe5ca&limit=200&namesearch=" + Utils::UrlEncode(combinedQuery));
            musicPlayer.LoadSongs(results); // Load songs to list of actual songs that can be played

            // List songs
            musicPlayer.ListLoadedSongs();

            break;
        }
        case Utils::Commands::Playlists: {
            musicPlayer.ListPlaylists(authorization.getUserInfo());
            break;
        }
        case Utils::Commands::Playlist: {
            if (splittedPrompt.size() < 2)
            {
                std::cout << "Wrog number of paramters. Use 'help'." << std::endl;
                break;
            }

            musicPlayer.ListPlaylistSongs(authorization.getUserInfo(), splittedPrompt[1]);
            break;
        }
        case Utils::Commands::Add: {
            if (splittedPrompt.size() > 2) {
                try {
                    int songId = std::stoi(splittedPrompt[2]);
                    musicPlayer.AddSongToPlaylist(authorization.getUserInfo(), songId, splittedPrompt[2]);
                }
                catch (const std::invalid_argument& e) {
                    std::cout << "\nInvalid number format.\n";
                }
                catch (const std::out_of_range& e) {
                    std::cout << "\nNumber out of range.\n";
                }
            }
            else {
                std::cout << "\nMissing argument for song ID.\n";
            }

            break;
        }
        case Utils::Commands::DeleteSong: {
            if (splittedPrompt.size() > 2) {
                try {
                    int songId = std::stoi(splittedPrompt[2]);
                    musicPlayer.DeleteSongFromPlaylist(authorization.getUserInfo(), songId, splittedPrompt[1]);
                }
                catch (const std::invalid_argument& e) {
                    std::cout << "\nInvalid number format.\n";
                }
                catch (const std::out_of_range& e) {
                    std::cout << "\nNumber out of range.\n";
                }
            }
            else {
                std::cout << "\nMissing argument for song ID.\n";
            }
            
            break;
        }
        case Utils::Commands::Create: {
            if (splittedPrompt.size() == 1)
            {
                std::cout << "Wrong number of parameters. Use 'help'." << std::endl;
                break;
            }

            musicPlayer.CreatePlaylist(authorization.getUserInfo(), splittedPrompt[1]);
            break;
        }
        case Utils::Commands::Delete: {
            if (splittedPrompt.size() != 1)
            {
                std::cout << "Wrong number of parameters. Use 'help'." << std::endl;
                break;
            }

            musicPlayer.DeletePlaylist(authorization.getUserInfo(), splittedPrompt[1]);
            break;
        }
        case Utils::Commands::Exit: {
            // Save last songs at the end of program for user
            musicPlayer.SaveLastSongs(authorization.getUserInfo());
            return 0;
        }
        default: {
            std::cout << "Unknown command! Try 'help'\n";
            break;
        }
        }
    }

    return 0;
}
