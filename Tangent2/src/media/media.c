/*
 * media.c
 * Implementation of media handling
 *  Created on: Jan 4, 2026
 *      Author: harma
 */

#include "media.h"

#define IMAGE_WIDTH 192
#define IMAGE_HEIGHT 63
#define BYTES_PER_ROW 24
#define IMAGE_PIXELS (IMAGE_WIDTH * IMAGE_HEIGHT)
#define BITPLANE_SIZE 0x600
#define RENDER_BUFFER_SIZE 0xC00

#define LZ_STATES 12
#define LZ_POS_STATES 16
#define LZ_LEN_TO_POS_STATES 4
#define LZ_POS_SLOT_BITS 6
#define LZ_ALIGN_BITS 4
#define LZ_END_POS_MODEL_INDEX 14
#define LZ_FULL_DISTANCES (1 << (LZ_END_POS_MODEL_INDEX / 2))
#define LZ_POS_SPECIAL_PROBS (LZ_FULL_DISTANCES - LZ_END_POS_MODEL_INDEX)
#define LZ_MATCH_MIN 2
#define LZ_LOW_LEN_BITS 3
#define LZ_MID_LEN_BITS 3
#define LZ_HIGH_LEN_BITS 8
#define LZ_LOW_LEN_SYMBOLS (1 << LZ_LOW_LEN_BITS)
#define LZ_MID_LEN_SYMBOLS (1 << LZ_MID_LEN_BITS)
#define LZ_HIGH_LEN_SYMBOLS (1 << LZ_HIGH_LEN_BITS)
#define LZ_PROB_INIT 1024

typedef struct {
    const byte *source;
    word source_size;
    word source_pos;
    dword range;
    dword code;
    byte error;
} lz_decoder_t;

typedef struct {
    byte *dest;
    word dest_size;
    word dest_pos;
    unsigned long long low;
    dword range;
    dword cache_size;
    byte cache;
    byte error;
} lz_encoder_t;

typedef struct {
    word choice_1;
    word choice_2;
    word low[LZ_POS_STATES][LZ_LOW_LEN_SYMBOLS];
    word mid[LZ_POS_STATES][LZ_MID_LEN_SYMBOLS];
    word high[LZ_HIGH_LEN_SYMBOLS];
} lz_length_model_t;

static word prob_is_match[LZ_STATES][LZ_POS_STATES];
static word prob_is_rep[LZ_STATES];
static word prob_rep_g0[LZ_STATES];
static word prob_rep_g1[LZ_STATES];
static word prob_rep_g2[LZ_STATES];
static word prob_rep0_long[LZ_STATES][LZ_POS_STATES];
static word prob_pos_slot[LZ_LEN_TO_POS_STATES][1 << LZ_POS_SLOT_BITS];
static word prob_pos_special[LZ_POS_SPECIAL_PROBS];
static word prob_pos_align[1 << LZ_ALIGN_BITS];
static word prob_literal[0x300];
static lz_length_model_t len_model;
static lz_length_model_t rep_len_model;

static byte render_buffer[RENDER_BUFFER_SIZE];

static byte state_is_literal(byte state);
static void update_literal_state(byte *state);
static void reset_decoder_model(void);

static byte write_encoded_byte(lz_encoder_t *encoder, byte value) {
    if (encoder->dest_pos >= encoder->dest_size) {
        encoder->error = 1;
        return 1;
    }
    encoder->dest[encoder->dest_pos++] = value;
    return 0;
}

static void shift_low(lz_encoder_t *encoder) {
    dword low32;
    dword high32;

    if (encoder->error) {
        return;
    }

    low32 = (dword)encoder->low;
    high32 = (dword)(encoder->low >> 32);

    if (low32 < 0xFF000000UL || high32 != 0) {
        byte temp = encoder->cache;
        do {
            if (write_encoded_byte(encoder, (byte)(temp + (byte)high32))) {
                return;
            }
            temp = 0xFF;
        } while (--encoder->cache_size != 0);
        encoder->cache = (byte)(low32 >> 24);
    }

    encoder->cache_size++;
    encoder->low = (unsigned long long)((dword)encoder->low << 8);
}

