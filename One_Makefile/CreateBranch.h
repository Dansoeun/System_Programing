#ifndef CREATEBRANCH_H
#define CREATEBRANCH_H

void CreateBranch(const char *b_name, int shmid, void *shmaddr, const char *master_fifo_path);

#endif // CREATEBRANCH_H
