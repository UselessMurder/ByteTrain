#include "bt.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

typedef struct file_writer {
    int fd;
} file_writer;

uint32_t write_to_file(void *writer, uint8_t *buf, uint64_t count) {
  uint32_t size = write(((file_writer*)writer)->fd, buf, count);
  if (size != (uint32_t)count) {
      return BT_ERROR_TIMEOUT;
  }
  return BT_ERROR_SUCCESS;
} 

unsigned char *gen_rdm_bytestream (size_t num_bytes)
{
    unsigned char *stream = malloc(num_bytes);
    printf("%p\n", stream);
    size_t i;
    for (i = 0; i < num_bytes; i++) {
        stream[i] = rand();
    }
    return stream;
}


int main() {
    int fd = open("hello", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return 1;
    file_writer fw = {fd};
    byte_train bt = {&fw, write_to_file}; 
    uint8_t byte = 0x32;
    if(bt_write_byte(&bt, byte)) {
        printf("fail\n");
        return 1;
    }
    uint16_t word = 0xFC16;
    if(bt_write_word(&bt, word)) {
        printf("fail\n");
        return 1;
    }
    uint32_t dword = 0xCCABBADC;
    if(bt_write_dword(&bt, dword)) {
        printf("fail\n");
        return 1;
    }
    uint64_t qword = 0xDDDDDDDDFFFFFFFF;
    if(bt_write_qword(&bt, qword)) {
        printf("fail\n");
        return 1;
    }
    srand((unsigned int)time(NULL));
    uint8_t *arr = NULL;
    arr = gen_rdm_bytestream(0xF3);
    if(bt_write_buffer(&bt, arr, 0xF3)) {
        printf("fail\n");
        return 1;
    }
    free(arr); 
    arr = gen_rdm_bytestream(0xFF3);
    if(bt_write_buffer(&bt, arr, 0xFF3)) {
        printf("fail\n");
        return 1;
    }
    free(arr); 
    arr = gen_rdm_bytestream(0xFFFF3);
    if(bt_write_buffer(&bt, arr, 0xFFFF3)) {
        printf("fail\n");
        return 1;
    }
    free(arr); 
    printf("done\n");
    return 0;
}
