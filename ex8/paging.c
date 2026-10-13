#include <stdio.h>
#include <stdlib.h>

#define MAX_PAGES 100
#define MAX_FRAMES 100

int main()
{
    int pageSize;
    int processSize;
    int memorySize;

    int numPages;
    int numFrames;

    int pageTable[MAX_PAGES];
    int freeList[MAX_FRAMES];

    int choice;
    int i;

    printf("Enter process size: ");
    scanf("%d", &processSize);

    printf("Enter page size: ");
    scanf("%d", &pageSize);

    if (processSize <= 0 || pageSize <= 0)
    {
        printf("Invalid process size or page size.\n");
        return 1;
    }
    numPages = (processSize + pageSize - 1) / pageSize;

    printf("Enter physical memory size: ");
    scanf("%d", &memorySize);

    if (memorySize <= 0)
    {
        printf("Invalid memory size.\n");
        return 1;
    }
    numFrames = memorySize / pageSize;

    if (numPages > MAX_PAGES || numFrames > MAX_FRAMES)
    {
        printf("Memory or process size exceeds the program limit.\n");
        return 1;
    }

    if (numFrames < numPages)
    {
        printf("\nNot enough frames to allocate all pages.\n");
        printf("Pages required  : %d\n", numPages);
        printf("Frames available: %d\n", numFrames);
        return 1;
    }
    for (i = 0; i < numFrames; i++)
    {
        freeList[i] = i;
    }
    for (i = numFrames - 1; i > 0; i--)
    {
        int randomIndex = rand() % (i + 1);

        int temp = freeList[i];
        freeList[i] = freeList[randomIndex];
        freeList[randomIndex] = temp;
    }
    for (i = 0; i < numPages; i++)
    {
        pageTable[i] = freeList[i];
    }

    printf("\nPaging initialized successfully.\n");
    printf("Number of pages  : %d\n", numPages);
    printf("Number of frames : %d\n", numFrames);

    do
    {
        printf("1. Display Page Table\n");
        printf("2. Display Number of Pages\n");
        printf("3. Display Internal Fragmentation\n");
        printf("4. Display External Fragmentation\n");
        printf("5. Logical to Physical Address Conversion\n");
        printf("6. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("\nPage Table:\n");
                printf("Page Number\tFrame Number\n");

                for (i = 0; i < numPages; i++)
                {
                    printf("%d\t\t%d\n", i, pageTable[i]);
                }
                break;

            case 2:
                printf("\nNumber of Pages  = %d\n", numPages);
                printf("Number of Frames = %d\n", numFrames);
                break;

            case 3:
            {
                int allocatedMemory;
                int internalFragmentation;

                allocatedMemory = numPages * pageSize;
                internalFragmentation = allocatedMemory - processSize;

                printf("\nAllocated Memory       = %d bytes\n",
                       allocatedMemory);
                printf("Process Size           = %d bytes\n",
                       processSize);
                printf("Internal Fragmentation = %d bytes\n",
                       internalFragmentation);

                break;
            }

            case 4:
            {
                int unusedFrames;
                int externalFragmentation;

                unusedFrames = numFrames - numPages;
                externalFragmentation = unusedFrames * pageSize;

                printf("\nUnused Frames          = %d\n", unusedFrames);
                printf("External Fragmentation = %d bytes\n",
                       externalFragmentation);
                printf("In paging, unused frames remain available.\n");

                break;
            }

            case 5:
            {
                int logicalAddress;
                int pageNumber;
                int offset;
                int frameNumber;
                int physicalAddress;

                printf("\nEnter Logical Address: ");
                scanf("%d", &logicalAddress);

                if (logicalAddress < 0 ||
                    logicalAddress >= processSize)
                {
                    printf("Invalid Logical Address!\n");
                    break;
                }

                pageNumber = logicalAddress / pageSize;
                offset = logicalAddress % pageSize;

                frameNumber = pageTable[pageNumber];

                physicalAddress =
                    frameNumber * pageSize + offset;

                printf("\nLogical Address  = %d\n", logicalAddress);
                printf("Page Number      = %d\n", pageNumber);
                printf("Offset           = %d\n", offset);
                printf("Frame Number     = %d\n", frameNumber);
                printf("Physical Address = %d\n", physicalAddress);

                break;
            }

            case 6:
                printf("\nExiting program...\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 6);

    return 0;
}
[24bcs006@mepcolinux ex8]$cat out.txt
[24bcs006@mepcolinux ex8]$./a.out
Enter process size: 1000
Enter page size: 100
Enter physical memory size: 2000

Paging initialized successfully.
Number of pages  : 10
Number of frames : 20
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 1

Page Table:
Page Number     Frame Number
0               6
1               7
2               12
3               16
4               13
5               5
6               8
7               11
8               4
9               2
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 2

Number of Pages  = 10
Number of Frames = 20
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 3

Allocated Memory       = 1000 bytes
Process Size           = 1000 bytes
Internal Fragmentation = 0 bytes
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 4

Unused Frames          = 10
External Fragmentation = 1000 bytes
In paging, unused frames remain available.
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 5

Enter Logical Address: 10

Logical Address  = 10
Page Number      = 0
Offset           = 10
Frame Number     = 6
Physical Address = 610
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 6

Exiting program...
[24bcs006@mepcolinux ex8]$./a.out
Enter process size: 1050
Enter page size: 100
Enter physical memory size: 2000

Paging initialized successfully.
Number of pages  : 11
Number of frames : 20
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 1

Page Table:
Page Number     Frame Number
0               6
1               7
2               12
3               16
4               13
5               5
6               8
7               11
8               4
9               2
10              18
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 2

Number of Pages  = 11
Number of Frames = 20
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 3

Allocated Memory       = 1100 bytes
Process Size           = 1050 bytes
Internal Fragmentation = 50 bytes
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 4

Unused Frames          = 9
External Fragmentation = 900 bytes
In paging, unused frames remain available.
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 5

Enter Logical Address: 9

Logical Address  = 9
Page Number      = 0
Offset           = 9
Frame Number     = 6
Physical Address = 609
1. Display Page Table
2. Display Number of Pages
3. Display Internal Fragmentation
4. Display External Fragmentation
5. Logical to Physical Address Conversion
6. Exit
Enter your choice: 6

Exiting program...
