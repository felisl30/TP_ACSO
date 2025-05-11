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


// int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
//     // Verificar argumentos válidos
//     if (fs == NULL || inp == NULL || inumber < 1) {
//         return -1;
//     }

//     // Calcular la ubicación del inodo
//     int inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
//     int block_number = INODE_START_SECTOR + ((inumber - 1) / inodes_per_block);
//     int offset = ((inumber - 1) % inodes_per_block) * sizeof(struct inode);
//     long int abs_offset = (block_number * BLOCK_SIZE) + offset;

//     // Leer el inodo directamente usando pread
//     long int bytes_read = pread(fs->dfd, inp, sizeof(struct inode), abs_offset);


//     // int bytes_read = diskimg_readsector(fs->dfd, block_number, inp);
//     if (bytes_read != sizeof(struct inode)) {
//         return -1;
//     }


//     return 0;
// }



int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (fs == NULL || inp == NULL || inumber < 1) {
        return -1;
    }

    // Calcular el sector y offset dentro del sector
    int inodes_per_block = BLOCK_SIZE / sizeof(struct inode);
    int sector_number = INODE_START_SECTOR + ((inumber - 1) / inodes_per_block);
    int offset = ((inumber - 1) % inodes_per_block) * sizeof(struct inode);

    // Buffer temporal para el sector completo
    char sector_buffer[BLOCK_SIZE];
    
    // Leer el sector completo
    if (diskimg_readsector(fs->dfd, sector_number, sector_buffer) != BLOCK_SIZE) {
        return -1;
    }

    // Copiar solo el inode que necesitamos
    memcpy(inp, sector_buffer + offset, sizeof(struct inode));
    return 0;
}


/**
 * TODO
 */
// int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  

//     if ((inp->i_mode & ILARG)!= 0){
//         uint16_t indirect_block[256];

//         int aux = blockNum;
//         for (int i = 0; i < 7; i++){

//             if(blockNum < 256){
            
//                 long int bytes_read = pread(fs->dfd, indirect_block, BLOCK_SIZE, inp->i_addr[i] * BLOCK_SIZE);
//                 // long int bytes_read = diskimg_writesector(fs->dfd, inp->i_addr[i] * BLOCK_SIZE, indirect_block);
                
//                 if (bytes_read != BLOCK_SIZE) {
//                     return -1;
//                 }
//                 // int offset = blockNum % 256;
//                 return indirect_block[blockNum];
    
//             }
//             blockNum -=256;

//         }

        

//         if(blockNum < 256 * 256){
//             uint16_t indirect_block_idx[256];
//             long int bytes_read_idx = pread(fs->dfd, indirect_block_idx, BLOCK_SIZE, inp->i_addr[7] * BLOCK_SIZE);
//             // long int bytes_read_idx = diskimg_writesector(fs->dfd, inp->i_addr[7] * BLOCK_SIZE, indirect_block);

            
//             if (bytes_read_idx != BLOCK_SIZE) {
//                 return -1;
//             }
           
//             int block_idx = blockNum/256;
//             int idx = indirect_block_idx[block_idx];
//             long int bytes_read = pread(fs->dfd, indirect_block, BLOCK_SIZE, idx * BLOCK_SIZE);
//             // long int bytes_read = diskimg_writesector(fs->dfd, idx * BLOCK_SIZE, indirect_block);
            
//             if (bytes_read != BLOCK_SIZE) {
//                 return -1;
//             }
            
//             int real_addr = blockNum%256;
//             return indirect_block[real_addr];

//         }
//         return -1;


//     }

//     return inp->i_addr[blockNum];



// }


int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {  
    if ((inp->i_mode & ILARG) != 0) {
        uint16_t indirect_block[256];
        
        // Manejo de los primeros 7 bloques indirectos
        for (int i = 0; i < 7; i++) {
            if (blockNum < 256) {
                if (diskimg_readsector(fs->dfd, inp->i_addr[i], indirect_block) != BLOCK_SIZE) {
                    return -1;
                }
                return indirect_block[blockNum];
            }
            blockNum -= 256;
        }

        // Manejo del bloque doblemente indirecto
        if (blockNum < 256 * 256) {
            uint16_t indirect_block_idx[256];
            
            // Leer el bloque de índices
            if (diskimg_readsector(fs->dfd, inp->i_addr[7], indirect_block_idx) != BLOCK_SIZE) {
                return -1;
            }
            
            int block_idx = blockNum / 256;
            int idx = indirect_block_idx[block_idx];
            
            // Leer el bloque de direcciones
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
