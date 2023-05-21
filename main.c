#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#define MAX_KEYS 100

typedef struct {
    char *data;
    size_t len;
} response_buffer;

size_t writeApiResponseToBuffer(void *ptr, size_t size, size_t nmemb, response_buffer *buffer) {
    size_t new_len = buffer->len + size * nmemb;
    buffer->data = realloc(buffer->data, new_len + 1);
    if (buffer->data == NULL) {
        fprintf(stderr, "Pas assez de mémoire (realloc returned NULL)\n");
        return 0;
    }
    memcpy(buffer->data + buffer->len, ptr, size * nmemb);
    buffer->data[new_len] = '\0';
    buffer->len = new_len;

    return size * nmemb;
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

void fetchApiDataAndPrintValues(const char *apiUrl, const char *searchKeys[], size_t numOfKeys) {
    CURL *apiRequest;
    CURLcode requestResult;
    response_buffer buffer = {0};

    apiRequest = curl_easy_init();
    if (apiRequest) {
        curl_easy_setopt(apiRequest, CURLOPT_URL, apiUrl);
        curl_easy_setopt(apiRequest, CURLOPT_WRITEFUNCTION, writeApiResponseToBuffer);
        curl_easy_setopt(apiRequest, CURLOPT_WRITEDATA, &buffer);

        requestResult = curl_easy_perform(apiRequest);

        if (requestResult != CURLE_OK) {
            fprintf(stderr, "Erreur lors de la requête CURL: %s\n", curl_easy_strerror(requestResult));
        }
        else {
            json_error_t jsonError;
            json_t *jsonRoot = json_loads(buffer.data, 0, &jsonError);
            if (jsonRoot) {
                for (size_t i = 0; i < numOfKeys; i++) {
                    json_t *value = extractValueFromJsonByKey(jsonRoot, searchKeys[i]);
                    if (value) {
                        if (json_is_number(value)) {
                            double numberValue = json_number_value(value);
                            printf("La valeur de '%s' est %f.\n", searchKeys[i], numberValue);
                        }
                        else if (json_is_string(value)) {
                            const char *stringValue = json_string_value(value);
                            printf("La valeur de '%s' est : %s.\n", searchKeys[i], stringValue);
                        }
                        else {
                            fprintf(stderr, "La clé '%s' n'a pas été trouvée ou n'est pas du bon type.\n", searchKeys[i]);
                        }
                    }
                }
                json_decref(jsonRoot);
            }
            else {
                fprintf(stderr, "Erreur lors de l'analyse de la réponse JSON: %s\n", jsonError.text);
            }
        }
        curl_easy_cleanup(apiRequest);
    }
    if (buffer.data) {
        free(buffer.data);
    }
}

int main(void) {
    printf("\n----------------------------------------------------\n");
    printf("      Bienvenue dans l'outil d'extraction d'API     \n");
    printf("----------------------------------------------------\n\n");
    
    char apiUrl[1024];
    printf("Veuillez entrer l'URL de l'API :\n");
    scanf("%1023s", apiUrl);

    int numOfKeys;
    printf("\nCombien de clés voulez-vous entrer ?\n");
    scanf("%d", &numOfKeys);

    char searchKeys[MAX_KEYS][256];
    printf("\nVeuillez entrer les clés une par une :\n");
    for (int i = 0; i < numOfKeys; i++) {
        printf("Clé n°%d : ", i + 1);
        scanf("%255s", searchKeys[i]);
    }

    printf("\nTraitement en cours...\n\n");

    const char *keysPtrs[MAX_KEYS];
    for (int i = 0; i < numOfKeys; i++) {
        keysPtrs[i] = searchKeys[i];
    }

    fetchApiDataAndPrintValues(apiUrl, keysPtrs, numOfKeys);

    printf("\nL'extraction des données est terminée. Merci d'avoir utilisé l'outil d'extraction d'API.\n\n");

    return 0;
}