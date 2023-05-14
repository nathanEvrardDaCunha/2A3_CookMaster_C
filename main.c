#include <stdio.h>
#include <curl/curl.h>

int main(void) {
    CURL *apiRequest;
    CURLcode requestResult;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    apiRequest = curl_easy_init();

    if (apiRequest) {
        curl_easy_setopt(apiRequest, CURLOPT_URL, "http://example.com");
        requestResult = curl_easy_perform(apiRequest);

        if (requestResult != CURLE_OK) {
            fprintf(stderr, "Erreur lors de la requête CURL: %s\n", curl_easy_strerror(requestResult));
        }

        curl_easy_cleanup(apiRequest);
    }

    curl_global_cleanup();
    return 0;
}
