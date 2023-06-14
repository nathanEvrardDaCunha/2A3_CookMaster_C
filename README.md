# 2A3_CookMaster_C

COMMANDE POUR COMPILER :
gcc -o requeteurAPI main.c -lcurl -ljansson

CLEF QUI FONCTIONNE :
- https://api.openweathermap.org/data/2.5/weather?lat=47.34&lon=10.99&appid=a55d1a3187441c503ba1f6f5a914b0be
    + name

- https://api.nasa.gov/neo/rest/v1/feed?start_date=2023-05-01&end_date=2023-05-07&api_key=DEMO_KEY
    + name

- https://www.themealdb.com/api/json/v1/1/search.php?s=Arrabiata
    + strMeal
    + strInstructions
