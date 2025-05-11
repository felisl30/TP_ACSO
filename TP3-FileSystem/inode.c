#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include "inode.h"
#include "diskimg.h"


/**
 * TODO
 */

#define BLOCK_SIZE 512


int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (fs == NULL || inp == NULL || inumber < 1) {
        return -1;
    }

    int inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
    int sector_number = INODE_START_SECTOR + ((inumber - 1) / inodes_per_block);
    int offset = ((inumber - 1) % inodes_per_block) * sizeof(struct inode);

    char sector_buffer[BLOCK_SIZE];
    
    if (diskimg_readsector(fs->dfd, sector_number, sector_buffer) != BLOCK_SIZE) {
        return -1;
    }

    // copio solo el inode que necesito
    memcpy(inp, sector_buffer + offset, sizeof(struct inode));
    return 0;
}




int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  
    if ((inp->i_mode & ILARG) != 0) {
        uint16_t indirect_block[256];
        
        // indirecto
        for (int i = 0; i < 7; i++) {
            if (blockNum < 256) {
                if (diskimg_readsector(fs->dfd, inp->i_addr[i], indirect_block) != BLOCK_SIZE) {
                    return -1;
                }
                return indirect_block[blockNum];
            }
            blockNum -= 256;
        }

        // doble indirecto
        if (blockNum < 256 * 256) {
            uint16_t indirect_block_idx[256];
            
            if (diskimg_readsector(fs->dfd, inp->i_addr[7], indirect_block_idx) != BLOCK_SIZE) {
                return -1;
            }
            
            int block_idx = blockNum / 256;
            int idx = indirect_block_idx[block_idx];
            
            if (diskimg_readsector(fs->dfd, idx, indirect_block) != BLOCK_SIZE) {
                return -1;
            }
            
            return indirect_block[blockNum % 256];
        }
        return -1;
    }

    return inp->i_addr[blockNum];
}



int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
