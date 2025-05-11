#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

#define BLOCK_SIZE 512

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode ino;

    if (inode_iget(fs, inumber, &ino) == -1){
        return -1;
    }

    int file_size = inode_getsize(&ino);
    if (blockNum * BLOCK_SIZE >= file_size) {
        return -1;
    }

    int real_addr = inode_indexlookup(fs, &ino, blockNum);
    if (real_addr == -1){
        return -1;
    }

    // leeo el bloque completo
    if (diskimg_readsector(fs->dfd, real_addr, buf) != BLOCK_SIZE) {
        return -1;
    }

    int bytes_block = BLOCK_SIZE;

    if (blockNum * BLOCK_SIZE + BLOCK_SIZE > file_size) {
        bytes_block = file_size - (blockNum * BLOCK_SIZE);
    }

    return bytes_block;

}

