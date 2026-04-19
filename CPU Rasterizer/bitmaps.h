// Bit Map masks for showing fps
//
// for now I'll use constants for the FPS letters and have a uti that generates
// the maske for the numbers
//
//

struct CharacterBitMap {
    uint8_t data[7];
};


CharacterBitMap segmentsMask[7];
uint8_t digitMask[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static void generate7SegmentMask(){
    // A: top horizontal
    segmentsMask[0] = { 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // B: upper-right vertical
    segmentsMask[1] = { 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00 };

    // C: lower-right vertical
    segmentsMask[2] = { 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00 };

    // D: bottom horizontal
    segmentsMask[3] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C };

    // E: lower-left vertical
    segmentsMask[4] = { 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00 };

    // F: upper-left vertical
    segmentsMask[5] = { 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 };

    // G: middle horizontal
    segmentsMask[6] = { 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00 };
}

static void generateFPSMask(CharacterBitMap (&fps)[3]) {
    CharacterBitMap F = { 0xFF, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80 };
    CharacterBitMap P = { 0xFC, 0x84, 0x84, 0xFC, 0x80, 0x80, 0x80 };

    CharacterBitMap S = { 0x7C, 0x80, 0x80, 0x7C, 0x04, 0x04, 0xF8 };

    fps[0] = F;
    fps[1] = P;
    fps[2] = S;

}

static CharacterBitMap generateEqualMask() {
    return { 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
}

void clearBitMap(CharacterBitMap& bitmap){
    bitmap = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
}

void orBitMaps(CharacterBitMap& bitmap, const CharacterBitMap& other) {
    for(int i = 0; i < 7; i++) {
        bitmap.data[i] |= other.data[i];
    }
}

static void generateNumberMask(CharacterBitMap (&numbers)[10]) {

    for(int i = 0; i < 10; i++) {
        clearBitMap(numbers[i]);
        for (int s = 0; s < 7; s++) {
            if (digitMask[i] & (1 << s)) {
                orBitMaps(numbers[i], segmentsMask[s]);
            }
        }
     }
}
