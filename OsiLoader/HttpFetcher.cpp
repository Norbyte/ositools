#include "stdafx.h"
#include "HttpFetcher.h"
#include <sstream>
#include <iomanip>


HttpFetcher::HttpFetcher(wchar_t const * host)
{
	session_ = WinHttpOpen(L"OsiLoader/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (session_ == NULL) {
		LogError("WinHttpOpen");
		return;
	}

	WinHttpSetTimeouts(session_, 3000, 3000, 3000, 3000);

	httpSession_ = WinHttpConnect(session_, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (httpSession_ == NULL) {
		LogError("WinHttpConnect");
		return;
	}
}

HttpFetcher::~HttpFetcher()
{
	if (httpSession_ != NULL) WinHttpCloseHandle(httpSession_);
	if (session_ != NULL) WinHttpCloseHandle(session_);
}


void HttpFetcher::LogError(char const * activity)
{
	auto lastError = ::GetLastError();
	std::stringstream ss;
	ss << activity << " returned error 0x" << std::hex << std::setw(8) << std::setfill('0')
		<< lastError << ": " << GetLastErrorString(lastError);
	lastError_ = ss.str();
}

bool HttpFetcher::Fetch(wchar_t const * path, std::vector<uint8_t> & response)
{
	if (httpSession_ == NULL) {
		return false;
	}

	auto request = WinHttpOpenRequest(httpSession_, L"GET", path, NULL,
		WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (request == NULL) {
		LogError("WinHttpOpenRequest for content fetch");
		return false;
	}

	bool result = false;
	if (SendRequest(request, path)) {
		result = FetchBody(request, response);
	}

	WinHttpCloseHandle(request);
	return result;
}

bool HttpFetcher::FetchETag(wchar_t const * path, std::string & etag)
{
	if (httpSession_ == NULL) {
		return false;
	}

	auto request = WinHttpOpenRequest(httpSession_, L"HEAD", path, NULL,
		WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
	if (request == NULL) {
		LogError("WinHttpOpenRequest for ETag fetch");
		return false;
	}

	bool result = false;
	if (SendRequest(request, path)) {
		result = FetchETag(request, etag);
	}

	WinHttpCloseHandle(request);
	return result;
}

bool HttpFetcher::SendRequest(HINTERNET request, wchar_t const * path)
{
	auto sent = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, -1l,
		WINHTTP_NO_REQUEST_DATA, 0, 0, NULL);
	if (sent != TRUE) {
		LogError("WinHttpSendRequest");
		return false;
	}

	auto recvdHeaders = WinHttpReceiveResponse(request, NULL);
	if (recvdHeaders != TRUE) {
		LogError("WinHttpReceiveResponse");
		return false;
	}

	wchar_t statusCode[4];
	DWORD statusCodeLength = sizeof(statusCode);
	auto gotStatus = WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE,
		WINHTTP_HEADER_NAME_BY_INDEX, statusCode, &statusCodeLength, 0);
	if (gotStatus != TRUE) {
		LogError("WinHttpQueryHeaders for status code");
		return false;
	}

	if (wcscmp(statusCode, L"200") != 0) {
		lastError_ = "Server returned status code ";
		lastError_ += ToUTF8(statusCode);
		return false;
	}

	return true;
}

bool HttpFetcher::FetchBody(HINTERNET request, std::vector<uint8_t> & response)
{
	wchar_t contentLengthStr[32];
	DWORD contentLengthLength = sizeof(contentLengthStr);
	auto gotLength = WinHttpQueryHeaders(request, WINHTTP_QUERY_CONTENT_LENGTH,
		NULL, contentLengthStr, &contentLengthLength, 0);
	if (gotLength != TRUE) {
		LogError("WinHttpQueryHeaders for length");
		return false;
	}

	auto contentLength = (uint32_t)std::stoi(contentLengthStr);
	if (contentLength > 0x1000000) {
		lastError_ = "HTTP response body too long";
		return false;
	}

	response.resize(contentLength);
	DWORD totalBytesRead = 0;

	while (totalBytesRead < contentLength) {
		DWORD bytesToRead = contentLength - totalBytesRead;
		DWORD bytesRead = 0;
		auto readBytes = WinHttpReadData(request, response.data() + totalBytesRead,
			bytesToRead, &bytesRead);
		if (readBytes != TRUE) {
			LogError("WinHttpReadData");
			return false;
		}

		totalBytesRead += bytesRead;
	}

	return true;
}

bool HttpFetcher::FetchETag(HINTERNET request, std::string & etag)
{
	wchar_t etagStr[0x100];
	DWORD etagLength = sizeof(etagStr);
	auto gotETag = WinHttpQueryHeaders(request, WINHTTP_QUERY_ETAG,
		NULL, etagStr, &etagLength, 0);
	if (gotETag != TRUE) {
		LogError("WinHttpQueryHeaders for ETag");
		return false;
	}

	etag = ToUTF8(etagStr);

	return true;
}