static void encode_bit(lz_encoder_t *encoder, word *probability, byte bit) {
    dword bound;

    if (encoder->error) {
        return;
    }

    bound = (encoder->range >> 11) * (*probability);
    if (bit == 0) {
        encoder->range = bound;
        *probability += (word)((2048 - *probability) >> 5);
    } else {
        encoder->low += bound;
        encoder->range -= bound;
        *probability -= (word)(*probability >> 5);
    }

    if (encoder->range < 0x01000000UL) {
        encoder->range <<= 8;
        shift_low(encoder);
    }
}

static void encode_literal_symbol(lz_encoder_t *encoder, byte state, word rep0, const byte *history, word history_pos, byte value) {
    word context = 1;
    byte symbol = value;

    if (!state_is_literal(state) && history_pos > 0 && rep0 < history_pos) {
        byte match_byte = history[history_pos - rep0 - 1];
        do {
            byte match_bit = (byte)((match_byte >> 7) & 1);
            byte bit = (byte)((symbol >> 7) & 1);
            match_byte <<= 1;
            symbol <<= 1;
            encode_bit(encoder, &prob_literal[((1 + match_bit) << 8) + context], bit);
            context = (word)((context << 1) | bit);
            if (match_bit != bit) {
                break;
            }
        } while (context < 0x100);
    }

    while (context < 0x100) {
        byte bit = (byte)((symbol >> 7) & 1);
        symbol <<= 1;
        encode_bit(encoder, &prob_literal[context], bit);
        context = (word)((context << 1) | bit);
    }
}

static byte compress_lzma_raw(const byte *input, word input_size, byte **out_data, word *out_size) {
    lz_encoder_t encoder;
    byte state;
    word rep0;
    word i;
    word max_out;
    byte *buffer;

    if (input == 0 || out_data == 0 || out_size == 0) {
        return 1;
    }

    max_out = (word)(13 + input_size + (input_size >> 1) + 64);
    buffer = (byte*)halloc(max_out);
    if (buffer == 0) {
        return 1;
    }

    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0x00;
    buffer[3] = 0x01;
    buffer[4] = 0x00;
    buffer[5] = (byte)(input_size & 0xFF);
    buffer[6] = (byte)((input_size >> 8) & 0xFF);
    buffer[7] = 0x00;
    buffer[8] = 0x00;
    buffer[9] = 0x00;
    buffer[10] = 0x00;
    buffer[11] = 0x00;
    buffer[12] = 0x00;

    encoder.dest = buffer;
    encoder.dest_size = max_out;
    encoder.dest_pos = 13;
    encoder.low = 0;
    encoder.range = 0xFFFFFFFFUL;
    encoder.cache_size = 1;
    encoder.cache = 0;
    encoder.error = 0;

    reset_decoder_model();

    state = 0;
    rep0 = 0;

    for (i = 0; i < input_size; i++) {
        encode_bit(&encoder, &prob_is_match[state][0], 0);
        encode_literal_symbol(&encoder, state, rep0, input, i, input[i]);
        update_literal_state(&state);
        if (encoder.error) {
            break;
        }
    }

    for (i = 0; i < 5 && !encoder.error; i++) {
        shift_low(&encoder);
    }

    if (encoder.error) {
        hfree(buffer);
        return 1;
    }

    *out_data = buffer;
    *out_size = encoder.dest_pos;
    return 0;
}

static void init_probabilities(word *array, word count) {
    word i;
    for (i = 0; i < count; i++) {
        array[i] = LZ_PROB_INIT;
    }
}

static byte read_source_byte(lz_decoder_t *decoder, byte *value) {
    if (decoder->source_pos >= decoder->source_size) {
        decoder->error = 1;
        *value = 0;
        return 1;
    }
    *value = decoder->source[decoder->source_pos++];
    return 0;
}

static void normalize_decoder(lz_decoder_t *decoder) {
    byte b;
    while (decoder->range < 0x01000000UL) {
        decoder->range <<= 8;
        if (read_source_byte(decoder, &b)) {
            return;
        }
        decoder->code = (decoder->code << 8) | b;
    }
}

