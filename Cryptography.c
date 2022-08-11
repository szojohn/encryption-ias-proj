#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>

// use shift for 2d array of hex

static const uint32_t roundConst[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1b000000, 0x36000000, 0x6c000000, 0xd8000000, 0xab000000, 0xed000000, 0x9a000000};

static const uint8_t substiBox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

uint8_t getBox(uint8_t val)
{
    return substiBox[val];
}

uint32_t subShift(uint32_t val)
{
    uint32_t tmp = 0;
   
    tmp |= ((uint32_t)getBox((uint8_t)((val >>  0) & 0xFF))) <<  0;
    tmp |= ((uint32_t)getBox((uint8_t)((val >>  8) & 0xFF))) <<  8;
    tmp |= ((uint32_t)getBox((uint8_t)((val >> 16) & 0xFF))) << 16;
    tmp |= ((uint32_t)getBox((uint8_t)((val >> 24) & 0xFF))) << 24;

    return tmp;
}

uint32_t rot(uint32_t val)
{
    uint32_t tmp = val;
   
    return (val >> 8) | ((tmp & 0xFF) << 24);
}

uint32_t shiftOr(uint32_t val)
{
    return (((val & 0x000000FF) << 24) |
            ((val & 0x0000FF00) <<  8) |
            ((val & 0x00FF0000) >>  8) |
            ((val & 0xFF000000) >> 24) );
}

void keyExpansion(uint8_t *key, uint8_t *round)
{
    uint32_t *w = (uint32_t *)round;
    uint32_t  t;
    int      i = 0;

    printf("Key Expansion:\n");
    do {
        w[i] = *((uint32_t *)&key[i * 4 + 0]);
        printf("    %2.2d:  rs: %8.8x\n", i, shiftOr(w[i]));
    } while (++i < 4);
   
    do {
        printf("    %2.2d: ", i);
        if ((i % 4) == 0) {
            t = rot(w[i - 1]);
            printf(" rot: %8.8x", shiftOr(t));
            t = subShift(t);
            printf(" sub: %8.8x", shiftOr(t));
            printf(" rcon: %8.8x", roundConst[i/4 - 1]);
            t = t ^ shiftOr(roundConst[i/4 - 1]);
            printf(" xor: %8.8x", t);
        } else if (4 > 6 && (i % 4) == 4) {
            t = subShift(w[i - 1]);
            printf(" sub: %8.8x", shiftOr(t));
        } else {
            t = w[i - 1];
            printf(" equ: %8.8x", shiftOr(t));
        }
        w[i] = w[i - 4] ^ t;
        printf(" rs: %8.8x\n", shiftOr(w[i]));
    } while (++i < 4 * (10 + 1));
}

void addRKey(uint8_t *state, uint8_t *round, int nr)
{
    uint32_t *w = (uint32_t *)round;
    uint32_t *s = (uint32_t *)state;
    int i;
   
    for (i = 0; i < 4; i++) {
        s[i] ^= w[nr * 4 + i];
    }
}

void sub(uint8_t *state)
{
    int i, j;
   
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i * 4 + j] = getBox(state[i * 4 + j]);
        }
    }
}

void shiftRows(uint8_t *state)
{
    uint8_t *s = (uint8_t *)state;
    int i, j, r;
   
    for (i = 1; i < 4; i++) {
        for (j = 0; j < i; j++) {
            uint8_t tmp = s[i];
            for (r = 0; r < 4; r++) {
                s[i + r * 4] = s[i + (r + 1) * 4];
            }
            s[i + (4 - 1) * 4] = tmp;
        }
    }
}

