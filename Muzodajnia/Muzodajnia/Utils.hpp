#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <conio.h>
#include <string>


class Utils {
public:
    static enum Commands
    {
        //Login screen
        Help,
        LogIn,
        Register,
        //Music player screen
        WeekPopular,
        MonthPopular,
        LastSongs,
        Search,
        Play,
        Playlist,
        Playlists,
        Add,
        Create,
        Delete,
        DeleteSong,
        //General
        Exit,
        Unkown
    };

    static inline Commands translatePrompt(std::string prompt)
    {
        std::transform(prompt.begin(), prompt.end(), prompt.begin(), ::tolower); // Make whole user prompt to lower case

	    if (prompt == "help") return Commands::Help;
	    if (prompt == "login") return Commands::LogIn;
	    if (prompt == "register") return Commands::Register;
	    if (prompt == "weekpopular") return Commands::WeekPopular;
	    if (prompt == "monthpopular") return Commands::MonthPopular;
	    if (prompt == "lastsongs") return Commands::LastSongs;
	    if (prompt == "search") return Commands::Search;
	    if (prompt == "play") return Commands::Play;
	    if (prompt == "playlist") return Commands::Playlist;
	    if (prompt == "playlists") return Commands::Playlists;
	    if (prompt == "add") return Commands::Add;
	    if (prompt == "create") return Commands::Create;
	    if (prompt == "delete") return Commands::Delete;
	    if (prompt == "deletesong") return Commands::DeleteSong;
	    if (prompt == "register") return Commands::Register;
	    if (prompt == "exit") return Commands::Exit;
	    return Commands::Unkown;
    }

    static inline std::vector<std::string> split_sentence(std::string sen)
    {
        // Create a stringstream object
        std::stringstream ss(sen);

        // Variable to hold each word
        std::string word;

        // Vector to store the words
        std::vector<std::string> words;

        // Extract words from the sentence
        while (ss >> word) {

            // Add the word to the vector
            remove_if(word.begin(), word.end(), isspace); //remove white spaces
            if(word != "")
                words.push_back(word);
        }

        return words;
    }

    static inline std::string getHiddenInput(const std::string& promptMessage)
    {
        std::string password;
        char ch;

        std::cout << promptMessage;

        while ((ch = _getch()) != '\r') { // Enter key
            if (ch == '\b') { // Backspace
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            }
            else if (ch >= 32 && ch <= 126) { // Printable characters
                password += ch;
                std::cout << '*';
            }
        }

        std::cout << std::endl;
        return password;
    }

    static inline void WelcomeMessage()
    {
        std::cout << "\n++======================================++" << std::endl;
        std::cout << "        Welcome to Muzodajnia!" << std::endl;
        std::cout << "   Your terminal-based music library" << std::endl;
        std::cout << "++======================================++" << std::endl;
        std::cout << "Type 'help' to see available commands.\n" << std::endl;
    }

    static inline void LoginScreenHelpMessage()
    {
        std::cout << "\nAvailable commands:\n" << std::endl;
        std::cout << "  help      - Show this help message" << std::endl;
        std::cout << "  login     - Log in to your account" << std::endl;
        std::cout << "  register  - Register a new user" << std::endl;
        std::cout << "  exit      - Exit the application" << std::endl;
        std::cout << "\nEnter one of the commands above and press Enter.\n" << std::endl;
    }

    static inline void MusicPlayerHelpMessage()
    {
        std::cout << "\n=== Music Command Help ===\n" << std::endl;
        std::cout << "  weekpopular    - Show and load the most popular songs this week" << std::endl;
        std::cout << "  monthpopular   - Show and load the most popular songs this month" << std::endl;
        std::cout << "  lastsongs      - Show and load the most recently added songs" << std::endl;
        std::cout << "  search <query> - Search for songs by title or artist and load them" << std::endl;
        std::cout << "  play <track_id>      - Play a song by its ID. To play a song you need to first use 'weekpopular' 'monthpopular' or 'search' to load some songs." << std::endl;
        std::cout << "  playlist <playlist_name>      - Show songs in playlist" << std::endl;
        std::cout << "  playlists      - Show your playlists" << std::endl;
        std::cout << "  add <track_id> <playlist_name>     - Add song to playlist" << std::endl;
        std::cout << "  create <playlist_name>      - Add song to playlist" << std::endl;
        std::cout << "  deletesong <playlist_name> <track_id>      - Delete song from playlist" << std::endl;
        std::cout << "  delete <playlist_name>      - Delete playlist" << std::endl;
        std::cout << "\nType a command followed by any required arguments and press Enter.\n" << std::endl;
        std::cout << "==============================\n" << std::endl;
    }

    //  LOGIN SCREEN COMMANDS
    static inline void LoginHelpMessage()
    {
        std::cout << "\n=== Login Command Help ===\n" << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  login <username>" << std::endl;
        std::cout << "\nDescription:" << std::endl;
        std::cout << "  Logs in to an existing user account." << std::endl;
        std::cout << "  You will be prompted to enter your password securely." << std::endl;
        std::cout << "\nExample:" << std::endl;
        std::cout << "  login john_doe" << std::endl;
        std::cout << "\n===========================\n" << std::endl;
    }

    static inline void RegisterHelpMessage()
    {
        std::cout << "\n=== Register Command Help ===\n" << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  register <username>" << std::endl;
        std::cout << "\nDescription:" << std::endl;
        std::cout << "  Registers a new user account." << std::endl;
        std::cout << "  You will be prompted to enter and confirm your password securely." << std::endl;
        std::cout << "\nExample:" << std::endl;
        std::cout << "  register john_doe" << std::endl;
        std::cout << "\nNotes:" << std::endl;
        std::cout << "  - Usernames must be unique." << std::endl;
        std::cout << "  - Passwords are hidden as you type." << std::endl;
        std::cout << "  - Both password entries must match." << std::endl;
        std::cout << "\n==============================\n" << std::endl;
    }

    static inline std::string UrlEncode(const std::string& value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : value) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            else {
                escaped << '%' << std::setw(2) << int((unsigned char)c);
            }
        }

        return escaped.str();
    }

};