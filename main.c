#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

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
            json_error_t jsonError;
            json_t *jsonRoot = json_loads(apiResponse, 0, &jsonError);

            if (!jsonRoot) {
                fprintf(stderr, "Erreur lors de l'analyse de la réponse JSON: %s\n", jsonError.text);
            } else {
                printf("Réponse JSON chargée avec succès.\n");
                json_decref(jsonRoot);
            }
        }

        curl_easy_cleanup(apiRequest);
    }

    curl_global_cleanup();
    return 0;
}
