#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define INODE_START_BLOCK 2
#define BLOCK_SIZE 512

/**
 * Looks up the specified name (name) in the specified directory (dirinumber).  
 * If found, return the directory entry in space addressed by dirEnt.  Returns 0
 * on success and something negative on failure. 
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  struct inode ino;
  int num = inode_iget(fs, dirinumber, &ino);
  if (num == -1) return -1;

  int filesize = ((int)ino.i_size0 << 16) | ino.i_size1;
  int numblocks = (filesize + BLOCK_SIZE - 1) / BLOCK_SIZE;

  char buffer[BLOCK_SIZE];
  struct direntv6 *dir_entry;
  
  for (int blknum = 0; blknum < numblocks; blknum++) {
      int bytes_read = file_getblock(fs, dirinumber, blknum, buffer);
      if (bytes_read <= 0) return -1;

      dir_entry = (struct direntv6 *)buffer;
      int entries_per_block = bytes_read / sizeof(struct direntv6);

      for (int i = 0; i < entries_per_block; i++) {
          if (dir_entry[i].d_inumber != 0) {
              if (strcmp(dir_entry[i].d_name, name) == 0) {
                  dirEnt->d_inumber = dir_entry[i].d_inumber;
                  strcpy(dirEnt->d_name, dir_entry[i].d_name);
                  return 0;
              }
          }
      }
  }

  return -1;
}