/*
 *  Copyright (C) 2021 CS416 Rutgers CS
 *	Tiny File System
 *	File:	tfs.c
 *
 */

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <libgen.h>
#include <limits.h>

#include "block.h"
#include "tfs.h"

char diskfile_path[PATH_MAX];

bitmap_t ibitmap; // in memory inode bitmap
bitmap_t dbitmap; // in memory data bitmap
struct superblock sb; // in memory superblock
struct inode *inodes[MAX_INUM+1]; // in memory inode array; +1 for 0th inode (invalid)

// Declare your in-memory data structures here

/*
 * Get available inode number from bitmap
 */
int get_avail_ino() {

 	// Step 1: Read inode bitmap from disk

 	bio_read(sb->i_bitmap_blk, ibitmap);

 	// Step 2: Traverse inode bitmap to find an available slot

 	int stat = -1;

 	for (int i = 0; i < MAX_INUM; i++){
 		if (get_bitmap(i_bitmap_blk, i) == 0){
 			stat = i;
 			break;
 		}
 	}

 	// Step 3: Update inode bitmap and write to disk

 	if (stat != -1){
 		set_bitmap(i_bitmap_blk, i);
 		bio_write(sb->i_bitmap_blk, ibitmap);
 	}

 	return stat;
 }

 /*
  * Get available data block number from bitmap
  */
int get_avail_blkno() {

 	// Step 1: Read data bitmap from disk

 	bio_read(sb->d_bitmap_blk, dbitmap);

 	// Step 2: Traverse data bitmap to find an available slot

 	int stat = -1;

 	for (int i = 0; i < MAX_DNUM; i++){
 		if (get_bitmap(d_bitmap_blk, i) == 0){
 			stat = i;
 			break;
 		}
 	}

 	// Step 3: Update inode bitmap and write to disk

 	if (stat != -1){
 		set_bitmap(d_bitmap_blk, i);
 		bio_write(sb->d_bitmap_blk, dbitmap);
 	}

 	return stat;
 }

	// Step 3: Update data block bitmap and write to disk

	if(index = -1){
		return -1;
	}else{
		set_bitmap(bitmap, index);
		bio_write(sb->d_bitmap_blk, bitmap);
	}

	return index;
}

/*
 * inode operations
 */
int readi(uint16_t ino, struct inode *inode) {

  // Step 1: Get the inode's on-disk block number

	int block = ino / (BLOCK_SIZE/sizeof(struct inode));

  // Step 2: Get offset of the inode in the inode on-disk block

	int offset = ino % (BLOCK_SIZE/sizeof(struct inode));

  // Step 3: Read the block from disk and then copy into inode structure

	struct inode *temp = malloc(BLOCK_SIZE);

	bio_read(block, temp);

	memcpy(inode, temp[offset], sizeof(struct inode));

	free(temp);

	return 0;
}

int writei(uint16_t ino, struct inode *inode) {

	// Step 1: Get the block number where this inode resides on disk

	int block = ino / (BLOCK_SIZE/sizeof(struct inode));

	// Step 2: Get the offset in the block where this inode resides on disk

	int offset = ino % (BLOCK_SIZE/sizeof(struct inode));

	// Step 3: Write inode to disk

	struct inode *temp = malloc(BLOCK_SIZE);

	bio_write(block, temp);

	memcpy(inode, temp[offset], sizeof(struct inode));

	free(temp);

	return 0;
}


/*
 * directory operations
 */
 //later
