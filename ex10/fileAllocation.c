#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 50
#define DIRECT_COUNT 4
#define INDEX_PTRS 4   // Number of pointers per index block

// Core File Meta Information
typedef struct {
    char file_name[20];
    int file_size;    // Size in bytes (user input)
    int length;       // Number of blocks needed (calculated)
    int start;
    int allocated;
} FileMeta;

// --- LINKED ALLOCATION STRUCTURES ---
typedef struct Node {
    int block_no;
    struct Node* next;
} Node;

typedef struct {
    char file_name[20];
    Node* head;
} LinkedFile;

// --- MULTILEVEL INDEXED ALLOCATION STRUCTURES ---
typedef struct {
    int block_no;       // Disk block number of this index block
    int ptr[INDEX_PTRS];// Pointers to data blocks or other index blocks
    int used;           // Number of valid pointers in this block
} IdxBlock;

typedef struct {
    char file_name[20];
    int file_size;
    int num_data_blocks;

    // Direct pointers
    int direct[DIRECT_COUNT];
    int num_direct;

    // Single indirect
    int single_indirect_block;
    IdxBlock single_ib;

    // Double indirect (hierarchical tree)
    int double_indirect_block;   // Root index block
    IdxBlock double_ib_level1;   // Points to level 2 index blocks
    IdxBlock double_ib_level2[INDEX_PTRS]; // Each points to data blocks
} MLIndexedFile;

int BLOCK_SIZE = 1024; // Will be set by user

/* Helper to get a random free block */
int getRandomFreeBlock(int freeBlock[]) {
    int count = 0;
    int available[MAX];
    for (int i = 0; i < MAX; i++) {
        if (freeBlock[i] == 1) {
            available[count++] = i;
        }
    }
    if (count == 0) return -1;
    int index = rand() % count;
    int allocated_block = available[index];
    freeBlock[allocated_block] = 0;
    return allocated_block;
}

/* Helper to find a contiguous range of free blocks (First-Fit) */
int findContiguousFreeBlocks(int freeBlock[], int length) {
    int count = 0;
    int start_candidate = -1;
    for (int i = 0; i < MAX; i++) {
        if (freeBlock[i] == 1) {
            if (count == 0) start_candidate = i;
            count++;
            if (count == length) return start_candidate;
        } else {
            count = 0;
            start_candidate = -1;
        }
    }
    return -1;
}

void resetFreeBlocks(int freeBlock[]) {
    for (int i = 0; i < MAX; i++) freeBlock[i] = 1;
}

void displayFreeBlocks(int freeBlock[]) {
    printf("\nFree Block List:\n");
    for (int i = 0; i < MAX; i++) {
        if (freeBlock[i] == 1) printf("%d ", i);
    }
    printf("\n");
}

/* 1. Contiguous Allocation */
void contiguous(FileMeta f[], int n, int freeBlock[]) {
    printf("\n--- CONTIGUOUS ALLOCATION ---\n");
    printf("Block Size: %d bytes\n\n", BLOCK_SIZE);
    printf("File\tSize(Bytes)\tBlocks\tStart\tEnd\tStatus\t\tAllocated Blocks\n");

    for (int i = 0; i < n; i++) {
        if (f[i].length == 0) {
            printf("%s\t%d\t\t0\t-\t-\tNo blocks needed\t-\n", f[i].file_name, f[i].file_size);
            continue;
        }
        int auto_start = findContiguousFreeBlocks(freeBlock, f[i].length);
        if (auto_start != -1) {
            f[i].start = auto_start;
            f[i].allocated = 1;
            int end_block = auto_start + f[i].length - 1;
            for (int j = 0; j < f[i].length; j++) {
                freeBlock[auto_start + j] = 0;
            }
            printf("%s\t%d\t\t%d\t%d\t%d\tAllocated\t",
                   f[i].file_name, f[i].file_size, f[i].length, f[i].start, end_block);
            for (int j = 0; j < f[i].length; j++) {
                printf("%d ", f[i].start + j);
            }
            printf("\n");
        } else {
            f[i].start = -1;
            f[i].allocated = 0;
            printf("%s\t%d\t\t%d\t-\t-\tFailed (No space)\n",
                   f[i].file_name, f[i].file_size, f[i].length);
        }
    }
}

