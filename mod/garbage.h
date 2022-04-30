#pragma once
#if 0
//GBK转UTF8
static string GbkToUtf8(const char* src_str) {
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}
//UTF8转GBK
static string Utf8ToGbk(const char* src_str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
//转宽字符
static wstring ToWstring(string_view s) {
	string curlLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const char* _Source = s.data();
	size_t _Dsize = s.size() + 1;

	wchar_t* _Dest = new wchar_t[_Dsize];
	size_t i;
	mbstowcs_s(&i, _Dest, _Dsize, _Source, s.size());
	wstring result = _Dest;
	delete[] _Dest;
	setlocale(LC_ALL, curlLocale.c_str());
	return result;
}
//访问url
static Json AccessUrlForJson(const wchar_t* url) {
	string data;
	//非空终止
	char buffer[BLOCK_SIZE];

	HINTERNET hSession = InternetOpen(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hSession)
		return nullptr;
	HINTERNET handle2 = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (!handle2)
		return nullptr;
	DWORD size = 0;
	do {
		InternetReadFile(handle2, buffer, BLOCK_SIZE, &size);
		data.append(buffer, size);
	}
	while (size);
	InternetCloseHandle(handle2);
	InternetCloseHandle(hSession);
	return StrToJson(data);
}
//访问url
static void AccessUrlForFile(const wchar_t* url, string_view filename) {
	//非空终止
	char buffer[BLOCK_SIZE];

	HINTERNET hSession = InternetOpen(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hSession)
		return;
	HINTERNET handle2 = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (!handle2)
		return;
	DWORD total = 0;
	DWORD size = 0;
	ofstream file(filename.data(), ios::out | ios::binary);
	do {
		total += size;
		cout << "Downloading " << filename << "... " << total << "bytes\r";
		InternetReadFile(handle2, buffer, BLOCK_SIZE, &size);
		file.write(buffer, size);
	}
	while (size);
	InternetCloseHandle(handle2);
	InternetCloseHandle(hSession);
}
//检查版本
static void CheckPluginVersion() {
	if (!fs::exists(BAT_PATH))
		return;
	cout << "[BDSpyrunner] Checking plugin version..." << endl;
	Json info = AccessUrlForJson(L"https://api.github.com/repos/twoone-3/BDSpyrunner/releases/latest");
	if (info["tag_name"] == PYR_VERSION) {
		cout << "[BDSpyrunner] Your plugin version is latest." << endl;
		return;
	}
	cout << "[BDSpyrunner] Your plugin version isn't latest, auto downloading..." << endl;
	for (auto& asset : info["assets"]) {
		string download_url = asset["browser_download_url"];
		download_url.replace(8, 10, "hub.fastgit.org");
		AccessUrlForFile(ToWstring(download_url).c_str(), CACHE_PATH + string(asset["name"]));
		cout << asset["name"] << " was downloaded successfully, size: " << asset["size"] << endl;
	}
	cout << "[BDSpyrunner] The new version has been downloaded to plugins/download, please restart the server to replace it" << endl;
	system("start /min " BAT_PATH);
	exit(0);
}
#endif