int dir_find(uint16_t ino, const char *fname, size_t name_len, struct dirent *dirent) {

	char *fp = strdup(fname);
	char *bp = basename(fp);
  // Step 1: Call readi() to get the inode using ino (inode number of current directory)
	uint16_t validity = searchFile(inodes[0], inodes[0]->ino, fp);

	if(validity > 0){
		readi(validity, inodes[ino - 1]);
		dirent->ino = validity;
		dirent->valid = 1;
		strcpy(dirent->name, bp);
		return 0;
	}else{
		return -1;
	}

  // Step 2: Get data block of current directory from inode


  // Step 3: Read directory's data block and check each directory entry.
  //If the name matches, then copy directory entry to dirent structure

	return 0;
}
//later
int dir_add(struct inode dir_inode, uint16_t f_ino, const char *fname, size_t name_len) {


  int numEntries = (int)(BLOCK_SIZE / sizeof(struct dirent));
  int start;
  int ptr;
  int off;
  struct dirent *buffer = (struct dirent *)malloc(BLOCK_SIZE);
  dir_inode.link = dir_inode.link + 1;

  if(dir_inode.link < 15){
    start = 3;
    ptr = 0;
  }else{
    start = 0;
    if(dir_inode.link == 15){
      ptr = 1;
    }else{
      ptr = (int)((dir_inode.link + 1) / 16);
    }
  }

  if(ptr > 0 && dir_inode.direct_ptr[ptr] == -1){
    dir_inode.direct_ptr[ptr] = get_avail_blkno();
    off = sb->d_start_blk + dir_inode.direct_ptr[ptr];

    for(int i = 0; i < numEntries; i++){
      buffer[i].valid = 0;
    }
    bio_write(off, buffer);
    start = 0;
  }else{
    off = sb->d_start_blk + dir_inode.direct_ptr[ptr]
  }
  memset(buffer, 0, BLOCK_SIZE);
  bio_read(off, buffer);

  char *basePath = strdup(fname);
  char *base = basename(basePath);
  char toAdd[name_len];
  strcpy(toAdd, base);
  toAdd[name_len - 1] = '\0';

  for(int i = 0; i < numEntries; i++){
    if(buffer[i].valid == 0){
      buffer[i].valid = 1;
      buffer[i].ino = f_ino;
      strcpy(buffer[i].name, base);
      break;
    }
  }
  bio_write(off, buffer);
  writei(dir_inode.ino, &dir_inode);
  free(buffer);
  buffer = NULL;



	// Step 1: Read dir_inode's data block and check each directory entry of dir_inode

	// Step 2: Check if fname (directory name) is already used in other entries

	// Step 3: Add directory entry in dir_inode's data block and write to disk

	// Allocate a new data block for this directory if it does not exist

	// Update directory inode

	// Write directory entry

	return 0;
}
//later
int dir_remove(struct inode dir_inode, const char *fname, size_t name_len) {

  char *pn = strdup(fname);
  char *pb = strdup(fname);
  char *base = basename(pb);
  int numEntries = (int)(BLOCK_SIZE / (sizeof(struct dirent)));
  struct inode toDel;

  uint16_t ino = searchFile(inodes[0], inodes[0]->ino, pn);
  if(ino == 0 || inodes[ino - 1]->link > 2){//might need to make 2 if statements
    return -1;
  }
  dir_inode.link = dir_inode.link - 1;
  int off = sb->d_start_blk + dir_inode.direct_ptr[0];
  struct dirent *buffer = (struct dirent *)malloc(BLOCK_SIZE);
  bio_read(off, buffer);

  for(int i = 0; i < numEntries; i++){
    if(strcmp(buffer[i].name, base) == 0){
      buffer[i].valid = 0;
    }
  }

  unset_bitmap(i_bitmap, toDel.ino);

  bio_write(off, buffer);
  writei(inodes[dir_inode.ino-1]->ino,inodes[dir_inode.ino-1]);
  writei(inodes[ino - 1]->ino, inodes[ino - 1]);
  bio_write(sb->i_bitmap_blk, i_bitmap);
  free(buffer);
  buffer = NULL;


	// Step 1: Read dir_inode's data block and checks each directory entry of dir_inode

	// Step 2: Check if fname exist

	// Step 3: If exist, then remove it from dir_inode's data block and write to disk

	return 0;
}

/*
 * namei operation
 */
 //later
int get_node_by_path(const char *path, uint16_t ino, struct inode *inode) {

  const char *fp1 = strdup(path);
  char *fp2 = strdup(path);
  int retrieved;

  uint16_t nino = searchFile(inodes[0], inodes[0]->ino, fp2);

  if(nino > 0){
    struct dirent *dir = (struct dirent *)malloc(sizeof(dirent));
    retrieved = dir_find(1, fp1, (strlen(fp1) + 1), dir);

    if(retrieved == -1){
      free(dir);
      dir = -1;
      return -1
    }
    readi(inodes[(dir->ino - 1)]->ino, inodes[(dir->ino - 1)]);
    readi(inodes[(dir->ino - 1)]->ino, inode);
    inode->ino = inodes[(dir->ino - 1)]->ino;
    inode->valid = dir->valid;
    inode->size = inodes[(dir->ino - 1)]->size;
    inode->type = inodes[(dir->ino - 1)]->type;
    inode->link = inodes[(dir->ino - 1)]->link;

    for(int i = 0; i < 16; i++){
      inode->direct_ptr[i] = inodes[(dir->ino - 1)]->direct_ptr[i];
      if(i < 8){
        inode->direct_ptr[i] = inodes[(dir->ino - 1)]->indirect_ptr[i];
      }
    }

    writei(inodes[(dir->ino - 1)]->ino, inodes[(dir->ino - 1)]);
    free(dir);
    dir = NULL;
    return 0;


  }else{
    return -1;
  }

	// Step 1: Resolve the path name, walk through path, and finally, find its inode.

	// Note: You could either implement it in a iterative way or recursive way

	return 0;
}