static byte decode_bit(lz_decoder_t *decoder, word *probability) {
    dword bound;
    byte bit;

    normalize_decoder(decoder);
    if (decoder->error) {
        return 0;
    }

    bound = (decoder->range >> 11) * (*probability);
    if (decoder->code < bound) {
        decoder->range = bound;
        *probability += (word)((2048 - *probability) >> 5);
        bit = 0;
    } else {
        decoder->range -= bound;
        decoder->code -= bound;
        *probability -= (word)(*probability >> 5);
        bit = 1;
    }
    return bit;
}

static word decode_bit_tree(lz_decoder_t *decoder, word *probabilities, byte bit_count) {
    word symbol;
    byte i;
    symbol = 1;
    for (i = 0; i < bit_count; i++) {
        symbol = (symbol << 1) | decode_bit(decoder, &probabilities[symbol]);
    }
    return (word)(symbol - ((word)1 << bit_count));
}

static word decode_bit_tree_reverse(lz_decoder_t *decoder, word *probabilities, byte bit_count) {
    word symbol;
    word value;
    byte i;
    symbol = 1;
    value = 0;
    for (i = 0; i < bit_count; i++) {
        byte bit = decode_bit(decoder, &probabilities[symbol]);
        symbol = (symbol << 1) | bit;
        value |= ((word)bit << i);
    }
    return value;
}

static dword decode_direct_bits(lz_decoder_t *decoder, byte bit_count) {
    dword value;
    byte i;
    value = 0;
    for (i = 0; i < bit_count; i++) {
        decoder->range >>= 1;
        if (decoder->code >= decoder->range) {
            decoder->code -= decoder->range;
            value = (value << 1) | 1;
        } else {
            value <<= 1;
        }
        normalize_decoder(decoder);
        if (decoder->error) {
            return 0;
        }
    }
    return value;
}

static word decode_length(lz_decoder_t *decoder, lz_length_model_t *length_model, byte pos_state) {
    if (decode_bit(decoder, &length_model->choice_1) == 0) {
        return decode_bit_tree(decoder, length_model->low[pos_state], LZ_LOW_LEN_BITS);
    }
    if (decode_bit(decoder, &length_model->choice_2) == 0) {
        return LZ_LOW_LEN_SYMBOLS + decode_bit_tree(decoder, length_model->mid[pos_state], LZ_MID_LEN_BITS);
    }
    return LZ_LOW_LEN_SYMBOLS + LZ_MID_LEN_SYMBOLS + decode_bit_tree(decoder, length_model->high, LZ_HIGH_LEN_BITS);
}

static byte state_is_literal(byte state) {
    return (state < 7) ? 1 : 0;
}

static void update_literal_state(byte *state) {
    if (*state < 4) {
        *state = 0;
    } else if (*state < 10) {
        *state -= 3;
    } else {
        *state -= 6;
    }
}

static void update_match_state(byte *state) {
    *state = (*state < 7) ? 7 : 10;
}

static void update_rep_state(byte *state) {
    *state = (*state < 7) ? 8 : 11;
}

static void update_short_rep_state(byte *state) {
    *state = (*state < 7) ? 9 : 11;
}

static void reset_decoder_model(void) {
    init_probabilities(&prob_is_match[0][0], LZ_STATES * LZ_POS_STATES);
    init_probabilities(prob_is_rep, LZ_STATES);
    init_probabilities(prob_rep_g0, LZ_STATES);
    init_probabilities(prob_rep_g1, LZ_STATES);
    init_probabilities(prob_rep_g2, LZ_STATES);
    init_probabilities(&prob_rep0_long[0][0], LZ_STATES * LZ_POS_STATES);
    init_probabilities(&prob_pos_slot[0][0], LZ_LEN_TO_POS_STATES * (1 << LZ_POS_SLOT_BITS));
    init_probabilities(prob_pos_special, LZ_POS_SPECIAL_PROBS);
    init_probabilities(prob_pos_align, 1 << LZ_ALIGN_BITS);
    init_probabilities(prob_literal, 0x300);

    len_model.choice_1 = LZ_PROB_INIT;
    len_model.choice_2 = LZ_PROB_INIT;
    init_probabilities(&len_model.low[0][0], LZ_POS_STATES * LZ_LOW_LEN_SYMBOLS);
    init_probabilities(&len_model.mid[0][0], LZ_POS_STATES * LZ_MID_LEN_SYMBOLS);
    init_probabilities(len_model.high, LZ_HIGH_LEN_SYMBOLS);

    rep_len_model.choice_1 = LZ_PROB_INIT;
    rep_len_model.choice_2 = LZ_PROB_INIT;
    init_probabilities(&rep_len_model.low[0][0], LZ_POS_STATES * LZ_LOW_LEN_SYMBOLS);
    init_probabilities(&rep_len_model.mid[0][0], LZ_POS_STATES * LZ_MID_LEN_SYMBOLS);
    init_probabilities(rep_len_model.high, LZ_HIGH_LEN_SYMBOLS);
}

