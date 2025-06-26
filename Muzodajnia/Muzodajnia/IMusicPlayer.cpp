#include "IMusicPlayer.hpp"

std::ostream& operator<<(std::ostream& os, const Song& song)
{
    return os << "Track: " << song.name << " | Artist: " << song.author;
}
