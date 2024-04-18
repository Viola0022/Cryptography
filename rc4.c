#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

int main (void) {
    unsigned char ot[1024] = "Text pro rc4.";  // open text
    unsigned char st[1024];  // sifrovany text
    unsigned char key[EVP_MAX_KEY_LENGTH] = "Muj klic";  // klic pro sifrovani
    unsigned char iv[EVP_MAX_IV_LENGTH] = "inicial. vektor";  // inicializacni vektor
    const char cipherName[] = "RC4";
    const EVP_CIPHER * cipher;

    OpenSSL_add_all_ciphers();
    /* sifry i hashe by se nahraly pomoci OpenSSL_add_all_algorithms() */
    cipher = EVP_get_cipherbyname(cipherName);
    if (!cipher) {
        printf("Sifra %s neexistuje.\n", cipherName);
        return 1;
    }

    int otLength = strlen((const char *) ot), stLength = 0, tmpLength = 0;

    EVP_CIPHER_CTX * ctx; // context structure
    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL)
        return 2;

    printf("OT: %s\n", ot);

    /* Sifrovani */
    if (!EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) // context init - set cipher, key, init vector
        return 3;

    if (!EVP_EncryptUpdate(ctx,  st, &tmpLength, ot, otLength)) // encryption of pt
        return 4;

    stLength += tmpLength;

    if (!EVP_EncryptFinal_ex(ctx, st + stLength, &tmpLength)) // get the remaining ct
        return 5;

    stLength += tmpLength;

    printf ("Zasifrovano %d znaku.\n", stLength);

    /* Desifrovani */
    if (!EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) // nastaveni kontextu pro desifrovani
        return 6;

    if (!EVP_DecryptUpdate(ctx, ot, &tmpLength,  st, stLength)) // desifrovani st
        return 7;

    otLength += tmpLength;

    if (!EVP_DecryptFinal_ex(ctx, ot + otLength, &tmpLength)) // dokonceni (ziskani zbytku z kontextu)
        return 8;

    otLength += tmpLength;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    /* Vypsani zasifrovaneho a rozsifrovaneho textu. */
    printf("ST: %s\nDT: %s\n", st, ot);
    return 0;
}