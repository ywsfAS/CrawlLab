# CrawlLab — Multithreaded Web Crawler in C

CrawlLab is a high-performance multithreaded web crawler written in C.  
The project focuses on low-level networking, concurrency, synchronization, and scalable task scheduling.

It fetches web pages, extracts links, avoids duplicates, and distributes work across threads using a work-stealing thread pool.

---

## Features Implemented

### HTTP Page Fetching
- TCP socket based HTTP client
- Supports both HTTP and HTTPS
- Downloads full HTML pages
- Handles network failures safely

### HTML Link Extraction
- Parses HTML content
- Extracts hyperlinks from `<a href="">` tags
- Handles absolute and relative URLs
- Filters invalid or malformed links

### Multithreaded Crawling Engine
- Custom thread pool implementation
- Work-stealing scheduler
- Per-worker local queues (circular buffer)
- Global task distribution support
- Efficient synchronization using:
  - pthread mutexes
  - pthread condition variables

### Task Scheduling System
- Each crawling task is represented as a job
- Jobs are dynamically created for newly discovered URLs
- Distributed across worker threads

### Thread-Safe Visited URL Set
- Implemented using a hash table
- Prevents crawling duplicate pages
- Atomic "check and add" operation
- Protected with mutex locking

### Memory Management
- Manual memory allocation tracking
- Proper cleanup of:
  - URLs
  - Job arguments
  - HTML buffers
  - Link lists
- Leak-safe queue and crawler shutdown

### URL Queue System
- Double-ended queue using head/tail
- Supports concurrent producers and consumers
- Condition variable based blocking

---

## Project Structure

```
CrawlLab/
├─ include/
│  ├─ config.h
│  ├─ error.h
│  ├─ hash_table.h
│  ├─ normalizer.h
│  ├─ socket.h
│  ├─ task_queue.h
│  ├─ visited_set.h
│  ├─ crawler.h
│  ├─ fetcher.h
│  ├─ job.h
│  ├─ parser.h
│  ├─ stats.h
│  └─ thread_pool.h
│
├─ src/
│  ├─ crawler.c
│  ├─ fetcher.c
│  ├─ main.c
│  ├─ parser.c
│  ├─ stats.c
│  └─ thread_pool.c
│
├─ CRAWLED_HTML/
├─ CRAWLED_ROBOTS/
├─ build/
├─ crawler_stats.csv
├─ LICENSE
├─ Makefile
└─ README.md
```

---

## Build and Run Instructions

Clean previous builds:

```
make clean
```

Build all binaries and prepare folders:

```
make all
```

Run the crawler:

```
make run
```

---

## Configuration

All project constants and macros are defined in:

```
include/config.h
```

Including:

- WORKERS  
- TEST_RUNTIME_SEC  
- STACK_SIZE_THREAD  
- HASH_MAP_SIZE  
- MAX_DEPTH  
- ROBOTS_DIR  
- HTML_DIR  

You can edit this file to adjust crawler behavior.

---

## Statistics

After running, the crawler prints:

- URLs discovered
- URLs visited
- Pages fetched OK / failed
- Robots.txt fetched
- Duplicate links filtered
- Jobs submitted, completed, and stolen
- Per-thread job counts

This allows tracking the efficiency of the work-stealing scheduler and thread pool.

---

## Output Directories

### CRAWLED_HTML

Stores downloaded HTML pages. Each file is named using a sanitized version of the URL.

Example:

```
CRAWLED_HTML/
├─ https___github.com
├─ https___github.com_features
├─ https___docs.github.com
```

---

### CRAWLED_ROBOTS

Stores robots.txt files fetched per domain.

Example:

```
CRAWLED_ROBOTS/
├─ https___github.com
├─ https___docs.github.com
```

---

## Statistics Output

After crawling, the program prints runtime statistics.

### Example Output

```
========== CRAWLER STATISTICS ==========
URLs Discovered      : 27822
URLs Visited         : 506
Domains Seen         : 48
----------------------------------------
Pages Fetched OK     : 506
Pages Failed         : 38
Robots.txt Fetched   : 48
----------------------------------------
Links Disallowed     : 0
Links Duplicates     : 21155
----------------------------------------
Jobs Submitted       : 797
Jobs Completed       : 699
Jobs Stolen          : 640
========================================
========= Thread Job Counts =========
Thread 0 executed 59 tasks
Thread 1 executed 94 tasks
Thread 2 executed 93 tasks
Thread 3 executed 89 tasks
Thread 4 executed 95 tasks
Thread 5 executed 93 tasks
Thread 6 executed 81 tasks
Thread 7 executed 95 tasks
```

---

## Performance Insights

These metrics help evaluate:

- Crawling efficiency
- Work-stealing effectiveness
- Thread load balancing
- Duplicate filtering performance