/*
 * Make file system
 */
int tfs_mkfs() {

 	// Call dev_init() to initialize (Create) Diskfile

 	dev_init(diskfile_path);

 	// write superblock information

 	sb = (struct superblock *)calloc(1, BLOCK_SIZE);
 	sb->magic_num = MAGIC_NUM;
 	sb->max_inum = MAX_INUM;
 	sb->max_dnum = MAX_DNUM;
 	sb->i_bitmap_blk = 1;
 	sb->d_bitmap_blk = 2;
 	sb->i_start_blk = 3;
 	sb->d_start_blk = sb->i_start_blk + (sizeof(struct inode)*MAX_INUM / BLOCK_SIZE);

 	// initialize in memory inodes

 	for (int i = 0; i < MAX_INUM; i++){
 		inodes[i] = (struct inode)calloc(1, sizeof(struct inode));
 		for (int j = 0; j < DIR_PTR; j++){ // invalidate pointers
 			inodes[i]->direct_ptr[j] = -1;
 			if (j < INDIR_PTR) inodes[i]->indirect_ptr[j] = -1;
 		}
 	}

 	// initialize inode bitmap

 	ibitmap = (bitmap_t)calloc(1, BLOCK_SIZE);
 	get_avail_ino(); // sets 0th inode to in use

 	// initialize data block bitmap

 	dbitmap = (bitmap_t)calloc(1, BLOCK_SIZE);

 	for (i = 0; i < sb->d_start_blk; i++){
 		set_bitmap(dbitmap, i); // sets the blocks for sb, bitmaps, inodes, to in use
 	}

 	// update bitmap information for root directory

 	int root_node = get_avail_ino(); // 1st inode now in use

 	// update inode for root directory

	inodes[root_node]->root_node;
	inodes[root_node]->valid = 1;
	inodes[root_node]->size = BLOCK_SIZE;
	inodes[root_node]->type = DIR;
	inodes[root_node]->link = 2;
	inodes[root_node]->direct_ptr[0] = get_avail_blkno();
	inodes[root_node]->vstat.st_atime = time(NULL);
	inodes[root_node]->vstat.st_mtime = time(NULL);

	struct dirent *dir = (struct dirent *)calloc(1, BLOCK_SIZE);
	int maxEntries = BLOCK_SIZE/sizeof(struct dirent);z

	dir[0].ino = 1;
	dir[1].ino = 1;
	dir[2].ino = 1;
	dir[0].valid = 1;
	dir[1].valid = 1;
	dir[2].valid = 1;
	strcpy(dir[0].name, "/");
	strcpy(dir[1].name, ".");
	strcpy(dir[2].name, "..");

	for (int i = 3; i < maxEntries; i++){
		dir[i].valid = 0;
	}

	bio_write(inode[root_node]->direct_ptr[0]);

 	return 0;
}


/*
 * FUSE file operations
 */
static void *tfs_init(struct fuse_conn_info *conn) {

	// Step 1a: If disk file is not found, call mkfs
	if(dev_open(diskfile) == -1){
		tfs_mkfs();
	} else {
  // Step 1b: If disk file is found, just initialize in-memory data structures
  // and read superblock from disk

    ibitmap = malloc(BLOCK_SIZE);
    dbitmap = malloc(BLOCK_SIZE);
    sb = malloc(BLOCK_SIZE);

    bio_read(0, sb);
    if (sb->magic_num != MAGIC_NUM){
      puts("MAGIC_NUM Mismatch");
      exit(1);
    }
    bio_read(1, i_bitmap);
    bio_read(2, d_bitmap);
  }

	return NULL;
}

