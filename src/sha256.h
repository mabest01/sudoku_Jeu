#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

#define SHA256_TAILLE_BLOC 32

typedef struct {
  uint8_t donnees[64];
  uint32_t long_donnees;
  uint64_t long_bits;
  uint32_t etat[8];
} ContexteSHA256;

void sha256_initialiser(ContexteSHA256 *ctx);
void sha256_mettre_a_jour(ContexteSHA256 *ctx, const uint8_t *donnees,
                          size_t long_donnees);
void sha256_finaliser(ContexteSHA256 *ctx, uint8_t hachage[]);

#endif
