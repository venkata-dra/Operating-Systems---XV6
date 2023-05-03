#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <sys/mman.h>


#include <sys/types.h>
#include <unistd.h>

// #include "elf.c"
// Format of an ELF executable file

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

// File header
struct elfhdr {
  unsigned int magic;  // must equal ELF_MAGIC
  unsigned char elf[12];
  unsigned short type;
  unsigned short machine;
  unsigned int version;
  unsigned int entry;
  unsigned int phoff;
  unsigned int shoff;
  unsigned int flags;
  unsigned short ehsize;
  unsigned short phentsize;
  unsigned short phnum;
  unsigned short shentsize;
  unsigned short shnum;
  unsigned short shstrndx;
};

// Program section header
struct proghdr {
  unsigned int type;
  unsigned int off;
  unsigned int vaddr;
  unsigned int paddr;
  unsigned int filesz;
  unsigned int memsz;
  unsigned int flags;
  unsigned int align;
};

// Values for Proghdr type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

int main(int argc, char* argv[]) {
    struct elfhdr elf;
    struct proghdr ph;
    int (*sum)(int a, int b);
    void *entry = NULL;
    int ret; 
    char *code_va =NULL;

    int fd = open(argv[1], O_RDONLY);
    if (fd != -1)
    {
      read(fd, &elf, sizeof(elf));
        lseek(fd,elf.phoff,SEEK_SET);
        for(int i=0;i<elf.phnum;i++) {
           read(fd, &ph, sizeof(ph));
           if(ph.type==ELF_PROG_LOAD) {
              code_va = mmap(NULL, ph.memsz, PROT_READ | PROT_WRITE | PROT_EXEC,
              MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
              lseek(fd,ph.off,SEEK_SET);
              read(fd, code_va, ph.filesz);
              break;
           }
        }
    }
    entry=code_va;
    
    if (entry != NULL) {
        sum = entry; 
        ret = sum(1, 2);
        printf("sum:%d\n", ret); 
    };


}

