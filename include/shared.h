#ifndef HEADER_FILE
#define HEADER_FILE

/** @file */

// for storing encoding settings
#define HEADER_RESERVED4B 0x06

#define HEADER_DATA_LOCATION 0x0A
#define HEADER_DATA_LOCATION_SIZE 4

// correct Microsoft bitmap should start with 'BM' (0x424D) but we read in little endian
#define CORRECT_FILE_TYPE 0x4D42
#define HEADER_FILE_TYPE 0
#define HEADER_FILE_TYPE_SIZE 2

#define HEADER_TYPE 0x0E
#define HEADER_TYPE_SIZE 4
// this one we want, everyone use it
#define BITMAPCOREHEADER 0x0C
// this is old school
#define BITMAPINFOHEADER 0x28

#define HEADER_WIDTH 0x12
#define HEADER_WIDTH_SIZE 4
#define HEADER_HEIGHT 0x16
#define HEADER_HEIGHT_SIZE 4

#define HEADER_BITS_PER_PIXEL 0x1C
#define HEADER_BITS_PER_PIXEL_SIZE 2

struct Options /// czy przełącznik został podany podczas uruchamiania?
{
    int o, i, s;
};

struct Colors
{
    int r, g, b, a;
};

enum Depth
{
    c32bTrueColorRGBA,
    c32bTrueColorRGB,
    c24bTrueColor,
    c16b5551RGBA,
    c16b5551RGB,
    c16b545RGB
};
// 1, 4 and 8 bit depths are indexed, therfore they won't be implemented

struct Meta
{
    /// użyte LSB z subpikseli do ukrycia informacji
    struct Colors lsbUsed;
    /// ilość bitów na subpiksel
    struct Colors colorsUsed;
    /// szerokość obrazka
    int width;
    /// wysokość obrazka
    int height;
    /// plik wejściowy
    FILE *in;
    /// plik wyjściowy
    FILE *out;
    /// plik wejściowy z sekretem
    FILE *secretIn;
    /// głębokość bitmapy
    enum Depth depth;
    /// wielkość piksela w bajtach
    int pixelSize;
    /// wielkość paddingu na końcu każdego wiersza w strukturze BMP
    int padding;
    /// wielkość wiersza w strukturze BMP
    int rowSize;
    /// kawałek sekretu który aktualnie przetwarzam
    unsigned secretChunk;
    /// bit w secretChunk który aktualnie przetwarzamy
    int secredHeadPosition;
    /// wielkość pliku z sekretem
    int secretSize;
    /// informacja że skończyliśmy zapisywać sekret
    int secretSizeEncoded;
    // int secretEOF;
    // int fileSize;
};

/**
 * @brief zwraca wybrane bajty z przekazanego pliku
 * 
 * @param f plik z którego chcemy bajty
 * @param offset na jakim offsecie są nasze dane
 * @param size wielkośc tych danych (max size(unsigned))
 * @return int 
 */
int get_bytes(FILE *f, int offset, int size);

/**
 * @brief sprawdza ilość pikseli w bitmapie
 * 
 * @param info właściwości
 */
void pixel_count(struct Meta *info);

/**
 * @brief sprawdza czy nie wybraliśmy więcej bitów do zaszyfrowania niż istnieje w pliku
 * 
 * @param info właściwości
 * @return int błąd
 */
int wrong_depth(struct Meta *info);

/**
 * @brief oblicza ilość pustych bitów występujących w strukturze pliku pomiędzy wierszami
 * 
 * @param info właściwości
 * @param colorDepth głębokość bitmapy
 */
void check_padding(struct Meta *info, int colorDepth);

/**
 * @brief waliduje wejściową bitmapę
 * 
 * @param info właściwości
 * @return int błąd
 */
int validate_in(struct Meta *info);

/**
 * @brief sprawdza jaka jest maksymalna możliwa wielkość sekretu do zaszyfrowania
 * 
 * @param info właściwości
 * @return int maksymalna wielkość sekretu w bajtach
 */
int max_secret(struct Meta *info);

/**
 * @brief zwraca wybraną część wejściowej liczby
 * 
 * @param dataChunk wejściowe dane
 * @param offset gdzie znajduje się początek żądanych danych
 * @param size jak długi jest ten ciąg
 * @return unsigned wycięte dane
 */
unsigned separate_return(unsigned dataChunk, unsigned offset, unsigned size);

/**
 * @brief zwraca wartość wybranego subpixela z całego pixela
 * 
 * @param info właściwości
 * @param dataChunk pixel wejściowy
 * @param color żądany kolor 'a'/'r'/'g'/'b'
 * @return unsigned wartość wybranego koloru
 */
unsigned separate(struct Meta *info, unsigned dataChunk, char color);

#endif