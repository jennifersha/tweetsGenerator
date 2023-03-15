#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int occurences;
  int nextWord_cnt;
  //... Add your own fields here
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  //... Add your own fields here
  float probability;
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
  WordStruct *data;
  struct Node *next;
} Node;

typedef struct LinkList {
  Node *first;
  Node *last;
  int size;
} LinkList;


/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
  {
	return 1;
  }
  *new_node = (Node){data, NULL};

  if (link_list->first == NULL)
  {
	link_list->first = new_node;
	link_list->last = new_node;
  }
  else
  {
	link_list->last->next = new_node;
	link_list->last = new_node;
  }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand()%max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary)
{
    WordStruct* item;
    Node* node=dictionary->first;
    int i, index;
    while(1){
        index = get_random_number(dictionary->size);
        i=0;
        while(i<index){
          item=node->data;
          node=node->next;
          i++;
        }
        if(i==index){
            if(item->word[strlen(item->word)-1]=='.') return node->data;
            else return item;
        }

    }
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{
    WordStruct* item;
    float max=0.0;
    int max_i;
    if(word_struct_ptr->prob_list==NULL) return NULL;

    for(int i=0;i<word_struct_ptr->nextWord_cnt;i++){
        float prob=word_struct_ptr->prob_list[i].probability;
        if(prob>=max) max=prob, item = word_struct_ptr->prob_list[i].word_struct_ptr;
    }
    return item;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{
    int amount_words=0;
    char sentence[MAX_SENTENCE_LENGTH]="";
    WordStruct* first_word, *next_word;
    first_word=get_first_random_word(dictionary);
    printf("first word: %s\n", first_word->word);

    strcat(sentence, first_word->word);
    do{
        amount_words++;
        next_word=get_next_random_word(first_word);
        if(next_word==NULL) break;
        strcat(sentence, " " );
        strcat(sentence, next_word->word );
        first_word=next_word;

    }  while(amount_words<MAX_WORDS_IN_SENTENCE_GENERATION && next_word->word[strlen(next_word->word)-1]!='.');

    printf("sentence: %s\n", sentence);

    return amount_words;

}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word,
                                 WordStruct *second_word)
{
    WordProbability* tem;
    tem = first_word->prob_list;
    int i=0;
    while(i<first_word->nextWord_cnt){
         if(!strcmp(tem[i].word_struct_ptr->word, second_word->word)) {
            tem[i].probability += (1/(float)first_word->occurences);
            return 0;
        }

        i++;
    }

    if(tem==NULL) first_word->prob_list = (WordProbability*)malloc(sizeof(WordProbability));
    else first_word->prob_list=(WordProbability*)realloc(first_word->prob_list, sizeof(WordProbability)*(first_word->nextWord_cnt+1));

    first_word->prob_list[first_word->nextWord_cnt].probability = 1/(float)first_word->occurences;
    first_word->prob_list[first_word->nextWord_cnt].word_struct_ptr = second_word;
    first_word->nextWord_cnt++;

    return 1;
}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary)
{

    char line[MAX_SENTENCE_LENGTH];
    char* _token;
    bool quit=false;
    int index=0;
    char prev[MAX_WORD_LENGTH]="__";
    do{
	        fgets(line, MAX_SENTENCE_LENGTH, fp);
            line[strlen(line)-1]='\0';

			_token= strtok(line," ");

    	    while(_token!=NULL){
                 index++;
    	         char *word=(char*)malloc(sizeof(char)*strlen(_token)+1);
              	 strcpy(word,_token);

                 WordStruct* item=(WordStruct*)malloc(sizeof(WordStruct));
                 WordStruct* _item;
                 item->word=word;
                 //WordProbability *prob_list=(WordProbability*)malloc(sizeof(WordProbability));
                 item->prob_list = NULL;
                 item->nextWord_cnt=0;
                 item->occurences = 1;

                 Node* iter=dictionary->first;

                 while(iter!=NULL){
                    if(!strcmp(word,iter->data->word)) {
                        iter->data->occurences++;
                        int k=0;
                        WordProbability* tem=iter->data->prob_list;
                        while(k<iter->data->nextWord_cnt){
                            float p=tem[k].probability;
                            tem[k].probability = tem[k].probability*(iter->data->occurences-1)/(float)iter->data->occurences;
                            k++;
                        }

                        break;
                    }
                    else iter=iter->next;

                 }
                 if(iter==NULL){
                    add(dictionary, item);
                 }
                 else{
                        free(word);
                        free(item);
                        item = iter->data;
                 }

                 iter = dictionary->first;
                 while(iter!=NULL){
                    if(!strcmp(prev,iter->data->word)) {
                            add_word_to_probability_list(iter->data, item);
                            break;
                    }
                    else iter=iter->next;

                 }

		    	 strcpy(prev,_token);
                _token = strtok(NULL," ");
                 if(index==words_to_read) {quit=true; break;}


            }
            if(quit || feof(fp)) break;
     }while(1);

}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary)
{
    Node* node;
    node = dictionary->first;
    while(node!=NULL){
        free(node->data->word);
        free(node->data->prob_list);
        node=node->next;
    }
    free(dictionary);
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[])
{
    bool optional;
    int seed, sentences, words;
    if(argc!=4 && argc!=5){
       exit(0);
    }
    if(argc==4) optional = false;
    else optional = true;
    seed=atoi(argv[1]);

    char* filePath = argv[3];
    sentences = atoi(argv[2]);
    FILE* fd=fopen(filePath, "r");
    words=(optional==true)?atoi(argv[4]):-1;
    LinkList *dictionary=(LinkList*)malloc(sizeof(LinkList));
    dictionary->size=0;
    dictionary->first= dictionary->last= NULL;
    fill_dictionary(fd, words, dictionary);
    srand(seed);
    for(int i=0; i<sentences;i++){
        generate_sentence(dictionary);
    }
    free_dictionary(dictionary);
    return 0;
}
