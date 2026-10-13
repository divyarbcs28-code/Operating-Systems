#include <stdio.h>

#define MAX 50

void display(int frames[], int f)
{
    int i;

    for (i = 0; i < f; i++)
    {
        if (frames[i] == -1)
            printf("- ");
        else

            printf("%d ", frames[i]);
    }

    printf("\n");
}

/* ---------- FIFO ---------- */

void fifo(int pages[], int n, int f)
{
    int frames[MAX];
    int i, j;
    int pointer = 0;
    int fault = 0;
    int found;

    for (i = 0; i < f; i++)
        frames[i] = -1;

    for (i = 0; i < n; i++)
    {
        found = 0;

        for (j = 0; j < f; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            frames[pointer] = pages[i];

            pointer++;

            if (pointer == f)
                pointer = 0;

            fault++;
        }

        printf("%d : ", pages[i]);
        display(frames, f);
    }

    printf("Page Faults = %d\n", fault);
}


/* ---------- LRU + OPTIMAL ---------- */

void lruOptimal(int pages[], int n, int f, int mode)
{
    int frames[MAX];
    int i, j;
    int found;
    int fault = 0;

    for (i = 0; i < f; i++)
        frames[i] = -1;

    for (i = 0; i < n; i++)
    {
        found = 0;

        /* Check whether page is already present */
        for (j = 0; j < f; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            int empty = -1;

            /* Find empty frame */
            for (j = 0; j < f; j++)
            {
                if (frames[j] == -1)
                {
                    empty = j;
                    break;
                }
            }

            if (empty != -1)
            {
                frames[empty] = pages[i];
            }
            else
            {
                int position = 0;

                if (mode == 0)
                {
                    /* -------- LRU : BACKWARD -------- */

                    int recent = -1;

                    for (j = 0; j < f; j++)
                    {
                        int backward = i - 1;
                        int lastUsed = -1;

                        while (backward >= 0)
                        {
                            if (frames[j] == pages[backward])
                            {
                                lastUsed = backward;
                                break;
                            }

                            backward--;
                        }

                        if (lastUsed < recent || recent == -1)
                        {
                            recent = lastUsed;
                            position = j;
                        }
                    }
                }
                else
                {
                    /* -------- OPTIMAL : FORWARD -------- */

                    int farthest = -1;

                    for (j = 0; j < f; j++)
                    {
                        int forward = i + 1;
                        int nextUsed = n + 1;

                        while (forward < n)
                        {
                            if (frames[j] == pages[forward])
                            {
                                nextUsed = forward;
                                break;
                            }

                            forward++;
                        }

                        if (nextUsed > farthest)
                        {
                            farthest = nextUsed;
                            position = j;
                        }
                    }
                }

                frames[position] = pages[i];
            }

            fault++;
        }

        printf("%d : ", pages[i]);
        display(frames, f);
    }

    printf("Page Faults = %d\n", fault);
}


/* ---------- MAIN ---------- */

int main()
{
    int pages[MAX];
    int n, f;
    int choice;
    int i;

    printf("Enter number of pages: ");
    scanf("%d", &n);

    printf("Enter reference string:\n");

    for (i = 0; i < n; i++)
        scanf("%d", &pages[i]);

    printf("Enter number of frames: ");
    scanf("%d", &f);

    do
    {
        printf("\n===== PAGE REPLACEMENT =====\n");
        printf("1. FIFO\n");
        printf("2. LRU\n");
        printf("3. Optimal\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                fifo(pages, n, f);
                break;

            case 2:
                lruOptimal(pages, n, f, 0);
                break;

            case 3:
                lruOptimal(pages, n, f, 1);
                break;

            case 4:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 4);

    return 0;
}
