#include "pagefetcher.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

// kmp.h must come after curl and json includes.
// It pulls in Dynamicarray.h which has "using namespace std;" —
// if that appears before the Windows headers that curl brings in,
// std::byte collides with the Windows byte typedef.
#include "kmp.h"


using json = nlohmann::json;


// Minimum visible text (in bytes) for a page to be considered static.
// If the body has fewer non-whitespace characters than this,
// we assume JS hasn't run yet and the page needs rendering.
static const int MIN_STATIC_TEXT_BYTES = 256;



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





// ─────────────────────────────────────────────────────────────────────────────
// Hybrid Fetching Helpers
// ─────────────────────────────────────────────────────────────────────────────

// Does a plain HTTP GET using libcurl.
// Returns raw HTML on success, empty string on any failure.
// Failures are silent on purpose — they just mean "try the browser instead."
std::string PageFetcher::directFetch(const std::string& url) const {

    CURL* curl = curl_easy_init();
    if (!curl) {
        return "";
    }

    std::string responseBody;

    // Set the URL we want to fetch.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Use the same callback that postJson() and isAdapterReachable() use.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    // Follow redirects (many sites redirect http -> https, www -> non-www).
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Reuse the same timeout as the Browser Adapter path.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeoutMs);

    // Set a User-Agent so servers don't reject us as a bare curl request.
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
        "Mozilla/5.0 (compatible; ProjectCrawler/1.0)");

    CURLcode res = curl_easy_perform(curl);

    // Check HTTP status code before cleaning up.
    long httpStatus = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);

    curl_easy_cleanup(curl);

    // Any curl-level error (timeout, DNS failure, etc.) -> return empty.
    if (res != CURLE_OK) {
        if (logger) {
            logger->debug(
                "PageFetcher: directFetch failed for " + url +
                ": " + std::string(curl_easy_strerror(res)));
        }
        return "";
    }

    // Non-200 status (403, 404, 500, etc.) -> return empty.
    if (httpStatus < 200 || httpStatus >= 300) {
        if (logger) {
            logger->debug(
                "PageFetcher: directFetch got HTTP " +
                std::to_string(httpStatus) + " for " + url);
        }
        return "";
    }

    return responseBody;
}



// Counts visible (non-tag, non-whitespace) characters inside <body>.
// Walks through the HTML character by character:
//   - skips everything inside < ... > tags
//   - skips whitespace (spaces, tabs, newlines)
//   - counts everything else as visible text
int PageFetcher::countVisibleTextInBody(const std::string& html) const {

    // Find where <body starts using KMP.
    int bodyStart = KMP::searchFirst(html, "<body");
    if (bodyStart == -1) {
        // No <body> tag at all — probably not a real HTML page.
        return 0;
    }

    // Skip past the <body ...> opening tag itself.
    int pos = bodyStart;
    while (pos < (int)html.size() && html[pos] != '>') {
        pos++;
    }
    pos++; // move past the '>'

    // Find </body> so we know where to stop.
    int bodyEnd = KMP::searchFirst(html, "</body", pos);
    if (bodyEnd == -1) {
        // No closing </body>, just scan to end of string.
        bodyEnd = (int)html.size();
    }

    int visibleCount = 0;
    bool insideTag = false;

    for (int i = pos; i < bodyEnd; i++) {
        char c = html[i];

        if (c == '<') {
            insideTag = true;
            continue;
        }
        if (c == '>') {
            insideTag = false;
            continue;
        }

        // Only count characters that are outside tags and not whitespace.
        if (!insideTag && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            visibleCount++;
        }
    }

    return visibleCount;
}



