#pragma once

#include <winhttp.h>
#include <vector>
#include <string>

class HttpFetcher
{
public:
	HttpFetcher(wchar_t const * host);
	~HttpFetcher();

	bool Fetch(wchar_t const * path, std::vector<uint8_t> & response);
	bool FetchETag(wchar_t const * path, std::string & etag);

private:
	HINTERNET session_{ NULL };
	HINTERNET httpSession_{ NULL };

	bool SendRequest(HINTERNET request, wchar_t const * path);

	bool FetchBody(HINTERNET request, std::vector<uint8_t> & response);
	bool FetchETag(HINTERNET request, std::string & etag);
};
