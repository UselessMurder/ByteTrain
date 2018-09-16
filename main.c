#include <stdint.h>

const uint8_t BT_CODE_BYTE = 1;
const uint8_t BT_CODE_WORD = 2;
const uint8_t BT_CODE_DWORD = 3;
const uint8_t BT_CODE_QWORD = 4;
const uint8_t BT_CODE_BYTE_BUFFER = 5;
const uint8_t BT_CODE_WORD_BUFFER = 6;
const uint8_t BT_CODE_DWORD_BUFFER = 7;
const uint8_t BT_CODE_QWORD_BUFFER = 8;
const uint8_t BT_CODE_HANDLER_BEGIN = 11;
const uint8_t BT_CODE_HANDLER_END = 12;
const uint8_t BT_CODE_BREAK = 13;

const uint16_t BT_CHAIN_MAGIC = 1;
const uint16_t BT_STRUCT_MAGIC = 2;

#define BT_ERROR_SUCCESS 0

#define RET_IF_ERROR(code) if (code != BT_ERROR_SUCCESS) return code

typedef struct byte_train {
    void *writer;
    uint32_t (*write) (void *, uint8_t *, uint64_t);
} byte_train;

void bt_set_writer(byte_train *bt, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
void bt_get_writer(byte_train *bt, void **writer, uint32_t (**write) (void *, uint8_t *, uint64_t));

uint32_t bt_write_byte_to_specific_writer(uint8_t byte, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_word_to_specific_writer(uint16_t word, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_dword_to_specific_writer(uint32_t dword, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_qword_to_specific_writer(uint64_t qword, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_buffer_to_specific_writer(uint8_t *buffer, uint64_t size, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_handler_begin_to_specific_writer(uint16_t magic, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_handler_end_to_specific_writer(void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));
uint32_t bt_write_break_to_specific_writer(uint8_t level, uint8_t count, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t));

uint32_t bt_write_byte(byte_train *bt, uint8_t byte);
uint32_t bt_write_word(byte_train *bt, uint16_t word);
uint32_t bt_write_dword(byte_train *bt, uint32_t dword);
uint32_t bt_write_qword(byte_train *bt, uint64_t qword);
uint32_t bt_write_buffer(byte_train *bt, uint8_t *buffer, uint64_t size);
uint32_t bt_write_handler_begin(byte_train *bt, uint16_t magic);
uint32_t bt_write_handler_end(byte_train *bt);
uint32_t bt_write_break(byte_train *bt, uint8_t level, uint8_t count);

uint8_t bt_get_length_per_bytes(uint64_t value) {
    if (value < 0xF)
      return 1;
    if (value < 0xFF)
      return 2;
    if (value < 0xFFFF)
      return 4;
    return 8; 
}

void bt_set_writer(byte_train *bt, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    bt->writer = writer;
    bt->write = write;
}
void bt_get_writer(byte_train *bt, void **writer, uint32_t (**write) (void *, uint8_t *, uint64_t)) {
    *writer = bt->writer;
    *write = bt->write;
}

uint32_t bt_write_byte_to_specific_writer(uint8_t byte, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_BYTE, 1);
    RET_IF_ERROR(code);
    code = write(writer, &byte, 1);
    return code;
}
uint32_t bt_write_word_to_specific_writer(uint16_t word, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_WORD, 1);
    RET_IF_ERROR(code);
    code = write(writer, (uint8_t *)&word, 2);
    return code;
}
uint32_t bt_write_dword_to_specific_writer(uint32_t dword, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_DWORD, 1);
    RET_IF_ERROR(code);
    code = write(writer, (uint8_t *)&dword, 4);
    return code;
}
uint32_t bt_write_qword_to_specific_writer(uint64_t qword, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_QWORD, 1);
    RET_IF_ERROR(code);
    code = write(writer, (uint8_t *)&qword, 8);
    return code;
}
uint32_t bt_write_buffer_to_specific_writer(uint8_t *buffer, uint64_t size, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    switch(bt_get_length_per_bytes(size)) {
        case 1: {
            code = write(writer, (uint8_t *)&BT_CODE_BYTE_BUFFER, 1);
            RET_IF_ERROR(code);
            uint8_t b_size = (uint8_t)size;
            code = write(writer, &b_size, 1);
            RET_IF_ERROR(code);
            break;
        }
        case 2: {
            code = write(writer, (uint8_t *)&BT_CODE_WORD_BUFFER, 1);
            RET_IF_ERROR(code);
            uint16_t w_size = (uint16_t)size;
            write(writer, (uint8_t *)&w_size, 2);
            RET_IF_ERROR(code);
            break;
        }
        case 4: {
            code = write(writer, (uint8_t *)&BT_CODE_DWORD_BUFFER, 1);
            RET_IF_ERROR(code);
            uint32_t d_size = (uint32_t)size;
            code = write(writer, (uint8_t *)&d_size, 4);
            RET_IF_ERROR(code);
            break;
        }
        case 8: {
            code = write(writer, (uint8_t *)&BT_CODE_QWORD_BUFFER, 1);
            RET_IF_ERROR(code);
            code = write(writer, (uint8_t *)&size, 8);
            RET_IF_ERROR(code);
            break;
        }
    }
    code = write(writer, buffer, size);
    return code;
}
uint32_t bt_write_handler_begin_to_specific_writer(uint16_t magic, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_HANDLER_BEGIN, 1);
    RET_IF_ERROR(code);
    code = write(writer, (uint8_t *)&magic, 2);
    return code;
}
uint32_t bt_write_handler_end_to_specific_writer(void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_HANDLER_END, 1);
    return code;
}
uint32_t bt_write_break_to_specific_writer(uint8_t level, uint8_t count, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) { 
    uint32_t code = 0;
    code = write(writer, (uint8_t *)&BT_CODE_BREAK, 1);
    RET_IF_ERROR(code);
    code = write(writer, &level, 1);
    RET_IF_ERROR(code);
    code = write(writer, &count, 1);
    return code;
}

uint32_t bt_write_byte(byte_train *bt, uint8_t byte) {
    return bt_write_byte_to_specific_writer(byte, bt->writer, bt->write);
}
uint32_t bt_write_word(byte_train *bt, uint16_t word) {
    return bt_write_word_to_specific_writer(word, bt->writer, bt->write);
}
uint32_t bt_write_dword(byte_train *bt, uint32_t dword) {
    return bt_write_dword_to_specific_writer(dword, bt->writer, bt->write);
}
uint32_t bt_write_qword(byte_train *bt, uint64_t qword) {
    return bt_write_dword_to_specific_writer(qword, bt->writer, bt->write);
}
uint32_t bt_write_buffer(byte_train *bt, uint8_t *buffer, uint64_t size) {
    return bt_write_buffer_to_specific_writer(buffer, size, bt->writer, bt->write);
}
uint32_t bt_write_handler_begin(byte_train *bt, uint16_t magic) {
    return bt_write_handler_begin_to_specific_writer(magic, bt->writer, bt->write);
}
uint32_t bt_write_handler_end(byte_train *bt) {
    return bt_write_handler_end_to_specific_writer(bt->writer, bt->write);
}
uint32_t bt_write_break(byte_train *bt, uint8_t level, uint8_t count) {
    return bt_write_break_to_specific_writer(level, count, bt->writer, bt->write);
}

int main() {
    return 0;
}