static byte decompress_lzma_raw(const byte *input, word input_size, byte *output, word output_size) {
    lz_decoder_t decoder;
    dword unpacked_size;
    byte state;
    word rep0, rep1, rep2, rep3;
    word output_pos;
    byte previous_byte;
    byte i;

    if (input == 0 || output == 0 || input_size < 13) {
        return 1;
    }

    if (input[0] != 0) {
        return 1;
    }

    unpacked_size = (dword)input[5] |
                    ((dword)input[6] << 8) |
                    ((dword)input[7] << 16) |
                    ((dword)input[8] << 24);

    if (unpacked_size != 0xFFFFFFFFUL && unpacked_size != output_size) {
        return 1;
    }

    decoder.source = input + 13;
    decoder.source_size = (word)(input_size - 13);
    decoder.source_pos = 0;
    decoder.range = 0xFFFFFFFFUL;
    decoder.code = 0;
    decoder.error = 0;

    for (i = 0; i < 5; i++) {
        byte b;
        if (read_source_byte(&decoder, &b)) {
            return 1;
        }
        decoder.code = (decoder.code << 8) | b;
    }

    reset_decoder_model();

    state = 0;
    rep0 = rep1 = rep2 = rep3 = 0;
    output_pos = 0;
    previous_byte = 0;

    while (output_pos < output_size && !decoder.error) {
        byte pos_state;
        pos_state = 0;

        if (decode_bit(&decoder, &prob_is_match[state][pos_state]) == 0) {
            word symbol;
            word *literal_probs;

            literal_probs = prob_literal;
            symbol = 1;

            if (!state_is_literal(state)) {
                byte match_byte;
                match_byte = (byte)output[output_pos - rep0 - 1];
                do {
                    byte match_bit;
                    byte bit;
                    match_bit = (byte)((match_byte >> 7) & 1);
                    match_byte <<= 1;
                    bit = decode_bit(&decoder, &literal_probs[((1 + match_bit) << 8) + symbol]);
                    symbol = (symbol << 1) | bit;
                    if (match_bit != bit) {
                        while (symbol < 0x100) {
                            symbol = (symbol << 1) | decode_bit(&decoder, &literal_probs[symbol]);
                        }
                        break;
                    }
                } while (symbol < 0x100);
            } else {
                while (symbol < 0x100) {
                    symbol = (symbol << 1) | decode_bit(&decoder, &literal_probs[symbol]);
                }
            }

            previous_byte = (byte)(symbol - 0x100);
            output[output_pos++] = previous_byte;
            update_literal_state(&state);
            continue;
        }

        if (decode_bit(&decoder, &prob_is_rep[state])) {
            word length;
            word distance;

            if (decode_bit(&decoder, &prob_rep_g0[state]) == 0) {
                if (decode_bit(&decoder, &prob_rep0_long[state][pos_state]) == 0) {
                    if (output_pos == 0 || rep0 >= output_pos) {
                        return 1;
                    }
                    update_short_rep_state(&state);
                    previous_byte = output[output_pos - rep0 - 1];
                    output[output_pos++] = previous_byte;
                    continue;
                }
                distance = rep0;
            } else {
                if (decode_bit(&decoder, &prob_rep_g1[state]) == 0) {
                    distance = rep1;
                } else {
                    if (decode_bit(&decoder, &prob_rep_g2[state]) == 0) {
                        distance = rep2;
                    } else {
                        distance = rep3;
                        rep3 = rep2;
                    }
                    rep2 = rep1;
                }
                rep1 = rep0;
                rep0 = distance;
            }

            length = (word)(decode_length(&decoder, &rep_len_model, pos_state) + LZ_MATCH_MIN);
            update_rep_state(&state);

            if (rep0 >= output_pos) {
                return 1;
            }

            while (length-- && output_pos < output_size) {
                previous_byte = output[output_pos - rep0 - 1];
                output[output_pos++] = previous_byte;
            }
            continue;
        }

        {
            word length;
            word pos_slot;
            word distance;
            word len_to_pos_state;

            length = (word)(decode_length(&decoder, &len_model, pos_state) + LZ_MATCH_MIN);
            update_match_state(&state);

            len_to_pos_state = (length < 6) ? (word)(length - 2) : (word)3;
            pos_slot = decode_bit_tree(&decoder, prob_pos_slot[len_to_pos_state], LZ_POS_SLOT_BITS);

            if (pos_slot < 4) {
                distance = pos_slot;
            } else {
                word num_direct_bits;
                num_direct_bits = (word)((pos_slot >> 1) - 1);
                distance = (word)(2 | (pos_slot & 1));
                distance = (word)(distance << num_direct_bits);

                if (pos_slot < LZ_END_POS_MODEL_INDEX) {
                    word base;
                    word *special_probs;
                    base = (word)(distance - pos_slot - 1);
                    if (base >= LZ_POS_SPECIAL_PROBS) {
                        return 1;
                    }
                    special_probs = prob_pos_special + base;
                    distance = (word)(distance + decode_bit_tree_reverse(&decoder, special_probs, (byte)num_direct_bits));
                } else {
                    dword direct_result;
                    direct_result = decode_direct_bits(&decoder, (byte)(num_direct_bits - LZ_ALIGN_BITS));
                    if (direct_result > 0xFFFFUL) {
                        return 1;
                    }
                    distance = (word)(distance + ((word)direct_result << LZ_ALIGN_BITS));
                    distance = (word)(distance + decode_bit_tree_reverse(&decoder, prob_pos_align, LZ_ALIGN_BITS));
                }
            }

            rep3 = rep2;
            rep2 = rep1;
            rep1 = rep0;
            rep0 = distance;

            if (rep0 >= output_pos) {
                return 1;
            }

            while (length-- && output_pos < output_size) {
                previous_byte = output[output_pos - rep0 - 1];
                output[output_pos++] = previous_byte;
            }
        }
    }

    if (decoder.error || output_pos != output_size) {
        return 1;
    }

    return 0;
}

