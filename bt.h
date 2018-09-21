#include <stdint.h>

#define BT_CODE_BYTE 1
#define BT_CODE_WORD 2
#define BT_CODE_DWORD 3
#define BT_CODE_QWORD 4
#define BT_CODE_BYTE_BUFFER 5
#define BT_CODE_WORD_BUFFER 6
#define BT_CODE_DWORD_BUFFER 7
#define BT_CODE_QWORD_BUFFER 8
#define BT_CODE_HANDLER_BEGIN 9
#define BT_CODE_HANDLER_END 10
#define BT_CODE_BREAK 11

#define BT_U_CODE_BYTE 1
#define BT_U_CODE_WORD 2
#define BT_U_CODE_DWORD 3
#define BT_U_CODE_QWORD 4
#define BT_U_CODE_BUFFER 5
#define BT_U_CODE_ARRAY 6
#define BT_U_CODE_ERROR 7

#define BT_CHAIN_MAGIC = 1
#define BT_ARRAY_MAGIC = 2

#define BT_ERROR_SUCCESS 0
#define BT_ERROR_TIMEOUT 1

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