uint8_t xTime(uint8_t x)
{
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

uint8_t xTimes(uint8_t x, int ts)
{
    while (ts-- > 0) {
        x = xTime(x);
    }
   
    return x;
}

uint8_t xOr(uint8_t x, uint8_t y)
{
    return ((((y >> 0) & 1) * xTimes(x, 0)) ^
            (((y >> 1) & 1) * xTimes(x, 1)) ^
            (((y >> 2) & 1) * xTimes(x, 2)) ^
            (((y >> 3) & 1) * xTimes(x, 3)) ^
            (((y >> 4) & 1) * xTimes(x, 4)) ^
            (((y >> 5) & 1) * xTimes(x, 5)) ^
            (((y >> 6) & 1) * xTimes(x, 6)) ^
            (((y >> 7) & 1) * xTimes(x, 7)) );
}

void mixColumns(uint8_t *state)
{
    uint8_t y[16] = { 2, 3, 1, 1,  1, 2, 3, 1,  1, 1, 2, 3,  3, 1, 1, 2};
    uint8_t s[4];
    int i, j, r;
   
    for (i = 0; i < 4; i++) {
        for (r = 0; r < 4; r++) {
            s[r] = 0;
            for (j = 0; j < 4; j++) {
                s[r] = s[r] ^ xOr(state[i * 4 + j], y[r * 4 + j]);
            }
        }
        for (r = 0; r < 4; r++) {
            state[i * 4 + r] = s[r];
        }
    }
}

void print(char *msg, uint8_t *data, int len)
{
    int i;
   
    printf("%8.8s: ", msg);
    for (i = 0; i < len; i++) {
        printf(" %2.2x", data[i]);
    }
    printf("\n");
}

void printEnd(char *msg, uint8_t *data, int len)
{
    int i;
   
    printf("%8.8s: ", msg);
    for (i = 0; i < len; i++) {
        printf(" %2.2c", data[i]);
    }
    printf("\n");
}

int encrypt(uint8_t *data, int len, uint8_t *key)
{
    uint8_t w[4 * 4 * 15] = {0}; 
    uint8_t s[4 * 4] = {0}; 
   
    int nr, i, j;
   
    keyExpansion(key, w);
   
    for (i = 0; i < len; i += 4 * 4) {

        printf("Encrypting block at %u ...\n", i);

        for (j = 0; j < 4 * 4; j++)
            s[j] = data[i + j];
       
        for (nr = 0; nr <= 10; nr++) {
            printf(" Round %d:\n", nr);
            print("input", s, 4 *4);
           
            if (nr > 0) {     
                sub(s);
                print("  sub", s, 4 * 4);
               
                
                shiftRows(s);
                print("  shift", s, 4 * 4);
               
                if (nr < 10) {
                    
                    mixColumns(s);
                    print("  mix", s, 4 * 4);
                }
            }
           
            addRKey(s, w, nr);
            print("  round", &w[nr * 4 * 4], 4 * 4);
            print("  state", s, 4 * 4);
        }
       
        for (j = 0; j < 4 * 4; j++)
            data[i + j] = s[j];
        printf("Output:\n");
        print("cypher", &data[i], 4 * 4);
    }
   
    return 0;
}

uint8_t charToHex(char input){
    return (0x00|(uint8_t)input);
}

void convertToHex(char* message, char* key){
    int i = 0, j = 0, loop = 0;
    int lenMes = (strlen(message) / 4) + 1;
    int lenKey = (strlen(key) / 4) + 1;
    uint8_t messHex[lenMes][lenMes];
    uint8_t keyHex[lenKey][lenKey];

    while (message[loop] != '\0'){
        messHex[i][j] = charToHex(message[loop]);
        loop++;
        j++;
        if(j == lenMes){
            j = 0;
            i++;
        }
    }
    
    loop = 0;
    i = 0;
    j = 0;
    
    while (key[loop] != '\0'){
        keyHex[i][j] = charToHex(key[loop]);
        loop++;
        j++;
        if(j == lenKey){
            j = 0;
            i++;
        }
    }
    
    printf("Encrypt:\n");
    encrypt(messHex, sizeof(messHex), keyHex);
}

void start(){
    char message[60];
    char key[60];
    printf("Cryptography (Group 4 BSCS)\n");
    printf("Enter message: ");
    scanf("%[^\n]%*c", message);

    printf("Enter key: ");
    scanf("%[^\n]%*c", key);

    message[strlen(message)] = '\0';
    key[strlen(key)] = '\0';

    convertToHex(message, key);
}
int main()
{
    start();
    return 0;
}