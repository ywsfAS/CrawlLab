#ifndef CONFIG_H
#define CONFIG_h
#define URL_SEED "https://github.com"
#define WORKERS 6
#define TEST_RUNTIME_SEC  (60)  // how long crawler runs (for testing)
#define MAX_DEPTH 5
#define ROBOTS_DIR "CRAWLED_ROBOTS"
#define HTML_DIR "CRAWLED_HTMLI"
#define QUEUE_CAP 640
#define HASH_MAP_SIZE 4000 // in bytes
#define STACK_SIZE_THREAD (1024 * 1024) // 1MB
#endif