// Decides whether the raw HTML is just an empty SPA shell
// that needs a headless browser to render the real content.
//
// Runs 4 checks in order. Each one can trigger an early return.
// The rule is: when in doubt, say YES (render) — a false positive
// wastes time, but a false negative loses data.
bool PageFetcher::needsRendering(const std::string& html) const {

    // Check 1: Is there almost no visible text in the body?
    //          SPA shells like React/Vue/Angular serve a near-empty body
    //          with a single <div id="root"></div> and a bunch of <script> tags.
    int visibleText = countVisibleTextInBody(html);
    if (visibleText < MIN_STATIC_TEXT_BYTES) {
        if (logger) {
            logger->debug(
                "PageFetcher: needsRendering=YES — only " +
                std::to_string(visibleText) + " bytes of visible text");
        }
        return true;
    }

    // Check 2: Does it have an empty SPA root container?
    //          These are the classic framework mounting points.
    //          If they're empty, the page content hasn't been injected yet.
    if (KMP::searchFirst(html, "id=\"root\"></div>") != -1 ||
        KMP::searchFirst(html, "id=\"app\"></div>") != -1 ||
        KMP::searchFirst(html, "id=\"__next\"></div>") != -1) {
        if (logger) {
            logger->debug(
                "PageFetcher: needsRendering=YES — empty SPA root container found");
        }
        return true;
    }

    // Check 3: Does it have a <noscript> tag warning about JavaScript?
    //          Pages that say "enable JavaScript" clearly need JS to work.
    int noscriptPos = KMP::searchFirst(html, "<noscript");
    if (noscriptPos != -1) {
        // Look for "javascript" somewhere after the <noscript> tag.
        // We only check the next 200 characters to avoid false matches
        // with unrelated <noscript> tags elsewhere on the page.
        int searchEnd = noscriptPos + 200;
        if (searchEnd > (int)html.size()) searchEnd = (int)html.size();
        std::string noscriptArea = html.substr(noscriptPos, searchEnd - noscriptPos);

        // Lowercase the snippet so we catch "JavaScript", "JAVASCRIPT", etc.
        std::string lower = noscriptArea;
        for (int i = 0; i < (int)lower.size(); i++) {
            if (lower[i] >= 'A' && lower[i] <= 'Z') {
                lower[i] = lower[i] + 32;
            }
        }

        if (KMP::searchFirst(lower, "javascript") != -1) {
            if (logger) {
                logger->debug(
                    "PageFetcher: needsRendering=YES — <noscript> mentions JavaScript");
            }
            return true;
        }
    }

    // Check 4: Does the page have zero <a tags?
    //          A real static page almost always has at least one link.
    //          If there are none, JS probably hasn't built the DOM yet.
    if (KMP::searchFirst(html, "<a ") == -1 &&
        KMP::searchFirst(html, "<a\n") == -1 &&
        KMP::searchFirst(html, "<a\t") == -1) {
        if (logger) {
            logger->debug(
                "PageFetcher: needsRendering=YES — no <a> tags found");
        }
        return true;
    }

    // All checks passed — the page looks like it has real content.
    return false;
}







// Fetches HTML for the given URL using a hybrid strategy.
//
// Step 1: Try a direct HTTP GET (fast, no browser).
// Step 2: Analyze the raw HTML — does it look like a static page?
// Step 3: If static, return immediately. If dynamic, fall back to Browser Adapter.
//
// The Crawler never knows which path was taken. It just gets HTML.
std::string PageFetcher::fetch(const std::string& url) const {

    if (logger) {
        logger->debug("PageFetcher: fetching " + url);
    }


    // ── Step 1: Try a direct HTTP GET (the fast path). ──
    std::string rawHtml = directFetch(url);

    // ── Step 2: If the direct fetch succeeded, analyze the HTML. ──
    if (!rawHtml.empty() && !needsRendering(rawHtml)) {

        // The page has real content — no browser needed.
        if (logger) {
            logger->info(
                "PageFetcher: static shortcut for " + url);
        }
        return rawHtml;
    }


    // ── Step 3: Fall back to Browser Adapter for dynamic pages. ──
    if (logger) {
        logger->info(
            "PageFetcher: dynamic fallback (Browser Adapter) for " + url);
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
            std::string(e.what()));
    }


    // Check response format.
    if (!responseJson.contains("success") ||
        !responseJson["success"].is_boolean()) {
        throw PageFetcherException(
            "PageFetcher: Browser Adapter response missing 'success' field");
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
                url + ": " + errorMsg);
        }

        throw PageFetcherException(
            "PageFetcher: fetch failed for '" +
            url + "': " + errorMsg);
    }


    if (!responseJson.contains("html") ||
        !responseJson["html"].is_string()) {
        throw PageFetcherException(
            "PageFetcher: Browser Adapter response missing 'html' field");
    }


    if (logger) {
        logger->info(
            "PageFetcher: successfully fetched (rendered) " + url);
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