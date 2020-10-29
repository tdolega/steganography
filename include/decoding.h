/** @file */

/**
 * @brief zapisuje sekret po jednym bicie
 * 
 * @param info właściwości
 * @param secretBit 
 * @return int 
 */
int copy_next_bit(struct Meta *info, unsigned secretBit);

/**
 * @brief odczytuje bity (sekret) w subpikselu
 * 
 * @param info właściwości
 * @param subChunk subpiksel
 * @param color jakim kolorem jest przekazany subpiksel
 * @return int 
 */
int decode_sub(struct Meta *info, unsigned subChunk, char color);

/**
 * @brief główna pętla dekodująca
 * 
 * @param info właściwości
 * @return int 
 */
int decode_data(struct Meta *info);

/**
 * @brief odczytuje ustawienia użyte do kodowania
 * 
 * @param info właściwości
 */
void decode_settings(struct Meta *info);

/**
 * @brief główna funkcja wywołująca sprawdzająca i przygotowywująca wszystko przed dekodowaniem
 * 
 * @param outputSecret plik wyjściowy do zapisania sekretu
 * @param inputBmp wejściowa bitmapa
 * @param info właściwości
 * @return int
 */
int decode(char *outputSecret, char *inputBmp, struct Meta *info);