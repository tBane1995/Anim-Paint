#include "Dialogs/FileDialog/FileFunctions.hpp" 
#include <cstdlib>
#include <shobjidl.h>
#include <cwctype>

std::filesystem::path resolve_lnk(const std::filesystem::path& lnkPath) {
	std::filesystem::path result;

	// COINIT_APARTMENTTHREADED jest OK do prostych wywołań Shell
	HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	bool comInit = SUCCEEDED(hrCo);

	IShellLinkW* psl = nullptr;
	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&psl)))) {
		IPersistFile* ppf = nullptr;
		if (SUCCEEDED(psl->QueryInterface(IID_PPV_ARGS(&ppf)))) {
			if (SUCCEEDED(ppf->Load(lnkPath.c_str(), STGM_READ))) {
				WCHAR szTarget[MAX_PATH] = {};
				WIN32_FIND_DATAW wfd{};
				// SLGP_UNCPRIORITY -> preferuj ścieżkę UNC, ale SLGP_RAWPATH też jest ok
				if (SUCCEEDED(psl->GetPath(szTarget, MAX_PATH, &wfd, SLGP_UNCPRIORITY))) {
					result = szTarget;
				}
			}
			ppf->Release();
		}
		psl->Release();
	}

	if (comInit) CoUninitialize();
	return result; // może być pusty, jeśli nie udało się rozwiązać
}


bool isExtension(const std::filesystem::path& p, std::wstring extension) {
	std::wstring ext = p.extension().wstring();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
	return ext == extension;
}

std::filesystem::path getPath(std::filesystem::path p) {
	if (isExtension(p, L".lnk")) {
		return resolve_lnk(p); // może zwrócić pustą ścieżkę
	}
	return p;
}

std::wstring to_lower_w(std::wstring s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
	return s;
}

bool sortkey(std::filesystem::path a, std::filesystem::path b)
{
	std::error_code ea, eb;
	bool ad = std::filesystem::is_directory(a, ea);
	bool bd = std::filesystem::is_directory(b, eb);
	if (ea) ad = false; // w razie błędu traktuj jak plik
	if (eb) bd = false;

	// katalogi najpierw
	if (ad != bd) return ad;

	// porównanie nazw case-insensitive (na wstring!)
	std::wstring na = to_lower_w(a.filename().wstring());
	std::wstring nb = to_lower_w(b.filename().wstring());
	if (na != nb) return na < nb;

	// tie-breaker żeby utrzymać ścisłe porządki (ważne dla std::sort)
	return a.native() < b.native();
}

bool onlyWhitespace(const std::wstring& s) {
	return std::all_of(s.begin(), s.end(),
		[](wchar_t ch) { return std::iswspace(ch) != 0; });
}

bool isDrive(const std::wstring& path) {
	UINT type = GetDriveTypeW(path.c_str());
	return type != DRIVE_NO_ROOT_DIR && type != DRIVE_UNKNOWN;
}

bool hasChildren(std::filesystem::path& p) {

	std::error_code ec;
	if (!std::filesystem::exists(p, ec) || !std::filesystem::is_directory(p, ec))
		return false;

	auto opts = std::filesystem::directory_options::skip_permission_denied;
	for (std::filesystem::directory_iterator it(p, opts, ec), end; it != end; it.increment(ec)) {

		if (ec) { ec.clear(); continue; }

		if (it->is_directory())
			return true;
	}
	return false;
}
