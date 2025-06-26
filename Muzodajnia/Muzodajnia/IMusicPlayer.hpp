#include <vector>
#include <string>
#include <cpr/cpr.h>			// From libcpr (HTTP client)
#include <json/json.h>			// From JsonCpp

struct Song {
	std::string name;
	std::string author;
	std::string link;

	Json::Value toJson() const {
		Json::Value val;
		val["name"] = name;
		val["author"] = author;
		val["link"] = link;
		return val;
	}

	static Song fromJson(const Json::Value& val) {
		Song song;
		song.name = val["name"].asString();
		song.author = val["author"].asString();
		song.link = val["link"].asString();
		return song;
	}

	friend std::ostream& operator<<(std::ostream& os, const Song& song);
};

class IMusicPlayer {
public:
    virtual void Play(int songId) = 0;
    virtual void LoadSongs(std::vector<Song> songs) = 0;
    virtual std::vector<Song> GetLastSongs() = 0;
    virtual ~IMusicPlayer() = default;
};