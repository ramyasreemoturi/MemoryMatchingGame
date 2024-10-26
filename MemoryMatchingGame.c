#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Structure to represent the game board
typedef struct {
    int rows, cols;
    int* revealed;
    char* cards;
} Board;

// Queue structure
typedef struct {
    int front, rear, size;
    unsigned capacity;
    int* array;
} Queue;

// Function to initialize a queue
Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}

// Function to free the memory allocated for the queue
void freeQueue(Queue* queue) {
    free(queue->array);
    free(queue);
}

// Function to check if the queue is empty
int isEmpty(Queue* queue) {
    return (queue->size == 0);
}

// Function to enqueue an element
void enqueue(Queue* queue, int item) {
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to dequeue an element
int dequeue(Queue* queue) {
    if (isEmpty(queue))
        return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to check if all cards are matched
int allMatched(int* revealed, int size) {
    int i;
    for (i = 0; i < size; ++i) {
        if (!revealed[i]) {
            return 0; // Not all cards are matched
        }
    }
    return 1; // All cards are matched
}

// Function to initialize the cards
void initializeCards(char* cards, int size) {
    char symbols[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'};
    int i;
    for (i = 0; i < size; i += 2) {
        cards[i] = symbols[i / 2];
        cards[i + 1] = symbols[i / 2];
    }

    // Add ">_<" as the last card for odd-sized boards
    if (size % 2 != 0) {
        cards[size - 1] = '$';
        //cards[size - 2] = '_';
        //cards[size - 3] = '<';
    }
}

// Function to shuffle the cards
void shuffleCards(char* cards, int size) {
    int i;
    for (i = size - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        // Swap cards[i] and cards[j]
        char temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }
}

// Function to create a game board
Board createBoard(int rows, int cols) {
    Board board;
    board.rows = rows;
    board.cols = cols;
    board.revealed = (int*)malloc(rows * cols * sizeof(int));
    board.cards = (char*)malloc(rows * cols * sizeof(char));

    int i, j, k = 0;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            board.revealed[i * cols + j] = 0;
            board.cards[i * cols + j] = ' ';
        }
    }

    return board;
}

// Function to free the memory allocated for the game board
void freeBoard(Board* board) {
    free(board->revealed);
    free(board->cards);
}

// Function to display the board
void displayBoard(Board* board, int score) {
    printf("\nCurrent Board (Score: %d):\n", score);
    int i, j;
    for (i = 0; i < board->rows; ++i) {
        for (j = 0; j < board->cols; ++j) {
            int index = i * board->cols + j + 1;
            if (board->revealed[i * board->cols + j]) {
                printf("%c ", board->cards[i * board->cols + j]);
            } else {
                printf("* ");
            }
        }
        printf("\n");
    }

    printf("\n");
}

// Function to play the game
int playGame(Board* board) {
    Queue* selectedCards = createQueue(2); // Queue to store indices of selected cards
    int score = 0; // Player's score

    while (1) {
        displayBoard(board, score);

        // Get user input for the card
        int selectedCard;
        printf("Enter the index of the card (1-%d), or enter 0 to quit: ", board->rows * board->cols);
        if (scanf("%d", &selectedCard) != 1) {
            printf("Invalid input. Please enter a valid index (1-%d) or 0 to quit.\n", board->rows * board->cols);
            // Clear the input buffer
            while (getchar() != '\n');
            continue;
        }

        // Check if the user wants to quit
        if (selectedCard == 0) {
            printf("Quitting the game. Your final score is %d.\n", score);
            return 0;
        }

        // Check if the index is valid
        if (selectedCard < 1 || selectedCard > board->rows * board->cols || board->revealed[selectedCard - 1]) {
            printf("Invalid index. Try again.\n");
            continue;
        }

        // Convert the index to row and column
        int row = (selectedCard - 1) / board->cols;
        int col = (selectedCard - 1) % board->cols;

        // Reveal the selected card
        board->revealed[row * board->cols + col] = 1;
        enqueue(selectedCards, row * board->cols + col);

        displayBoard(board, score);

        // Check if two cards are selected
        if (selectedCards->size == 2) {
            int card1 = dequeue(selectedCards);
            int card2 = dequeue(selectedCards);

            // Check if the selected cards match
            if (board->cards[card1] == board->cards[card2]) {
                printf("Matched %c! +10 points\n", board->cards[card1]);
                score += 10;
            } else {
                // If not, hide both cards
                board->revealed[card1] = 0;
                board->revealed[card2] = 0;
            }
        }

        // Check for game over
        if (allMatched(board->revealed, board->rows * board->cols)) {
            printf("Congratulations! You've matched all the cards. Your final score is %d.\n", score);
            printf("Thanks for playing!\n");
            return 1;
        }
    }
}

int main() {
    srand(time(NULL)); // Seed for random shuffle

    int level = 2; // Starting level
    int quitGame = 0;

    while (!quitGame) {
        int rows = level;
        int cols = level;

        Board board = createBoard(rows, cols);
        initializeCards(board.cards, rows * cols);
        shuffleCards(board.cards, rows * cols);

        quitGame = !playGame(&board);

        // Free allocated memory for the board
        freeBoard(&board);

        // Check if the user wants to try the next level
        if (!quitGame) {
            printf("Do you want to try the next level? (1 for Yes, 0 for No): ");
            int choice;
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input. Exiting.\n");
                return 1;
            }

            if (choice != 1) {
                quitGame = 1;
            } else {
                printf("Starting Level %d\n", level + 1);
                level++;
            }
        }
    }

    return 0;
}
