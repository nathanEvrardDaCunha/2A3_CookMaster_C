#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

size_t writeApiResponseToBuffer(char *data, size_t blockSize, size_t blockCount, void *responseBuffer) {
    size_t receivedDataSize = blockSize * blockCount; 
    char *responseStr = (char *)responseBuffer; 
    strcat(responseStr, data); 
    return receivedDataSize; 
}

int main(void) {
    CURL *apiRequest;
    CURLcode requestResult;
    char apiResponse[4096] = { 0 };

    curl_global_init(CURL_GLOBAL_DEFAULT);
    apiRequest = curl_easy_init();

    if (apiRequest) {
        curl_easy_setopt(apiRequest, CURLOPT_URL, "http://example.com");
        curl_easy_setopt(apiRequest, CURLOPT_WRITEFUNCTION, writeApiResponseToBuffer);
        curl_easy_setopt(apiRequest, CURLOPT_WRITEDATA, apiResponse);

        requestResult = curl_easy_perform(apiRequest);

        if (requestResult != CURLE_OK) {
            fprintf(stderr, "Erreur lors de la requête CURL: %s\n", curl_easy_strerror(requestResult));
        } else {
            printf("Réponse de l'API : %s\n", apiResponse);
        }

        curl_easy_cleanup(apiRequest);
    }

    curl_global_cleanup();
    return 0;
}