static void tfs_destroy(void *userdata) {

	// Step 1: De-allocate in-memory data structures

  bio_write(0, sb);
  bio_write(sb->i_bitmap_blk, i_bitmap);
  bio_write(sb->d_bitmap_blk, d_bitmap);



	free(sb);
	free(i_bitmap);
	free(d_bitmap);

	// Step 2: Close diskfile

	dev_close();

}

static int tfs_getattr(const char *path, struct stat *stbuf) {

	// Step 1: call get_node_by_path() to get inode from path

	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}

	// Step 2: fill attribute of file into stbuf from inode

		struct inode *ncur;

		if (readi(icur, ncur)){
			return -1;
		}

		memcpy(stbuf, &ncur->vstat, sizeof(struct stat));

		stbuf->st_mode   = S_IFDIR | 0755;
		stbuf->st_nlink  = 2;
		time(&stbuf->st_mtime);

	return 0;
}

static int tfs_opendir(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}
	// Step 2: If not find, return -1

    return 0;
}
//UNFINISHED
static int tfs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path
	int icur = get_node_by_path(path);
	if (icur == 0){
		return -1;
	}
	// Step 2: Read directory entries from its data blocks, and copy them to filler


	return 0;
}

//later
static int tfs_mkdir(const char *path, mode_t mode) {

  char *fp = strdup(path);
  char *rfp = strdup(path);
  char *fpd = strdup(path);
  char *parent = dirname(rfp);
  char *child = basename(fpd);
  int numEntries = (int)(BLOCK_SIZE/(sizeof(struct dirent)));
  struct dirent *newDir = (struct dirent *)malloc(BLOCK_SIZE);

  struct inode addDir;

  int found = get_node_by_path(parent, 1, &addDir);

  if(found == 0){
    int childI = get_avail_ino();

    dir_add(addDir, childI, fp, (strlen(fp) + 1));

    inodes[childI - 1]->ino = childI;
    inodes[childI - 1]->valid = 1;
    inodes[childI - 1]->size = BLOCK_SIZE;
    inodes[childI - 1]->type = DIR; //dk what DIR is
    inodes[childI - 1]->link = 2;
    (inodes[childI - 1]->vstat).st_atime = time(NULL); //?
    (inodes[childI - 1]->vstat).st_mtime = time(NULL);
    inodes[childI - 1]->vstat.st_mode = mode;

    int freeBlock - get_avail_blkno();
    inodes[childI - 1]->direct_ptr[0] = freeBlock;
    int off = sb->d_start_blk + freeBlock;

    newDir[0].valid = 1;
    newDir[0].ino = childI;
    strcpy(newDir[0].name, child);

    newDir[1].valid = 1;
    newDir[1].ino = childI;
    strcpy(newDir[0].name, ".");

    newDir[1].valid = addDir.ino;
    newDir[1].ino = 1;
    strcpy(newDir[0].name, "..");

    for(int i = 0; i < numEntries; i++){
      newDir[i].valid = 0;
    }

    bio_write(off, newDir);
    writei(childI, inodes[childI - 1]);
    writei(addDir.ino, inodes[addDir.ino -1]);

    free(newDir);
    newDir = NULL;
    return 0;
  }
	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target directory to parent directory

	// Step 5: Update inode for target directory

	// Step 6: Call writei() to write inode to disk
  return -1;

	return 0;
}
//later
static int tfs_rmdir(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

  char *base_name = basename(path);
  char *dir_name  = dirname(path);

	// Step 2: Call get_node_by_path() to get inode of target directory

  struct inode *inode;
  if (get_node_by_path(dir_name, ROOT, inode) == -1){
    puts("Error: Directory not found in tfs_rmdir");
    exit(1);
  }

	// Step 3: Clear data block bitmap of target directory

  for (int i = 0; i < inode->link; i++){
    if (i < 16){
      if (inode->direct_ptr[i] != -1){
        unset_bitmap(d_bitmap, i);
      }
    } else {
      if (inode->indirect_ptr[i-16] != 1){
        unset_bitmap(d_bitmap, i);
      }
    }
  }

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target directory in its parent directory

	return 0;
}

