#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include "Utils.hpp"
#include "sha256.hpp"

enum UserRole {
	Free,
	Premium,
	Admin
};

struct User {
	std::string username;
	std::string password;
	UserRole role;

	bool operator==(const User& other) const {
		return username == other.username;
	}
};

class Auth
{
private:
	User actualUser;
	void CreateDB(std::string path);
	std::vector<User> getUsers();
	UserRole getRoleFromString(std::string roleString);
	bool LogIn(std::string username, std::string password);
	bool Register(std::string username, std::string password);
public:
	Auth();
	~Auth();
	bool InteractiveLogIn(std::string username);
	bool InteractiveRegister(std::string username);
	User getUserInfo();
	bool loggedIn = false;
};

