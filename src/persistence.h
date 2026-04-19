#pragma once
#include <string>

using namespace std;

// Save current in-memory index to folderPath
void saveIndex(const string& folderPath);

// Load index from folderPath into global structures
void loadIndex(const string& folderPath);