static int tfs_releasedir(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
  char *rcp = strdup(path);
  char *rcpd = strdup(path);
  char *pp = dirname(rcp);

  struct inode parent;

  int found = get_node_by_path(pp, 1, &parent);

  if(found == 0){

    size_t name = (size_t)(strlen(rcpd) + 1);
    uint16_t freeBlock = get_avail_ino();

    dir_add(parent, freeBlock, rcpd, name);

    inodes[freeBlock - 1]->ino = freeBlock;
    inodes[freeBlock - 1]->valid = 1;
    inodes[freeBlock - 1]->size = 0;
    inodes[freeBlock - 1]->type = FILE;
    inodes[freeBlock - 1]->link = 1;
    (inodes[freeBlock - 1]->vstat).st_atime = time(NULL);
    (inodes[freeBlock - 1]->vstat).st_mtime = time(NULL);
    inodes[freeBlock - 1]->vstat.st_mode = mode;

    for(int i = 0; i < DIRECT_PTRS; i++){//dk direct_ptrs
      inodes[freeBlock - 1]->direct_ptr[i] = -1;
      if(i < INDIRECT_PTRS){
        inodes[freeBlock - 1]->indirect_ptr[i] = -1;
      }
    }
    int numEntries = (int)(BLOCK_SIZE/(sizeof(struct dirent)));
    struct dirent *dirEntries = (struct dirent *)malloc(BLOCK_SIZE);

    for(int i = 0; i < numEntries; i++){
      dirEntries[i].valid = 0;
    }

    int freeDataBlock = get_avail_blkno();
    int off = sb->d_start_blk + freeDataBlock;

    inodes[freeBlock]->direct_ptr[0] = freeDataBlock;

    bio_write(off, dirEntries);
    writei(freeBlock, inodes[freeBlock - 1]);

    free(dirEntries);
    dirEntries = NULL;
    return 0;
  }
	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target file to parent directory

	// Step 5: Update inode for target file

	// Step 6: Call writei() to write inode to disk

	return -1;
}

static int tfs_open(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path

  struct inode *inode = (struct inode *)malloc(sizeof(struct inode));
  if (get_node_by_path(path, inode) == 0){
    free(inode);
    return 0;
  } else {
    free(inode);
    return -1;
  }

}

static int tfs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {

	// Step 1: You could call get_node_by_path() to get inode from path
  struct inode *inode;
	get_node_by_path(path, ROOT, inode);
	// Step 2: Based on size and offset, read its data blocks from disk
  int num_blocks;

  if (inode->size % BLOCK_SIZE == 0){
    num_blocks = inode->size / BLOCK_SIZE;
  } else {
    num_blocks = (inode->size / BLOCK_SIZE) + 1;
  }

  void *char = malloc(BLOCK_SIZE*num_blocks);

  for (int i = 0; i < num_blocks; i++){
    if (i < 16){
      bio_read(inode->direct_ptr[i], blocks[i*BLOCK_SIZE]);
    } else {
      bio_read(inode->indirect_ptr[i-16], blocks[i*BLOCK_SIZE]);
    }
  }

	// Step 3: copy the correct amount of data from offset to buffer

  memcpy(blocks, buffer, BLOCK_SIZE*num_blocks);

	// Note: this function should return the amount of bytes you copied to buffer
	return BLOCK_SIZE*num_blocks;
}
//UNFINISHED
static int tfs_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: Write the correct amount of data from offset to disk

	// Step 4: Update the inode info and write it to disk

	// Note: this function should return the amount of bytes you write to disk
	return size;
}
//later
static int tfs_unlink(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

  char *dir_name = dirname(path);
  char *base_name = basename(path);

	// Step 2: Call get_node_by_path() to get inode of target file

  struct inode *inode = malloc(sizeof(struct inode));
  if (get_node_by_path(path, ROOT, inode) == -1) return -1;

	// Step 3: Clear data block bitmap of target file

  for (int i = 0; i < inode->link; i++){
    if (i < 16){
      if (inode->direct_ptr[i] != -1){
        unset_bitmap(d_bitmap, i);
      }
    } else {
      if (inode->indirect_ptr[i-16] != 1){
        unset_bitmap(d_bitmap, i);
      }
    }
  }

	// Step 4: Clear inode bitmap and its data block

  unset_bitmap(i_bitmap, inode->ino);

	// Step 5: Call get_node_by_path() to get inode of parent directory

  struct inode *dir_inode = malloc(sizeof(struct inode));
  if (get_node_by_path(dir_name, ROOT, inode) == -1) return -1;

	// Step 6: Call dir_remove() to remove directory entry of target file in its parent directory

  dir_remove(dir_inode, base_name, strlen(basename));

	return 0;
}