/* 2. Linked Allocation */
void linked(FileMeta f[], int n, int freeBlock[]) {
    LinkedFile lf[MAX];
    printf("\n--- LINKED ALLOCATION ---\n");
    printf("Block Size: %d bytes\n\n", BLOCK_SIZE);
    printf("File\tSize(Bytes)\tBlocks\tLinked Blocks\n");

    for (int i = 0; i < n; i++) {
        strcpy(lf[i].file_name, f[i].file_name);
        lf[i].head = NULL;
        Node* tail = NULL;

        if (f[i].length == 0) {
            printf("%s\t%d\t\t0\t(No blocks allocated)\n", lf[i].file_name, f[i].file_size);
            continue;
        }

        for (int j = 0; j < f[i].length; j++) {
            int block = getRandomFreeBlock(freeBlock);
            if (block == -1) {
                printf("\nDisk full! Cannot allocate remaining blocks for %s\n", lf[i].file_name);
                break;
            }
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->block_no = block;
            newNode->next = NULL;
            if (lf[i].head == NULL) {
                lf[i].head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        }

        printf("%s\t%d\t\t%d\t", lf[i].file_name, f[i].file_size, f[i].length);
        Node* temp = lf[i].head;
        while (temp != NULL) {
            printf("%d", temp->block_no);
            Node* nextNode = temp->next;
            free(temp);
            temp = nextNode;
            if (temp != NULL) printf(" -> ");
        }
        printf(" -> END\n");
    }
}

/* 3. Multilevel Indexed Allocation (UNIX-style: Direct, Single Indirect, Double Indirect) */
void multilevelIndexed(FileMeta f[], int n, int freeBlock[]) {
    MLIndexedFile mlf[MAX];
    printf("\n--- MULTILEVEL INDEXED ALLOCATION ---\n");
    printf("Block Size: %d bytes\n", BLOCK_SIZE);
    printf("Direct pointers: %d | Pointers per index block: %d\n", DIRECT_COUNT, INDEX_PTRS);
    printf("Max addressable data blocks: Direct(%d) + Single(%d) + Double(%d) = %d blocks\n\n",
           DIRECT_COUNT, INDEX_PTRS, INDEX_PTRS * INDEX_PTRS,
           DIRECT_COUNT + INDEX_PTRS + INDEX_PTRS * INDEX_PTRS);

    for (int i = 0; i < n; i++) {
        strcpy(mlf[i].file_name, f[i].file_name);
        mlf[i].file_size = f[i].file_size;
        mlf[i].num_data_blocks = f[i].length;
        mlf[i].num_direct = 0;
        mlf[i].single_indirect_block = -1;
        mlf[i].double_indirect_block = -1;
        mlf[i].single_ib.used = 0;
        mlf[i].double_ib_level1.block_no = -1;
        mlf[i].double_ib_level1.used = 0;

        for (int j = 0; j < INDEX_PTRS; j++) {
            mlf[i].double_ib_level2[j].block_no = -1;
            mlf[i].double_ib_level2[j].used = 0;
        }

        int needed = mlf[i].num_data_blocks;
        int disk_full = 0;

        printf("File: %s | Size: %d bytes | Data Blocks Needed: %d\n",
               mlf[i].file_name, mlf[i].file_size, needed);

        if (needed == 0) {
            printf("  (Empty file - no data blocks allocated)\n\n");
            continue;
        }

        // 1. Allocate Direct Blocks
        for (int j = 0; j < DIRECT_COUNT && needed > 0; j++) {
            int b = getRandomFreeBlock(freeBlock);
            if (b == -1) { disk_full = 1; break; }
            mlf[i].direct[j] = b;
            mlf[i].num_direct++;
            needed--;
        }

        // 2. Allocate Single Indirect Block
        if (!disk_full && needed > 0) {
            int sb = getRandomFreeBlock(freeBlock);
            if (sb == -1) {
                disk_full = 1;
            } else {
                mlf[i].single_indirect_block = sb;
                mlf[i].single_ib.block_no = sb;
                for (int j = 0; j < INDEX_PTRS && needed > 0; j++) {
                    int b = getRandomFreeBlock(freeBlock);
                    if (b == -1) { disk_full = 1; break; }
                    mlf[i].single_ib.ptr[j] = b;
                    mlf[i].single_ib.used++;
                    needed--;
                }
            }
        }

        // 3. Allocate Double Indirect Block (Tree structure)
        if (!disk_full && needed > 0) {
            int db = getRandomFreeBlock(freeBlock);
            if (db == -1) {
                disk_full = 1;
            } else {
                mlf[i].double_indirect_block = db;
                mlf[i].double_ib_level1.block_no = db;

                for (int j = 0; j < INDEX_PTRS && needed > 0 && !disk_full; j++) {
                    int lvl2 = getRandomFreeBlock(freeBlock);
                    if (lvl2 == -1) { disk_full = 1; break; }

                    mlf[i].double_ib_level1.ptr[j] = lvl2;
                    mlf[i].double_ib_level1.used++;
                    mlf[i].double_ib_level2[j].block_no = lvl2;

                    for (int k = 0; k < INDEX_PTRS && needed > 0; k++) {
                        int b = getRandomFreeBlock(freeBlock);
                        if (b == -1) { disk_full = 1; break; }
                        mlf[i].double_ib_level2[j].ptr[k] = b;
                        mlf[i].double_ib_level2[j].used++;
                        needed--;
                    }
                }
            }
        }

        if (needed > 0) {
            printf("  WARNING: Disk full or max addressing exceeded! %d blocks unallocated.\n", needed);
        }

        // Display the multilevel structure
        printf("  [Direct Blocks] (%d blocks): ", mlf[i].num_direct);
        if (mlf[i].num_direct == 0) printf("None");
        for (int j = 0; j < mlf[i].num_direct; j++) printf("%d ", mlf[i].direct[j]);
        printf("\n");

        if (mlf[i].single_indirect_block != -1) {
            printf("  [Single Indirect] Index Block %d -> Data: ", mlf[i].single_indirect_block);
            for (int j = 0; j < mlf[i].single_ib.used; j++) printf("%d ", mlf[i].single_ib.ptr[j]);
            printf("\n");
        }

        if (mlf[i].double_indirect_block != -1) {
            printf("  [Double Indirect] Root Index Block %d\n", mlf[i].double_indirect_block);
            for (int j = 0; j < mlf[i].double_ib_level1.used; j++) {
                printf("    Level-1 -> Index Block %d -> Data: ", mlf[i].double_ib_level1.ptr[j]);
                for (int k = 0; k < mlf[i].double_ib_level2[j].used; k++) {
                    printf("%d ", mlf[i].double_ib_level2[j].ptr[k]);
                }
                printf("\n");
            }
        }

        printf("  Status: %s\n\n", (needed == 0) ? "FULLY ALLOCATED" : "PARTIALLY ALLOCATED");
    }
}

int main() {
    FileMeta f[MAX];
    int freeBlock[MAX];
    int n, choice;
    srand(time(NULL));
    resetFreeBlocks(freeBlock);

    printf("===== FILE ALLOCATION TECHNIQUES =====\n");
    printf("\nEnter Disk Block Size (in bytes): ");
    scanf("%d", &BLOCK_SIZE);
    if (BLOCK_SIZE <= 0) BLOCK_SIZE = 1024;

    printf("\nEnter number of files: ");
    scanf("%d", &n);

    printf("\nEnter File Name and File Size in bytes (Block size = %d bytes):\n", BLOCK_SIZE);
    for (int i = 0; i < n; i++) {
        printf("File %d [Name SizeInBytes]: ", i + 1);
        scanf("%s %d", f[i].file_name, &f[i].file_size);

        if (f[i].file_size <= 0) {
            f[i].length = 0;
        } else {
            f[i].length = (f[i].file_size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Ceiling division
        }
        f[i].start = -1;
        f[i].allocated = 0;
    }

    do {
        printf("\n===== MENU =====\n");
        printf("1. Contiguous Allocation\n");
        printf("2. Linked Allocation\n");
        printf("3. Multilevel Indexed Allocation\n");
        printf("4. Display Free Block List\n");
        printf("5. Reset Free Blocks\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                resetFreeBlocks(freeBlock);
                contiguous(f, n, freeBlock);
                break;
            case 2:
                resetFreeBlocks(freeBlock);
                linked(f, n, freeBlock);
                break;
            case 3:
                resetFreeBlocks(freeBlock);
                multilevelIndexed(f, n, freeBlock);
                break;
            case 4:
                displayFreeBlocks(freeBlock);
                break;
            case 5:
                resetFreeBlocks(freeBlock);
                printf("\nFree block list reset completely.\n");
                break;
            case 6:
                printf("\nProgram Ended.\n");
                break;
            default:
                printf("\nInvalid Choice!\n");
        }
    } while (choice != 6);

    return 0;
}
