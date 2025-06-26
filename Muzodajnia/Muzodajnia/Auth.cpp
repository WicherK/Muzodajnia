#include "Auth.hpp"

Auth::Auth() {
}

Auth::~Auth() {
}

void Auth::CreateDB(std::string path)
{
    // Check if file exists
    if (std::filesystem::exists(std::filesystem::current_path() / path)) {
        return;
    }

    //Create path if it doesn't exist
    std::filesystem::create_directories(
        std::filesystem::current_path() / std::filesystem::path(path).parent_path()
    );

    // Try to create the file
    std::ofstream dbFile(std::filesystem::current_path() / path);

    if (dbFile.is_open()) {
        dbFile.close();
        std::cout << "Database created at: " << path << std::endl;
    }
    else {
        std::cerr << "Failed to create database at: " << path << std::endl;
    }
}

UserRole Auth::getRoleFromString(std::string roleString)
{
    if (roleString == "free") return UserRole::Free;
    if (roleString == "premium") return UserRole::Premium;
    if (roleString == "admin") return UserRole::Admin;
    return UserRole::Free;
}

std::vector<User> Auth::getUsers()
{
    std::ifstream file("db/users.db");
    std::vector<User> users;
    User userFromFile;

    // String to store each line of the file.
    std::string line;
    std::vector<std::string> splittedLine;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            splittedLine = Utils::split_sentence(line);
            userFromFile.username = splittedLine[0];
            userFromFile.password = splittedLine[1];
            userFromFile.role = getRoleFromString( (splittedLine.size() > 2) ? splittedLine[2] : "free");
            users.push_back(userFromFile);
        }

        file.close();
    }
    else {
        std::cerr << "Unable to open file!" << std::endl;
    }
    return users;
}

bool Auth::LogIn(std::string username, std::string password)
{
    CreateDB("db/users.db"); //First check and create DB if it doens't exist

    std::vector<User> users = getUsers(); //Get all users from DB

    //Check if user exists in DB
    for (const auto userFromFile : users)
    {
        if (userFromFile.username == username)
        {
            if (userFromFile.password == sha256(password))
            {
                loggedIn = true;
                actualUser = userFromFile;
                return true;
            }
        }
    }

	return false;
}

bool Auth::InteractiveLogIn(std::string username)
{
    std::string password = Utils::getHiddenInput("Password: ");

    if (LogIn(username, password))
    {
        std::cout << "Successfully logged in!" << std::endl;
        return true;
    }
    
    std::cout << "Wrong username or password." << std::endl;
    return false;
}

bool Auth::Register(std::string username, std::string password)
{
	CreateDB("db/users.db"); //First check and create DB if it doens't exist

    std::vector<User> users = getUsers(); //Get all users from DB

    //Check if user exists in DB
    for (const auto userFromFile : users)
    {
        if (userFromFile.username == username)
            return false;
    }

    //Create user
    std::ofstream myfile("db/users.db", std::ios::app);
    myfile << username << " " << sha256(password) << " " << "Free" << "\n";
    myfile.close();

    return true;
}

bool Auth::InteractiveRegister(std::string username)
{
    std::string password = Utils::getHiddenInput("Password: ");
    std::string confirmPassword = Utils::getHiddenInput("Confirm password: ");

    if (password == confirmPassword)
    {
        if (Register(username, password)) // If registered successfully
        {
            std::cout << "User successfully created! Now you can log in using 'login'." << std::endl;
            return true;
        }
        else
        {
            std::cout << "User already exists." << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "Password must be the same." << std::endl;
        return false;
    }
}

User Auth::getUserInfo()
{
    return actualUser;
}
