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

json_t *extractValueFromJsonByKey(json_t *jsonRoot, const char *searchKey) {
    if (!json_is_object(jsonRoot)) {
        fprintf(stderr, "Le JSON fourni n'est pas un objet.\n");
        return NULL;
    }

    json_t *foundValue = NULL;
    const char *currentKey;

    json_object_foreach(jsonRoot, currentKey, foundValue) {
        if (strcmp(searchKey, currentKey) == 0) {
            return foundValue;
        }
        else if (json_is_object(foundValue)) {
            json_t *nestedValue = extractValueFromJsonByKey(foundValue, searchKey);
            if (nestedValue != NULL) {
                return nestedValue;
            }
        }
        else if (json_is_array(foundValue)) {
            size_t arraySize = json_array_size(foundValue);
            for (size_t i = 0; i < arraySize; i++) {
                json_t *nestedValue = extractValueFromJsonByKey(json_array_get(foundValue, i), searchKey);
                if (nestedValue != NULL) {
                    return nestedValue;
                }
            }
        }
    }

    return NULL;
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
                const char *searchKey = "exampleKey";
                json_t *value = extractValueFromJsonByKey(jsonRoot, searchKey);
                if (value) {
                    if (json_is_number(value)) {
                        double numberValue = json_number_value(value);
                        printf("La valeur de '%s' est %f.\n", searchKey, numberValue);
                    }
                    else if (json_is_string(value)) {
                        const char *stringValue = json_string_value(value);
                        printf("La valeur de '%s' est : %s.\n", searchKey, stringValue);
                    }
                    else {
                        fprintf(stderr, "La clé '%s' n'a pas été trouvée ou n'est pas du bon type.\n", searchKey);
                    }
                }
                json_decref(jsonRoot);
            }
        }

        curl_easy_cleanup(apiRequest);
    }

    curl_global_cleanup();
    return 0;
}
