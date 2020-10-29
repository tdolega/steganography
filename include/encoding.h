/** @file */

/**
 * @brief zwraca rozmiar pliku
 * 
 * @param f plik
 * @return int 
 */
int get_file_size(FILE *f);

/**
 * @brief kopiuje header z wejściowej bitmapy do wyjściowej
 * 
 * @param info właściwości
 */
void copy_header(struct Meta *info);

/**
 * @brief czyta i zwraca plik wejściowy po jednym bicie
 * 
 * @param info właściwości
 * @return unsigned 
 */
unsigned get_next_bit(struct Meta *info);

/**
 * @brief ukrywa bity (sekret) w subpikselu
 * 
 * @param info właściwości
 * @param subChunk subpiksel
 * @param color jakim kolorem jest przekazany subpiksel
 * @return unsigned 
 */
unsigned encode_sub(struct Meta *info, unsigned subChunk, char color);

/**
 * @brief funkcja pomocnicza do funkcji `merge`
 * 
 * @param newChunk wyjściowy piksel
 * @param subChunk subpiksel do dodania
 * @param moveLeft pozycja na której wklejamy subpiksel do piksela
 * @return unsigned 
 */
unsigned merge_return(unsigned newChunk, unsigned subChunk, unsigned moveLeft);

/**
 * @brief łączy subpiksele w piksel
 * 
 * @param info właściwości
 * @param newChunk wyjściowy piksel
 * @param subChunk subpiksel do dodania
 * @param color jakim kolorem jest przekazany subpiksel
 * @return unsigned 
 */
unsigned merge(struct Meta *info, unsigned newChunk, unsigned subChunk, char color);

/**
 * @brief główna pętla kodująca
 * 
 * @param info właściwości
 * @return int 
 */
int encode_data(struct Meta *info);

/**
 * @brief ukrywa ustawienia użyte do kodowania. Potrzebne do automatycznego dekodowania
 * 
 * @param info właściwości
 */
void encode_settings(struct Meta *info);

/**
 * @brief główna funkcja wywołująca sprawdzająca i przygotowywująca wszystko przed kodowaniem
 * 
 * @param outputBmp wyjściowa bitmapa
 * @param inputBmp wejściowa bitmapa
 * @param inputSecret sekret do zakodowania
 * @param info właściwości
 * @return int
 */
int encode(char *outputBmp, char *inputBmp, char *inputSecret, struct Meta *info);