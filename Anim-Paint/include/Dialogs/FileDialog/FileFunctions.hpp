#pragma once
#include <filesystem>

std::filesystem::path resolve_lnk(const std::filesystem::path& lnkPath);
bool isExtension(const std::filesystem::path& p, std::wstring extension);
std::filesystem::path getPath(std::filesystem::path p);
std::wstring to_lower_w(std::wstring s);
bool sortkey(std::filesystem::path a, std::filesystem::path b);
bool onlyWhitespace(const std::wstring& s);
bool isDrive(const std::wstring& path);
bool hasChildren(std::filesystem::path& p);

enum class FileType { Empty, File, Drive, Directory };