byte show_media(fs_node_t *parent, const char *path) {
    fs_node_t *file;
    byte *compressed_data;
    word i;

    file = fs_lookup(parent, path);
    if (file == 0) {
        return 1;
    }

    compressed_data = (byte*)halloc(file->size);
    if (compressed_data == 0) {
        return 2;
    }

    if (fs_read_file(file, compressed_data, file->size) != file->size) {
        hfree(compressed_data);
        return 2;
    }

    if (file->size == RENDER_BUFFER_SIZE) {
        for (i = 0; i < RENDER_BUFFER_SIZE; i++) {
            render_buffer[i] = compressed_data[i];
        }
    } else {
        if (decompress_lzma_raw(compressed_data, file->size, render_buffer, RENDER_BUFFER_SIZE)) {
            hfree(compressed_data);
            return 3;
        }
    }

    tui_render_adr((word)render_buffer);

    hfree(compressed_data);
    return 0;
}

const byte* compress_media(const byte* data, word size, word* out_size) {
    byte *compressed;
    word compressed_size;

    if (data == 0 || out_size == 0) {
        return 0;
    }

    if (size != RENDER_BUFFER_SIZE) {
        *out_size = 0;
        return 0;
    }

    compressed = 0;
    compressed_size = 0;
    if (compress_lzma_raw(data, size, &compressed, &compressed_size)) {
        *out_size = 0;
        return 0;
    }

    *out_size = compressed_size;
    return compressed;
}
