#include "bt.h"

#define RET_IF_ERROR(code) if (code != BT_ERROR_SUCCESS) return code

uint8_t bt_get_length_per_bytes(uint64_t value) {
    if (value <= 0xFF)
      return 1;
    if (value <= 0xFFFF)
      return 2;
    if (value <= 0xFFFFFFFF)
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
    uint8_t payload[2] = {BT_CODE_BYTE, byte};
    return write(writer, payload, 2);
}
uint32_t bt_write_word_to_specific_writer(uint16_t word, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint8_t payload[3];
    payload[0] = BT_CODE_WORD;
    *((uint16_t *)(payload + 1)) = word;
    return write(writer, payload, 3);
}
uint32_t bt_write_dword_to_specific_writer(uint32_t dword, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint8_t payload[5];
    payload[0] = BT_CODE_DWORD;
    *((uint32_t *)(payload + 1)) = dword;
    return write(writer, payload, 5);
}
uint32_t bt_write_qword_to_specific_writer(uint64_t qword, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint8_t payload[9];
    payload[0] = BT_CODE_QWORD;
    *((uint64_t *)(payload + 1)) = qword;
    return write(writer, payload, 9);
}
uint32_t bt_write_buffer_to_specific_writer(uint8_t *buffer, uint64_t size, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint32_t code = 0;
    uint8_t payload[9];
    switch(bt_get_length_per_bytes(size)) {
        case 1: {
            payload[0] = BT_CODE_BYTE_BUFFER;
            payload[1] = (uint8_t)size;
            code = write(writer, payload, 2);
            RET_IF_ERROR(code);
            break;
        }
        case 2: {
            payload[0] = BT_CODE_WORD_BUFFER;
            *((uint16_t *)(payload + 1)) = (uint16_t)size;
            code = write(writer, payload, 3);
            RET_IF_ERROR(code);
            break;
        }
        case 4: {
            payload[0] = BT_CODE_DWORD_BUFFER;
            *((uint32_t *)(payload + 1)) = (uint32_t)size;
            code = write(writer, payload, 5);
            RET_IF_ERROR(code);
            break;
        }
        case 8: {
            payload[0] = BT_CODE_QWORD_BUFFER;
            *((uint64_t *)(payload + 1)) = size;
            code = write(writer, payload, 9);
            RET_IF_ERROR(code);
            break;
        }
    }
    return write(writer, buffer, size);
}
uint32_t bt_write_handler_begin_to_specific_writer(uint16_t magic, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint8_t payload[3];
    payload[0] = BT_CODE_HANDLER_BEGIN;
    *((uint16_t *)(payload + 1)) = magic;
    return write(writer, payload, 3);
}
uint32_t bt_write_handler_end_to_specific_writer(void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) {
    uint8_t code = BT_CODE_HANDLER_END;
    return write(writer, &code, 1);
}
uint32_t bt_write_break_to_specific_writer(uint8_t level, uint8_t count, void *writer, uint32_t (*write) (void *, uint8_t *, uint64_t)) { 
    uint8_t payload[3] = {BT_CODE_BREAK, level, count};
    return write(writer, payload, 3);
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
    return bt_write_qword_to_specific_writer(qword, bt->writer, bt->write);
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
