#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr) {
  return ((size_t*)ptr)[-1];
}

// Merge function for merging two halves
void merge(int arr[], int l, int m, int r) {
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = r - m;

  // Create temp arrays
  int *L = (int *)Alloc(n1 * sizeof(int));
  int *R = (int *)Alloc(n2 * sizeof(int));

  // Copy data to temp arrays L[] and R[]
  for (i = 0; i < n1; i++) L[i] = arr[l + i];
  for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

  // Merge the temp arrays back into arr[l..r]
  i = 0;
  j = 0;
  k = l;

  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  // Copy the remaining elements of L[], if there are any
  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  // Copy the remaining elements of R[], if there are any
  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }

  // Deallocate temp arrays
  DeAlloc(L);
  DeAlloc(R);
}

// MergeSort function
void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        // Find the middle point
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Merge the sorted halves
        merge(arr, l, m, r);
    }
}

// Function to parse data from file
int parseData(char *inputFileName, int **ppData) {
    FILE* inFile = fopen(inputFileName, "r");
    if (!inFile) {
        printf("File %s cannot be opened\n", inputFileName);
        return -1;
    }

    int dataSz = 0;
    fscanf(inFile, "%d\n", &dataSz);
    *ppData = (int *)Alloc(sizeof(int) * dataSz);

    if (*ppData == NULL) {
        printf("Cannot allocate memory\n");
        fclose(inFile);
        return -1;
    }

    for (int i = 0; i < dataSz; ++i) {
        fscanf(inFile, "%d ", (*ppData + i));
    }

    fclose(inFile);
    return dataSz;
}

// Function to print array
void printArray(int pData[], int dataSz) {
    int i, sz = dataSz < 100 ? dataSz : 100;
    printf("\tData:\n\t");
    for (i = 0; i < sz; ++i) {
      printf("%d ", pData[i]);
    }
    printf("\n\t");
    
    if (dataSz > 100) {
      for (i = dataSz - 100; i < dataSz; ++i) {
        printf("%d ", pData[i]);
      }
    }
    printf("\n\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    char* fileName = argv[1];
    int *pDataSrc, *pDataCopy;
    int dataSz = parseData(fileName, &pDataSrc);

    if (dataSz <= 0) return -1;

    pDataCopy = (int *)Alloc(sizeof(int) * dataSz);
    memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
    extraMemoryAllocated = 0;

    clock_t start = clock();
    mergeSort(pDataCopy, 0, dataSz - 1);
    clock_t end = clock();

    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("---------------------------\n");
    printf("Dataset Size : %d\n", dataSz);
    printf("---------------------------\n");
    printf("Merge Sort:\n");
    printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
    printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
    printArray(pDataCopy, dataSz);

    DeAlloc(pDataCopy);
    DeAlloc(pDataSrc);

    return 0;
}
