#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 

#define MAX_QUESTION_LENGTH 256
#define MAX_ANSWER_LENGTH 64
#define MAX_OPTION_LENGTH 32
#define MAX_OPTIONS 4
#define MAX_FILENAME_LENGTH 256
#define MAX_FEEDBACK_LENGTH 1000
#define MAX_USERNAME_LENGTH 256

// Structure to represent a quiz question
typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char options[MAX_OPTIONS][MAX_OPTION_LENGTH];
    char answer[MAX_ANSWER_LENGTH];
} QuizQuestion;

// Function to set console colors
void setConsoleColors(int bg, int fg) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bg << 4) | fg);
}

// Function to clear the screen
void clearScreen() {
    system("cls");
}

// Function to play audio using PlaySound
void playAudio(const char *audioFilename) {
    PlaySound(audioFilename, NULL, SND_FILENAME);
}

// Function to load quiz questions from a CSV file
int loadQuestions(const char *filename, QuizQuestion **questions, int *numQuestions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("\nError in opening file");
        playAudio("C:/Users/Ameem/Desktop/error.wav");
		return 0;
    }

    // Count the number of lines/questions in the file
    int count = 0;
    char buffer[MAX_QUESTION_LENGTH + (MAX_OPTION_LENGTH * MAX_OPTIONS) + MAX_ANSWER_LENGTH + 3];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        count++;
    }
    rewind(file);

    // Allocate memory for the array of questions
    *questions = (QuizQuestion *)malloc(count * sizeof(QuizQuestion));
    if (*questions == NULL) {
        perror("Error in allocating memory");
        playAudio("C:/Users/Ameem/Desktop/errorMemory.wav");
        fclose(file);
        return 0;
    }

    // Read questions from the file
    for (int i = 0; i < count; i++) {
        if (fscanf(file, " %[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]\n",
                   (*questions)[i].question,
                   (*questions)[i].options[0],
                   (*questions)[i].options[1],
                   (*questions)[i].options[2],
                   (*questions)[i].options[3],
                   (*questions)[i].answer) != 6) {
            fprintf(stderr, "Error in reading question from file\n");
            playAudio("C:/Users/Ameem/Desktop/errorReading.wav");
            fclose(file);
            free(*questions);
            return 0;
        }
    }

    fclose(file);
    *numQuestions = count;
    return 1;
}

// Function to run the quiz
void runQuiz(QuizQuestion *questions, int numQuestions, const char *userName, const char *filename) {
    int score = 0;
    char userAnswer;
    char feedback[MAX_FEEDBACK_LENGTH];

    for (int i = 0; i < numQuestions; i++) {

		printf("\nQuestion %d: %s\n", i + 1, questions[i].question);
		playAudio("C:/Users/Ameem/Desktop/question.wav");
		
        // Print options
        for (int j = 0; j < MAX_OPTIONS; j++) {
			printf("%c) %s   ", 'A' + j, questions[i].options[j]);
        }
        playAudio("C:/Users/Ameem/Desktop/options.wav");
        printf("\n");

        printf("\nYour answer, %s (A/B/C/D): ", userName);
        playAudio("C:/Users/Ameem/Desktop/answer.wav");
        scanf(" %c", &userAnswer); // Read a single character

        // Convert the user input to an index (0 for A, 1 for B, and so on)
        int userIndex = toupper(userAnswer) - 'A';

        if (userIndex >= 0 && userIndex < MAX_OPTIONS &&
            strcmp(questions[i].options[userIndex], questions[i].answer) == 0) {
            printf("Correct!");
			score++;
			playAudio("C:/Users/Ameem/Desktop/correct.wav"); // Replace "correct.wav" with the actual correct sound file
        } else {
            printf("Incorrect. The correct answer is: %s", questions[i].answer);
			playAudio("C:/Users/Ameem/Desktop/incorrect.wav"); // Replace "incorrect.wav" with the actual incorrect sound file
			}
        printf("\n");
    } 

	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    printf("\nQuiz completed, %s. Your score: %d/%d\n\n", userName, score, numQuestions);
    playAudio("C:/Users/Ameem/Desktop/quizCompleted.wav");
    
    // Ask for feedback
    printf("Please provide feedback on the quiz: ");
    playAudio("C:/Users/Ameem/Desktop/feedback.wav");
    scanf(" %[^\n]", feedback);

    // Save results to an output file
    FILE *outputFile = fopen("quiz_results.txt", "a");
    if (outputFile != NULL) {
        fprintf(outputFile, "Name: %s, Filename: %s, Score: %d/%d, Feedback: %s\n", userName, filename, score, numQuestions, feedback);
        fclose(outputFile);
    } else {
        printf("Error opening or creating the output file.\n");
    }
}

int main() {
    char filename[MAX_FILENAME_LENGTH];
    char playAgain = 'Y'; // Initialize playAgain to a default value
	char userName[MAX_USERNAME_LENGTH];
	
    do {
    	if (playAgain == 'y' || playAgain == 'Y') {
    		setConsoleColors(7, 1); // Set background color to gray (7) and text color to blue (1)
            clearScreen(); // Clear the screen only if the user chooses to play again
        }
        
        printf("\t\t*****************************\n");
		printf("\t\t*\tINTELLECTOPIA\t    *\n");
        printf("\t\t*****************************");
		playAudio("C:/Users/Ameem/Desktop/intro.wav");
		printf("\n\nEnter your name: "); 
        playAudio("C:/Users/Ameem/Desktop/userName.wav");
		scanf(" %[^\n]", userName);
        
        printf("Enter the CSV file name (e.g., questions.csv): ");
		playAudio("C:/Users/Ameem/Desktop/fileName.wav");
		scanf("%s", filename);
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        QuizQuestion *questions;
        int numQuestions;

        if (loadQuestions(filename, &questions, &numQuestions)) {
            runQuiz(questions, numQuestions, userName, filename);
            free(questions);
        } else {
            printf("Failed to load questions. Exiting...");
            return 1;
        }

        // Ask the user if they want to play again
        printf("\nDo you want to play again? (Press Y/y): ");
        playAudio("C:/Users/Ameem/Desktop/playAgain.wav");
        scanf(" %c", &playAgain); // Note the space before %c to consume the newline character
		
   } while (playAgain == 'y' || playAgain == 'Y');
    
	printf("\nThanks for playing, %s! Goodbye.", userName);
	playAudio("C:/Users/Ameem/Desktop/ending.wav");
	return 0;
}