char **split(char pn[]){

	int filenameLen = strlen(pn);
	int slash = 0;
	int index = 1;
	int start = -1;

	for(int i = 0; i <= filenameLen; i++){
		if(pn[i] == '/'){
			slash++;
		}
	}

	char **splitName = (char **)malloc((slash + 2) * sizeof(char *));

	if(strcmp(pn, "/") == 0){
		splitName[index] = (char *)malloc(2 * sizeof(char));
		strcpy(splitName[index], "/");
	}else{
		for(int j = 0; j <= filenameLen; j++){
			if(pn[j] == '/'){
				if(i == 0){
					splitName[index] = (char *)malloc(2 * sizeof(char));
					strcpy(splitName[index], "/");
				}else{
					int sublen = j - start - 1;
					 splitName[index] = (char *)malloc((sublen + 1) * sizeof(char));
					 memcpy(splitName[index], &pn[start + 1], sublen);
					 splitName[index][sublen] = '\0';
				}
				start = j;
				index++;
			}
		}
	}

	splitName[0] = malloc(11 * sizeof(char));
	sublen = filenameLen - start;
	splitName[index] = (char *)malloc((sublen + 1) * sizeof(char));
	memcpy(splitName[index], &pn[start + 1], sublen);
	splitName[index][sublen] = '\0';

	return splitname;

}

uint16_t searchFile(struct inode *in, uint16_t ino, char pn[]){
	double numentries = BLOCK_SIZE/(sizeof(struct dirent));
	struct dirent *subFandD = (struct dirent *)malloc(BLOCK_SIZE);

	char** splitPath = split(pn);
	int len = strtod(splitPath[0], NULL);
	int index = 0;
	int foundDisk = -1;
	struct inode found;

	readi(1, &found);

	int DirOff = sb->d_start_blk + found.direct_ptr[0];

	bio_read(DirOff, subFandD);

	if(strcmp(splitPath[len], "/") == 0){
		return 1;
	}

	while(1){

		for(int i = 0; i < DIR_PTR; i++){
			if(index == len && foundDisk == 0){
				i = 0;
				DirOff = sb->d_start_blk + found.direct_ptr[i];
				bio_read(DirOff, subFandD);
			}
			if(found.direct_ptr[i] == -1){
				continue;
			}
			DirOff = sb->d_start_blk + found.direct_ptr[i];
			bio_read(DirOff, subFandD);
			foundDisk = -1;

			for(int j = 0; j < numentries; j++){
				if(subFandD[j].valid == 1){
					if(strcmp(subFandD[j].name, splitPath[index] == 0){
						readi(subFandD[j].ino, &found);
						foundDisk = 0;
						found.ino = subFandD[j].ino;
						index++;
						break;
					}
				}
			}
			if(foundDisk == -1){
				continue;
			}
			if(index > len){
				free(subFandD);
				subFandD == NULL;
				return found.ino;
			}
		}
		if(foundDisk == -1){
			break;
		}

	}
	free(subFandD);
	subFandD == NULL;
	return 0;
}

static int tfs_truncate(const char *path, off_t size) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_release(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
	return 0;
}

static int tfs_flush(const char * path, struct fuse_file_info * fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_utimens(const char *path, const struct timespec tv[2]) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}


static struct fuse_operations tfs_ope = {
	.init		= tfs_init,
	.destroy	= tfs_destroy,

	.getattr	= tfs_getattr,
	.readdir	= tfs_readdir,
	.opendir	= tfs_opendir,
	.releasedir	= tfs_releasedir,
	.mkdir		= tfs_mkdir,
	.rmdir		= tfs_rmdir,

	.create		= tfs_create,
	.open		= tfs_open,
	.read 		= tfs_read,
	.write		= tfs_write,
	.unlink		= tfs_unlink,

	.truncate   = tfs_truncate,
	.flush      = tfs_flush,
	.utimens    = tfs_utimens,
	.release	= tfs_release
};


int main(int argc, char *argv[]) {
	int fuse_stat;

	getcwd(diskfile_path, PATH_MAX);
	strcat(diskfile_path, "/DISKFILE");

	fuse_stat = fuse_main(argc, argv, &tfs_ope, NULL);

	return fuse_stat;
}
