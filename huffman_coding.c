#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure for the Huffman tree node
struct TreeNode {
  char data;              // Character stored in the node
  int frequency;          // Frequency of the character
  struct TreeNode *left;  // Pointer to the left child
  struct TreeNode *right; // Pointer to the right child
};

// Function to create a new node for the Huffman tree
struct TreeNode *createNode(char data, int frequency) {
  struct TreeNode *newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));
  newNode->data = data;
  newNode->frequency = frequency;
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

// Function to calculate the frequency of each character in the input string
void calculateFrequency(const char *input, int *frequency) {
  int i = 0;
  while (input[i] != '\0') {
    if (i == strlen(input) - 1)
      break;
    frequency[(int)input[i]]++;
    i++;
  }
}

// Function to generate Huffman codes for each character and store them in a
// code table
void generateHuffmanCodes(struct TreeNode *root, char *code, int level,
                          char **codeTable) {
  if (root->left == NULL && root->right == NULL) {
    code[level] = '\0'; // Null-terminate the code
    strcpy(codeTable[(int)root->data], code);
    return;
  }

  code[level] = '0';
  generateHuffmanCodes(root->left, code, level + 1, codeTable);

  code[level] = '1';
  generateHuffmanCodes(root->right, code, level + 1, codeTable);
}

// Function to build the Huffman tree
struct TreeNode *buildHuffmanTree(int *frequency) {
  // Assuming we have a maximum of 128 ASCII characters
  const int ASCII_SIZE = 128;

  // Create an array of TreeNode pointers to store the nodes
  struct TreeNode *nodes[ASCII_SIZE] = {0};

  // Create initial nodes for each character with non-zero frequency
  for (int i = 0; i < ASCII_SIZE; i++) {
    if (frequency[i] > 0) {
      nodes[i] = createNode((char)i, frequency[i]);
    }
  }

  // Construct the Huffman tree by combining nodes until only one root node
  // remains
  while (1) {
    int firstMinIndex = -1;
    int secondMinIndex = -1;

    // Find the two nodes with the minimum frequency that haven't been merged
    for (int i = 0; i < ASCII_SIZE; i++) {
      if (nodes[i] != NULL) {
        if (firstMinIndex == -1 ||
            nodes[i]->frequency < nodes[firstMinIndex]->frequency) {
          secondMinIndex = firstMinIndex;
          firstMinIndex = i;
        } else if (secondMinIndex == -1 ||
                   nodes[i]->frequency < nodes[secondMinIndex]->frequency) {
          secondMinIndex = i;
        }
      }
    }

    // If only one node is left, it's the root of the Huffman tree
    if (secondMinIndex == -1) {
      return nodes[firstMinIndex];
    }

    // Create a new internal node with the sum of frequencies of the two minimum
    // nodes
    struct TreeNode *internalNode =
        createNode('\0', nodes[firstMinIndex]->frequency +
                             nodes[secondMinIndex]->frequency);

    internalNode->left = nodes[firstMinIndex];
    internalNode->right = nodes[secondMinIndex];

    // Mark the two minimum nodes as merged by setting them to NULL
    nodes[firstMinIndex] = NULL;
    nodes[secondMinIndex] = internalNode;
  }
}

// Function to display Huffman codes
void displayHuffmanCodes(char **huffmanCodes, int *frequency) {
  int freq_sum = 0;
  double threshold = 0.0;
  int compressed_size = 0;
  int orignal_size = 0;
  printf("\nHuffman Codes:\n");
  printf("%-10s %-10s %-15s %s\n", "Characters", "Frequency", "Code-word",
         "Size");

  for (int i = 0; i < 128; i++) {
    if (frequency[i] > 0) {
      printf("%-10c %-10d %-15s %d*%d\n", (char)i, frequency[i],
             huffmanCodes[i], frequency[i], (int)strlen(huffmanCodes[i]));
      compressed_size += frequency[i] * (int)strlen(huffmanCodes[i]);
      orignal_size += frequency[i] * 8;
      freq_sum += frequency[i];
    }
  }
  printf("\n\nOrignal Size: %d bits", orignal_size);
  printf("\nCompressed Size: %d bits", compressed_size);
  threshold = compressed_size * 1.0 / freq_sum;
  printf("\ncompression threshold: %.2f bits", threshold);
}
// Function to display the Huffman tree
void displayHuffmanTree(struct TreeNode *root, char *code, int level) {
  if (root == NULL) {
    return;
  }

  for (int i = 0; i < level; i++) {
    printf("  ");
  }

  if (root->left == NULL && root->right == NULL) {
    printf("'%c' (%d)\n", root->data, root->frequency);
    return;
  }

  printf("Internal Node (%d)\n", root->frequency);

  code[level] = '0';
  displayHuffmanTree(root->left, code, level + 1);

  code[level] = '1';
  displayHuffmanTree(root->right, code, level + 1);
}

// function to clear data
void resetData(int *frequency, char **huffmanCodes, struct TreeNode *root) {
  memset(frequency, 0, 128 * sizeof(int));
  for (int i = 0; i < 128; i++) {
    if (huffmanCodes[i] != NULL) {
      free(huffmanCodes[i]);
      huffmanCodes[i] = NULL;
    }
  }
  free(root);
}
int main() {
  const int ASCII_SIZE = 128;
  char inputString[1000];
  int frequency[ASCII_SIZE] = {0};
  char *huffmanCodes[ASCII_SIZE] = {0};
  struct TreeNode *huffmanTreeRoot = NULL;
  char code[ASCII_SIZE];

  int choice;
  do {
    printf("\nMenu:\n");
    printf("1. Encode a string\n");
    printf("2. Display Huffman Codes\n");
    printf("3. Display Built Tree\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character after scanf

    switch (choice) {
    case 1:

      printf("Enter a string to compress: ");
      resetData(frequency, huffmanCodes, huffmanTreeRoot);
      fgets(inputString, sizeof(inputString), stdin);
      calculateFrequency(inputString, frequency);
      huffmanTreeRoot = NULL;
      huffmanTreeRoot = buildHuffmanTree(frequency);

      for (int i = 0; i < ASCII_SIZE; i++) {
        huffmanCodes[i] = (char *)malloc(ASCII_SIZE);
        memset(huffmanCodes[i], 0, ASCII_SIZE);
      }

      generateHuffmanCodes(huffmanTreeRoot, code, 0, huffmanCodes);
      printf("String encoded successfully!\n");

      break;

    case 2:
      if (huffmanTreeRoot != NULL) {
        displayHuffmanCodes(huffmanCodes, frequency);
      } else {
        printf("Please encode a string first.\n");
      }
      break;

    case 3:
      if (huffmanTreeRoot != NULL) {
        displayHuffmanCodes(huffmanCodes, frequency);
        printf("\nHuffman Tree:\n");
        displayHuffmanTree(huffmanTreeRoot, code, 0);
      } else {
        printf("Please encode a string first.\n");
      }
      break;

    case 4:
      printf("Exiting...\n");
      break;

    default:
      printf("Invalid choice! Please try again.\n");
    }
  } while (choice != 4);

  // Clean up allocated memory
  for (int i = 0; i < ASCII_SIZE; i++) {
    free(huffmanCodes[i]);
  }
  free(huffmanTreeRoot);

  return 0;
}

