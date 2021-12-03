
#include "jobs.h"

typedef struct nodes{ struct nodes *next; job_t job;} jobs_node_t;
typedef struct {jobs_node_t *head; jobs_node_t *tail;} jobs_list_t;

void free_node(jobs_node_t *node){
	if(node) {
		if(node->job.name)
		free(node->job.name);
		free(node);
	}
}

void* jobs_create() {
	return calloc(1, sizeof(jobs_list_t));
}

int jobs_refresh(void *jobs) { // disable signals inside
	jobs_list_t *list = (jobs_list_t *) jobs;
	jobs_node_t *prev = NULL;
	jobs_node_t *node;
	int pid, status;
	int finished, stopped;

	if(!jobs) {
		printf("%s: called with NULL pointer for jobs\n", __func__);
		return -1;
	}

	node = list->head;

	while(node) {
		pid = node->job.pid;

		printf("%s: Querying pid %d for stats\n", __func__, pid);
		// get state:
		finished = waitpid(pid, &status, WNOHANG||WUNTRACED) == pid;
		stopped = WIFSTOPPED(status);
		finished &= !stopped;

		if(finished) { // remove
			printf("%s: pid %d finished. Cleaning\n", __func__, pid);
      if(! node->next) {
        list->tail = prev;
      }
			if(prev) {
				prev->next = node->next;
				free_node(node);
				node = prev->next;
			} else {
				list->head = node->next;
				free_node(node);
				node = list->head;
			}
		} else {  // update
			node->job.running = !stopped;
			prev = node;
			node = node->next;
		}
	}

	return 0;
}

// return the id>0 or -1 on fail
int jobs_add(void *jobs, int pid, char const* name) {
	jobs_list_t *list;
	jobs_node_t *node;
	int old_max_id;
	time_t now;
	char *name_copy;

	if (jobs_refresh(jobs) < 0)
		return -1;

	now  = time(NULL);
	list = (jobs_list_t *) jobs;
	node = (jobs_node_t *) malloc(sizeof(jobs_node_t));
	if(!node)
		return -1;

	name_copy = (char *) malloc((MAX_LINE_SIZE+1)*sizeof(char));
	if(!name_copy){
		free(node);
		return -1;
	}

  strcpy(name_copy, name);
	old_max_id = (list->tail)? list->tail->job.id : 0;
	node->next = NULL;
	node->job.id = old_max_id + 1;
	node->job.pid = pid;
	node->job.joined_time = now;
	node->job.name = name_copy;
	if(list->head == NULL) {
		list->head = list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}

	printf("Adding pid %d to job list\n", pid);

	return old_max_id + 1;
}

#define jobs_foreach(temp_node) for(; temp_node; temp_node = temp_node->next)

// return {0} if not found
job_t jobs_get_id(void *jobs, int id) {
	job_t zero = {};
	if (jobs_refresh(jobs) < 0)
		return zero;

	jobs_node_t *node = ((jobs_list_t *)jobs)->head;
	jobs_foreach(node) {
		if(node->job.id == id)
			return node->job;
	}

	return zero;
}

int jobs_remove(void* jobs, int id) {
	jobs_list_t *list = (jobs_list_t *) jobs;
	jobs_node_t *prev = NULL;
	jobs_node_t *node;

  if(jobs_refresh(jobs) < 0){
    perror("remove failed.");
    return -1;
  }
	node = list->head;

	while(node) {

		if(node->job.id == id) { // remove
			printf("%s: id %d removed.\n", __func__, id);
      if(! node->next) {
        list->tail = prev;
      }
			if(prev) {
				prev->next = node->next;
				free_node(node);
				node = prev->next;
			} else {
				list->head = node->next;
				free_node(node);
				node = list->head;
			}
		}
	}
  return 0;
}

job_t jobs_get_last(void *jobs, int only_stopped) {
  job_t result = {};
  jobs_node_t *node;

  if(jobs_refresh(jobs) < 0)
    return result;
  node =((jobs_list_t *) jobs)->head;

  jobs_foreach(node) {
    if(only_stopped && ! node->job.running)
      continue;
    if(result.id < node->job.id)
      result = node->job; 
  }
  return result;
}

int jobs_print_all(void *jobs) {
	jobs_list_t *list;
	jobs_node_t *node;
	int id, pid;
	time_t now;
	double diff;
	char const* format = "[%d] %s %d %d secs\n";
	char const* stopped_format = "[%d] %s %d %d secs (Stopped)\n";

	if (jobs_refresh(jobs) < 0)
		return -1;

	now  = time(NULL);
	list = (jobs_list_t *) jobs;
	node = list->head;
	jobs_foreach(node) {
		diff = difftime(now, node->job.joined_time);
		id = node->job.id;
		pid = node->job.pid;
		format = node->job.running ? format : stopped_format;
		printf(format, id, node->job.name, pid, (int)diff);
	}
	return 0;
}
