#include "pagefetcher.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>


using json = nlohmann::json;



// Receives data from curl response and stores it in a string.
static size_t curlWriteCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp
) {
    size_t totalSize = size * nmemb;

    std::string* out = static_cast<std::string*>(userp);

    out->append(static_cast<char*>(contents), totalSize);

    return totalSize;
}




PageFetcher::PageFetcher(
    const std::string& adapterBaseUrl,
    long timeoutMs,
    Logger* logger
)
    : adapterBaseUrl(adapterBaseUrl),
      timeoutMs(timeoutMs),
      logger(logger)
{

    // Initialize curl for HTTP communication.
    curl_global_init(CURL_GLOBAL_DEFAULT);
}




PageFetcher::~PageFetcher() {

    // Cleanup curl resources.
    curl_global_cleanup();

}





// Sends HTTP POST request to Browser Adapter.
std::string PageFetcher::postJson(
    const std::string& path,
    const std::string& jsonBody
) const {

    CURL* curl = curl_easy_init();

    if (!curl) {
        throw PageFetcherException(
            "PageFetcher: failed to initialize curl handle"
        );
    }



    std::string url = adapterBaseUrl + path;

    std::string responseBody;



    // Request header.
    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );



    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(
        curl,
        CURLOPT_POSTFIELDS,
        jsonBody.c_str()
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        curlWriteCallback
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &responseBody
    );

    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT_MS,
        timeoutMs
    );



    CURLcode res = curl_easy_perform(curl);



    long httpStatus = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &httpStatus
    );



    curl_slist_free_all(headers);

    curl_easy_cleanup(curl);




    // Curl-level errors.
    if (res != CURLE_OK) {

        throw PageFetcherException(
            "PageFetcher: request to Browser Adapter failed: " +
            std::string(curl_easy_strerror(res))
        );

    }



    // HTTP response validation.
    if (httpStatus != 200) {

        throw PageFetcherException(
            "PageFetcher: Browser Adapter returned unexpected HTTP status " +
            std::to_string(httpStatus)
        );

    }



    return responseBody;
}







// Fetch rendered HTML from Browser Adapter.
std::string PageFetcher::fetch(const std::string& url) const {

    if (logger) {
        logger->debug("PageFetcher: fetching " + url);
    }



    json requestBody;

    requestBody["url"] = url;



    std::string responseBody =
        postJson("/fetch", requestBody.dump());



    json responseJson;


    try {

        responseJson = json::parse(responseBody);

    }
    catch (const json::parse_error& e) {

        throw PageFetcherException(
            "PageFetcher: Browser Adapter returned invalid JSON: " +
            std::string(e.what())
        );

    }




    // Check response format.
    if (!responseJson.contains("success") ||
        !responseJson["success"].is_boolean()) {

        throw PageFetcherException(
            "PageFetcher: Browser Adapter response missing 'success' field"
        );

    }




    bool success = responseJson["success"].get<bool>();



    if (!success) {

        std::string errorMsg =
            responseJson.contains("error")
            ? responseJson["error"].get<std::string>()
            : "unknown error";



        if (logger) {

            logger->warn(
                "PageFetcher: Browser Adapter reported failure for " +
                url + ": " + errorMsg
            );

        }



        throw PageFetcherException(
            "PageFetcher: fetch failed for '" +
            url + "': " + errorMsg
        );

    }




    if (!responseJson.contains("html") ||
        !responseJson["html"].is_string()) {

        throw PageFetcherException(
            "PageFetcher: Browser Adapter response missing 'html' field"
        );

    }




    if (logger) {

        logger->info(
            "PageFetcher: successfully fetched " + url
        );

    }



    return responseJson["html"].get<std::string>();

}







// Checks if Browser Adapter is running.
bool PageFetcher::isAdapterReachable() const {

    CURL* curl = curl_easy_init();

    if (!curl) {
        return false;
    }



    std::string url = adapterBaseUrl + "/health";

    std::string responseBody;



    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url.c_str()
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        curlWriteCallback
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &responseBody
    );

    // Health check uses a smaller timeout.
    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT_MS,
        5000L
    );



    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);



    if (res != CURLE_OK) {
        return false;
    }



    try {

        json responseJson = json::parse(responseBody);

        return responseJson.contains("status") &&
               responseJson["status"] == "ok";

    }
    catch (...) {

        return false;

    }
}